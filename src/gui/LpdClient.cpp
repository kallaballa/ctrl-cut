/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "LpdClient.h"

LpdClient::LpdClient(QObject *parent) : QObject(parent), lpdstate(LPD_IDLE)
{
  this->socket = new QTcpSocket(this);
  this->socket->setObjectName("socket");
  QMetaObject::connectSlotsByName(this);
}

bool LpdClient::print(const QString &host, const QString &jobname, QByteArray rtldata)
{
  this->header.sprintf("\002EpilogCapture\n");
  this->ctrlfile.sprintf
    ("H%.31s\n"	/* RFC 1179, Section 7.2 - host name <= 31 chars */
     "P%.31s\n"	/* RFC 1179, Section 7.2 - user name <= 31 chars */
     "J%.99s\n"	/* RFC 1179, Section 7.2 - job name <= 99 chars */
     "ldfA%03d%.15s\n"
     "UdfA%03d%.15s\n"
     "N%.131s\n", /* RFC 1179, Section 7.2 - sourcefile name <= 131 chars */
     "vraag", "kintel", "somejob", 999, "vraal.local", 999, "vraag.local", "filename");
  
  this->ctrlheader.sprintf("\002%d cfA%03.3d%.15s\n", this->ctrlfile.length(), 999, "vraag.local");
  
  this->fileheader.sprintf("\003%lld dfA%03.3d%.15s\n", rtldata.size(), 999, "vraag.local");
  this->rtldata = rtldata;
  this->totalsize = this->header.size() + this->ctrlfile.size() + this->ctrlheader.size() + this->fileheader.size() + this->rtldata.size();
  this->totalprogress = 0;

  this->socket->connectToHost(host, 515);
}

void LpdClient::on_socket_connected()
{
  printf("connected\n");
  handle_lpd();
}

void LpdClient::on_socket_readyRead()
{
  //  printf("readyRead\n");
  char status = 0xff;
  int numread = this->socket->read(&status, 1);
  if (numread != 1) {
    fprintf(stderr, "No bytes to read\n");
  }
  else {
    //    printf("Status: %d\n", status);
    if (status == 1 && this->lpdstate != LPD_FILE) {
      emit done(true);
      this->lpdstate = LPD_IDLE;
      return;
    }
    handle_lpd();
  }
}

void LpdClient::on_socket_disconnected()
{
  printf("disconnected\n");
  if (this->lpdstate == LPD_DONE) {
    emit done(false);
  }
  this->lpdstate = LPD_IDLE;
}

void LpdClient::on_socket_error(QAbstractSocket::SocketError err)
{
  printf("error: %s\n", this->socket->errorString().toLocal8Bit().data());
}

void LpdClient::on_socket_bytesWritten(qint64 bytes)
{
  this->totalprogress += bytes;
  emit progress(this->totalprogress, this->totalsize);
}

void LpdClient::handle_lpd()
{
  switch (this->lpdstate) {
  case LPD_IDLE:
    this->socket->write(this->header.toLocal8Bit());
    //    printf(this->header.toLocal8Bit());
    this->lpdstate = LPD_HEADER;
    break;
  case LPD_HEADER:
    this->socket->write(this->ctrlheader.toLocal8Bit());
    //    printf(this->ctrlheader.toLocal8Bit());
    this->lpdstate = LPD_CTRL_HEADER;
    break;
  case LPD_CTRL_HEADER:
    this->socket->write(this->ctrlfile.toLocal8Bit());
    this->socket->write("\0", 1);
    //    printf(this->ctrlfile.toLocal8Bit());
    this->lpdstate = LPD_CTRL_FILE;
    break;
  case LPD_CTRL_FILE:
    this->socket->write(this->fileheader.toLocal8Bit());
    //    printf(this->fileheader.toLocal8Bit());
    this->lpdstate = LPD_FILE_HEADER;
    break;
  case LPD_FILE_HEADER:
    this->socket->write(this->rtldata);
    this->socket->write("\0", 1);
    this->lpdstate = LPD_FILE;
    break;
  case LPD_FILE:
    this->lpdstate = LPD_DONE;
    this->socket->disconnectFromHost();
    break;
  }
  this->socket->flush();
}

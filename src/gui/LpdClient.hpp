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
#ifndef LPDCLIENT_H_
#define LPDCLIENT_H_

#include <QtCore>
#include <QtNetwork>

class LpdClient : public QObject
{
  Q_OBJECT
public:
  LpdClient(QObject *parent = 0);
  ~LpdClient() {}

  bool print(const QString &host, const QString &jobname, QByteArray rtldata);

signals:
  void done(bool error);
  void progress(int done, int total);

private slots:
  void on_socket_connected();
  void on_socket_disconnected();
  void on_socket_error(QAbstractSocket::SocketError);
  void on_socket_readyRead();
  void on_socket_bytesWritten(qint64 bytes);

private:
  void handle_lpd();

  class QTcpSocket *socket;
  enum LpdState { LPD_IDLE, LPD_HEADER, LPD_CTRL_HEADER, LPD_CTRL_FILE, LPD_FILE_HEADER, LPD_FILE, LPD_DONE } lpdstate;
  QString header;
  QString ctrlheader;
  QString ctrlfile;
  QString fileheader;
  QByteArray rtldata;
  
  qint64 totalprogress;
  qint64 totalsize;
};

#endif

#include "LaosClient.h"

LaosClient::LaosClient(QObject *parent) : QObject(parent), laosstate(LAOS_IDLE)
{
  this->socket = new QTcpSocket(this);
  this->socket->setObjectName("socket");
  QMetaObject::connectSlotsByName(this);
}

bool LaosClient::print(const QString &host, const QString &jobname, QByteArray data)
{
  this->data = data;
  this->totalsize = this->data.size();
  this->totalprogress = 0;

  this->socket->connectToHost(host, 4000);
}

void LaosClient::on_socket_connected()
{
  printf("connected\n");
  handle_laos();
}

void LaosClient::on_socket_disconnected()
{
  printf("disconnected\n");
  if (this->laosstate == LAOS_DONE) {
    emit done(false);
  }
  this->laosstate = LAOS_IDLE;
}

void LaosClient::on_socket_error(QAbstractSocket::SocketError err)
{
  printf("error: %s\n", this->socket->errorString().toLocal8Bit().data());
}

void LaosClient::on_socket_bytesWritten(qint64 bytes)
{
  this->totalprogress += bytes;
  emit progress(this->totalprogress, this->totalsize);
}

void LaosClient::handle_laos()
{
  this->socket->write(this->data);
  this->socket->flush();
  this->socket->disconnectFromHost();
  this->laosstate = LAOS_DONE;
}

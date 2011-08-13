#ifndef LAOSCLIENT_H_
#define LAOSCLIENT_H_

#include <QtCore>
#include <QtNetwork>

class LaosClient : public QObject
{
  Q_OBJECT;
public:
  LaosClient(QObject *parent = 0);
  ~LaosClient() {}

  bool print(const QString &host, const QString &jobname, QByteArray laosdata);

signals:
  void done(bool error);
  void progress(int done, int total);

private slots:
  void on_socket_connected();
  void on_socket_disconnected();
  void on_socket_error(QAbstractSocket::SocketError);
  void on_socket_bytesWritten(qint64 bytes);

private:
  void handle_laos();
  enum LaosState { LAOS_IDLE, LAOS_DONE } laosstate;

  class QTcpSocket *socket;
  QByteArray laosdata;
  
  qint64 totalprogress;
  qint64 totalsize;
};

#endif

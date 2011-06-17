#ifndef LPDCLIENT_H_
#define LPDCLIENT_H_

#include <QtCore>
#include <QtNetwork>

class LpdClient : public QObject
{
  Q_OBJECT;
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

#ifndef PASSIVE_DATA_THREAD_H
#define PASSIVE_DATA_THREAD_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QColor>

class PassiveDataThread : public QObject
{
    Q_OBJECT
public:
    explicit PassiveDataThread(QObject *parent = nullptr);
    ~PassiveDataThread();
signals:
    void writeTextSignal(QString text, QColor color = {});
    void sendDataSignal(const QByteArray& data);

    void disconnectedSignal();
    void dataReceivedSignal(const QByteArray &data);

public slots:
    void startThread();
    void restartConnection(const QHostAddress &serverIp, int port);
    void stopConnection();

    void sendData(const QByteArray &data);

private slots:
    void connected();
    void onStarted();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);

private:
    QHostAddress m_serverIp;
    int m_serverPort;

    QThread m_thread;
    QTcpSocket *m_socket;
};

#endif // PASSIVE_DATA_THREAD_H

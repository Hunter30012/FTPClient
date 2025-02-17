#ifndef ACTIVE_DATA_THREAD_H
#define ACTIVE_DATA_THREAD_H

#include <QObject>
#include <QTcpServer>
#include <QThread>
#include <QTcpSocket>
#include <QColor>
#include <QNetworkInterface>
#include <QCoreApplication>

class ActiveDataThread : public QObject
{
    Q_OBJECT
    friend class ServerController;
public:
    explicit ActiveDataThread(QObject *parent = nullptr);
    ~ActiveDataThread();
signals:
    void writeTextSignal(QString text, QColor color = {});

    void sendDataSignal(const QByteArray& data);
    void dataReceivedSignal(const QByteArray &data);
public slots:
    void startThread();
    void restartListening(int port);
    void stopListening();

private slots:
    void onStarted();

    void onNewConnection();
    void onReadyRead();
    void disconnected();

    void sendData(const QByteArray& data);

private:
    int m_port;
    QHostAddress m_address;

    QThread m_thread;
    QTcpServer* m_server;
    QTcpSocket* m_socket;

};

#endif // ACTIVE_DATA_THREAD_H

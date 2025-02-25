#ifndef COMMAND_THREAD_H
#define COMMAND_THREAD_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QColor>
#include <QNetworkInterface>
#include "request_manager.h"
#include "data_converter.h"

class CommandThread : public QObject
{
    Q_OBJECT
    friend class ClientController;
public:
    explicit CommandThread(QObject *parent = nullptr);
    ~CommandThread();
signals:
    void writeTextSignal(QString text, QColor color = {});

    // void sendDataSignal(const QByteArray& data);
    void disconnectedSignal();
    void dataReceivedSignal(const QByteArray &data);

    // acticve mode
    void startActiveDataThreadSignal();
    void restartActiveDataSignal(int port);
    void stopActiveDataSignal();

    // passive mode
    void startPassiveDataThreadSignal();
    void restartPassiveDataThreadSignal(const QHostAddress& address, int port);
    void stopPassiveDataSignal();

    // Control Button
    void enableDisconnectSignal();
    void disableDisconnectSignal();

public slots:
    void startThread(const QHostAddress& address, int port, bool isActiveMode);
    void stopConnection();

    void sendData(const QByteArray &data);

private slots:
    void disconnected();
    void connected();
    void onStarted();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);
    void parseResponse(const QByteArray &requestData);

private:
    bool m_isActiveMode;
    QHostAddress m_serverIp;
    int m_serverPort;

    QThread m_thread;
    QTcpSocket *m_socket;
};

#endif // COMMAND_THREAD_H

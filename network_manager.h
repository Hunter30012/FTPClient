#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QColor>
#include <QMetaEnum>
#include "tcpclient_thread.h"
#include "tcpserver_thread.h"

class NetworkManager : public QObject
{
    Q_OBJECT
    friend class ClientController;
public:
    explicit NetworkManager(QObject *parent = nullptr);
    static bool isValidPort(const QString& port);

signals:
    void writeTextSignal(QString text, QColor color = {});
    void parseJsonRecdSignal(const QByteArray& jsonArray);

public slots:
    void connectToServer(const QString& serverAddress, const QString& serverPort, const bool& isActive);
    void disconnect();

    // TCP Socket
    void connected();
    void disconnected();
    void readyRead();
    void stateChanged(QAbstractSocket::SocketState socketState);
    void errorOccured(QAbstractSocket::SocketError socketError);

    void witeData(const QByteArray& data);
    // QByteArray readAll();

    // QByteArray parseByteData();
    // void parseByteDownload(const QByteArray& data);
    //
    // void uploadFileData();

private:
    TcpClientThread m_commandThread;

    TcpServerThread m_activeDataThread;
    TcpClientThread m_passiveDataThread;

    bool m_isDownloading;
    bool m_isUploading;

    QByteArray dataToSend;
};

#endif // NETWORK_MANAGER_H

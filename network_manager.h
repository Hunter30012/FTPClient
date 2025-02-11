#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QColor>
#include <QMetaEnum>

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);
    static bool isValidPort(const QString& port);

signals:
    void writeTextSignal(QString text, QColor color = {});

public slots:
    // TCP Socket
    void connectToServer(const QString& serverAddress, const QString& serverPort);
    void disconnect();

    void connected();
    void disconnected();
    void readyRead();
    void stateChanged(QAbstractSocket::SocketState socketState);
    void errorOccured(QAbstractSocket::SocketError socketError);

    void witeData(const QByteArray& data);
    QByteArray readAll();
    //
    // void uploadFileData();

private:
    QTcpSocket m_tcpSocket;

    bool m_isDownloading;

    QByteArray dataToSend;
};

#endif // NETWORK_MANAGER_H

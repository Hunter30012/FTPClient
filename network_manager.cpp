#include "network_manager.h"

NetworkManager::NetworkManager(QObject *parent)
    : QObject{parent}, m_isDownloading {false}
{

}

bool NetworkManager::isValidPort(const QString &port)
{
    if (port.isEmpty()) {
        return false;
    }

    bool isNumber;
    int portNumber = port.toInt(&isNumber);

    return isNumber && (portNumber >= 1 && portNumber <= 65535);
}

void NetworkManager::connectToServer(const QString &serverAddress, const QString &serverPort, const bool& isActive)
{
    emit writeTextSignal("Attemping to connecting to server at " + serverAddress + ":" + serverPort);

    // if(m_tcpSocket.isOpen()) {
    //     disconnect();
    // }
    // int port = serverPort.toUInt();

    // m_tcpSocket.connectToHost(serverAddress, port);
}

void NetworkManager::disconnect()
{
    // m_tcpSocket.close();
    // m_tcpSocket.waitForDisconnected();
    // m_isDownloading = false;
}

void NetworkManager::connected()
{
    emit writeTextSignal("Connected to server.", Qt::blue);

}

void NetworkManager::disconnected()
{
    emit writeTextSignal("Disconnected from server!", Qt::red);
}

void NetworkManager::readyRead()
{
    // QByteArray data = (m_isDownloading) ? m_tcpSocket.readAll() : parseByteData();

    // if (data.isEmpty()) {
    //     return;
    // }

    // if (m_isDownloading) {
    //     parseByteDownload(data);
    // } else {
    //     emit parseJsonRecdSignal(data);
    // }
}

void NetworkManager::stateChanged(QAbstractSocket::SocketState socketState)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    const char* state = metaEnum.valueToKey(socketState);
    if(state) {
        QString stateString(state);
        emit writeTextSignal("State connection from server: " + stateString);
    }
}

void NetworkManager::errorOccured(QAbstractSocket::SocketError socketError)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    const char* error = metaEnum.valueToKey(socketError);
    if(error) {
        QString errorString(error);
        emit writeTextSignal("Error: " + errorString, Qt::red);
    }
}

void NetworkManager::witeData(const QByteArray &data)
{
    // m_tcpSocket.write(data);
}

// QByteArray NetworkManager::readAll()
// {
//     return m_tcpSocket.readAll();
// }

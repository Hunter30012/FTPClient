#include "network_manager.h"

NetworkManager::NetworkManager(QObject *parent)
    : QObject{parent}, m_isDownloading {false}
{
    // connect to ActiveData Thread
    connect(&m_commandThread, &CommandThread::startActiveDataThreadSignal, &m_activeDataThread, &ActiveDataThread::startThread, Qt::QueuedConnection);
    connect(&m_commandThread, &CommandThread::stopActiveDataSignal, &m_activeDataThread, &ActiveDataThread::stopListening, Qt::QueuedConnection);
    connect(&m_commandThread, &CommandThread::restartActiveDataSignal, &m_activeDataThread, &ActiveDataThread::restartListening, Qt::QueuedConnection);
    //connect to PassiveDataThread
    connect(&m_commandThread, &CommandThread::startPassiveDataThreadSignal, &m_passiveDataThread, &PassiveDataThread::startThread, Qt::QueuedConnection);
    connect(&m_commandThread, &CommandThread::stopPassiveDataSignal, &m_passiveDataThread, &PassiveDataThread::stopConnection, Qt::QueuedConnection);
    connect(&m_commandThread, &CommandThread::restartPassiveDataThreadSignal, &m_passiveDataThread, &PassiveDataThread::restartConnection, Qt::QueuedConnection);
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

void NetworkManager::connectToServer(const QHostAddress &serverAddress, int serverPort, const bool& isActive)
{
    if(isActive) {
        emit writeTextSignal("Active Mode!", Qt::darkGreen);
    } else {
        emit writeTextSignal("Passive Mode!", Qt::darkGreen);
    }
    emit writeTextSignal("Attemping to connecting to server at " + serverAddress.toString() + ":" + QString::number(serverPort), Qt::darkGreen);
    m_commandThread.startThread(serverAddress, serverPort, isActive);
}

void NetworkManager::stopConnectingToServer()
{
    emit stopServerSignal();
}

void NetworkManager::witeData(const QByteArray &data)
{
    // m_tcpSocket.write(data);
}


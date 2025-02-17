#include "active_data_thread.h"

ActiveDataThread::ActiveDataThread(QObject *parent)
    : QObject{parent}, m_server {nullptr}, m_socket {nullptr}
{
    const QHostAddress& localhost = QHostAddress(QHostAddress::LocalHost);
    const QList<QHostAddress> listAddress = QNetworkInterface::allAddresses();
    for (const QHostAddress &address : listAddress) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost) {
            m_address = address;
            break;
        }
    }
    connect(this, &ActiveDataThread::sendDataSignal, this, &ActiveDataThread::sendData);
}

ActiveDataThread::~ActiveDataThread()
{
    if (m_socket) {
        delete m_socket;
        m_socket = nullptr;
    }

    if (m_server) {
        m_server->close();
        delete m_server;
        m_server = nullptr;
    }

    if (m_thread.isRunning()) {
        m_thread.quit();
        m_thread.wait();
    }
}

void ActiveDataThread::startThread()
{
    if (!m_thread.isRunning()) {
        this->moveToThread(&m_thread);
        connect(&m_thread, &QThread::started, this, &ActiveDataThread::onStarted);
        qDebug() << "Start Active Data Thread";
        m_thread.start();

    }
}

void ActiveDataThread::onStarted()
{
    QThread::currentThread()->setObjectName("ActiveData Thread");
    qDebug() << "After started: " << QThread::currentThread();
}

void ActiveDataThread::onNewConnection()
{
    if(m_socket) {
        m_socket->disconnect();
        m_socket->close();
        delete m_socket;
        m_socket = nullptr;
    }

    m_socket = m_server->nextPendingConnection();
    connect(m_socket, &QTcpSocket::disconnected, this, &ActiveDataThread::disconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &ActiveDataThread::onReadyRead);
    qInfo() << "Active Data Socket connected!" << QThread::currentThread();
}

void ActiveDataThread::restartListening(int port)
{
    qDebug() << "RestartListening in :" << QThread::currentThread();
    m_port = port;
    if (!m_server) {
        m_server = new QTcpServer();
        m_server->moveToThread(&m_thread);
        connect(m_server, &QTcpServer::newConnection, this, &ActiveDataThread::onNewConnection);
    } else {
        m_server->close();
    }

    // listen on port
    bool isListening = m_server->listen(m_address, m_port);
    if(isListening) {
        qDebug() << "ActiveDataThread listening on address: " << m_server->serverAddress().toString()
        << " , port: " << QString::number(m_server->serverPort());
    } else {
        qDebug() << "ActiveDataThread failed to start.";
    }
}

void ActiveDataThread::sendData(const QByteArray &data)
{
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->write(data);
    } else {
        qWarning() << "No active connection to write data.";
    }
}

void ActiveDataThread::onReadyRead()
{
    if (m_socket) {
        QByteArray data = m_socket->readAll();
        qDebug() << "ActiveData received: " << data;
        sendData("ActiveData response: " + data);  // Response to Client
    }
}

void ActiveDataThread::stopListening()
{
    qDebug() << "Stop Listening in ActiveDataThread";
    if (m_server) {
        m_server->close();
    }
    if (m_socket) {
        disconnect(m_socket, nullptr, this, nullptr);
        m_socket->disconnectFromHost();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
}

void ActiveDataThread::disconnected()
{
    qInfo() << "ActiveDataThread Disconnected";
    if (m_socket) {
        m_socket->disconnectFromHost();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
}



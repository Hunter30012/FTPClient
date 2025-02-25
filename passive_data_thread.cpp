#include "passive_data_thread.h"

PassiveDataThread::PassiveDataThread(QObject *parent)
    : QObject{parent}, m_socket {nullptr}
{
    connect(this, &PassiveDataThread::sendDataSignal, this, &PassiveDataThread::sendData);
}

PassiveDataThread::~PassiveDataThread()
{
    if (m_socket) {
        disconnect(m_socket, nullptr, this, nullptr);
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->waitForDisconnected(3000);
        }
        delete m_socket;
        m_socket = nullptr;
    }
    m_thread.quit();
    m_thread.wait();
}

void PassiveDataThread::startThread()
{
    qDebug() << "Start Passive Data Thread";
    if (!m_thread.isRunning()) {
        this->moveToThread(&m_thread);
        connect(&m_thread, &QThread::started, this, &PassiveDataThread::onStarted);
        m_thread.start();
    }
}

void PassiveDataThread::onStarted()
{
    QThread::currentThread()->setObjectName("PassiveData Thread");
    qDebug() << "After started: " << QThread::currentThread();
}

void PassiveDataThread::restartConnection(const QHostAddress &serverIp, int port)
{
    m_serverIp = serverIp;
    m_serverPort = port;

    m_socket = new QTcpSocket();
    connect(m_socket, &QTcpSocket::connected, this, &PassiveDataThread::connected);
    connect(m_socket, &QTcpSocket::readyRead, this, &PassiveDataThread::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &PassiveDataThread::onError);
    qDebug() << "restartConnection: " << QThread::currentThread();
    qInfo() << "Trying to connect to " << m_serverIp.toString() << ":" << m_serverPort;
    m_socket->connectToHost(m_serverIp, m_serverPort);

    if (!m_socket->waitForConnected(30000)) {
        qWarning() << "Connection failed!";
    }
}

void PassiveDataThread::stopConnection()
{
    qDebug() << "Stopping connection in: " << QThread::currentThread();

    if (m_socket) {
        disconnect(m_socket, nullptr, this, nullptr);
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->waitForDisconnected(3000);
        }
        m_socket->deleteLater();
        m_socket = nullptr;
    }
}

void PassiveDataThread::sendData(const QByteArray &data)
{
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->write(data);
    } else {
        qWarning() << "Cannot send data, no Passive connection!";
    }
}

void PassiveDataThread::onReadyRead()
{
    qDebug() << "onReadyRead: " << QThread::currentThread();
    QByteArray data = m_socket->readAll();
    // qDebug() << "Received from Server: " << data;
    // handle Data
    emit dataReceivedSignal(data);
}

void PassiveDataThread::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Error:" << socketError << " " << m_socket->errorString();
    emit writeTextSignal(m_socket->errorString(), Qt::darkRed);
}

void PassiveDataThread::connected()
{
    qDebug() << QThread::currentThread();
    qDebug() << "Connected from Data thread";
}

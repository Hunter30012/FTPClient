#include "command_thread.h"

CommandThread::CommandThread(QObject *parent)
    : QObject{parent}, m_socket {nullptr}
{
    this->moveToThread(&m_thread);
    connect(&m_thread, &QThread::started, this, &CommandThread::onStarted);
    connect(this, &CommandThread::sendDataSignal, this, &CommandThread::sendData);
}

CommandThread::~CommandThread()
{

}

void CommandThread::startThread(const QHostAddress& address, int port, bool isActiveMode)
{
    m_isActiveMode = isActiveMode;
    m_serverIp = address;
    m_serverPort = port;

    qDebug() << "Start Commnad Thread";
    if (!m_thread.isRunning()) {
        // Active mode
        emit startActiveDataThreadSignal();
        // Passive mode
        emit startPassiveDataThreadSignal();

        m_thread.start();
    } else {
        qDebug() << "Thread is running";
        QMetaObject::invokeMethod(this, "onStarted", Qt::QueuedConnection);
    }
}

void CommandThread::onStarted()
{
    if(!m_socket) {
        m_socket = new QTcpSocket(this);
        m_socket->moveToThread(QThread::currentThread());
        qInfo() << "Trying to connect to " << m_serverIp.toString() << ":" << m_serverPort;
        m_socket->connectToHost(m_serverIp, m_serverPort);
        connect(m_socket, &QTcpSocket::connected, this, &CommandThread::connected);
        connect(m_socket, &QTcpSocket::readyRead, this, &CommandThread::onReadyRead);
        connect(m_socket, &QTcpSocket::errorOccurred, this, &CommandThread::onError);
        connect(m_socket, &QTcpSocket::disconnected,this, &CommandThread::disconnected);
    } else {
        if(m_socket->isOpen()) {
            disconnect();
        }
    }

    QThread::currentThread()->setObjectName("Command Thread");
    emit writeTextSignal("Client connecting to: " + m_serverIp.toString() +
                             " , port: " + QString::number(m_serverPort), Qt::darkGreen);
    if (!m_socket->waitForConnected(5000)) {
        qWarning() << "Connection failed!";
    }


}

void CommandThread::connected()
{
    qDebug() << QThread::currentThread();
    qDebug() << "Connected from Command thread";

    emit writeTextSignal("Connected from IP: " + m_socket->peerAddress().toString() +  ":" + QString::number(m_serverPort), Qt::darkBlue);
    emit enableStopSignal();

    // Check mode

    if(m_isActiveMode) {
        emit restartActiveDataSignal(2121);
    } else {
        // QHostAddress addr_test("169.254.198.94");
        // emit restartPassiveDataThreadSignal(addr_test, m_serverPort + 1);
    }
}


void CommandThread::stopConnection()
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
    emit writeTextSignal("Stop connection!", Qt::darkBlue);
    emit disableStopSignal();
}

void CommandThread::sendData(const QByteArray &data)
{
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->write(data);
    } else {
        qWarning() << "Cannot send data, no active connection!";
    }
}

void CommandThread::disconnected()
{
    // if(m_isActiveMode) {
    //     emit stopActiveDataSignal();
    // } else {
    //     emit stopPassiveDataSignal();
    // }

    qInfo() << "CommandThread Disconnected";
    emit writeTextSignal("Disconected!", Qt::red);
    if (m_socket) {
        m_socket->disconnectFromHost();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
}

void CommandThread::onReadyRead()
{
    qDebug() << "onReadyRead: " << QThread::currentThread();
    QByteArray data = m_socket->readAll();
    qDebug() << "Received from Server: " << data;
    // handle Data
    emit dataReceivedSignal(data);
}

void CommandThread::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Error:" << socketError << " " << m_socket->errorString();
    emit writeTextSignal(m_socket->errorString(), Qt::darkRed);
}

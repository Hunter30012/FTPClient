#include "command_thread.h"

CommandThread::CommandThread(QObject *parent)
    : QObject{parent}, m_socket {nullptr}
{
    this->moveToThread(&m_thread);
    connect(&m_thread, &QThread::started, this, &CommandThread::onStarted);
    // connect(this, &CommandThread::sendDataSignal, this, &CommandThread::sendData);
}

CommandThread::~CommandThread()
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
    if (m_socket) {
        if (m_socket->isOpen()) {
            m_socket->close();
        }
        m_socket->deleteLater();
        m_socket = nullptr;
    }

    m_socket = new QTcpSocket(this);
    m_socket->moveToThread(QThread::currentThread());

    qInfo() << "Trying to connect to " << m_serverIp.toString() << ":" << m_serverPort;
    connect(m_socket, &QTcpSocket::connected, this, &CommandThread::connected);
    connect(m_socket, &QTcpSocket::readyRead, this, &CommandThread::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &CommandThread::onError);
    connect(m_socket, &QTcpSocket::disconnected, this, &CommandThread::disconnected);

    m_socket->connectToHost(m_serverIp, m_serverPort);

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
    emit enableDisconnectSignal();

    // Check mode
    if(m_isActiveMode) {
        // start data socket
        emit restartActiveDataSignal(5050);
        qDebug() << "Send Active Mode command";

        QHostAddress hostAddress;
        const QHostAddress& localhost = QHostAddress(QHostAddress::LocalHost);
        const QList<QHostAddress> listAddress = QNetworkInterface::allAddresses();
        for (const QHostAddress &address : listAddress) {
            if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost) {
                hostAddress = address;
                break;
            }
        }
        QMap<QString, QString> requestVariables {
            {"address_data_thread", hostAddress.toString()},
            {"port_data_thread", QString::number(5050)},
        };
        QJsonObject request = RequestManager::createServerRequest(RequestManager::RequestType::ActiveConnect, requestVariables);
        QByteArray requestData = DataConverter::JsonObjectToByteArray(request);
        this->sendData(requestData);
    } else {
        qDebug() << "Send Passive Mode command";
        QJsonObject request = RequestManager::createServerRequest(RequestManager::RequestType::PassiveConnect);
        QByteArray requestData = DataConverter::JsonObjectToByteArray(request);
        this->sendData(requestData);
        emit restartPassiveDataThreadSignal(m_serverIp, m_serverPort + 1);
    }
}


void CommandThread::stopConnection()
{
    if(m_isActiveMode) {
        emit stopActiveDataSignal();
    } else {
        emit stopPassiveDataSignal();
    }
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
    emit disableDisconnectSignal();
}

void CommandThread::sendData(const QByteArray &data)
{
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->write(data);
        m_socket->flush();
    } else {
        qWarning() << "Cannot send data, no active connection!";
    }
}

void CommandThread::disconnected()
{
    if(m_isActiveMode) {
        emit stopActiveDataSignal();
    } else {
        emit stopPassiveDataSignal();
    }

    qInfo() << "CommandThread Disconnected";
    emit writeTextSignal("Disconected!", Qt::red);
    emit disableDisconnectSignal();
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
    // emit dataReceivedSignal(data);
}

void CommandThread::onError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Error:" << socketError << " " << m_socket->errorString();
    emit writeTextSignal(m_socket->errorString(), Qt::darkRed);
}

void CommandThread::parseResponse(const QByteArray &requestData)
{

}

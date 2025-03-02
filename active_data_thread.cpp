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
        qDebug() << "Start Active Data Thread";
        this->moveToThread(&m_thread);
        connect(&m_thread, &QThread::started, this, &ActiveDataThread::onStarted);
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
    connect(m_socket, &QTcpSocket::disconnected, this, &ActiveDataThread::disconnected, Qt::QueuedConnection);
    connect(m_socket, &QTcpSocket::readyRead, this, &ActiveDataThread::onReadyRead, Qt::QueuedConnection);
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
        m_socket->flush();
    } else {
        qWarning() << "No active connection to write data.";
    }
}

void ActiveDataThread::uploadRequest(bool isDir, const QString &filePath, const QString &fileName, const QString &serverCurDir)
{
    QFile qFile;
    const qint64 packetSize = 20000;
    quint64 writtenBytes = 0;
    quint64 size = 0;
    QByteArray fileData;

    // Folder
    if(isDir) {
        QJsonObject request = RequestManager::createUploadRequest(RequestManager::RequestType::UploadedFile,
                                                                  filePath,
                                                                  serverCurDir,
                                                                  fileName,
                                                                  true);
        qDebug() << "Request Upload: " << request;
        QByteArray data = DataConverter::JsonObjectToByteArray(request);
        this->sendData(data);
        return;
    }

    // File
    qFile.setFileName(filePath);
    size = qFile.size();
    if (!qFile.open(QIODevice::ReadOnly) || !qFile.isReadable()) {
        emit writeTextSignal("Can not open file: " + filePath, Qt::red);
        return;
    }
    // Send file immediately
    if(size < packetSize) {
        writtenBytes = size;
        fileData = qFile.read(size);
        QJsonObject request = RequestManager::createUploadRequest(
            RequestManager::RequestType::UploadedFile,
            filePath,
            serverCurDir,
            fileName,
            false,
            writtenBytes,
            size,
            fileData);
        this->sendData(DataConverter::JsonObjectToByteArray(request));
        emit writeTextSignal("Transfered file " + fileName +" to Server!", Qt::darkBlue);
        QThread::msleep(10);
        return;
    }
    // Split file into chunks and send
    while (writtenBytes < size) {
        qFile.seek(writtenBytes);
        fileData = qFile.read(packetSize);
        quint64 currentChunkSize = fileData.size();
        writtenBytes += currentChunkSize;

        QJsonObject request = RequestManager::createUploadRequest(
            RequestManager::RequestType::UploadingFile,
            filePath,
            serverCurDir,
            fileName,
            false,
            writtenBytes,
            size,
            fileData);
        emit writeTextSignal(QString("Sent %1/%2 bytes for file: %3")
                                 .arg(writtenBytes).arg(size).arg(fileName),
                             Qt::darkYellow);
        this->sendData(DataConverter::JsonObjectToByteArray(request));
        QThread::msleep(10);
    }
    // download big file: Done
    QJsonObject request = RequestManager::createUploadRequest(
        RequestManager::RequestType::UploadedFile,
        filePath,
        serverCurDir,
        fileName,
        false,
        writtenBytes,
        size,
        fileData);
    this->sendData(DataConverter::JsonObjectToByteArray(request));
    emit writeTextSignal("Finished sending file: " + fileName, Qt::darkGreen);
}

void ActiveDataThread::onReadyRead()
{
    if (m_socket) {
        qDebug() << "onReadyRead in :" << QThread::currentThread();
        QByteArray data = m_socket->readAll();
        // qDebug() << "ActiveData received: " << data;
        // sendData("ActiveData response: " + data);  // Response to Client
        emit dataReceivedSignal(data);
    }
}

void ActiveDataThread::stopListening()
{
    qDebug() << "Stop Listening in ActiveDataThread: " << QThread::currentThread();
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



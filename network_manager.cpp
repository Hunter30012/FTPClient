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

NetworkManager::~NetworkManager()
{
    m_saveFile.commit();
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
    emit stopClientSignal();
}

void NetworkManager::onDownloadedFile(const QJsonObject &obj)
{
    QString localPath = obj["localPath"].toString();
    QString filePathServer = obj["filePathServer"].toString();
    bool isDir = obj["isDir"].toBool();

    QString fileName = QFileInfo(filePathServer).fileName();
    QString localSavePath = localPath + "/" + fileName;

    if (!m_saveFile.isOpen()) {
        if (FileHandler::checkFileExists(localPath, fileName)) {
            fileName = FileHandler::changeFileName(fileName, localPath);
            localSavePath = localPath + "/" + fileName;
        }
    }

    qDebug() << "savePath: " << localSavePath;

    if (isDir) {
        QDir().mkpath(localSavePath);
        emit writeTextSignal("Created folder: " + localSavePath, Qt::darkGreen);

        QJsonArray fileList = obj["fileList"].toArray();
        QStringList downloadPaths;
        for(int i = 0; i < fileList.count(); i++) {
            downloadPaths.append(fileList[i].toString());
        }
        QJsonObject request = RequestManager::createDownloadRequest(localSavePath, downloadPaths);
        qDebug() << "Request Download: " << request;
        QByteArray data = DataConverter::JsonObjectToByteArray(request);
        m_commandThread.sendData(data);
        return;
    }

    // File
    quint64 sizeFile = obj["sizeFile"].toString().toULongLong();
    QByteArray dataPacket = QByteArray::fromBase64(obj["dataPacket"].toString().toLatin1());
    // small file
    if(sizeFile < packetSize) {
        m_saveFile.setFileName(localSavePath);
        bool ret = m_saveFile.open(QIODevice::WriteOnly);
        if(!ret) {
            emit writeTextSignal("Failed to save downloaded file: " + fileName, Qt::red);
            return;
        }
        m_saveFile.write(dataPacket);
        m_saveFile.commit();
        emit writeTextSignal("Download file " + fileName + " successfully!", Qt::darkBlue);
    } else {
        // downloaded big file
        m_saveFile.commit();
        emit writeTextSignal("Download file " + fileName + " successfully!", Qt::darkBlue);
    }
}

void NetworkManager::onDownloadingFile(const QJsonObject &obj)
{
    QString localPath = obj["localPath"].toString();
    QString filePathServer = obj["filePathServer"].toString();

    quint64 sizeFile = obj["sizeFile"].toString().toULongLong();
    quint64 writtenBytes = obj["writtenBytes"].toString().toULongLong();
    QByteArray dataPacket = QByteArray::fromBase64(obj["dataPacket"].toString().toLatin1());

    QString fileName = QFileInfo(filePathServer).fileName();
    QString localSavePath = localPath + "/" + fileName;

    if (!m_saveFile.isOpen()) {
        if (FileHandler::checkFileExists(localPath, fileName)) {
            fileName = FileHandler::changeFileName(fileName, localPath);
            localSavePath = localPath + "/" + fileName;
        }
        qDebug() << localSavePath;
        m_saveFile.setFileName(localSavePath);
        if (!m_saveFile.open(QIODevice::WriteOnly)) {
            emit writeTextSignal("Failed to open file: " + fileName, Qt::red);
            return;
        }
    }

    m_saveFile.write(dataPacket);
    emit writeTextSignal(QString("Received %1/%2 bytes for file: %3")
                             .arg(writtenBytes).arg(sizeFile).arg(fileName),
                         Qt::darkBlue);

    // if (writtenBytes >= sizeFile) {
    //     m_saveFile.commit();
    //     emit writeTextSignal("Download completed: " + fileName, Qt::darkGreen);
    // }
}




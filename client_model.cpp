#include "client_model.h"

ClientModel::ClientModel(QWidget *parent)
    : QObject{(QObject*) parent}
{
    // set information app
    QCoreApplication::setApplicationName("FTP Client");

    // setup local FileSystem
    m_currentLocalDir = m_settingsManager.getDefaultDirectory();
    m_localFileSystem = new QFileSystemModel(this);
    m_localFileSystem->setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    m_localFileSystem->setRootPath(m_currentLocalDir);
}

void ClientModel::init()
{
    // Get saved data
    savedConnection saved = m_settingsManager.getSavedConnection();

    emit initClientWindowSignal(saved.checkboxChecked,
                                saved.serverAddress,
                                saved.serverPort);
    emit setLocalFileSystemSignal(m_localFileSystem);
    emit writeTextSignal("App Start!", Qt::blue);
}

void ClientModel::browseHomeLocal()
{
    qDebug() << "Click Home in Local";
    setFileSystem("");
}

void ClientModel::returnPreviousFolderLocal()
{
    qDebug() << "Click Return in Local";
    QString path = FileHandler::getPreviousFolderPath(m_currentLocalDir);
    setFileSystem(path);
}

void ClientModel::openFolder(const QString &dir, bool searchInServer)
{
    qDebug() << "Open Folder";
    if(searchInServer) {
        // open Folder in Server by searching
    } else {
        // open Folder in Local
        QDir pathDir(dir);
        if(pathDir.exists()) {
            setFileSystem(dir);
            emit writeTextSignal("Open " + dir + " successfully.", Qt::blue);
        } else {
            emit writeTextSignal("Open " + dir + " unsuccessfully.", Qt::red);
        }
    }
}

void ClientModel::openSelectedDir(const QModelIndex &index)
{
    qDebug() << "Selected in Local: Clicked on row: " << index.row() << " on colum: " << index.column();
    // TBD
    QModelIndex selectedIndex = m_localFileSystem->index(index.row(), 0, index.parent());
    bool isDir = m_localFileSystem->isDir(selectedIndex);
    if(isDir) {
        QString filePath = m_localFileSystem->filePath(selectedIndex);
        setFileSystem(filePath);
    } else {
        QString fileName = m_localFileSystem->fileName(selectedIndex);
        emit writeTextSignal(fileName + " isn't a directory", Qt::red);
    }
}

void ClientModel::deleteInLocal(QModelIndexList &listIndex)
{
    for(int i = 0; i < listIndex.count(); i++) {
        QString filePath = m_localFileSystem->filePath(listIndex[i]);
        QFileInfo fileInfo(filePath);

        if(fileInfo.isFile()) {
            bool ret;
            ret = QFile::remove(filePath);
            if(ret) {
                emit writeTextSignal("Delete " + filePath + " successfully.", Qt::blue);
            } else {
                emit writeTextSignal("Can not delete " + filePath + ".", Qt::red);
            }
        } else if(fileInfo.isDir()) {
            bool ret;
            ret = QDir(filePath).removeRecursively();
            if(ret) {
                emit writeTextSignal("Delete " + filePath + " successfully.", Qt::blue);
            } else {
                emit writeTextSignal("Can not delete " + filePath + ".", Qt::red);
            }
        }
    }
}

void ClientModel::returnPreviousFolderServer()
{
    if(!m_serverFileList.isEmpty()) {
        QMap<QString, QString> requestVariables{
            {"requestPath", FileHandler::getPreviousFolderPath(m_currentServerDir) }
        };
        QJsonObject request = RequestManager::createServerRequest(RequestManager::RequestType::ChangeDir, requestVariables);
        QByteArray data = DataConverter::JsonObjectToByteArray(request);
        emit sendCommandDataSignal(data);
    }
}

void ClientModel::deleteInServer(QModelIndexList &listIndex)
{
    QStringList deletePaths;
    for (int i = 0; i < listIndex.count(); ++i) {
        deletePaths.append(m_serverFileList[listIndex[i].row()].m_filePath);
    }
    emit writeTextSignal("Deleting: " + QString::number(deletePaths.count()) + " items from the server.", Qt::blue);
    QJsonObject request = RequestManager::createServerRequest(RequestManager::RequestType::Delete, {}, deletePaths);
    QByteArray data = DataConverter::JsonObjectToByteArray(request);
    emit sendCommandDataSignal(data);
}

void ClientModel::connectToServer(const bool &saveInformation, const QString &serverAddress, const QString &serverPort, const bool& isActive)
{
    QHostAddress address(serverAddress);
    int port;
    if(address.isNull()) {
        emit writeTextSignal("Please enter a valid IP address!", Qt::red);
        return;
    }
    if(!NetworkManager::isValidPort(serverPort)) {
        emit writeTextSignal("Please enter a valid Port number!", Qt::red);
        return;
    }

    if(saveInformation) {
        m_settingsManager.setSavedConnection(saveInformation, serverAddress, serverPort);
    }
    port = serverPort.toInt();
    m_networkManager.connectToServer(address, port, isActive);
}

void ClientModel::disconnectButton()
{
    if(m_serverFileSystem) {
        delete m_serverFileSystem;
        m_serverFileSystem = nullptr;
    }
    m_networkManager.stopConnectingToServer();
}


// handle data from Server
void ClientModel::parseJsonRecd(const QByteArray &data)
{
    bool isJson = RequestManager::checkIfDataIsJson(data);
    if (!isJson) {
        emit writeTextSignal("Response from Server - Invalid Json format!", Qt::red);
        return;
    }
    QJsonParseError jsonError;
    QJsonArray jsonArray = QJsonDocument::fromJson(data, &jsonError).array();
    QJsonObject serverResponseInfo = jsonArray.first().toObject();
    RequestManager::ResponseType responseType = static_cast<RequestManager::ResponseType>(serverResponseInfo.value("response_status").toInt());

    m_currentServerDir = serverResponseInfo.value("directory").toString();
    qDebug() << "Server Dir: " << m_currentServerDir;
    emit writeTextSignal("Server Dir: " + m_currentServerDir, Qt::darkBlue);

    switch (responseType) {
    case RequestManager::ResponseType::ActiveConnected:
        qDebug() << "Response: Active Connected";
        m_isActiveMode = true;
        m_serverFileList.clear();
        m_serverFileList = FileHandler::getFileListFromJson(jsonArray);
        m_serverFileSystem = new FileListServerModel(m_serverFileList, this);
        emit writeTextSignal("Established connection successfully!", Qt::darkBlue);
        emit connectedToServerSignal(m_serverFileSystem, m_currentServerDir);
        break;

    case RequestManager::ResponseType::PassiveConnected:
        m_isActiveMode = false;
        qDebug() << "Response: Passive Connected";
        m_serverFileList.clear();
        m_serverFileList = FileHandler::getFileListFromJson(jsonArray);
        m_serverFileSystem = new FileListServerModel(m_serverFileList, this);
        emit writeTextSignal("Established connection successfully!", Qt::darkBlue);
        emit connectedToServerSignal(m_serverFileSystem, m_currentServerDir);
        break;

    case RequestManager::ResponseType::ChangedDir:
        qDebug() << "Response: Directory Changed";
        m_serverFileList.clear();
        m_serverFileList = FileHandler::getFileListFromJson(jsonArray);
        if(m_serverFileSystem) {
            delete m_serverFileSystem;
            m_serverFileSystem = nullptr;
        }
        m_serverFileSystem = new FileListServerModel(m_serverFileList, this);
        emit writeTextSignal("Changed directory successfully!", Qt::darkBlue);
        emit connectedToServerSignal(m_serverFileSystem, m_currentServerDir);
        break;

    case RequestManager::ResponseType::Deleted:
        qDebug() << "Response: File Deleted";
        emit writeTextSignal("Deleted successfully!", Qt::darkBlue);
        break;

    case RequestManager::ResponseType::UnDeleted:
        qDebug() << "Response: Some file were not deleted";
        emit writeTextSignal("Some file were not deleted", Qt::red);
        break;

    case RequestManager::ResponseType::DownloadedFile:
        qDebug() << "Response: File Downloaded";
        break;

    case RequestManager::ResponseType::DownloadingFile:
        qDebug() << "Response: Downloading File...";
        break;

    case RequestManager::ResponseType::UploadedFile:
        qDebug() << "Response: File Uploaded";
        break;

    case RequestManager::ResponseType::UploadingFile:
        qDebug() << "Response: Uploading File...";
        break;

    default:
        qDebug() << "Response: Unknown Response Type";
        break;
    }
}

// Private method
void ClientModel::setFileSystem(QString pathDir)
{

    m_currentLocalDir = pathDir;
    m_settingsManager.setDefaultDirectory(m_currentLocalDir);
    m_localFileSystem->setRootPath(m_currentLocalDir);
    emit setLocalFileSystemSignal(m_localFileSystem);
}

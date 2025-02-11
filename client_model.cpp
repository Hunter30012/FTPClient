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
    //
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

void ClientModel::deleteHandle(QModelIndexList &listIndex, bool deleteInServer)
{
    if(deleteInServer) {

    } else {
        // delete in Local
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
}

void ClientModel::connectToServer(const bool &saveInformation, const QString &serverAddress, const QString &serverPort)
{
    QHostAddress address(serverAddress);
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

    m_networkManager.connectToServer(serverAddress, serverPort);
}

void ClientModel::disconnectButton()
{
    m_networkManager.disconnect();

}

// Private method
void ClientModel::setFileSystem(QString pathDir)
{
    m_currentLocalDir = pathDir;
    m_settingsManager.setDefaultDirectory(m_currentLocalDir);
    m_localFileSystem->setRootPath(m_currentLocalDir);
    emit setLocalFileSystemSignal(m_localFileSystem);
}

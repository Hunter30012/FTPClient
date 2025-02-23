#ifndef CLIENT_MODEL_H
#define CLIENT_MODEL_H

#include <QObject>
#include <QFileSystemModel>
#include <QCoreApplication>

#include "settings_manager.h"
#include "file_handler.h"
#include "network_manager.h"
#include "file_list_server_model.h"
#include "request_manager.h"

class ClientModel : public QObject
{
    Q_OBJECT

    friend class ClientController;
public:
    explicit ClientModel(QWidget *parent = nullptr);
    void init();

signals:
    void writeTextSignal(QString text, QColor color = {});
    void setLocalFileSystemSignal(QFileSystemModel* model);
    void initClientWindowSignal(const bool& checkboxChecked,
                          const QString& serverAddress,
                          const QString& serverPort);

    void connectedToServerSignal(FileListServerModel *model, const QString& curDir);

private slots:
    // Local Interaction
    void browseHomeLocal();
    void returnPreviousFolderLocal();
    void openFolder(const QString& dir, bool searchInServer);
    void openSelectedDir(const QModelIndex& index);

    void deleteHandle(QModelIndexList& listIndex, bool deleteInServer);

    // connect to Server
    void connectToServer(const bool& saveInformation, const QString& serverAddress, const QString& serverPort, const bool& isActive);
    void disconnectButton();

    // handle data from Server
    void parseJsonRecd(const QByteArray& data);

private:
    void setFileSystem(QString pathDir);

    NetworkManager m_networkManager;
    SettingsManager m_settingsManager;

    QString m_pathFileUploading;
    QString m_currentLocalDir;
    QString m_currentServerDir;

    QList<File> m_serverFileList;
    FileListServerModel* m_serverFileSystem;
    QFileSystemModel* m_localFileSystem;
};

#endif // CLIENT_MODEL_H

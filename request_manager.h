#ifndef REQUEST_MANAGER_H
#define REQUEST_MANAGER_H

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class RequestManager
{
public:
    enum class RequestType
    {
        ActiveConnect = 0,
        PassiveConnect,
        ChangeDir,
        Delete,
        DownloadFile,
        UploadedFile,
        UploadingFile,
    };

    enum class ResponseType
    {
        ActiveConnected = 0,
        PassiveConnected,
        ChangedDir,
        Deleted,
        UnDeleted,
        DownloadedFile,
        DownloadingFile,
        UploadedFile,
        UploadingFile,
    };

    static QJsonObject createServerRequest(RequestManager::RequestType action, const QMap<QString, QString>& requestVariables = {}, const QStringList& deleteFiles ={});
    static QJsonObject createDownloadRequest(const QString& localPath, const QStringList& serverFiles);
    static QJsonObject createUploadRequest(RequestManager::RequestType action,
                                           const QString& localPath,
                                           const QString& serverPath,
                                           const QString& fileName,
                                           bool isDir,
                                           quint64 writtenBytes = 0,
                                           quint64 sizeFile = 0,
                                           const QByteArray& data = QByteArray());
    static bool checkIfDataIsJson(const QByteArray& data);
private:
    RequestManager();
};

#endif // REQUEST_MANAGER_H

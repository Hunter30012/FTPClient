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
        UploadFile,
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
    static bool checkIfDataIsJson(const QByteArray& data);
private:
    RequestManager();
};

#endif // REQUEST_MANAGER_H

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
        PassiveConnectd,
        ChangedDir,
        Delete,
        DownloadedFile,
        UploadedFile,
    };

    static QJsonObject createServerRequest(RequestManager::RequestType action, const QMap<QString, QString>& requestVariables);
    static bool checkIfDataIsJson(const QByteArray& data);
private:
    RequestManager();
};

#endif // REQUEST_MANAGER_H

#include "request_manager.h"

RequestManager::RequestManager() {}

QJsonObject RequestManager::createServerRequest(RequestManager::RequestType action,
                                                const QMap<QString, QString>& requestVariables,
                                                const QStringList& deleteFiles)
{
    QJsonObject request;
    request["request_type"] = static_cast<int>(action);

    for (auto it = requestVariables.begin(); it != requestVariables.end(); ++it) {
        request[it.key()] = it.value();
    }
    if (!deleteFiles.isEmpty()) {
        QJsonArray fileArray;
        for (const QString& file : deleteFiles) {
            fileArray.append(file);
        }
        request["filesDelete"] = fileArray;
    }
    return request;
}

QJsonObject RequestManager::createDownloadRequest(const QString &localPath, const QStringList &serverFiles)
{
    QJsonObject request;
    request["request_type"] = static_cast<int>(RequestManager::RequestType::DownloadFile);
    request["localPath"] = localPath;

    QJsonArray fileArray;
    for (const QString &file : serverFiles) {
        fileArray.append(file);
    }
    request["filePathServer"] = fileArray;

    return request;
}


bool RequestManager::checkIfDataIsJson(const QByteArray& data)
{
    QJsonParseError jsonError;
    QJsonDocument::fromJson(data, &jsonError);
    return (jsonError.error == QJsonParseError::NoError) ? true : false;
}


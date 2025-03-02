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

QJsonObject RequestManager::createUploadRequest(RequestType action,
                                                const QString &localPath,
                                                const QString &serverPath,
                                                const QString &fileName,
                                                bool isDir,
                                                quint64 writtenBytes,
                                                quint64 sizeFile,
                                                const QByteArray &data)
{
    QJsonObject request;
    request["request_type"] = static_cast<int>(action);
    request["localPath"] = localPath;
    request["isDir"] = isDir;
    request["fileNameLocal"] = fileName;
    request["serverPath"] = serverPath;
    if(!isDir) {
        request["writtenBytes"] = QString::number(writtenBytes);
        request["sizeFile"] = QString::number(sizeFile);
        request["dataPacket"] = QString::fromLatin1(data.toBase64());
    }
    return request;
}


bool RequestManager::checkIfDataIsJson(const QByteArray& data)
{
    QJsonParseError jsonError;
    QJsonDocument::fromJson(data, &jsonError);
    return (jsonError.error == QJsonParseError::NoError) ? true : false;
}


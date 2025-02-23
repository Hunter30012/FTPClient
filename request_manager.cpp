#include "request_manager.h"

RequestManager::RequestManager() {}

QJsonObject RequestManager::createServerRequest(RequestManager::RequestType action,
                                                const QMap<QString, QString>& requestVariables)
{
    QJsonObject request;
    request["request_type"] = static_cast<int>(action);

    for (auto it = requestVariables.begin(); it != requestVariables.end(); ++it) {
        request[it.key()] = it.value();
    }

    return request;
}


bool RequestManager::checkIfDataIsJson(const QByteArray& data)
{
    QJsonParseError jsonError;
    QJsonDocument::fromJson(data, &jsonError);
    return (jsonError.error == QJsonParseError::NoError) ? true : false;
}


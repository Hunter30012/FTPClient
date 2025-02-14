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
        Browse,
        Remove,
        Rename,
        CreateFolder,
        UploadFile,
        UploadCompleted,
        DownloadFile,
        DownloadFolder,
        NextPendingDownload,
    };

    enum class ResponseType
    {
        UnknownResponse = -1,
        Unauthorized = 0,
        Connected = 1,
        Closing = 2,
        FolderCreated = 3,
        FolderAlreadyExists = 4,
        Browse = 5,
        Rename = 6,
        RenameError = 7,
        DeletedFiles = 8,
        FileAlreadyExists = 9,
        BeginFileUpload = 10,
        FileUploading = 11,
        UploadCompleted = 12,
        BeginFileDownload = 13,
        DownloadComplete = 14,
        DownloadFolder = 15,
        DownloadFileError = 16,
    };

    enum class FileOverwrite
    {
        NoneSelected,
        OverwriteExisting,
        CreateNewFileName,
        SkipFile,
    };

    static QJsonObject createServerRequest(RequestManager::RequestType action, const QMap<QString, QString>& requestVariables, const QStringList& deleteFiles = {});
    static bool checkIfDataIsJson(const QByteArray& data);
private:
    RequestManager();
};

#endif // REQUEST_MANAGER_H

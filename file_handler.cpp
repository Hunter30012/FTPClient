#include "file_handler.h"

QAbstractFileIconProvider::IconType FileHandler::getIconTypeFromString(const QString &type)
{
    if (type == "File Folder") {
        return QFileIconProvider::Folder;
    } else
        return QFileIconProvider::File;
}

QString FileHandler::getPreviousFolderPath(QString fullpath)
{
    if (fullpath.endsWith(":/") || fullpath.endsWith(":")) {
        // is root path
        return "";
    }
    else if(fullpath.endsWith("/.")) {
        // is current path  -> cut /.
        fullpath = fullpath.left(fullpath.lastIndexOf("/"));
    }
    QString parentPath = fullpath.left(fullpath.lastIndexOf("/"));
    if (parentPath.endsWith(":")) {
        parentPath += "/";
    }
    return parentPath;
}

bool FileHandler::checkFileExists(const QString &filePath, const QString &fileName)
{
    return QDir(filePath).exists(fileName);
}

QString FileHandler::changeFileName(const QString &fileName, const QString &filePath)
{
    int fileNumToAppend = 0;
    QString newFileName = fileName;
    QFileInfo fileInfo(fileName);
    QString baseName = fileInfo.completeBaseName(); // fileName without suffix
    QString suffix = fileInfo.suffix(); // suffix

    do {
        ++fileNumToAppend;
        if (!suffix.isEmpty()) {
            newFileName = baseName + "_" + QString::number(fileNumToAppend) + "." + suffix;
        } else {
            newFileName = baseName + "_" + QString::number(fileNumToAppend);
        }
    } while (checkFileExists(filePath, newFileName));

    return newFileName;
}

QList<File> FileHandler::getFileListFromJson(const QJsonArray &jsonArray)
{
    QList<File> files;

    for (int i = 1; i < jsonArray.count(); ++i) {
        QJsonObject json = jsonArray[i].toObject();
        files.append(
            File {
                json.value("fileName").toString(),
                json.value("filePath").toString(),
                json.value("fileSize").toString().toULongLong(),
                json.value("isDir").toBool(),
                json.value("lastModified").toString(),
                DataConverter::decodePixmapFromString(json.value("icon").toString()),
                json.value("fileType").toString(),
            }
        );
    }
    return files;
}

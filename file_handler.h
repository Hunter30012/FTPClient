#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <QApplication>
#include <QFileIconProvider>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include "file.h"
#include "data_converter.h"

class FileHandler
{

public:
    static QFileIconProvider::IconType getIconTypeFromString(const QString& type);
    static QString getPreviousFolderPath(QString fullpath);
    static bool checkFileExists(const QString& filePath, const QString& fileName);
    static QString changeFileName(const QString& fileName, const QString& filePath);
    static QList<File> getFileListFromJson(const QJsonArray& jsonArray);

private:
    FileHandler();

};

#endif // FILE_HANDLER_H

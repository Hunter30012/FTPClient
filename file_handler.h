#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <QString>

class FileHandler
{

public:
    // static QFileIconProvider::IconType getIconTypeFromString(const QString& type);
    static QString getPreviousFolderPath(QString fullpath);
    static bool checkFileExists(const QString& filePath, const QString& fileName);
    static QString changeFileName(const QString& fileName, const QString& filePath);
    // static QList<File> getFileListFromJson(const QJsonArray& jsonArray);

private:
    FileHandler();

};

#endif // FILE_HANDLER_H

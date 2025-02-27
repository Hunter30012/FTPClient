#ifndef FILE_H
#define FILE_H

#include <QApplication>

class File
{

public:
    File(QString fileName, QString filePath, quint64 fileSize = 0, bool isDir = false, QString lastModified = {}, QPixmap icon = {}, QString fileType = {})
        : m_fileName{ fileName }, m_filePath{ filePath }, m_fileSize{ fileSize }, m_isDir{ isDir }, m_lastModified{ lastModified }, m_icon{ icon }, m_fileType { fileType }
    {};

    File() {};

    QString m_fileType;
    QString m_fileName;
    QString m_filePath;
    QString m_lastModified;
    quint64 m_fileSize = 0;
    bool m_isDir = false;
    QPixmap m_icon;
};

#endif // FILE_H

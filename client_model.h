#ifndef CLIENT_MODEL_H
#define CLIENT_MODEL_H

#include <QObject>
#include <QFileSystemModel>
#include <QCoreApplication>
#include "settings_manager.h"
#include "file_handler.h"

class ClientModel : public QObject
{
    Q_OBJECT

    friend class ClientController;
public:
    explicit ClientModel(QWidget *parent = nullptr);
    void init();

signals:
    void writeTextSignal(QString text, QColor color = {});
    void setLocalFileSystemSignal(QFileSystemModel* model);
    void initClientWindowSignal(const bool& checkboxChecked,
                          const QString& serverAddress,
                          const QString& serverPort);

private slots:
    // Local Interaction
    void browseHomeLocal();
    void returnPreviousFolderLocal();
    void openFolder(const QString& dir, bool searchInServer);
    void openSelectedDir(const QModelIndex& index);

    void deleteHandle(QModelIndexList& listIndex, bool deleteInServer);


private:
    void setFileSystem(QString pathDir);

    SettingsManager m_settingsManager;
    QString m_currentLocalDir;
    // QString currentServerDir;

    QFileSystemModel* m_localFileSystem;
};

#endif // CLIENT_MODEL_H

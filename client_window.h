#ifndef CLIENT_WINDOW_H
#define CLIENT_WINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QFileSystemModel>
#include <QFileDialog>
#include <QMessageBox>
#include "file_list_server_model.h"

#include "./ui_mainwindow.h"

class ClientWindow : public QMainWindow
{
    Q_OBJECT

    friend class ClientController;

public:
    explicit ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

signals:
    void openFolderSignal(const QString& dir, bool openInServer);
    void deleteSignal(QModelIndexList& listIndex, bool deleteInServer);
    void connectToServerSignal(const bool& saveInformation, const QString& serverAddress, const QString& serverPort, const bool& isActive);

public slots:
    void writeTextToOutput(QString text, QColor color);
    void setLocalFileSystem(QFileSystemModel* model);
    void initClientWindow(const bool& isChecked,
                          const QString& serverAddress,
                          const QString& serverPort);
    void openInLocal();
    void localSearch();
    void deleteInLocal(); // need add more logic

    // clear
    void clearOutput();

    // connect to Server
    void connectToServer();
    void enableDisconnect();
    void disableDisconnect();

    // connected
    void connectedToServer(FileListServerModel *model, const QString& curDir);
private:

    QString m_currentLocalPath;

    Ui::MainWindow *ui;
};
#endif // CLIENT_WINDOW_H

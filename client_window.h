#ifndef CLIENT_WINDOW_H
#define CLIENT_WINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QFileSystemModel>
#include <QFileDialog>
#include <QMessageBox>

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

public slots:
    void writeTextToOutput(QString text, QColor color);
    void setLocalFileSystem(QFileSystemModel* model);
    void initClientWindow(const bool& isChecked,
                          const QString& serverAddress,
                          const QString& serverPort,
                          const QString& userName,
                          const QString& userPassword);
    void openInLocal();
    void localSearch();
    void deleteInLocal(); // need add more logic

    // clear
    void clearOutput();

private:

    QString m_currentLocalPath;

    Ui::MainWindow *ui;
};
#endif // CLIENT_WINDOW_H

#include "client_window.h"

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("My FTP Client");

    ui->serverDeleteButton->setDisabled(true);
    ui->downloadButton->setDisabled(true);
    ui->serverReturnButton->setDisabled(true);
    ui->disconnectButton->setDisabled(true);
    ui->uploadButton->setDisabled(true);
}

ClientWindow::~ClientWindow()
{
    delete ui;
}

void ClientWindow::writeTextToOutput(QString text, QColor color)
{
    QString addColor = (color.isValid()) ? "style='color:" + color.name() + ";'" : "";
    ui->textOutput->append("<span " + addColor + " > [" + QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss ") + "] - " + text + "</span>");
}

void ClientWindow::setLocalFileSystem(QFileSystemModel* model)
{
    ui->localFSTableView->setModel(model);

    m_currentLocalPath = model->rootPath();
    ui->localFSTableView->setRootIndex(model->index(m_currentLocalPath));
    ui->localFSTableView->setColumnWidth(0, 260);
    ui->localFSTableView->setColumnWidth(3, 120);

    ui->localSearchEdit->setText(m_currentLocalPath);
}

void ClientWindow::initClientWindow(const bool &isChecked, const QString &serverAddress, const QString &serverPort)
{
    ui->checkBox->setChecked(isChecked);
    ui->addressEdit->setText(serverAddress);
    ui->portEdit->setText(serverPort);
}

void ClientWindow::openInLocal()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", m_currentLocalPath,
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks | QFileDialog::ReadOnly);
    if(!dir.isEmpty()) {
        // signal to set selected folder in Model
        emit openFolderSignal(dir);
    }
}

void ClientWindow::localSearch()
{
    emit openFolderSignal(ui->localSearchEdit->text());
}

void ClientWindow::deleteInLocal()
{
    qDebug() << "Click delete";
    QModelIndexList selectedRows = ui->localFSTableView->selectionModel()->selectedRows();
    if(selectedRows.isEmpty()) {
        writeTextToOutput("No files're selected", Qt::red);
        return;
    }

    QMessageBox::StandardButton ret = QMessageBox::question(this, "FTP Client", "Do you want to delete?",
                                                              QMessageBox::Yes | QMessageBox::No);

    // Need to hanle when this file is transfering
    if(ret == QMessageBox::Yes) {
        emit deleteInLocalSignal(selectedRows);
    }
}

void ClientWindow::deleteInServer()
{
    QModelIndexList selectedRows = ui->serverTableView->selectionModel()->selectedRows();
    if(selectedRows.isEmpty()) {
        writeTextToOutput("No files in Server are selected", Qt::red);
        return;
    }

    QMessageBox::StandardButton ret = QMessageBox::question(this, "FTP Server", "Do you want to delete?",
                                                            QMessageBox::Yes | QMessageBox::No);

    if(ret == QMessageBox::Yes) {
        emit deleteInServerSignal(selectedRows);
    }
}

void ClientWindow::downloadFiles()
{
    QModelIndexList selectedRows = ui->serverTableView->selectionModel()->selectedRows();
    if(selectedRows.isEmpty()) {
        writeTextToOutput("Please select files to download!", Qt::red);
        return;
    }
    emit downloadFilesSignal(selectedRows);
}

void ClientWindow::uploadFiles()
{
    QModelIndexList selectedRows = ui->localFSTableView->selectionModel()->selectedRows();
    if(selectedRows.isEmpty()) {
        writeTextToOutput("Please select files to upload!", Qt::red);
        return;
    }
    emit uploadFilesSignal(selectedRows);
}

void ClientWindow::clearOutput()
{
    ui->textOutput->clear();
}

void ClientWindow::connectToServer()
{
    bool isActive;
    if(ui->activeButton->isChecked()) {
        isActive = true;
        qDebug() << "Selected Active mode";
    } else if(ui->passiveButton->isChecked()) {
        isActive = false;
        qDebug() << "Selected Passive mode";
    } else {
        writeTextToOutput("Please select the mode!", Qt::red);
        return;
    }
    emit connectToServerSignal(ui->checkBox->isChecked(), ui->addressEdit->text(), ui->portEdit->text(), isActive);
}

void ClientWindow::enableDisconnect()
{
    ui->disconnectButton->setDisabled(false);
    ui->connectButton->setDisabled(true);
    ui->serverDeleteButton->setDisabled(false);
    ui->downloadButton->setDisabled(false);
    ui->serverReturnButton->setDisabled(false);
    ui->uploadButton->setDisabled(false);
}

void ClientWindow::disableDisconnect()
{
    ui->serverTableView->setModel(nullptr);
    ui->disconnectButton->setDisabled(true);
    ui->connectButton->setDisabled(false);
    ui->serverDeleteButton->setDisabled(true);
    ui->downloadButton->setDisabled(true);
    ui->serverReturnButton->setDisabled(true);
    ui->uploadButton->setDisabled(true);
}

void ClientWindow::connectedToServer(FileListServerModel *model, const QString &curDir)
{
    // TBD
    ui->serverTableView->setModel(model);
    ui->serverTableView->setColumnWidth(0, 5);
    ui->serverTableView->setColumnWidth(1, 200);
    ui->serverTableView->setColumnWidth(2, 90);
    ui->serverTableView->setColumnWidth(3, 90);
    ui->serverTableView->setColumnWidth(4, 150);
    ui->serverTableView->setColumnWidth(5, 200);
}


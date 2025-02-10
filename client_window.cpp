#include "client_window.h"

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("My FTP Client");


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

void ClientWindow::initClientWindow(const bool &isChecked, const QString &serverAddress, const QString &serverPort, const QString &userName, const QString &userPassword)
{
    ui->checkBox->setChecked(isChecked);
    ui->addressEdit->setText(serverAddress);
    ui->portEdit->setText(serverPort);
    ui->usernameEdit->setText(userName);
    ui->passwordEdit->setText(userPassword);
}

void ClientWindow::openInLocal()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", m_currentLocalPath,
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks | QFileDialog::ReadOnly);
    if(!dir.isEmpty()) {
        // signal to set selected folder in Model
        emit openFolderSignal(dir, false);
    }
}

void ClientWindow::localSearch()
{
    emit openFolderSignal(ui->localSearchEdit->text(), false);
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
        emit deleteSignal(selectedRows, false);
    }
}

void ClientWindow::clearOutput()
{
    ui->textOutput->clear();
}


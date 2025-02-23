#include "client_controller.h"

ClientController::ClientController(int argc, char *argv[], QWidget *parent)
    : QObject{parent}, m_app{argc, argv}, m_window{parent}, m_model{parent}
{
    QList<bool> connectionResults;

    connectWindowSignalSlots(connectionResults);
    connectModelSignalSlots(connectionResults);

    Q_ASSERT(!connectionResults.contains(false));
}

int ClientController::init()
{
    m_model.init();
    m_window.show();
    return m_app.exec();
}

void ClientController::connectWindowSignalSlots(QList<bool> &connectionResults)
{
    /*
     * Local File System Function
     */
    connectionResults.append(connect(m_window.ui->localHomeButton, &QPushButton::clicked, &m_model, &ClientModel::browseHomeLocal));
    connectionResults.append(connect(m_window.ui->localReturnButton, &QPushButton::clicked, &m_model, &ClientModel::returnPreviousFolderLocal));

    connectionResults.append(connect(m_window.ui->localOpenButton, &QPushButton::clicked, &m_window, &ClientWindow::openInLocal));
    connectionResults.append(connect(&m_window, &ClientWindow::openFolderSignal, &m_model, &ClientModel::openFolder));
    connectionResults.append(connect(m_window.ui->localSearchButton, &QPushButton::clicked, &m_window, &ClientWindow::localSearch));
    connectionResults.append(connect(m_window.ui->localDeleteButton, &QPushButton::clicked, &m_window, &ClientWindow::deleteInLocal));
    connectionResults.append(connect(&m_window, &ClientWindow::deleteSignal, &m_model, &ClientModel::deleteHandle));
    // doubleClick
    connectionResults.append(connect(m_window.ui->localFSTableView, &QTableView::doubleClicked, &m_model, &ClientModel::openSelectedDir));

    /*
     * Connect to Server
     */
    connectionResults.append(connect(m_window.ui->connectButton, &QPushButton::clicked, &m_window, &ClientWindow::connectToServer));
    connectionResults.append(connect(&m_window, &ClientWindow::connectToServerSignal, &m_model, &ClientModel::connectToServer));
    connectionResults.append(connect(m_window.ui->disconnectButton, &QPushButton::clicked, &m_model, &ClientModel::disconnectButton));

    /*
     * Other functions
     */


    // clear Output log
    connectionResults.append(connect(m_window.ui->clearButton, &QPushButton::clicked, &m_window, &ClientWindow::clearOutput));
}

void ClientController::connectModelSignalSlots(QList<bool> &connectionResults)
{
    // connect init ClientView: setup saved information
    connectionResults.append(connect(&m_model, &ClientModel::initClientWindowSignal, &m_window, &ClientWindow::initClientWindow));

    // Connect Data (model) to UI (window)
    connectionResults.append(connect(&m_model, &ClientModel::writeTextSignal, &m_window, &ClientWindow::writeTextToOutput));
    connectionResults.append(connect(&m_model, &ClientModel::setLocalFileSystemSignal, &m_window, &ClientWindow::setLocalFileSystem));

    // Network
    connectionResults.append(connect(&m_model.m_networkManager, &NetworkManager::writeTextSignal, &m_window, &ClientWindow::writeTextToOutput));
    connectionResults.append(connect(&m_model.m_networkManager, &NetworkManager::stopClientSignal, &m_model.m_networkManager.m_commandThread, &CommandThread::stopConnection));
    connectionResults.append(connect(&m_model.m_networkManager.m_commandThread, &CommandThread::enableDisconnectSignal, &m_window, &ClientWindow::enableDisconnect));
    connectionResults.append(connect(&m_model.m_networkManager.m_commandThread, &CommandThread::disableDisconnectSignal, &m_window, &ClientWindow::disableDisconnect));

    connectionResults.append(connect(&m_model.m_networkManager.m_commandThread, &CommandThread::writeTextSignal, &m_window, &ClientWindow::writeTextToOutput));
    // connectionResults.append(connect(&m_model.m_networkManager, &NetworkManager::parseJsonRecdSignal, &m_model, &ClientModel::parseJsonRecd));
    connectionResults.append(connect(&m_model.m_networkManager.m_activeDataThread, &ActiveDataThread::dataReceivedSignal, &m_model, &ClientModel::parseJsonRecd));
    // connected
    connectionResults.append(connect(&m_model, &ClientModel::connectedToServerSignal, &m_window, &ClientWindow::connectedToServer));

}

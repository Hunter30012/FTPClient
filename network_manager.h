#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <QObject>
#include <QAbstractSocket>
#include <QSaveFile>
#include <QJsonObject>
#include <QFileInfo>
#include <QDir>
#include "file_handler.h"
#include "active_data_thread.h"
#include "command_thread.h"
#include "passive_data_thread.h"

class NetworkManager : public QObject
{
    Q_OBJECT
    friend class ClientController;
public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();
    static bool isValidPort(const QString& port);

signals:
    void writeTextSignal(QString text, QColor color = {});

    void stopClientSignal();

public slots:
    void connectToServer(const QHostAddress &serverAddress, int serverPort, const bool& isActive);
    void stopConnectingToServer();
    void onDownloadedFile(const QJsonObject& obj);
    void onDownloadingFile(const QJsonObject& obj);
private:
    const qint64 packetSize = 10000;
    bool m_isDownloading;
    bool m_isUploading;

    QSaveFile m_saveFile;
    QByteArray dataToSend;
    CommandThread m_commandThread;
    PassiveDataThread m_passiveDataThread;
    ActiveDataThread m_activeDataThread;
};

#endif // NETWORK_MANAGER_H

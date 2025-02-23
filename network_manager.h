#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <QObject>
#include <QAbstractSocket>
#include "active_data_thread.h"
#include "command_thread.h"
#include "passive_data_thread.h"

class NetworkManager : public QObject
{
    Q_OBJECT
    friend class ClientController;
public:
    explicit NetworkManager(QObject *parent = nullptr);
    static bool isValidPort(const QString& port);

signals:
    void writeTextSignal(QString text, QColor color = {});

    void stopClientSignal();

public slots:
    void connectToServer(const QHostAddress &serverAddress, int serverPort, const bool& isActive);
    void stopConnectingToServer();

    void witeData(const QByteArray& data);
    // QByteArray readAll();

    // QByteArray parseByteData();
    // void parseByteDownload(const QByteArray& data);
    //
    // void uploadFileData();

private:

    bool m_isDownloading;
    bool m_isUploading;

    QByteArray dataToSend;

    CommandThread m_commandThread;

    PassiveDataThread m_passiveDataThread;
    ActiveDataThread m_activeDataThread;
};

#endif // NETWORK_MANAGER_H

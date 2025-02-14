#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <QObject>
#include <QSettings>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#define IS_SAVED "isSaved"
#define DEFAULT_LOCAL_DIR "defaultLocalDirectory"
#define ADDRESS "serverAddress"
#define PORT "serverPort"

struct savedConnection
{
    savedConnection(bool _checkboxChecked,QString _serverAddress, QString _serverPort) :
        checkboxChecked(_checkboxChecked), serverAddress(_serverAddress), serverPort(_serverPort)
    {};

    bool checkboxChecked;
    QString serverAddress;
    QString serverPort;
};

class SettingsManager : public QObject
{
    Q_OBJECT
public:
    explicit SettingsManager(QObject *parent = nullptr);

    QString getDefaultDirectory();
    savedConnection getSavedConnection();

    void setSavedConnection(const bool& checkboxChecked ,const QString& serverAddress, const QString& serverPort);
    void setDefaultDirectory(QString directory);

signals:

private:
    QSettings* m_settings;
    const QString m_settingsDirectory = "./settings/client_settings.json";
    QSettings::Format m_jsonFormat = QSettings::registerFormat("json", SettingsManager::readJsonFile, SettingsManager::writeJsonFile);

    static bool readJsonFile(QIODevice& device, QSettings::SettingsMap& map);
    static bool writeJsonFile(QIODevice& device, const QSettings::SettingsMap& map);
};

#endif // SETTINGS_MANAGER_H

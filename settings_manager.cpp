#include "settings_manager.h"

SettingsManager::SettingsManager(QObject *parent)
    : QObject{parent}
{
    m_settings = new QSettings(m_settingsDirectory, m_jsonFormat);
}

// get
QString SettingsManager::getDefaultDirectory()
{
    return m_settings->value(DEFAULT_LOCAL_DIR).toString();
}

savedConnection SettingsManager::getSavedConnection()
{
    return savedConnection(m_settings->value(IS_SAVED).toBool(),
                           m_settings->value(ADDRESS).toString(),
                           m_settings->value(PORT).toString());
}

// set
void SettingsManager::setSavedConnection(const bool &checkboxChecked, const QString &serverAddress, const QString &serverPort)
{
    m_settings->setValue(IS_SAVED, checkboxChecked);
    m_settings->setValue(ADDRESS, serverAddress);
    m_settings->setValue(PORT, serverPort);
}

void SettingsManager::setDefaultDirectory(QString directory)
{
    m_settings->setValue(DEFAULT_LOCAL_DIR, directory);
}


bool SettingsManager::readJsonFile(QIODevice &device, QSettings::SettingsMap &map)
{
    QByteArray data = device.readAll();
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &error);

    if (jsonDoc.isNull()) {
        qWarning() << "JSON Parse Error:" << error.errorString();
    }

    map = jsonDoc.object().toVariantMap();
    return true;
}

bool SettingsManager::writeJsonFile(QIODevice &device, const QSettings::SettingsMap &map)
{
    QByteArray jsonData = QJsonDocument(QJsonObject::fromVariantMap(map)).toJson();
    if (device.write(jsonData) < 0) {
        qWarning() << "Failed to write JSON data!";
        return false;
    }
    return true;
}



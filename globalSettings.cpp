#include "GlobalSettings.h"

bool GlobalSettings::m_enableAutomaticDownload = true;
bool GlobalSettings::m_enableInternet = true;
bool GlobalSettings::m_fakeInternetCut = true;

GlobalSettings& GlobalSettings::instance() {
    static GlobalSettings instance;
    return instance;
}

GlobalSettings::GlobalSettings() {
    loadSettings();
}

void GlobalSettings::loadSettings() {
    QSettings settings("putPoznan", "GIOS_API");
    m_enableAutomaticDownload = settings.value("enableAutomaticDownload", true).toBool();
    m_enableInternet = settings.value("enableInternet", true).toBool();
    m_fakeInternetCut = settings.value("fakeInternetCut", true).toBool();
}

void GlobalSettings::saveSettings() {
    QSettings settings("putPoznan", "GIOS_API");
    settings.setValue("enableAutomaticDownload", m_enableAutomaticDownload);
    settings.setValue("enableInternet", m_enableInternet);
    settings.setValue("fakeInternetCut", m_fakeInternetCut);
}

void GlobalSettings::enableAutomaticDownload() {
    m_enableAutomaticDownload = true;
    saveSettings();
}

void GlobalSettings::disableAutomaticDownload() {
    m_enableAutomaticDownload = false;
    saveSettings();
}

void GlobalSettings::enableInternet() {
    m_enableInternet = true;
    saveSettings();
}

void GlobalSettings::disableInternet() {
    m_enableInternet = false;
    saveSettings();
}

void GlobalSettings::enableFakeInternet() {
    m_fakeInternetCut = true;
    saveSettings();
}

void GlobalSettings::disableFakeInternet() {
    m_fakeInternetCut = false;
    saveSettings();
}
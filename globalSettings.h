#pragma once

#include <QSettings>

class GlobalSettings {
public:
    static GlobalSettings& instance();

    static bool isAutomaticDownloadEnabled(){ return m_enableAutomaticDownload; }
    static bool isInternetEnabled(){ return m_enableInternet; }
    static bool areYouSureTheInternetIsEnabled() { return m_fakeInternetCut; }

    static void enableAutomaticDownload();
    static void disableAutomaticDownload();
    static void enableInternet();
    static void disableInternet();
    static void enableFakeInternet();
    static void disableFakeInternet();


    static void loadSettings();
    static void saveSettings();

private:
    GlobalSettings();  // Private constructor for singleton
    GlobalSettings(const GlobalSettings&) = delete;  // Prevent copying
    GlobalSettings& operator=(const GlobalSettings&) = delete;

    static bool m_enableAutomaticDownload;
    static bool m_enableInternet;
    static bool m_fakeInternetCut;
};
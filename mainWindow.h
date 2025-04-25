#pragma once
#include <QMainWindow>
#include <QWidget>
#include <QPalette>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <QLabel>
#include <QHBoxLayout>
#include <QComboBox>
#include <QSettings>
#include <QFile>
#include <QDialog>
#include <QCheckBox>
#include <QObject>
#include <QThread>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QProcess>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <thread>
#include <mutex>
#include <algorithm>
#include <atomic>
#include "ui_mainWindow.h"
#include "APIcontroller.h"
#include "Station.h"
#include "GlobalSettings.h"

using namespace std;

class settingsWindow;

/**
 * @brief Główne okno aplikacji odpowiadające za interakcję z użytkownikiem.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor klasy MainWindow.
     * @param parent Wskaźnik do obiektu nadrzędnego (opcjonalny).
     */
    MainWindow(QWidget* parent = nullptr);

    /**
     * @brief Destruktor klasy MainWindow.
     */
    ~MainWindow();

private:
    Ui::MainWindow ui; ///< Interfejs użytkownika.
    GIOS_APImanagement* apiManager; ///< Menedżer do komunikacji z API GIOŚ.
    vector<Station*> loadedStations; ///< Załadowane stacje pomiarowe.
    vector<Station*> displayedStations; ///< Aktualnie wyświetlane stacje.
    settingsWindow* settingsDialog; ///< Okno dialogowe ustawień.
    QLabel* apiConnectionWarning; ///< Etykieta ostrzegająca o braku połączenia z API.
    QTimer* connectionCheckTimer; ///< Timer sprawdzający status połączenia.
    QChartView* chartView; ///< Obiekt do wyświetlania wykresu wartości danego sensora.

    /**
     * @brief Skraca tekst do maksymalnej długości.
     * @param text Tekst wejściowy.
     * @param maxLength Maksymalna długość.
     * @return Skrócony tekst jako QString.
     */
    QString truncate(const std::string& text, int maxLength);

    /**
     * @brief Sprawdza status połączenia z API i aktualizuje interfejs.
     */
    void checkConnectionStatus();

    /**
     * @brief Otwiera folder z lokalnymi danymi.
     */
    void openFolder();

    /**
     * @brief Rozpoczyna pobieranie danych ze stacji.
     */
    void downloadRequest();

    /**
     * @brief Usuwa dane lokalne stacji.
     */
    void deleteRequest();

    /**
     * @brief Otwiera okno ustawień.
     */
    void settingsPopup();

    /**
     * @brief Ładuje dane stacji z pliku lub API.
     */
    void loadStations();

    /**
     * @brief Czyści aktualnie załadowane dane stacji.
     */
    void clearStations();

    /**
     * @brief Wyszukuje stacje na podstawie kryteriów użytkownika.
     */
    void searchStations();

    /**
     * @brief Aktualizuje ostrzeżenie o stanie połączenia.
     */
    void updateConnectionWarning();

    /**
     * @brief Obsługuje podwójne kliknięcie na liście stacji.
     * @param item Element kliknięty na liście.
     */
    void stationDoubleClicked(QListWidgetItem* item);

    /**
     * @brief Obsługuje podwójne kliknięcie na liście sensorów.
     * @param item Element kliknięty na liście.
     */
    void sensorDoubleClicked(QListWidgetItem* item);
};

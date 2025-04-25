#pragma once

#include <QObject>
#include <QThread>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "jsonController.h"
#include "globalSettings.h"

#pragma comment(lib, "wininet.lib")

using namespace std;
using json = nlohmann::json;

/**
 * @brief Klasa bazowa zawierająca podstawowe operacje sieciowe z użyciem cURL.
 */
class CurlServiceFundaments {
public:
    /**
     * @brief Inicjalizuje bibliotekę cURL i przygotowuje uchwyt.
     * @param curl Referencja do wskaźnika cURL, który będzie zainicjalizowany.
     */
    void initializeCurl(CURL*& curl);

    /**
     * @brief Funkcja używana do odbioru danych z odpowiedzi HTTP.
     * @param contents Dane wejściowe.
     * @param size Rozmiar pojedynczego elementu.
     * @param nmemb Liczba elementów.
     * @param userp Wskaźnik na bufor danych.
     * @return Ilość przetworzonych danych.
     */
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

protected:
    /**
     * @brief Sprawdza czy dostępne jest połączenie internetowe.
     * @return true jeśli połączenie działa, false w przeciwnym razie.
     */
    bool checkInternetStatus();

    /**
     * @brief Wysyła zapytanie HTTP GET pod wskazany adres i zwraca odpowiedź jako JSON.
     * @param url Adres URL zapytania.
     * @return Odpowiedź w formacie JSON.
     */
    json makeRequest(const std::string& url);
};

/**
 * @brief Klasa do zarządzania zapytaniami API GIOŚ.
 */
class GIOS_APImanagement : public QObject, public CurlServiceFundaments {
    Q_OBJECT
private:
    jsonController* jsonController_; ///< Wskaźnik do obiektu obsługującego dane JSON.

public:
    /**
     * @brief Konstruktor klasy GIOS_APImanagement.
     * @param parent Obiekt rodzica dla Qt.
     */
    GIOS_APImanagement(QObject* parent = nullptr);

    /**
     * @brief Ustawia status połączenia z API.
     */
    void setConnectionStatus();

    /**
     * @brief Pobiera listę dostępnych stacji pomiarowych.
     * @return Lista stacji w formacie JSON.
     */
    json getStationList();

    /**
     * @brief Pobiera listę sensorów dla danej stacji.
     * @param stationId ID stacji.
     * @return Lista sensorów w formacie JSON.
     */
    json getSensorList(int stationId);

    /**
     * @brief Pobiera dane z konkretnego sensora.
     * @param stationId ID stacji.
     * @param sensorId ID sensora.
     * @param key Klucz identyfikujący typ danych.
     * @return Dane sensora w formacie JSON.
     */
    json getSensorData(int stationId, int sensorId, string key);

    /**
     * @brief Pobiera indeks jakości powietrza dla danej stacji.
     * @param stationId ID stacji.
     * @return Indeks jakości powietrza w formacie JSON.
     */
    json getAirQualityIndex(int stationId);

    /**
     * @brief Pobiera kompletne dane z API dla wszystkich stacji.
     */
    void downloadAllData();

    /**
     * @brief Usuwa wszystkie pobrane dane.
     */
    void removeAllData();

    /**
     * @brief Destruktor klasy.
     */
    ~GIOS_APImanagement();

signals:
    /**
     * @brief Sygnał emitowany po zakończeniu operacji.
     */
    void finished();
};

#pragma once
#include <nlohmann\json.hpp>
#include <string>
#include <windows.h>
#include <direct.h>
#include <fstream>
#include <iostream>
#include <curl\curl.h>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include <functional>
#include "GlobalSettings.h"

using json = nlohmann::json;

/**
 * @brief Klasa zarządzająca operacjami na danych JSON, zapisem i odczytem lokalnym.
 */
class jsonController {
public:
    /**
     * @brief Sprawdza, czy folder o podanej ścieżce istnieje.
     * @param folderPath Ścieżka do folderu.
     * @return true jeśli folder istnieje, false w przeciwnym razie.
     */
    bool folderExists(std::string folderPath);

    /**
     * @brief Zapisuje listę stacji do pliku lokalnego.
     * @param stationList Dane stacji w formacie JSON.
     */
    void saveStationList(const json& stationList);

    /**
     * @brief Wczytuje listę stacji z lokalnego pliku.
     * @return Lista stacji w formacie JSON.
     */
    json loadStationList();

    /**
     * @brief Zapisuje listę sensorów do pliku lokalnego.
     * @param sensors Dane sensorów w formacie JSON.
     */
    void saveSensorList(const json& sensors);

    /**
     * @brief Wczytuje listę sensorów dla podanej stacji.
     * @param stationId Identyfikator stacji.
     * @return Lista sensorów w formacie JSON.
     */
    json loadSensorList(const unsigned short& stationId);

    /**
     * @brief Zapisuje dane sensora do pliku lokalnego.
     * @param sensorData Dane sensora.
     * @param stationId Identyfikator stacji.
     */
    void saveSensorData(const json& sensorData, const unsigned short& stationId);

    /**
     * @brief Wczytuje dane sensora z lokalnego pliku.
     * @param stationId Identyfikator stacji.
     * @param key Klucz identyfikujący typ danych.
     * @return Dane sensora w formacie JSON.
     */
    json loadSensorData(const unsigned short& stationId, std::string key);

    /**
     * @brief Wczytuje dane indeksu jakości powietrza z lokalnego pliku.
     * @param stationId Identyfikator stacji.
     * @return Indeks jakości powietrza w formacie JSON.
     */
    json loadAirIndex(const unsigned short& stationId);

    /**
     * @brief Zapisuje wszystkie dane (stacje, sensory, dane pomiarowe).
     */
    void saveAllData(std::atomic<bool>* stopFlag = nullptr);

    /**
     * @brief Usuwa wszystkie lokalnie zapisane dane.
     */
    void deleteAllData();

    friend class GIOS_APImanagement; ///< Klasa GIOS_APImanagement ma dostęp do prywatnych składowych.
    friend class curlServiceFundaments; ///< Klasa curlServiceFundaments ma dostęp do prywatnych składowych.
};

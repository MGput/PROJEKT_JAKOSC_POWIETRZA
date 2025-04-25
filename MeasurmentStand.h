#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <locale.h>
#include <iostream>
#include "APIcontroller.h"
#include "MeasurementData.h"
#include "GlobalSettings.h"

using namespace std;
using json = nlohmann::json;

/**
 * @brief Klasa przechowująca podstawowe parametry zanieczyszczeń.
 */
class Parameters {
protected:
    string paramName_; ///< Nazwa parametru.
    string paramFormula_; ///< Wzór chemiczny parametru.
    string paramCode_; ///< Kod parametru.
    unsigned short idParam_; ///< Identyfikator parametru.
    string qualityIndex_; ///< Indeks jakości powietrza.

public:
    /**
     * @brief Konstruktor klasy Parameters.
     * @param paramName Nazwa parametru.
     * @param paramFormula Wzór chemiczny.
     * @param paramCode Kod parametru.
     * @param idParam Identyfikator parametru.
     */
    Parameters(string paramName, string paramFormula, string paramCode, unsigned short idParam);
};

/**
 * @brief Klasa reprezentująca stanowisko pomiarowe wraz z danymi pomiarowymi.
 */
class MeasurmentStand : private Parameters {
private:
    unsigned short id_; ///< Identyfikator stanowiska.
    unsigned short stationId_; ///< Identyfikator stacji, do której należy stanowisko.
    Data* standData_; ///< Dane pomiarowe związane z tym stanowiskiem.

public:
    /**
     * @brief Konstruktor stanowiska pomiarowego.
     * @param id ID stanowiska.
     * @param stationId ID stacji.
     * @param paramName Nazwa parametru.
     * @param paramFormula Wzór chemiczny parametru.
     * @param paramCode Kod parametru.
     * @param idParam ID parametru.
     */
    MeasurmentStand(unsigned short id, unsigned short stationId, string paramName, string paramFormula, string paramCode, unsigned short idParam);

    /**
     * @brief Konstruktor na podstawie danych JSON.
     * @param StandEntry Dane stanowiska w formacie JSON.
     */
    MeasurmentStand(const json& StandEntry);

    /**
     * @brief Ładuje dane pomiarowe z API.
     */
    void loadData();

    /**
     * @brief Konstruktor kopiujący.
     * @param other Wskaźnik do innego obiektu MeasurmentStand.
     */
    MeasurmentStand(const MeasurmentStand* other);

    /**
     * @brief Zwraca identyfikator stanowiska.
     * @return ID stanowiska.
     */
    unsigned short getId() const;

    /**
     * @brief Zwraca identyfikator stacji.
     * @return ID stacji.
     */
    unsigned short getStationId() const;

    /**
     * @brief Zwraca nazwę parametru.
     * @return Nazwa parametru.
     */
    string getParamName() const;

    /**
     * @brief Zwraca wzór chemiczny parametru.
     * @return Wzór chemiczny.
     */
    string getParamFormula() const;

    /**
     * @brief Zwraca kod parametru.
     * @return Kod parametru.
     */
    string getParamCode() const;

    /**
     * @brief Zwraca identyfikator parametru.
     * @return ID parametru.
     */
    unsigned short getIdParam() const;

    /**
     * @brief Zwraca wskaźnik do danych pomiarowych stanowiska.
     * @return Wskaźnik do obiektu Data.
     */
    Data* getStandData() const;

    /**
     * @brief Zwraca indeks jakości powietrza.
     * @return Indeks jakości jako string.
     */
    string getQualityIndex() const;

    /**
     * @brief Destruktor klasy MeasurmentStand.
     */
    ~MeasurmentStand();
};

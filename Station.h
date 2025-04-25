#pragma once
#include "MeasurmentStand.h"
#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <algorithm>
#include "GlobalSettings.h"

using namespace std;
using json = nlohmann::json;

/**
 * @brief Klasa reprezentująca gminę.
 */
class Commune {
protected:
    string communeName_; ///< Nazwa gminy.
    string districtName_; ///< Nazwa powiatu.
    string provinceName_; ///< Nazwa województwa.

public:
    /**
     * @brief Konstruktor klasy Commune.
     * @param communeName Nazwa gminy.
     * @param districtName Nazwa powiatu.
     * @param provinceName Nazwa województwa.
     */
    Commune(string communeName, string districtName, string provinceName);
};

/**
 * @brief Klasa reprezentująca miasto, dziedziczy po Commune.
 */
class City : public Commune {
protected:
    unsigned short id_; ///< ID miasta.
    string name_; ///< Nazwa miasta.

public:
    /**
     * @brief Konstruktor klasy City.
     * @param id ID miasta.
     * @param name Nazwa miasta.
     * @param communeName Nazwa gminy.
     * @param districtName Nazwa powiatu.
     * @param provinceName Nazwa województwa.
     */
    City(unsigned short id, string name, string communeName, string districtName, string provinceName);
};

/**
 * @brief Klasa reprezentująca lokalizację, dziedziczy po City.
 */
class Location : public City {
protected:
    string gegrLat_; ///< Szerokość geograficzna.
    string gegrLon_; ///< Długość geograficzna.
    string addressStreet_; ///< Adres ulicy.

public:
    /**
     * @brief Konstruktor klasy Location.
     * @param gegrLat Szerokość geograficzna.
     * @param gegrLon Długość geograficzna.
     * @param id ID miasta.
     * @param name Nazwa miasta.
     * @param communeName Nazwa gminy.
     * @param districtName Nazwa powiatu.
     * @param provinceName Nazwa województwa.
     * @param addressStreet Adres ulicy.
     */
    Location(string gegrLat, string gegrLon, unsigned short id, string name, string communeName, string districtName, string provinceName, string addressStreet);
};

/**
 * @brief Klasa reprezentująca stację pomiarową wraz z przypisanymi stanowiskami pomiarowymi.
 */
class Station : private Location {
private:
    unsigned short id_; ///< ID stacji.
    string stationName_; ///< Nazwa stacji.
    vector<MeasurmentStand*> measurmentStands_; ///< Lista stanowisk pomiarowych.

public:
    /**
     * @brief Konstruktor klasy Station.
     * @param id ID stacji.
     * @param stationName Nazwa stacji.
     * @param gegrLat Szerokość geograficzna.
     * @param gegrLon Długość geograficzna.
     * @param CityId ID miasta.
     * @param nameCity Nazwa miasta.
     * @param communeName Nazwa gminy.
     * @param districtName Nazwa powiatu.
     * @param provinceName Nazwa województwa.
     * @param addressStreet Adres ulicy.
     */
    Station(unsigned short id, string stationName, string gegrLat, string gegrLon, unsigned short CityId,
        string nameCity, string communeName, string districtName, string provinceName, string addressStreet);

    /**
     * @brief Konstruktor na podstawie danych JSON.
     * @param StationEntry Dane stacji w formacie JSON.
     */
    Station(const json& StationEntry);

    /**
     * @brief Ładuje stanowiska pomiarowe przypisane do stacji.
     */
    void loadStands();

    /**
     * @brief Zwraca identyfikator stacji.
     * @return ID stacji.
     */
    unsigned short getStationId() const;

    /**
     * @brief Zwraca nazwę stacji.
     * @return Nazwa stacji jako string.
     */
    string getStationName() const;

    /**
     * @brief Przypisuje współrzędne do przekazanej tablicy.
     * @param Coordinates Tablica dwuelementowa na współrzędne (lat, lon).
     */
    void assignCoordinates(string(&Coordinates)[2]) const;

    /**
     * @brief Zwraca ID miasta.
     * @return ID miasta jako liczba całkowita.
     */
    unsigned short getCityId() const;

    /**
     * @brief Zwraca nazwę miasta.
     * @return Nazwa miasta.
     */
    string getCityName() const;

    /**
     * @brief Zwraca nazwę gminy.
     * @return Nazwa gminy.
     */
    string getCommuneName() const;

    /**
     * @brief Zwraca nazwę powiatu.
     * @return Nazwa powiatu.
     */
    string getDistrictName() const;

    /**
     * @brief Zwraca nazwę województwa.
     * @return Nazwa województwa.
     */
    string getProvinceName() const;

    /**
     * @brief Zwraca adres ulicy stacji.
     * @return Adres jako string.
     */
    string getAddressStreet() const;

    /**
     * @brief Dodaje stanowisko pomiarowe do stacji.
     * @param measurmentStand Wskaźnik do obiektu MeasurmentStand.
     */
    void addMeasurmentStand(MeasurmentStand* measurmentStand);

    /**
     * @brief Sprawdza, czy stacja zawiera sensor danego typu.
     * @param parCode Kod parametru sensora.
     * @return true jeśli zawiera, false w przeciwnym razie.
     */
    bool containsSensor(string parCode);

    /**
     * @brief Sprawdza czy dane o stacji zawierają zadany tekst.
     * @param searchText Tekst do wyszukania.
     * @return true jeśli znaleziono, false w przeciwnym razie.
     */
    bool checkForInfo(string searchText) const;

    /**
     * @brief Zwraca wskaźnik do stanowiska o danym typie zanieczyszczenia.
     * @param pollutionType Typ zanieczyszczenia.
     * @return Wskaźnik do MeasurmentStand.
     */
    MeasurmentStand* filteredMeasureStand(const string& pollutionType);

    /**
     * @brief Zwraca wszystkie stanowiska pomiarowe tej stacji.
     * @return Wektor wskaźników do MeasurmentStand.
     */
    const vector<MeasurmentStand*>& getMeasurmentStands() const;

    /**
     * @brief Destruktor klasy Station.
     */
    ~Station();
};

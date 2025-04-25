#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "GlobalSettings.h"

using namespace std;
using json = nlohmann::json;

/**
 * @brief Klasa reprezentująca pojedynczą wartość pomiaru wraz z datą.
 */
class Values {
protected:
    string date_; ///< Data pomiaru.
    double value_; ///< Wartość pomiaru.

public:
    /**
     * @brief Konstruktor klasy Values.
     * @param date Data pomiaru.
     * @param value Zmierzona wartość.
     */
    Values(string date, double value);

    /**
     * @brief Zwraca datę pomiaru.
     * @return Data jako string.
     */
    string getDate() const;

    /**
     * @brief Zwraca wartość pomiaru.
     * @return Wartość jako double.
     */
    double getValue() const;
};

/**
 * @brief Klasa przechowująca dane pomiarowe dla konkretnego parametru.
 */
class Data {
private:
    string key_; ///< Klucz określający typ danych.
    vector<Values*> values_; ///< Lista wartości powiązanych z kluczem.

public:
    /**
     * @brief Konstruktor przyjmujący klucz danych.
     * @param key Klucz (np. "PM10", "NO2").
     */
    Data(string key);

    /**
     * @brief Konstruktor tworzący obiekt na podstawie danych JSON.
     * @param DataEntry Wpis JSON zawierający dane pomiarowe.
     */
    Data(const json& DataEntry);

    /**
     * @brief Konstruktor kopiujący.
     * @param other Obiekt klasy Data do skopiowania.
     */
    Data(const Data& other);

    /**
     * @brief Dodaje nową wartość pomiarową.
     * @param date Data pomiaru.
     * @param value Wartość pomiaru.
     */
    void addValue(string date, double value);

    /**
     * @brief Zwraca klucz danych.
     * @return Klucz jako string.
     */
    string getKey() const;

    /**
     * @brief Zwraca wskaźniki do listy wartości.
     * @return Referencja do wektora wskaźników Values.
     */
    const vector<Values*>& getValues() const;

    /**
     * @brief Destruktor klasy Data.
     */
    ~Data();
};

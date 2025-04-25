#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAbstractButton>
#include <windows.h>
#include <direct.h>

/**
 * @brief Klasa reprezentująca okno dialogowe z ustawieniami aplikacji.
 *
 * Pozwala użytkownikowi skonfigurować opcje, takie jak automatyczne pobieranie
 * danych oraz połączenie z usługą API. Umożliwia również pobranie lub usunięcie danych.
 */
class settingsWindow : public QDialog {
public:
    /**
     * @brief Konstruktor okna ustawień.
     * @param parent Wskaźnik na rodzica (opcjonalnie).
     */
    settingsWindow(QWidget* parent = nullptr);

    /**
     * @brief Zwraca wskaźnik do przycisków dialogowych (np. OK).
     * @return Wskaźnik do QDialogButtonBox.
     */
    QDialogButtonBox* buttonBox() const { return buttonBox_; };

    /**
     * @brief Zwraca wskaźnik do pola wyboru automatycznego pobierania.
     * @return Wskaźnik do QCheckBox.
     */
    QCheckBox* autoDownloadCheckBox() const { return autoDownloadCheckBox_; };

    /**
     * @brief Zwraca wskaźnik do pola wyboru połączenia z API.
     * @return Wskaźnik do QCheckBox.
     */
    QCheckBox* apiConnectCheckBox() const { return apiConnectCheckBox_; };

    /**
     * @brief Zwraca wskaźnik do przycisku pobierania danych.
     * @return Wskaźnik do QPushButton.
     */
    QPushButton* downloadButton() const { return downloadButton_; };

    /**
     * @brief Zwraca wskaźnik do przycisku usuwania danych.
     * @return Wskaźnik do QPushButton.
     */
    QPushButton* deleteButton() const { return deleteButton_; };

private:
    /** Przycisk OK (lub inne przyciski dialogowe). */
    QDialogButtonBox* buttonBox_ = new QDialogButtonBox();

    /** Pole wyboru umożliwiające automatyczne pobieranie danych. */
    QCheckBox* autoDownloadCheckBox_ = new QCheckBox();

    /** Pole wyboru do aktywowania połączenia z usługą API. */
    QCheckBox* apiConnectCheckBox_ = new QCheckBox();

    /** Przycisk do ręcznego pobierania wszystkich danych. */
    QPushButton* downloadButton_ = new QPushButton();

    /** Przycisk do usuwania wszystkich danych. */
    QPushButton* deleteButton_ = new QPushButton();

    /** Główny układ pionowy dla całego okna. */
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    /** Układ poziomy dla przycisku pobierania. */
    QHBoxLayout* downloadLayout = new QHBoxLayout();

    /** Układ poziomy dla przycisku usuwania. */
    QHBoxLayout* deleteLayout = new QHBoxLayout();

    /** Układ poziomy dla pól wyboru. */
    QHBoxLayout* checkboxLayout = new QHBoxLayout();

    /** Układ pionowy po prawej stronie (np. przyciski OK/Anuluj). */
    QVBoxLayout* rightLayout = new QVBoxLayout(); //ok cancel guziki

    /** Główny układ poziomy, dzielący lewą i prawą stronę. */
    QHBoxLayout* horizontalLayout = new QHBoxLayout();

    /** Układ pionowy po lewej stronie (checkboxy, przyciski akcji). */
    QVBoxLayout* leftLayout = new QVBoxLayout();

    /**
     * @brief Sprawdza, czy dane projektu istnieją.
     * @return true jeśli katalog z danymi istnieje, false w przeciwnym razie.
     */
    bool dataExists();

    /**
     * @brief Reakcja na zmianę stanu pola "automatyczne pobieranie".
     * @param checked Czy pole jest zaznaczone.
     */
    void onAutoDownloadChanged(bool checked);

    /**
     * @brief Reakcja na zmianę stanu pola "połączenie z API".
     * @param checked Czy pole jest zaznaczone.
     */
    void onApiConnectChanged(bool checked);

    /**
     * @brief Inicjalizuje przyciski wyboru (np. OK).
     */
    void initChoiceButtons();

    /**
     * @brief Inicjalizuje pola wyboru (checkboxy).
     */
    void initCheckboxes();

    /**
     * @brief Inicjalizuje przyciski do pobierania i usuwania danych.
     */
    void initPushButtons();

    /**
     * @brief Włącza możliwość zaznaczenia pola "połącz z API".
     */
    void enableInternetCheckbox();
};

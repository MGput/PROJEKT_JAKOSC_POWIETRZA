#include "APIcontroller.h"
#include <iostream>


using namespace std;
using json = nlohmann::json;

void CurlServiceFundaments::initializeCurl(CURL*& curl) {
    curl_global_init(CURL_GLOBAL_DEFAULT);                                          // Inicjalizacja curl
    curl = curl_easy_init();                                                        // Inicjalizacja uchwytu
    //if (!curl) cerr << "Błąd inicjalizacji CURL" << endl;
}

size_t CurlServiceFundaments::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {    // Funkcja do odbierania danych
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool CurlServiceFundaments::checkInternetStatus() {
    CURL* curl;
    initializeCurl(curl);
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.gios.gov.pl");
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 1000);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
        //cout << "rozpoczęcie testu internetu..." << endl;
        CURLcode res = curl_easy_perform(curl);
        //cout << "test internetu zakonczony." << endl;
        curl_easy_cleanup(curl);
        //cout << "test internetu zakonczony." << endl;
        return (res == CURLE_OK);
    }
    return false;
}

json CurlServiceFundaments::makeRequest(const string& url) {
    json responseFinal = json();
    CURL* curl;
    initializeCurl(curl);
    if (curl) {
        string responseSingle;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());                           // Ustawienie adresu URL
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);               // Ustawienie funkcji i wskaźnika do odbierania danych
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseSingle);
        CURLcode reqResult = curl_easy_perform(curl);                               // Zapytanie
        if (reqResult != CURLE_OK) {
            //cerr << "Błąd zapytania: " << curl_easy_strerror(reqResult) << endl;
        }
        else {
            long HTTPstatus = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &HTTPstatus);
            if (HTTPstatus == 200) {                                                // Kod OK
                try {
                    responseFinal = json::parse(responseSingle);
                }
                catch (const json::parse_error& e) {                                // Obsługa błędów JSON
                    //cerr << "Błąd parsowania JSON: " << e.what() << endl;
                }
            }
            else {                                                                  // Obsługa błędów HTTP
                //cerr << "Błąd HTTP " << HTTPstatus << endl;
            }
        }
    }
    curl_easy_cleanup(curl);
    return responseFinal;
}

GIOS_APImanagement::GIOS_APImanagement(QObject* parent) :
    QObject(parent),
    jsonController_(new jsonController())
{}

void GIOS_APImanagement::setConnectionStatus() {
    if (!checkInternetStatus()) GlobalSettings::disableInternet();
    else GlobalSettings::enableInternet();
}

json GIOS_APImanagement::getStationList() {
    setConnectionStatus();
    if (GlobalSettings::isInternetEnabled() and GlobalSettings::areYouSureTheInternetIsEnabled()) {
        json response = makeRequest("https://api.gios.gov.pl/pjp-api/rest/station/findAll");
        if(GlobalSettings::isAutomaticDownloadEnabled()) jsonController_->saveStationList(response);
        return response;
    }
    //cerr << "BRAK POŁĄCZENIA Z USŁUGĄ API!!!" << endl;
    //cerr << "ODCZYTYWANIE DANYCH Z PLIKU..." << endl;
    return jsonController_->loadStationList();
}

json GIOS_APImanagement::getSensorList(int stationId) {
    if (GlobalSettings::isInternetEnabled() and GlobalSettings::areYouSureTheInternetIsEnabled()) {
        json response = makeRequest("https://api.gios.gov.pl/pjp-api/rest/station/sensors/" + to_string(stationId));
        if(GlobalSettings::isAutomaticDownloadEnabled()) jsonController_->saveSensorList(response);
        return response;
    }
    //cerr << "BRAK POŁĄCZENIA Z USŁUGĄ API!!!" << endl;
    //cerr << "ODCZYTYWANIE DANYCH Z PLIKU..." << endl;
    return jsonController_->loadSensorList(stationId);
}

json GIOS_APImanagement::getSensorData(int stationId, int sensorId, string key) {
    if (GlobalSettings::isInternetEnabled() and GlobalSettings::areYouSureTheInternetIsEnabled()) {
        json response = makeRequest("https://api.gios.gov.pl/pjp-api/rest/data/getData/" + to_string(sensorId));
        if(GlobalSettings::isAutomaticDownloadEnabled()) jsonController_->saveSensorData(response, stationId);
        return response;
    }
    //cerr << "BRAK POŁĄCZENIA Z USŁUGĄ API!!!" << endl;
    //cerr << "ODCZYTYWANIE DANYCH Z PLIKU..." << endl;
    return jsonController_->loadSensorData(stationId, key);
}

json GIOS_APImanagement::getAirQualityIndex(int stationId) {
    if (GlobalSettings::isInternetEnabled() and GlobalSettings::areYouSureTheInternetIsEnabled()) {
        return makeRequest("https://api.gios.gov.pl/pjp-api/rest/aqindex/getIndex/" + to_string(stationId));
    }
    //cerr << "BRAK POŁĄCZENIA Z USŁUGĄ API!!!" << endl;
    //cerr << "ODCZYTYWANIE DANYCH Z PLIKU..." << endl;
    return jsonController_->loadAirIndex(stationId);
}

void GIOS_APImanagement::downloadAllData() {
    if (GlobalSettings::isInternetEnabled() and GlobalSettings::areYouSureTheInternetIsEnabled()) {
        jsonController_->saveAllData();
        emit finished();
        return;
    }
  //cout << "BRAK INTERNETU, NIE MOŻNA POBRAĆ DANYCH.";
    emit finished();
}

void GIOS_APImanagement::removeAllData() {
    jsonController_->deleteAllData();
    emit finished();
}

GIOS_APImanagement::~GIOS_APImanagement() {
	delete jsonController_;
}
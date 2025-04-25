# AirQualityMonitor

Aplikacja desktopowa do monitorowania jakości powietrza na podstawie danych z Głównego Inspektoratu Ochrony Środowiska (GIOŚ), stworzona z użyciem C++, Qt oraz CURL.

## Funkcje

- Pobieranie listy stacji pomiarowych i czujników z API GIOŚ
- Odczyt danych pomiarowych oraz indeksu jakości powietrza
- Możliwość działania offline – odczyt danych z lokalnych plików JSON
- Przejrzysty interfejs graficzny wykonany w Qt
- Konfigurowalne ustawienia (automatyczne pobieranie danych, status internetu)

## Wymagania

- Qt 6.9 lub nowszy
- Visual Studio 2022
- cURL
- biblioteka `nlohmann/json`
- Windows 10+

## Kompilacja

1. Otwórz plik `AirQualityMonitor.sln` w Visual Studio.
2. Wybierz konfigurację `Debug` lub `Release`.
3. Zbuduj projekt (`Build > Build Solution`).

## Struktura katalogów

- `*.cpp / *.h` – kod źródłowy aplikacji
- `*.ui` – pliki interfejsu Qt
- `images/` – zasoby graficzne GUI
- `resources.qrc` – zasób Qt dla plików graficznych

## Autorzy

<sub>Maksym Górny</sub>
<sub>Projekt stworzony w ramach zajęć JPO na Politechnice Poznańskiej.</sub>

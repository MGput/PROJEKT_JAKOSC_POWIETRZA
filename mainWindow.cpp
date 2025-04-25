#include "mainWindow.h"
#include "settingsWindow.h"
#include <string>
#include <QtConcurrent/QtConcurrent>


MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    apiManager(new GIOS_APImanagement()),
    loadedStations(vector<Station*>()),
    connectionCheckTimer(new QTimer(this)),
    chartView(new QChartView(this))
{
    ui.setupUi(this);

    ui.lineEdit->setPlaceholderText("Wyszukaj stacji...");

    //this->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

    ui.folderButton->setIcon(QIcon(":/images/folder.png"));
    ui.folderButton->setIconSize(QSize(23, 23));
    ui.folderButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

    ui.searchButton->setIcon(QIcon(":/images/search.png"));
    ui.searchButton->setIconSize(QSize(23, 23));

    ui.refreshButton->setIcon(QIcon(":/images/refresh.png"));
    ui.refreshButton->setIconSize(QSize(23, 23));

    ui.settings->setIcon(QIcon(":/images/settings.png"));
    ui.settings->setIconSize(QSize(23, 23));

    QTableWidgetItem* ___qtablewidgetitem = ui.tableWidget->horizontalHeaderItem(0);
    ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "Nazwa", nullptr));
    QTableWidgetItem* ___qtablewidgetitem1 = ui.tableWidget->horizontalHeaderItem(1);
    ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "Miasto", nullptr));
    QTableWidgetItem* ___qtablewidgetitem2 = ui.tableWidget->horizontalHeaderItem(2);
    ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Wojew\303\263dztwo", nullptr));
    ui.tableWidget->setColumnWidth(0, 180);
    ui.tableWidget->setColumnWidth(1, 150);
    ui.tableWidget->setColumnWidth(2, 227);
    ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui.mainLabel->setText(QCoreApplication::translate("MainWindow", "Program do obsługi API GIOS", nullptr));
    ui.stationId->setText(QCoreApplication::translate("MainWindow", "Projekt JPO", nullptr));
    ui.adress->setText(QCoreApplication::translate("MainWindow", "160330 Maksym Górny LAB04", nullptr));

    ui.sensorTableWidget->setGeometry(QRect(1010, 160, 241, 21));
    ui.sensorTableWidget->setColumnWidth(0, 83);
    ui.sensorTableWidget->setColumnWidth(1, 155);
    ui.sensorTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui.sensorListWidget->hide();
    ui.sensorTableWidget->hide();
    ui.dataListWidget->hide();
    chartView->hide();
    ui.dataListWidget->hide();

    setWindowTitle("GIOŚ Jakość Powietrza");
    setFixedSize(QSize(1280, 572));

    apiConnectionWarning = new QLabel(this);
    apiConnectionWarning->setStyleSheet("color: red;");
    apiConnectionWarning->setAlignment(Qt::AlignCenter);
    apiConnectionWarning->setFixedWidth(182);
    apiConnectionWarning->adjustSize();
    QPoint refreshPos = ui.refreshButton->pos();
    apiConnectionWarning->move(refreshPos.x(), refreshPos.y() - 20);

    connect(ui.refreshButton, &QPushButton::clicked, this, &MainWindow::loadStations);
    connect(ui.settings, &QPushButton::clicked, this, &MainWindow::settingsPopup);
    connect(ui.folderButton, &QPushButton::clicked, this, &MainWindow::openFolder);
    connect(ui.searchButton, &QPushButton::clicked, this, &MainWindow::searchStations);
    connect(ui.lineEdit, &QLineEdit::returnPressed, this, &MainWindow::searchStations);
    connect(ui.listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::stationDoubleClicked);
    connect(ui.sensorListWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::sensorDoubleClicked);

    connect(connectionCheckTimer, &QTimer::timeout, this, &MainWindow::checkConnectionStatus);
    connectionCheckTimer->start(2500);

    loadStations();
}

void MainWindow::checkConnectionStatus() {
    //statusBar()->showMessage("Sprawdzono połączenie z usługą API", 3000);

    if (!GlobalSettings::areYouSureTheInternetIsEnabled()) return;

    QFuture<void> future = QtConcurrent::run([this]() {
        apiManager->setConnectionStatus();
    });
    
    QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
    watcher->setFuture(future);

    connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher]() {
        updateConnectionWarning();
        watcher->deleteLater();
        });
}

void MainWindow::updateConnectionWarning() {
    if (!GlobalSettings::isInternetEnabled() || !GlobalSettings::areYouSureTheInternetIsEnabled()) {
        apiConnectionWarning->setText("BRAK POŁĄCZENIA Z USŁUGĄ API");
        apiConnectionWarning->show();

    }
    else {
        apiConnectionWarning->hide();

    }
}

QString MainWindow::truncate(const std::string& text, int maxLength) {
    QString qText = text.c_str();
    if (qText.length() > maxLength) {
        return qText.left(maxLength - 3) + "...";
    }
    return qText;
}

void MainWindow::openFolder() {
    QString folderPath = "projectData";
    QDir dir(folderPath);

    if (dir.exists()) {
        QString absolutePath = dir.absolutePath();
        QDesktopServices::openUrl(QUrl::fromLocalFile(absolutePath));
    }
    else {
        QMessageBox msgBox;
        msgBox.setText("Brak danych");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }
}

void MainWindow::downloadRequest(){
    QMessageBox msgBox;
    
    msgBox.setText("Brak połączenia z usługą API");
    if (!GlobalSettings::isInternetEnabled() || !GlobalSettings::areYouSureTheInternetIsEnabled()) {
        msgBox.exec();
        return;
    }

    msgBox.setWindowTitle("UWAGA");
    msgBox.setText("Proces pobierania może potrwać dłuższą chwilę.");
    msgBox.setInformativeText("Czy napewno chcesz pobrać wszystkie dane?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int res = msgBox.exec();
    if (res == QMessageBox::Yes){
        QDialog* loadingDialog = new QDialog(this);
        loadingDialog->setAttribute(Qt::WA_DeleteOnClose);
        loadingDialog->setWindowModality(Qt::WindowModal);
        loadingDialog->setFixedSize(150, 100);
        loadingDialog->setWindowTitle(" ");

        QLabel* loadingLabel = new QLabel("Pobieranie.", loadingDialog);
        loadingLabel->setAlignment(Qt::AlignCenter);

        QPushButton* stopButton = new QPushButton("Zatrzymaj", loadingDialog);

        QVBoxLayout* layout = new QVBoxLayout(loadingDialog);
        layout->addWidget(loadingLabel);
        layout->addWidget(stopButton, 0, Qt::AlignCenter);
        loadingDialog->setLayout(layout);

        QTimer* timer = new QTimer(loadingDialog);
        int counter = 0;
        QObject::connect(timer, &QTimer::timeout, [loadingLabel, &counter]() {
            counter = (counter + 1) % 3;
            switch (counter) {
            case 0: loadingLabel->setText("Pobieranie."); break;
            case 1: loadingLabel->setText("Pobieranie.."); break;
            case 2: loadingLabel->setText("Pobieranie..."); break;
            }
            });

        timer->start(500);

        shared_ptr<atomic<bool>> stopFlag = make_shared<atomic<bool>>(false);
        shared_ptr<atomic<bool>> isCompleted = make_shared<atomic<bool>>(false);

        QObject::connect(stopButton, &QPushButton::clicked, [stopFlag, loadingLabel]() {
            *stopFlag = true;
        });

        shared_ptr<std::thread> downloadThread = make_shared<std::thread>([this, stopFlag, isCompleted]() {
            jsonController controller;
            controller.saveAllData(stopFlag.get());
            *isCompleted = true;
        });

        QTimer* checkThread = new QTimer(loadingDialog);
        QObject::connect(checkThread, &QTimer::timeout, [downloadThread, loadingDialog, timer, checkThread, isCompleted]() {
            if (*isCompleted) {
                timer->stop();
                checkThread->stop();
                loadingDialog->close();
            }
        });
        checkThread->start(500);

        QObject::connect(loadingDialog, &QDialog::finished, [timer, checkThread, downloadThread, stopFlag]() {
            timer->stop();
            checkThread->stop();

            *stopFlag = true;

            if (downloadThread->joinable()) {
                downloadThread->join();
            }
            });

        loadingDialog->show();
    }
}

void MainWindow::deleteRequest() {
    QMessageBox msgBox;
    msgBox.setWindowTitle("UWAGA");
    msgBox.setInformativeText("Czy napewno chcesz usunąć wszystkie dane?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int res = msgBox.exec();
    if (res == QMessageBox::Yes) {
        QDialog* loadingDialog = new QDialog(this);
        loadingDialog->setWindowModality(Qt::WindowModal);
        loadingDialog->setFixedSize(150, 100);

        QLabel* loadingLabel = new QLabel("Usuwanie.", loadingDialog);
        loadingLabel->setAlignment(Qt::AlignCenter);

        QVBoxLayout* layout = new QVBoxLayout(loadingDialog);
        layout->addWidget(loadingLabel);
        loadingDialog->setLayout(layout);

        QTimer* timer = new QTimer(loadingDialog);
        int counter = 0;
        QObject::connect(timer, &QTimer::timeout, [loadingLabel, &counter]() {
            counter = (counter + 1) % 3;
            switch (counter) {
            case 0: loadingLabel->setText("Usuwanie."); break;
            case 1: loadingLabel->setText("Usuwanie.."); break;
            case 2: loadingLabel->setText("Usuwanie..."); break;
            }
            });

        timer->start(500);
        loadingDialog->show();

        QThread* workerThread = new QThread;
        GIOS_APImanagement* tempApiSession = new GIOS_APImanagement();
        tempApiSession->moveToThread(workerThread);

        QObject::connect(workerThread, &QThread::started, tempApiSession, &GIOS_APImanagement::removeAllData);
        QObject::connect(tempApiSession, &GIOS_APImanagement::finished, [loadingDialog, timer, workerThread, tempApiSession]() {
            timer->stop();
            loadingDialog->close();
            loadingDialog->deleteLater();
            workerThread->quit();
            workerThread->wait();
            workerThread->deleteLater();
            tempApiSession->deleteLater();
            });

        workerThread->start();
    }
}

void MainWindow::settingsPopup() {
    settingsWindow* uiSettings = new settingsWindow(this);
    uiSettings->resize(400, 119);
    uiSettings->setFixedSize(QSize(400, 119));
    connect(uiSettings->downloadButton(), &QPushButton::clicked, this, &MainWindow::downloadRequest);
    connect(uiSettings->deleteButton(), &QPushButton::clicked, this, &MainWindow::deleteRequest);
    connect(uiSettings->deleteButton(), &QPushButton::clicked, [=]() {
        uiSettings->deleteButton()->setDisabled(true);
        });
    uiSettings->exec();
    updateConnectionWarning();
    delete uiSettings;
}

void MainWindow::loadStations() {
    updateConnectionWarning();
    clearStations();
    ui.mainLabel->setText(QCoreApplication::translate("MainWindow", "Program do obsługi API GIOS", nullptr));
    ui.stationId->setText(QCoreApplication::translate("MainWindow", "Projekt JPO", nullptr));
    ui.adress->setText(QCoreApplication::translate("MainWindow", "160330 Maksym Górny LAB04", nullptr));
    ui.province->setText("");
    ui.communeAndProvince->setText("");
    ui.city->setText("");
    ui.coords->setText("");
    ui.airIndex->setText("");
    ui.lineEdit->setText("");
    ui.sensorListWidget->hide();
    ui.sensorTableWidget->hide();
    ui.dataListWidget->hide();
    chartView->hide();
    ui.dataListWidget->hide();
    ui.comboBox->setCurrentIndex(0);
    ui.listWidget->clear();
    ui.listWidget->setSpacing(2);
    json stationList = apiManager->getStationList();
    if (stationList.empty()) {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText("BRAK DANYCH");
        item->setTextAlignment(Qt::AlignCenter);
        ui.listWidget->addItem(item);
        statusBar()->showMessage("Nie znaleziono żadnych stacji.", 10000);
        return;
    }
    for (const auto& stationData : stationList) {
        Station* stat = new Station(stationData);
        loadedStations.push_back(stat);

        QWidget* customWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(customWidget);

        QLabel* stationName = new QLabel(truncate(stat->getStationName().c_str(), 25));
        QLabel* stationCity = new QLabel(truncate(stat->getCityName().c_str(), 15));
        QLabel* stationProvince = new QLabel(truncate(stat->getProvinceName().c_str(), 18));

        stationName->setFixedWidth(180);
        stationCity->setFixedWidth(120);
        stationProvince->setFixedWidth(140);

        stationName->setStyleSheet("font-weight: bold;");

        layout->addWidget(stationName);
        layout->addWidget(stationCity);
        layout->addWidget(stationProvince);

        layout->setContentsMargins(5, 5, 5, 5);

        QListWidgetItem* item = new QListWidgetItem();
        ui.listWidget->addItem(item);
        ui.listWidget->setItemWidget(item, customWidget);

        item->setSizeHint(customWidget->sizeHint());
    }
    statusBar()->showMessage("Załadowano " + QString::number(loadedStations.size()) + " stacji", 10000);
    displayedStations = loadedStations;
}

void MainWindow::clearStations() {
    for (const auto& stat : loadedStations) {
        delete stat;
    }
    loadedStations.clear();
}

void MainWindow::searchStations() {
    std::string selectOptions[7] = {"", "PM10", "PM2.5", "NO2", "SO2", "O3", "C6H6"};
    int selectedIndex = ui.comboBox->currentIndex();
    std::string selectedOption = selectOptions[selectedIndex];

    QString searchQuery = ui.lineEdit->text();
    if (searchQuery.isEmpty() and selectedOption == "") {
        loadStations();
        displayedStations = loadedStations;
        return;
    }
  
    std::string searchQueryString = searchQuery.toUtf8().constData();

    QTimer* timer = nullptr;

    if (!selectedOption.empty()) {
        ui.listWidget->clear();
        QListWidgetItem* loadingItem = new QListWidgetItem("Wyszukiwanie...");
        loadingItem->setTextAlignment(Qt::AlignCenter);
        ui.listWidget->addItem(loadingItem);
        QApplication::processEvents();
    };

    vector<Station*> searchResults;
    if (searchQuery.isEmpty()) searchResults = loadedStations;
    else {
        for (const auto& stat : loadedStations) {
            if (stat->checkForInfo(searchQueryString)) {
                searchResults.push_back(stat);
            }
        }
    }

    vector<Station*> results;
    if (selectedOption != "" and !searchResults.empty()) {
        vector<Station*> toCheck;
        toCheck.reserve(searchResults.size());
        copy(searchResults.begin(), searchResults.end(), back_inserter(toCheck));

        vector<std::thread> threads;
        mutex resultsMutex;

        unsigned int numThreads = thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4;
        numThreads = min(numThreads, (unsigned int)toCheck.size());

        size_t batchSize = (toCheck.size() + numThreads - 1) / numThreads;

        for (unsigned int i = 0; i < numThreads; ++i) {
            size_t start = i * batchSize;
            size_t end = min(start + batchSize, toCheck.size());

            if (start >= toCheck.size()) break;

            threads.push_back(std::thread([start, end, &toCheck, &results, &resultsMutex, &selectedOption, this]() {
                for (size_t j = start; j < end; ++j) {
                    Station* stat = toCheck[j];
                    if (stat->containsSensor(selectedOption)) {
                        lock_guard<mutex> lock(resultsMutex);
                        results.push_back(stat);
                    }
                }
                }));
        }

        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

    }
    else {
        results = searchResults;
    }

    ui.listWidget->clear();
    ui.listWidget->setSpacing(2);

    displayedStations.clear();

    int matchCount = 0;
    for (const auto& stat : results) {
        matchCount++;
        displayedStations.push_back(stat);
        QWidget* customWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(customWidget);

        QLabel* stationName = new QLabel(truncate(stat->getStationName().c_str(), 25));
        QLabel* stationCity = new QLabel(truncate(stat->getCityName().c_str(), 15));
        QLabel* stationProvince = new QLabel(truncate(stat->getProvinceName().c_str(), 18));

        stationName->setFixedWidth(180);
        stationCity->setFixedWidth(120);
        stationProvince->setFixedWidth(140);
        stationName->setStyleSheet("font-weight: bold;");

        layout->addWidget(stationName);
        layout->addWidget(stationCity);
        layout->addWidget(stationProvince);
        layout->setContentsMargins(5, 5, 5, 5);

        QListWidgetItem* item = new QListWidgetItem();
        ui.listWidget->addItem(item);
        ui.listWidget->setItemWidget(item, customWidget);
        item->setSizeHint(customWidget->sizeHint());
    }

    if (results.empty()) {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText("BRAK DANYCH");
        item->setTextAlignment(Qt::AlignCenter);
        ui.listWidget->addItem(item);
        statusBar()->showMessage("Nie znaleziono żadnych stacji.", 10000);
        return;
    }else statusBar()->showMessage("Znaleziono " + QString::number(matchCount) + " stacji", 10000);

    searchResults.clear();
    results.clear();
}

void MainWindow::sensorDoubleClicked(QListWidgetItem* item) {
    if (ui.sensorListWidget->count() == 1) {
        QListWidgetItem* firstItem = ui.sensorListWidget->item(0);
        if (firstItem->text() == "BRAK DANYCH") {
            return;
        }
    }

    int clickedIndex = ui.sensorListWidget->row(item);
    MeasurmentStand* selectedSensor = displayedStations[ui.listWidget->currentRow()]->getMeasurmentStands()[clickedIndex];
    int stationId = displayedStations[ui.listWidget->currentRow()]->getStationId();
    int sensorId = selectedSensor->getId();
    string paramFormula = selectedSensor->getParamFormula();
    
    json sensorData = apiManager->getSensorData(stationId, sensorId, paramFormula);

    auto values = sensorData["values"];
    int rowCount = std::min(static_cast<int>(values.size()), 24);

    QLineSeries* series = new QLineSeries();
    series->setName(QString::fromUtf8(paramFormula));

    QDateTime latestDate;
    bool firstValidDate = true;
    vector<tuple<QDateTime, double, bool>> dataPoints;

    for (int i = 0; i < rowCount; i++) {
        QString dateString = QString::fromUtf8(values[i]["date"].get<string>());
        QDateTime dateTime = QDateTime::fromString(dateString, Qt::ISODate);

        double value = 0;
        bool hasValue = false;
        QString valueString = "-";
        if (!values[i]["value"].is_null()) {
            value = values[i]["value"].get<double>();
            valueString = QString::number(value, 'f', 2);
            hasValue = true;

            if (firstValidDate) {
                latestDate = dateTime;
                firstValidDate = false;
            }

            double timeOffset = latestDate.secsTo(dateTime) / 3600.0;
            series->append(timeOffset, value);
        }
        dataPoints.push_back(make_tuple(dateTime, value, hasValue));
    }

    sort(dataPoints.begin(), dataPoints.end(),
        [](const auto& a, const auto& b) {
            return get<0>(a) > get<0>(b);
        });
    //a
    ui.dataListWidget->clear();

    for (const auto& dataPoint : dataPoints) {
        QDateTime dateTime = std::get<0>(dataPoint);
        double value = std::get<1>(dataPoint);
        bool hasValue = std::get<2>(dataPoint);

        QString valueString = hasValue ? QString::number(value, 'f', 2)+" μg/m³" : "-";

        QWidget* itemWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(itemWidget);

        QLabel* dateLabel = new QLabel(dateTime.toString("yyyy-MM-dd HH:mm"));
        dateLabel->setFixedWidth(25);
        dateLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        QLabel* valueLabel = new QLabel(valueString);
        dateLabel->setFixedWidth(95);
        valueLabel->setAlignment(Qt::AlignCenter);

        layout->addWidget(dateLabel);
        layout->addWidget(valueLabel);
        layout->setContentsMargins(5, 2, 5, 2);
        layout->setSpacing(5);

        itemWidget->setFixedHeight(25);

        QListWidgetItem* listItem = new QListWidgetItem();
        listItem->setSizeHint(itemWidget->sizeHint());
        ui.dataListWidget->addItem(listItem);
        ui.dataListWidget->setItemWidget(listItem, itemWidget);
    }

        QChart* chart = new QChart();
        chart->addSeries(series);
        chart->legend()->hide();
        chart->setTitle(QString("Pomiary %1").arg(QString::fromStdString(paramFormula)));

        QValueAxis* axisX = new QValueAxis();
        axisX->setLabelFormat("%.1f h");
        axisX->setTitleText("Czas [h]");
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        QValueAxis* axisY = new QValueAxis();
        axisY->setTitleText("Wartosc");
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        if (chartView) {
            delete chartView;
        }

        chartView = new QChartView(chart, this);
        chartView->setChart(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setGeometry(QRect(630, 310, 370, 200));

        ui.dataListWidget->show();
        chartView->show();
}

void MainWindow::stationDoubleClicked(QListWidgetItem* item) {
    if (ui.listWidget->count() == 1) {
        QListWidgetItem* firstItem = ui.listWidget->item(0);
        if (firstItem->text() == "BRAK DANYCH") {
            return;
        }
    }
    chartView->hide();
    ui.dataListWidget->hide();
    int clickedIndex = ui.listWidget->row(item);
    Station* selectedStation = displayedStations[clickedIndex];
    QString stationName = truncate(selectedStation->getStationName(), 45);
    ui.mainLabel->setText(stationName.isEmpty() ? "BŁĄD ŁADOWANIA NAZWY" : stationName);

    ui.stationId->setText(QString("ID: %1").arg(selectedStation->getStationId()));
    if (!selectedStation->getAddressStreet().empty()) {
        ui.adress->setText(QString::fromUtf8(selectedStation->getAddressStreet()));
    }
    else {
        ui.adress->setText("");
    }
    QString cityName = QString::fromUtf8(selectedStation->getCityName());
    ui.city->setText(QString("%1 (ID: %2)").arg(
        cityName.isEmpty() ? "BŁĄD ŁADOWANIA MIASTA" : cityName)
        .arg(selectedStation->getCityId()));


    QString communeName = QString::fromUtf8(selectedStation->getCommuneName());
    QString districtName = QString::fromUtf8(selectedStation->getDistrictName());
    ui.communeAndProvince->setText(QString("Gmina %1; Powiat %2")
        .arg(communeName.isEmpty() ? "BŁĄD ŁADOWANIA GMINY" : communeName)
        .arg(districtName.isEmpty() ? "BŁĄD ŁADOWANIA POWIATU" : districtName));

    ui.province->setText(QString::fromUtf8("WOJEWÓDZTWO "+selectedStation->getProvinceName()));

    string coords[2];
    selectedStation->assignCoordinates(coords);
    QString coord1 = QString::fromUtf8(coords[0]);
    QString coord2 = QString::fromUtf8(coords[1]);
    if (!coord1.isEmpty() && !coord2.isEmpty()) {
        ui.coords->setText(QString("%1°N %2°E").arg(coord1).arg(coord2));
    }
    else {
        ui.coords->setText("BŁĄD ŁADOWANIA KOORDYNATÓW");
    }

    if (selectedStation->getMeasurmentStands().empty()) {
        selectedStation->loadStands();
    }


    auto response = apiManager->getAirQualityIndex(selectedStation->getStationId());
    if (response.contains("stIndexLevel") and response["stIndexLevel"].contains("indexLevelName") and 
        response["stIndexLevel"]["indexLevelName"].is_string())
        {
        string indexLevel = apiManager->getAirQualityIndex(selectedStation->getStationId())["stIndexLevel"]["indexLevelName"].get<string>();
        string indexDate = apiManager->getAirQualityIndex(selectedStation->getStationId())["stSourceDataDate"].get<string>();
        ui.airIndex->setText(QString::fromUtf8("Ogólny indeks powietrza: " + indexLevel + "\n(" + indexDate + ")"));
    }
    else ui.airIndex->setText(QString(""));

    ui.sensorListWidget->clear();
    ui.sensorListWidget->setSpacing(2);

    vector<MeasurmentStand*> stands = selectedStation->getMeasurmentStands();
    if (stands.empty()) ui.sensorListWidget->addItem(new QListWidgetItem(QString("BRAK DANYCH")));
    else {
        for (const auto& stand : selectedStation->getMeasurmentStands()) {
            QWidget* customWidget = new QWidget();
            QHBoxLayout* layout = new QHBoxLayout(customWidget);

            string paramFormula = stand->getParamFormula();
            string indexPrefix = paramFormula;
            if (!indexPrefix.empty()) {											// Pętla do zmiany wielkich liter na małe
                for (char& let : indexPrefix) {
                    let = tolower(let);
                }
            }

            string paramIndexLevel;

            //if(indexPrefix == "pm2.5") indexPrefix = "pm25";
            indexPrefix += "IndexLevel";

            auto airQualityIndex = apiManager->getAirQualityIndex(selectedStation->getStationId());
            if (airQualityIndex.contains(indexPrefix) and airQualityIndex[indexPrefix]["indexLevelName"].is_string()) {
                paramIndexLevel = airQualityIndex[indexPrefix]["indexLevelName"].get<string>();
            }
            else {
                paramIndexLevel = "-";
            }

            QLabel* sensorFormula = new QLabel(QString::fromUtf8(paramFormula));
            QLabel* sensorIndex = new QLabel(QString::fromUtf8(paramIndexLevel));

            sensorFormula->setFixedWidth(70);
            sensorFormula->setAlignment(Qt::AlignCenter);
            sensorIndex->setFixedWidth(135);
            sensorIndex->setAlignment(Qt::AlignCenter);

            layout->addWidget(sensorFormula);
            layout->addWidget(sensorIndex);

            layout->setContentsMargins(0, 5, 0, 5);

            QListWidgetItem* item = new QListWidgetItem();
            ui.sensorListWidget->addItem(item);
            ui.sensorListWidget->setItemWidget(item, customWidget);

            item->setSizeHint(customWidget->sizeHint());
        }
    }
    ui.sensorTableWidget->show();
    ui.sensorListWidget->show();
}



MainWindow::~MainWindow() {
    connectionCheckTimer->stop();
    delete apiManager;
    delete apiConnectionWarning;
    if (chartView != nullptr) {
        delete chartView;
    }
    if (ui.dataListWidget != nullptr) {
        delete ui.dataListWidget;
    }
    clearStations();
}
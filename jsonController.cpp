#include "jsonController.h"
#include "APIcontroller.h"

using namespace std;

using json = nlohmann::json;

bool jsonController::folderExists(string folderPath) {
	DWORD attributes = GetFileAttributesA(folderPath.c_str());
	return (attributes == INVALID_FILE_ATTRIBUTES or !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

void jsonController::saveStationList(const json& stationList) {
	if (folderExists("projectData")) {
		//cout << "TWORZENIE FOLDERU projectData..." << endl;
		_mkdir("projectData");
		//cout << "FOLDER UTWORZONY" << endl;
	}
	ofstream outFile("projectData/stationList.json");
	//cout << "ZAPISYWANIE DO PLIKU..." << endl;
	outFile << stationList.dump(0);
	outFile.close();
	//cout << "ZAPISANO" << endl;
}

json jsonController::loadStationList() {
	ifstream inFile("projectData/stationList.json");
	json stationList;
	if (inFile.is_open()) {
		inFile >> stationList;
		inFile.close();
		if (stationList.empty()) {
			//cerr << "BRAK DANYCH :(" << endl;
			return json();
		}
//cout << "WCZYTANO DANE Z PLIKU!" << endl;
	}
	else {
		//cerr << "BRAK DANYCH :(" << endl;
	}
	return stationList;
}

void jsonController::saveSensorList(const json& sensors) {		// funkcja zapisuje rowniez indeks powietrza
	short statId = sensors[0]["stationId"].get<unsigned short>();
	string folderPath = "projectData/" + to_string(statId) + "stationData";
	if (folderExists(folderPath)) {
//cout << "TWORZENIE FOLDERÓW..." << endl;
		_mkdir(folderPath.c_str());
		_mkdir((folderPath + "/sensorsData").c_str());
//cout << "FOLDERY UTWORZONY" << endl;
	}

	string filePath = folderPath + "/sensorList.json";			// zapisywanie listy sensorow
	ofstream outFile(filePath.c_str());//cout << "ZAPISYWANIE DO PLIKU..." << endl;
	outFile << sensors.dump(0);//cout << "ZAPISANO" << endl;
	outFile.close();

	filePath = folderPath + "/airIndex.json";					// zapisywanie indeksu powietrza
	outFile.open(filePath);//cout << "ZAPISYWANIE DO PLIKU..." << endl;
	GIOS_APImanagement* tempApi = new GIOS_APImanagement();
	outFile << (tempApi->getAirQualityIndex(statId)).dump(0);
	delete tempApi;
	outFile.close();//cout << "ZAPISANO" << endl;
}

json jsonController::loadSensorList(const unsigned short& stationId) {
	string folderPath = "projectData/" + to_string(stationId) + "stationData";
	string filePath = folderPath + "/sensorList.json";
	ifstream inFile(filePath.c_str());
	json sensorList;
	if (inFile.is_open()) {
		inFile >> sensorList;
		inFile.close();
		if (sensorList.empty()) {
			//cerr << "emptBRAK DANYCH :(" << endl;
			return json();
		}
//cout << "WCZYTANO DANE Z PLIKU!" << endl;
	}
	else {
		//cerr << "elseBRAK DANYCH :(" << endl;
	}
	return sensorList;
};

void jsonController::saveSensorData(const json& sensorData, const unsigned short& stationId) {
	string folderPath = "projectData/" + to_string(stationId) + "stationData";
	if (folderExists(folderPath)) {
//cout << "TWORZENIE FOLDERU..." << endl;
		_mkdir(folderPath.c_str());
//cout << "FOLDER UTWORZONY" << endl;
	}
	string filePath = folderPath + "/sensorsData/"+ sensorData["key"].get<string>() +".json";
	ofstream outFile(filePath.c_str());//cout << "ZAPISYWANIE DO PLIKU..." << endl;
	outFile << sensorData.dump(0);
	outFile.close();//cout << "ZAPISANO DO " << filePath << endl;
}

json jsonController::loadSensorData(const unsigned short& stationId, string key) {
	string folderPath = "projectData/" + to_string(stationId) + "stationData/sensorsData/";
	string filePath = folderPath + key + ".json";
	ifstream inFile(filePath.c_str());
	json data;
	if (inFile.is_open()) {
		inFile >> data;
		inFile.close();
		if (data.empty()) {
			//cerr << "emptBRAK DANYCH :(" << endl;
			return json();
		}
//cout << "WCZYTANO DANE Z PLIKU!" << endl;
	}
	else {
		//cerr << "elseBRAK DANYCH :(??????" << endl;
	}
	return data;
}

json jsonController::loadAirIndex(const unsigned short& stationId) {
	string folderPath = "projectData/" + to_string(stationId) + "stationData";
	string filePath = folderPath + "/airIndex.json";
	ifstream inFile(filePath.c_str());
	json airIndex;
	if(inFile.is_open()){
		inFile >> airIndex;
		inFile.close();
		if (airIndex.empty()) return json();
	}
	return airIndex;
}

void jsonController::saveAllData(atomic<bool>* stopFlag) {
	unsigned short tempStatId, tempSensId;
	string tempSensParam;
	json tempSensorList;
	GIOS_APImanagement* tempAPI = new GIOS_APImanagement();
	json tempStationList = tempAPI->getStationList();
	saveStationList(tempStationList);

	mutex saveMutex;
	vector<thread> threads;

	auto downloadSensorData = [&saveMutex, tempAPI, stopFlag, this](
		unsigned short stationId, unsigned short sensorId, string paramCode) {		// definiowanie funkcji zapisujacej dane sensora, do pozniejszego wywolania (to na niej bedzie stosowana wielowatkowosc)
		if (stopFlag and *stopFlag) return;			// 2 razy stopFlag, zeby sprawdzic czy nie jest nullptr i jest bezpieczny do dereferencji
		
		json tempSensorData = tempAPI->getSensorData(stationId, sensorId, paramCode);
		{
			lock_guard<mutex> lock(saveMutex); // zapobiega konkurencji watkow
			saveSensorData(tempSensorData, stationId);
		}
		};

	const unsigned int maxThreads = thread::hardware_concurrency() > 0 ?
									thread::hardware_concurrency() : 4;  // sprawdza ile optymalnie moze zaimplementowac watkow dla systemu,
																		 // domyślnie 4

	vector<tuple<unsigned short, unsigned short, string>> tasks;
	for (auto& tempStat : tempStationList) {
		if (stopFlag and *stopFlag) break;
		unsigned short tempStatId = tempStat["id"].get<unsigned short>();
		tempSensorList = tempAPI->getSensorList(tempStatId);
		{
			lock_guard<mutex> lock(saveMutex);
			saveSensorList(tempSensorList);
		}

		for (auto& tempSens : tempSensorList) {
			if (stopFlag and *stopFlag) break;
			unsigned short tempSensId = tempSens["id"].get<unsigned short>();
			string tempSensParam = tempSens["param"]["paramCode"].get<string>();

			tasks.emplace_back(tempStatId, tempSensId, tempSensParam);
		}
	}

	size_t taskIndex = 0;
	while (taskIndex < tasks.size()) {
		if (stopFlag and *stopFlag) break;

		while (threads.size() < maxThreads and taskIndex < tasks.size()) {
			if (stopFlag and *stopFlag) break;
			auto& [statId, sensId, paramCode] = tasks[taskIndex++];
			threads.emplace_back(downloadSensorData, statId, sensId, paramCode);
		}

		if (!threads.empty()) {
			for (auto it = threads.begin(); it != threads.end();) {
				if (!it->joinable()) {
					it = threads.erase(it);
				}
				else {
					++it;
				}
			}

			if (threads.size() == maxThreads) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
		}
	}

	for (auto& thread : threads) { //czeka na wszystkie watki
		if (thread.joinable()) {
			thread.join();
		}
	}
	delete tempAPI;
}

void jsonController::deleteAllData() {
	remove("projectData/stationList.json");
	WIN32_FIND_DATAA findData;
	HANDLE hFind;
	string basePath = "projectData";
	string searchPattern = basePath + "/*";
	hFind = FindFirstFileA(searchPattern.c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) return;
	do {
		if (!strcmp(findData.cFileName, ".") || !strcmp(findData.cFileName, "..")) continue;
		string stationDirPath = basePath + "/" + findData.cFileName;
		DWORD attrs = GetFileAttributesA(stationDirPath.c_str());
		if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
			string sensorsDataPath = stationDirPath + "/sensorsData";
			DWORD sensorAttrs = GetFileAttributesA(sensorsDataPath.c_str());
			if (sensorAttrs != INVALID_FILE_ATTRIBUTES && (sensorAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
				WIN32_FIND_DATAA sensorData;
				HANDLE hSensorFind = FindFirstFileA((sensorsDataPath + "/*").c_str(), &sensorData);
				if (hSensorFind != INVALID_HANDLE_VALUE) {
					do {
						if (strcmp(sensorData.cFileName, ".") && strcmp(sensorData.cFileName, ".."))
							remove((sensorsDataPath + "/" + sensorData.cFileName).c_str());
					} while (FindNextFileA(hSensorFind, &sensorData));
					FindClose(hSensorFind);
				}
				RemoveDirectoryA(sensorsDataPath.c_str());
			}
			remove((stationDirPath + "/airIndex.json").c_str());
			remove((stationDirPath + "/sensorList.json").c_str());
			RemoveDirectoryA(stationDirPath.c_str());
		}
	} while (FindNextFileA(hFind, &findData));
	FindClose(hFind);
	RemoveDirectoryA(basePath.c_str());
}
#include "MeasurmentStand.h"

using namespace std;

Parameters::Parameters(string paramName, string paramFormula, string paramCode, unsigned short idParam) :
	paramName_(paramName),
	paramFormula_(paramFormula),
	paramCode_(paramCode),
	idParam_(idParam)
{
}

MeasurmentStand::MeasurmentStand(unsigned short id, unsigned short stationId, string paramName, string paramFormula, string paramCode, unsigned short idParam) :
	id_(id),
	stationId_(stationId),
	Parameters(paramName, paramFormula, paramCode, idParam),
	standData_(nullptr)
{}

MeasurmentStand::MeasurmentStand(const json& StandEntry) :
	id_(StandEntry["id"].get<unsigned short>()),
	stationId_(StandEntry["stationId"].get<unsigned short>()),
	standData_(nullptr),
	Parameters(
		StandEntry["param"]["paramName"].get<string>(),
		StandEntry["param"]["paramFormula"].get<string>(),
		StandEntry["param"]["paramCode"].get<string>(),
		StandEntry["param"]["idParam"].get<unsigned short>()
	)
{}

void MeasurmentStand::loadData() {
	GIOS_APImanagement* API = new GIOS_APImanagement();
	standData_ = new Data(API->getSensorData(stationId_, id_, paramFormula_));
	//cout << API->getAirQualityIndex(stationId_) << endl << endl;
	string result = paramFormula_;
	if (!result.empty()) {											// Pętla do zmiany wielkich liter na małe
		for (char& let : result) {
			let = tolower(let);
		}
	}
	//cout << (result + "IndexLevel") << endl;
	string indexParameter = result;
	if (indexParameter == "so2" || indexParameter == "pm10" || indexParameter == "pm25" || indexParameter == "o3" || indexParameter == "no2") {
		qualityIndex_ = API->getAirQualityIndex(stationId_)[(result + "IndexLevel")]["indexLevelName"].get<string>();
	}
	else qualityIndex_ = "";
	delete API;
}

MeasurmentStand::MeasurmentStand(const MeasurmentStand* other) :
	id_(other->getId()),
	stationId_(other->stationId_),
	Parameters(other->getParamName(), other->getParamFormula(), other->getParamCode(), other->getIdParam()),
    standData_(other->standData_ ? new Data(*other->standData_) : nullptr)
{}

unsigned short MeasurmentStand::getId() const { return id_; }

unsigned short MeasurmentStand::getStationId() const { return stationId_; }

string MeasurmentStand::getParamName() const { return paramName_; }

string MeasurmentStand::getParamFormula() const { return paramFormula_; }

string MeasurmentStand::getParamCode() const { return paramCode_; }

unsigned short MeasurmentStand::getIdParam() const { return idParam_; }

Data* MeasurmentStand::getStandData() const {
	return standData_;
}

string MeasurmentStand::getQualityIndex() const { return qualityIndex_; }

MeasurmentStand::~MeasurmentStand() { delete standData_; }
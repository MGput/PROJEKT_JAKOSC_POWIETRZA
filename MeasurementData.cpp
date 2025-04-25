#include "MeasurementData.h"

using namespace std;

Values::Values(string date, double value) :
	date_(date),
	value_(value)
{}

string Values::getDate() const { return date_; }

double Values::getValue() const { return value_; }


Data::Data(string key) : key_(key){}

Data::Data(const json& DataEntry) :
	key_(DataEntry["key"].get<string>())
{
	if (!DataEntry["values"].empty() and DataEntry["values"].is_array()) {
		for (const auto& value : DataEntry["values"]) {
			if (!value["date"].is_null() and !value["value"].is_null()) {
				addValue(value["date"].get<string>(), value["value"].get<double>());
			}
		}
	}
}

Data::Data(const Data& other) :
	key_(other.key_)
{
	for (const Values* value : other.values_) {
		values_.push_back(new Values(value->getDate(), value->getValue()));
	}
}

void Data::addValue(string date, double value) {
	values_.push_back(new Values(date, value));
}

string Data::getKey() const { return key_; }

const vector<Values*>& Data::getValues() const {
	return values_;
}

Data::~Data() {
	for (Values* value : values_) {
		delete value;
	}
}
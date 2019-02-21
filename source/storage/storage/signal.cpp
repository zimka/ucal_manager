#include "signal.h"
using namespace storage;

SignalData::SignalData(){}
SignalData::SignalData(SignalValue* start, size_t len){}
SignalData::SignalData(SignalData const& other){}
SignalData::SignalData(SignalData&& other) noexcept{}
SignalData& SignalData::operator=(SignalData const& other){
    return *this;
}
SignalData& SignalData::operator=(SignalData&& other) noexcept {
    return *this;
}

SignalData SignalData::detachBack(size_t len){
    return SignalData();
}
void SignalData::attachBack(SignalData& other){}

size_t SignalData::size() const{
    return 0;
}

std::string SignalData::repr() const{
    return "";
}

std::ostream& storage::operator<<(std::ostream& os, const SignalData& sd){
    return os;
}

std::istream& storage::operator>>(std::istream& is, SignalData& sd){
    return is;
}

SignalValue SignalData::operator[](size_t pos){
	return 0.;
}

void SignalData::swap(SignalData& other){}

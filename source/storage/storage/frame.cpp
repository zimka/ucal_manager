#include "frame.h"
#include "common/exceptions.h"

using namespace storage;

Frame::Frame(common::TimeStamp ts):ts_(ts){};
Frame::Frame(Frame&& other) noexcept:Frame(){
	std::swap(other.data_, data_);
	std::swap(other.size_, size_);
	std::swap(other.ts_, ts_);
};

Frame::KeyArray Frame::keys() const{
	KeyArray keys;
	for(auto it = data_.begin(); it != data_.end(); ++it)
  		keys.push_back(common::SignalKey::_from_string(it->first.c_str()));
	return keys;
}
bool Frame::hasKey(common::SignalKey key) const{
	return data_.count(key._to_string());
}
bool Frame::delKey(common::SignalKey key){
	if (!hasKey(key))
		return false;
	data_.erase(key._to_string());
	if (!keys().size())
		size_ = 0;
	return true;
}

common::TimeStamp Frame::getTs() const{
	return ts_;
}
void Frame::setTs(common::TimeStamp ts){
	ts_ = ts;
}

size_t Frame::size() const{
	return size_;
}

Frame Frame::detachBack(size_t len){
	if (len > size()){
		std::string message = "Frame size " + std::to_string(size());
		message += " is less than requested detachBack len: " + std::to_string(len);
		throw common::ValueError(message);
	}
	Frame f(computeDetachedTs(len));
	for (auto k: keys()){
		f[k] = data_[k._to_string()].detachBack(len);
	}
	size_ = size() - len;
	return f;
};
bool Frame::attachBack(Frame& other, bool enforce_ts){
	if (keys().size() && (other.keys() != keys()))
		return false;
	if ((!enforce_ts) && (other.getTs() < ts_))
		return false;
	auto current_keys = keys();
	if (!current_keys.size())
		current_keys = other.keys();
	for (auto k: current_keys){
		data_[k._to_string()].attachBack(other[k]);
	}
	size_ = size() + other.size();
	return true;
}
common::TimeStamp Frame::computeDetachedTs(size_t len) const{
	return common::TimeStamp(ts_.step, ts_.count + size() - len);
}

Frame::SignalDataProxy::SignalDataProxy(
	Frame::SignalKeyValue& source, size_t& size, common::SignalKey key
	):source_(source), size_(size), key_(key){}
Frame::SignalDataProxy Frame::operator[] (const common::SignalKey key){
	return Frame::SignalDataProxy(data_, size_,  key);
}
SignalData const& Frame::operator[] (const common::SignalKey key) const{
	return data_.at(key._to_string());
}

void validateSizes(size_t size_frame, size_t size_sd){
	if ((size_frame) && (size_frame != size_sd)){
		std::string message = "Can't assign SignalData with size " + std::to_string(size_sd);
		message += "to Frame with size "+ std::to_string(size_frame);
		throw common::ValueError(message);
	}
}
Frame::SignalDataProxy& Frame::SignalDataProxy::operator=(SignalData&& sd){
	validateSizes(size_, sd.size());
	size_ = sd.size();
	source_[key_._to_string()] = std::move(sd);
	return *this;
}
Frame::SignalDataProxy& Frame::SignalDataProxy::operator=(SignalData const& sd){
	validateSizes(size_, sd.size());
	size_ = sd.size();
	source_[key_._to_string()] = SignalData(sd);
	return *this;
}
Frame::SignalDataProxy::operator SignalData&(){
	if (!source_.count(key_._to_string())){
		std::string message = "No value in Frame with key ";
		message += key_._to_string();
		throw common::ValueError( message);
	}
	return source_[key_._to_string()];
}


void storage::to_json(json& j, const Frame& f){
	j = json();
	// TODO: very unefficient implementation, 
	// because SignalData is dumped then parsed. 
	// Should be changed, if ever really used for serialization
	for (auto k: f.keys()){
		SignalData const& v = f[k];
		j[k._to_string()] = json::parse(v.repr());
	}
}
std::string Frame::repr() const{
	json j = *this;
	return j.dump();
}
std::ostream& storage::operator<<(std::ostream& os, const Frame& f){
    os << f.repr();
    return os;
}
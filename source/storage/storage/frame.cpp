#include "frame.h"
using namespace storage;

Frame::Frame(common::TimeStamp ts):ts_(ts){};

Frame::KeyArray Frame::keys() const{
	return KeyArray();
}
bool Frame::hasKey(common::SignalKey key) const{
	return false;
}
bool Frame::delKey(common::SignalKey key){
	return false;
}

common::TimeStamp Frame::getTs() const{
	return ts_;
}
size_t Frame::size() const{
	return size_;
}


Frame Frame::detachBack(size_t len){
	return Frame(ts_);
};
bool Frame::attachBack(Frame const& other){
	return false;
}

Frame::SignalDataProxy::SignalDataProxy(Frame::SignalKeyValue& source, common::SignalKey key):source_(source), key_(key){}
Frame::SignalDataProxy Frame::operator[] (const common::SignalKey key){
	return Frame::SignalDataProxy(data_, key);
}
Frame::SignalDataProxy& Frame::SignalDataProxy::operator=(SignalData const& sd){
	return *this;
}
Frame::SignalDataProxy& Frame::SignalDataProxy::operator=(SignalData&& sd){
	return *this;
}
Frame::SignalDataProxy::operator SignalData&(){
	return stub;
}
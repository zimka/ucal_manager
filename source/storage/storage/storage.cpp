#include "storage.h"
#include "common/exceptions.h"
#include "common/utils.h"
#include <algorithm>

using namespace storage;

Storage::Storage(size_t frame_size, bool enforce_ts)
	:frame_size_(frame_size), enforce_ts_(enforce_ts){
		reset();
	};

bool Storage::append(Frame && f){
	// No checks for keys set of f, it is up to the client
	// Append consists of 3 stages: 
	// 1. attach new frame to head
	// 2. transfer frame_sized frame from head to data_ if needed
	// 3. update head_ ts 
	size_t current_size = size();

	common::TimeStamp last_ts = f.getTs();
	if (size())
		last_ts = data_[size() - 1].getTs();
	if (head_ != nullptr)
		last_ts = head_->getTs();
	else
		head_ = std::make_unique<Frame>(f.getTs());
	
	bool invalid_ts = last_ts > f.getTs();
	if (invalid_ts){
		if (!enforce_ts_)
			return false;
		else
			f.setTs(last_ts);
	}
	last_ts = f.getTs();
	bool status_ok = head_->attachBack(f);
	if (!status_ok)
		return false;
	size_t data_chunks_num = head_->size() / frame_size_;

	for (size_t i=1; i<=data_chunks_num; ++i){
		size_t cut_size = head_->size() - frame_size_;
		Frame rest = head_->detachBack(cut_size); 
		data_.push_back(std::move(*head_));
		head_ = std::make_unique<Frame>(std::move(rest));
	}

	if (!head_->size())
		head_ = nullptr;
	return true;
};

Frame const& Storage::operator[](size_t ind){
	if (ind >= size())
		throw common::ValueError(
			"Index " + std::to_string(ind) + " is higher than len " + std::to_string(size())
		);
	return data_.at(ind);
};
	
size_t Storage::size() const{
	return data_.size();
};
bool Storage::empty() const{
	return data_.empty();
};
    
Storage::Iterator Storage::begin() const{
	return data_.begin();
};
Storage::Iterator Storage::end() const{
	return data_.end();
};
Storage::Iterator Storage::afterTs(common::TimeStamp ts) const{
	// TODO: search is O(N) while can be O(logN), because frames are sorted by ts
	return std::find_if(
		data_.begin(), 
		data_.end(),
    	[&ts](const Frame& f) { return (f.getTs() > ts);}
    );
};

std::string Storage::getHash() const{
	return hash_;
};

size_t Storage::getFrameSize() const{
	return frame_size_;
}
bool Storage::reset(){
	data_.clear();
	head_ = nullptr;
	hash_ = common::getRandomString(hash_len_);
	return true;
};

bool Storage::finalize(){
	if (head_ == nullptr)
		return false;
	data_.push_back(std::move(*head_));
	head_ = nullptr;
	return true;
};

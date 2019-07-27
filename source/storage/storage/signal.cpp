#include "signal.h"
#include "common/exceptions.h"
#include "json/single_include/nlohmann/json.hpp"


using namespace storage;
using nlohmann::json;

SignalData::SignalData() {
    data_ = std::make_unique<Array>();
}

SignalData::SignalData(SignalValue *start, size_t len, size_t step) : SignalData() {
    auto finish = start + len * step;
    data_->reserve(len);
    for (auto val = start; val < finish; val += step)
        data_->push_back(*val);
}

SignalData::SignalData(SignalData const &other) {
    data_ = std::make_unique<Array>(*other.data_);
}

SignalData::SignalData(SignalData &&other) noexcept: SignalData() {
    swap(other);
}

SignalData &SignalData::operator=(SignalData const &other) {
    if (this != &other)
        SignalData(other).swap(*this);
    return *this;
}

SignalData &SignalData::operator=(SignalData &&other) noexcept {
    data_ = std::make_unique<Array>();
    swap(other);
    return *this;
}

SignalData SignalData::detachBack(size_t len) {
    if (len == 0)
        return SignalData();
    if (size() < len) {
        auto message = "Cant .detachBack SignalData of len " + std::to_string(len);
        message += ", when size is " + std::to_string(size());
        throw common::ValueError(message);
    }
    if (size() == len) {
        auto copy = SignalData();
        copy.swap(*this);
        return std::move(copy);
    }

    auto origin = std::move(data_);
    auto separator = origin->begin() + (origin->size() - len);
    data_ = std::make_unique<Array>(origin->begin(), separator);
    return SignalData(&*separator, len);
}

void SignalData::attachBack(SignalData const &other) {
    if (this == &other) {
        // This case is rare, and otherwise insert will fail
        auto copy = SignalData(other);
        return attachBack(copy);
    }
    data_->reserve(size() + other.size());
    data_->insert(data_->end(), other.data_->begin(), other.data_->end());
}

size_t SignalData::size() const {
    return data_->size();
}

std::string SignalData::repr() const {
    return json(*data_).dump();
}

std::ostream &storage::operator<<(std::ostream &os, const SignalData &sd) {
    os << sd.repr();
    return os;
}

std::istream &storage::operator>>(std::istream &is, SignalData &sd) {
    json j;
    is >> j;
    bool is_valid = j.is_array();
    if (is_valid) {
        for (auto &element : j)
            is_valid = is_valid && element.is_number();
    }
    if (!is_valid) {
        throw common::ValueError("SignalData can't be built from " + j.dump());
    }
    auto values = j.get<SignalData::Array>();
    if (!values.size())
        sd = SignalData();
    else
        sd = SignalData(&(*values.begin()), values.size());
    return is;
}

SignalValue SignalData::operator[](size_t pos) const {
    if (pos >= this->size())
        throw common::ValueError(
                "SignalData length" + std::to_string(this->size()) + " is less than position" + std::to_string(pos)
        );
    return (*data_)[pos];
}

void SignalData::swap(SignalData &other) {
    data_.swap(other.data_);
}

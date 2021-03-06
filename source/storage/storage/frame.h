#ifndef UCAL2_STORAGE_FRAME_H
#define UCAL2_STORAGE_FRAME_H

#include "common/keys.h"
#include "common/timestamp.h"
#include "signal.h"
#include "json/single_include/nlohmann/json.hpp"

#include <unordered_map>

using nlohmann::json;

namespace storage {
    /*!
    * Timestamped serializable key-value storage for signal data.
    * Stored signals must have the same size.
    */
    class Frame {
    public:
        using KeyArray = std::vector<common::SignalKey>;
        using SignalKeyValue = std::unordered_map<std::string, SignalData>;

        Frame() = default;

        Frame(common::TimeStamp ts);

        Frame(Frame&& other) noexcept;

        Frame& operator=(Frame&& other) = default;

        Frame(Frame const& other) = delete;

        KeyArray keys() const;

        bool hasKey(common::SignalKey key) const;

        bool delKey(common::SignalKey key);

        common::TimeStamp getTs() const;

        void setTs(common::TimeStamp ts);

        size_t size() const;

        std::string repr() const;

        Frame detachBack(size_t len);

        // TODO: Frame const& would be better, but
        // cant use it because of non-const operator[]
        bool attachBack(Frame& other, bool enforce_ts = false);

        class SignalDataProxy {
        public:
            SignalDataProxy() = delete;

            SignalDataProxy(
                    SignalKeyValue& source,
                    size_t& size,
                    common::SignalKey key = common::SignalKey::Undefined
            );

            operator SignalData&();

            SignalData stub;

            SignalDataProxy& operator=(SignalData const& sd);

            SignalDataProxy& operator=(SignalData&& sd);

        private:
            SignalKeyValue& source_;
            size_t& size_;
            common::SignalKey key_;
        };

        SignalDataProxy operator[](const common::SignalKey key);

        SignalData const& operator[](const common::SignalKey key) const;

    private:
        SignalKeyValue data_;
        common::TimeStamp ts_ = common::TimeStamp(0, 0);
        size_t size_ = 0;

        common::TimeStamp computeDetachedTs(size_t len) const;
    };

    void to_json(json& j, Frame const& f);

    std::ostream& operator<<(std::ostream& os, Frame const& sd);
}
#endif //UCAL2_STORAGE_FRAME_H

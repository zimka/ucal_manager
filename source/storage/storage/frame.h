#ifndef UCAL2_STORAGE_FRAME_H
#define UCAL2_STORAGE_FRAME_H
#include "common/keys.h"
#include "common/timestamp.h"
#include "signal.h"
#include <unordered_map>

namespace storage{
	class Frame{
	public:
		using KeyArray = std::vector<common::SignalKey>;
		using SignalKeyValue = std::unordered_map<std::string, SignalData>;
		Frame() = default;
		Frame(common::TimeStamp ts);
		Frame(Frame&& other) noexcept;
		Frame(Frame const& other) = delete;

		KeyArray keys() const;
		bool hasKey(common::SignalKey key) const;
		bool delKey(common::SignalKey key);

		common::TimeStamp getTs() const;
		size_t size() const;

		Frame detachBack(size_t len);
		// TODO: Frame const& would be better, but
		// cant use it because of non-const operator[]
		bool attachBack(Frame& other);

		class SignalDataProxy{
		public:
			SignalDataProxy() = delete;
			SignalDataProxy(
				SignalKeyValue& source, 
				size_t& size,
				common::SignalKey key=common::SignalKey::Default
			);
	        operator SignalData&();
	        SignalData stub;
			SignalDataProxy& operator=(SignalData const& sd);
			SignalDataProxy& operator=(SignalData && sd);
		private:
			SignalKeyValue& source_;
			size_t& size_;
			common::SignalKey key_;
		};
		SignalDataProxy operator[](const common::SignalKey key);
	private:
		SignalKeyValue data_;
		common::TimeStamp ts_=common::TimeStamp(0,0);
		size_t size_ = 0;
		common::TimeStamp computeDetachedTs(size_t len) const;
	};
}
#endif //UCAL2_STORAGE_FRAME_H
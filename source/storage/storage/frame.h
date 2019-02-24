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
		Frame(common::TimeStamp ts=common::TimeStamp(0,0));
		Frame(Frame&& other) = default;
		Frame(Frame const& other) = delete;

		KeyArray keys() const;
		bool hasKey(common::SignalKey key) const;
		bool delKey(common::SignalKey key);

		common::TimeStamp getTs() const;
		size_t size() const;

		Frame detachBack(size_t len);
		bool attachBack(Frame const& other);		

		class SignalDataProxy{
		public:
			SignalDataProxy() = delete;
			SignalDataProxy(
				SignalKeyValue& source, 
				common::SignalKey key=common::SignalKey::Default
			);
	        operator SignalData&();
	        SignalData stub;
			SignalDataProxy& operator=(SignalData const& sd);
			SignalDataProxy& operator=(SignalData && sd);
		private:
			SignalKeyValue& source_;
			common::SignalKey key_;
		};
		SignalDataProxy operator[](const common::SignalKey key);
	private:
		SignalKeyValue data_;
		const common::TimeStamp ts_;
		size_t size_ = 0;
		SignalData stub;
	};
}
#endif //UCAL2_STORAGE_FRAME_H
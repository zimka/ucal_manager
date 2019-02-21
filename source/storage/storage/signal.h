#ifndef UCAL2_STORAGE_SIGNAL_H
#define UCAL2_STORAGE_SIGNAL_H
#include <vector>
#include <iostream>
#include <memory>
#include <string>

namespace storage{
	/*!
	 *  Value representation in a signal.
 	*/
	using SignalValue = float;

	/*!
	*  Linear signal array.
	*/
	class SignalData{
	public:
		SignalData();
		SignalData(SignalValue* start, size_t len);
		SignalData(SignalData const& other);
		SignalData(SignalData&& other) noexcept;

		SignalData& operator=(SignalData const& other);
		SignalData& operator=(SignalData&& other) noexcept;

		size_t size() const;
		std::string repr() const;
		SignalValue operator[](const size_t pos);

		SignalData detachBack(size_t len);
		void attachBack(SignalData& other);

		~SignalData() = default;
	private:
		using Array = std::vector<SignalValue>;
		std::unique_ptr<Array> data_;
		void swap(SignalData& other);
	};
	std::ostream& operator<<(std::ostream& os, const SignalData& sd);
	std::istream& operator>>(std::istream& is, SignalData& sd);

}
#endif //UCAL2_STORAGE_SIGNAL_H
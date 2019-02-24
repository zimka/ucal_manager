#ifndef UCAL2_COMMON_EXCEPTIONS_H
#define UCAL2_COMMON_EXCEPTIONS_H
#include <exception>
#include <stdexcept>
#include <string>

namespace common{
	/*!
	 * Default project exception
	 */
	class UcalManagerException: public std::runtime_error{
	public:
	    explicit UcalManagerException(std::string const& s): runtime_error(s), what_message(s){};
	    virtual const char* what() noexcept{
	        return what_message.c_str();
	    }
	private:
	    std::string what_message="";
	};

	/*!
	 * Call to non-existent or forbidden Key
	 */
	class KeyError: public UcalManagerException{
	public:
	    explicit KeyError(const std::string& key_type, const std::string& key_bogus_value):
	    	UcalManagerException("Key of type <" + key_type + "> does not allow value: " + key_bogus_value){};
	};

	/*!
	* Violation of design assumptions, means bug in logic
	*/
	class AssertionError: public UcalManagerException{
		public:
	    explicit AssertionError(const std::string& message):
	    	UcalManagerException("Assertion failed: " + message){};
	    
	};

	/*!
	* Wrong input error
	*/
	class ValueError: public UcalManagerException{
		public:
	    explicit ValueError(const std::string& message):
	    	UcalManagerException("ValueError: " + message){};
	    
	};

	class NotImplementedError: public UcalManagerException{
	public:
	    explicit NotImplementedError(const std::string& s):
	    	UcalManagerException("Not implemented yet: " + s){};
	};
}
#endif //UCAL2_COMMON_EXCEPTIONS_H
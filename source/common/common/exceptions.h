#ifndef UCAL2_COMMON_EXCEPTIONS_H
#define UCAL2_COMMON_EXCEPTIONS_H

#include <exception>
#include <stdexcept>
#include <string>

namespace common {
    /*!
     * Default project exception
     */
    class UcalManagerException : public std::runtime_error {
    public:
        explicit UcalManagerException(std::string const& s) : runtime_error(s) {};
    };

    /*!
     * Generalizing class for simpler error handling
     * Used for AssertionError & NotImplementedError --
     * situations, which shouldn't happen, but can.
     * Program must stop/restart in this case, as
     * correct behavior is not guaranteed
     */
    class FatalError : public UcalManagerException {
    public:
        explicit FatalError(std::string const& s) : UcalManagerException(s) {};
    };

    /*!
     * Generalizing class for simpler error handling
     * Used for situations, when commands from user cannot
     * be executed due to inner logic, and we want to
     * inform user, that he is wrong
     */
    class InputError : public UcalManagerException {
    public:
        explicit InputError(std::string const& s) : UcalManagerException(s) {};
    };

    /*!
     * Call to non-existent or forbidden Key
     */
    class KeyError : public InputError {
    public:
        explicit KeyError(std::string const& key_type, std::string const& key_bogus_value) :
                InputError("Key of type <" + key_type + "> does not allow value: " + key_bogus_value) {};
    };

    /*!
    * Violation of design assumptions, means bug in logic
    */
    class AssertionError : public FatalError {
    public:
        explicit AssertionError(std::string const& message) :
                FatalError("Assertion failed: " + message) {};

    };

    /*!
    * Wrong input error
    */
    class ValueError : public InputError {
    public:
        explicit ValueError(std::string const& message) :
                InputError("ValueError: " + message) {};

    };

    /*!
     * Client command cannot be executed in current state
     */
    class StateViolationError : public UcalManagerException {
    public:
        explicit StateViolationError(std::string const& message) :
                UcalManagerException("StateViolationErrror: " + message) {};
    };

    class NotImplementedError : public FatalError {
    public:
        explicit NotImplementedError(std::string const& s) :
                FatalError("Not implemented yet: " + s) {};
    };

    /*!
    * Device usage error 
    */
    class DeviceError : public InputError {
    public:
        explicit DeviceError(const std::string& message) :
            InputError("DeviceError: " + message) {};
    };
}
#endif //UCAL2_COMMON_EXCEPTIONS_H

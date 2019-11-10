//
// Created by igor on 07.11.2019.
//

#ifndef UCAL_MANAGER_LOGGER_H
#define UCAL_MANAGER_LOGGER_H

#include <memory>

class ILogger {
public:
    virtual void log(char const* message) = 0;
    virtual void log(std::stringstream const& message_stream) = 0;
    virtual void withTime(char const* message) = 0;
    virtual std::string getAll() = 0;
    virtual void clean() = 0;
};

using LoggerPtr = std::shared_ptr<ILogger>;

LoggerPtr createLogger(std::string filename);
//class Logger;
#endif //UCAL_MANAGER_LOGGER_H

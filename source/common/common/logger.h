//
// Created by igor on 07.11.2019.
//

#ifndef UCAL_MANAGER_LOGGER_H
#define UCAL_MANAGER_LOGGER_H

#include <memory>
#include <vector>

class ILogger {
public:
    virtual void log(std::string const& message) = 0;
    virtual std::string getAll() = 0;
    virtual std::vector<std::string> getLines() = 0;
    virtual void clean() = 0;
};

using LoggerPtr = std::shared_ptr<ILogger>;

LoggerPtr createLogger(std::string filename);
#endif //UCAL_MANAGER_LOGGER_H

//
// Created by igor on 07.11.2019.
//

#include "logger.h"
#include <fstream>
#include <mutex>
#include <sstream>
#include <iomanip>

using namespace common;

class Logger : public ILogger
{
public:
    explicit Logger(std::string filename);

    void log(std::string const& message) override;
    std::vector<std::string> getLines() override;
    void clean() override;

private:
    std::string filename_;
    static std::mutex lock_;
};

std::mutex Logger::lock_;

LoggerPtr common::createLogger(std::string filename) {
    return std::make_shared<Logger>(std::move(filename));
}

LoggerPtr common::createDefaultLogger() {
    return std::make_shared<Logger>("errors.txt");
}

// Careful! logFile_ must be initialized in the first place
Logger::Logger(std::string filename)
    : filename_(std::move(filename))
{
}

void Logger::log(std::string const& message) {
    std::lock_guard<std::mutex> guard (lock_);
    std::fstream logFile (filename_, std::ios::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Cannot open file!");
    }
    using std::chrono::system_clock;
    auto now = system_clock::to_time_t(system_clock::now());
    logFile << std::put_time(std::localtime(&now), "%F %T") << '|' << message << std::endl;
}

std::vector<std::string> Logger::getLines() {
    std::lock_guard<std::mutex> guard (lock_);
    std::fstream logFile (filename_, std::ios::in);
    if (!logFile.is_open()) {
        throw std::runtime_error("Cannot open file!");
    }
    logFile.seekg(0);
    std::vector<std::string> result;
    for (std::string line; std::getline(logFile, line); )
    {
        result.push_back(line);
    }
    return result;
}

void Logger::clean() {
    std::lock_guard<std::mutex> guard (lock_);
    std::fstream logFile (filename_, std::ios::out | std::ios::trunc);
}

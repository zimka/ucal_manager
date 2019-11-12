//
// Created by igor on 07.11.2019.
//

#include "logger.h"
#include <fstream>
#include <mutex>
#include <sstream>
#include <iomanip>

class Logger : public ILogger
{
public:
    explicit Logger(std::string filename);

    void log(std::string const& message) override;
    std::string getAll() override;
    std::vector<std::string> getLines() override;
    void clean() override;

private:
    std::fstream logFile_;
    std::string filename_;
    std::mutex lock_;
};

LoggerPtr createLogger(std::string filename) {
    return std::make_shared<Logger>(std::move(filename));
}

// Careful! logFile_ must be initialized in the first place
Logger::Logger(std::string filename)
    : logFile_(filename, std::ios::in | std::ios::app)
    , filename_(std::move(filename))
{
    if (!logFile_.is_open())
        throw std::runtime_error("Cannot open file!");
}

void Logger::log(std::string const& message) {
    std::lock_guard<std::mutex> guard (lock_);
    using std::chrono::system_clock;
    auto now = system_clock::to_time_t(system_clock::now());
    logFile_ << std::put_time(std::localtime(&now), "%F %T") << '|' << message << std::endl;
}

std::string Logger::getAll() {
    std::lock_guard<std::mutex> guard (lock_);
    std::string content;
    logFile_.seekg(0, std::ios::end);
    unsigned size = logFile_.tellg();
    content.resize(size);
    logFile_.seekg(0);
    logFile_.read(&content[0], content.size());
    return content;
}

std::vector<std::string> Logger::getLines() {
    std::lock_guard<std::mutex> guard (lock_);
    logFile_.seekg(0);
    std::vector<std::string> result;
    for (std::string line; std::getline(logFile_, line); )
    {
        result.push_back(line);
    }
    return result;
}

void Logger::clean() {
    std::lock_guard<std::mutex> guard (lock_);
    logFile_.close();
    logFile_.open(filename_, std::ios::out | std::ios::in | std::ios::trunc);
}

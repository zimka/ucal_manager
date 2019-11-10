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

    void log(char const* message) override;
    void log(std::stringstream const&) override;
    void withTime(char const* message) override;
    std::string getAll() override;
    void clean() override;

private:
    std::fstream logFile_;
    std::string filename_;
    std::mutex lock_;
};

LoggerPtr createLogger(std::string filename) {
    return std::make_shared<Logger>(std::move(filename));
}

// Careful! Success of construction depends on order!
Logger::Logger(std::string filename)
    : logFile_(filename, std::ios::in | std::ios::app)
    , filename_(std::move(filename))
{
    if (!logFile_.is_open())
        throw std::runtime_error("Cannot open file!");
}

void Logger::log(char const* message) {
    std::lock_guard<std::mutex> guard (lock_);
    logFile_ << message << std::endl;
}

void Logger::log(std::stringstream const& message_stream) {
    std::lock_guard<std::mutex> guard (lock_);
    logFile_ << message_stream.rdbuf() << std::endl;
}

void Logger::withTime(char const* message) {
    using std::chrono::system_clock;
    auto now = system_clock::to_time_t(system_clock::now());
    std::stringstream stream;
    stream << std::put_time(std::localtime(&now), "%F %T") << " | " << message;
    log(stream);
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

void Logger::clean() {
    std::lock_guard<std::mutex> guard (lock_);
    logFile_.close();
    logFile_.open(filename_, std::ios::out | std::ios::in | std::ios::trunc);
}

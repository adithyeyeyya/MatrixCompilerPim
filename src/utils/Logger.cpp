/**
 * Logger.cpp
 * Implementation of logging utility
 */

#include "Logger.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : verbose(false) {}

Logger::~Logger() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

void Logger::setVerbose(bool verbose) {
    this->verbose = verbose;
}

void Logger::setOutputFile(const std::string& filename) {
    if (fileStream.is_open()) {
        fileStream.close();
    }
    
    outputFile = filename;
    fileStream.open(filename, std::ios::out | std::ios::app);
    
    if (!fileStream) {
        std::cerr << "Warning: Could not open log file: " << filename << std::endl;
    }
}

void Logger::log(const std::string& message) {
    // Get current timestamp
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    std::stringstream timestamp;
    timestamp << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S");
    
    // Format log message
    std::string formattedMessage = "[" + timestamp.str() + "] " + message;
    
    // Add to history
    history.push_back(formattedMessage);
    
    // Output if verbose
    if (verbose) {
        std::cout << formattedMessage << std::endl;
    }
    
    // Write to file if open
    if (fileStream.is_open()) {
        fileStream << formattedMessage << std::endl;
        fileStream.flush();
    }
}

void Logger::error(const std::string& message) {
    // Get current timestamp
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    std::stringstream timestamp;
    timestamp << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S");
    
    // Format error message
    std::string formattedMessage = "[" + timestamp.str() + "] ERROR: " + message;
    
    // Add to history
    history.push_back(formattedMessage);
    
    // Always output errors
    std::cerr << formattedMessage << std::endl;
    
    // Write to file if open
    if (fileStream.is_open()) {
        fileStream << formattedMessage << std::endl;
        fileStream.flush();
    }
}

const std::vector<std::string>& Logger::getHistory() const {
    return history;
}

/**
 * Logger.h
 * Simple logging utility for the compiler
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

class Logger {
public:
    // Singleton access
    static Logger& getInstance();
    
    // No copy/move constructor or assignment
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;
    
    // Set verbose mode
    void setVerbose(bool verbose);
    
    // Set output file
    void setOutputFile(const std::string& filename);
    
    // Log a message
    void log(const std::string& message);
    
    // Log an error
    void error(const std::string& message);
    
    // Retrieve log history
    const std::vector<std::string>& getHistory() const;

private:
    // Private constructor for singleton
    Logger();
    ~Logger();
    
    bool verbose;
    std::vector<std::string> history;
    std::string outputFile;
    std::ofstream fileStream;
};

#endif // LOGGER_H

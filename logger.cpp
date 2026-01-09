#include "logger.h"
#include <fstream>
#include <filesystem>
#include <shared_mutex>
#include <thread>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <future>

Logger* Logger::instance() {
    static Logger* singleton = nullptr;
    static std::once_flag flag;
    std::call_once(flag, []() {
        singleton = new Logger();
    });
    return singleton;
}

Logger::Logger() : isOpen(false) {
    const QString logDir = "LOG";

    if (!std::filesystem::exists(logDir.toStdString())) {
        try {
            std::filesystem::create_directory(logDir.toStdString());
            emit logMessage("Log directory created: " + logDir);
        }
        catch (const std::filesystem::filesystem_error& e) {
            emit logMessage("Error creating directory: " + QString::fromStdString(e.what()));
            return;
        }
    }

    fileStream.open(logFilePath.toStdString(), std::ios::app);
    if (fileStream.is_open()) {
        isOpen = true;
        writeLine("Logger initialized. Log file: " + logFilePath);
    } else {
        emit logMessage("Error: Could not open log file: " + logFilePath);
    }
}

Logger::~Logger() {
    if (isOpen) {
        writeLine("Logger shutting down.");
        fileStream.close();
    }
}

bool Logger::checkFileOpen() {
    if (!isOpen) {
        emit logMessage("Error: Log file is not open.");
        return false;
    }
    return true;
}

void Logger::writeLine(const QString& line) {
    if (!checkFileOpen()) return;

    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = {};
    localtime_s(&tm, &time_t);

    std::ostringstream timeStream;
    timeStream << std::setfill('0')
               << (tm.tm_year + 1900) << "-"
               << std::setw(2) << (tm.tm_mon + 1) << "-"
               << std::setw(2) << tm.tm_mday << " "
               << std::setw(2) << tm.tm_hour << ":"
               << std::setw(2) << tm.tm_min << ":"
               << std::setw(2) << tm.tm_sec;

    QString logEntry = QString::fromStdString(timeStream.str()) + " " + line;

    auto writeTask = std::async(std::launch::async, [this, logEntry]() {
        std::unique_lock lock(logMutex);
        fileStream << logEntry.toStdString() << std::endl;
        fileStream.flush();
    });

    emit logMessage(logEntry);
}

bool Logger::isFileOpen() const {
    return isOpen;
}

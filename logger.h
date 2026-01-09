#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <fstream>
#include <filesystem>
#include <shared_mutex>
#include <future>

class Logger : public QObject {
    Q_OBJECT

public:
    static Logger* instance();
    ~Logger();

    void writeLine(const QString& line);
    bool isFileOpen() const;

signals:
    void logMessage(const QString& message);

private:
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    bool checkFileOpen();
    std::fstream fileStream;
    bool isOpen;
    const QString logFilePath = "LOG/log.txt";
    mutable std::shared_mutex logMutex;
};

#define LOG(msg) Logger::instance()->writeLine(msg)
#endif // LOGGER_H

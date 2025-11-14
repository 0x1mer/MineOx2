#pragma once

#include "ILogger.h"

#include <fstream>
#include <iostream>
#include <mutex>
#include <chrono>
#include <iomanip>

class Logger : public ILogger {
public:
    static Logger& Instance() {
        static Logger instance;
        return instance;
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void Log(LogLevel level, std::string_view message) override;

    void Trace(std::string_view message) override { Log(LogLevel::Trace, message); }
    void Debug(std::string_view message) override { Log(LogLevel::Debug, message); }
    void Info(std::string_view message) override { Log(LogLevel::Info, message); }
    void Warning(std::string_view message) override { Log(LogLevel::Warning, message); }
    void Error(std::string_view message) override { Log(LogLevel::Error, message); }
    void Critical(std::string_view message) override { Log(LogLevel::Critical, message); }

    void SetLogLevel(LogLevel level) override { m_logLevel = level; }
    LogLevel GetLogLevel() const override { return m_logLevel; }

    void SetOutput(LogOutput output) override { m_output = output; }
    LogOutput GetOutput() const override { return m_output; }

    void SetLogFile(const std::string& folderPath) override;
    void CleanupOldLogs(const std::string& folderPath, size_t maxLogs = 10) override;
    const std::string& GetLogFile() const override { return m_filePath; }

private:
    Logger(LogLevel level = LogLevel::Info, LogOutput output = LogOutput::Console);
    ~Logger() override;

    std::mutex m_mutex;
    LogLevel m_logLevel;
    LogOutput m_output;
    std::string m_filePath;
    std::ofstream m_file;

    std::string GetTimeStamp() const;
    std::string LevelToString(LogLevel level) const;
    const char* LevelColor(LogLevel level) const;
};
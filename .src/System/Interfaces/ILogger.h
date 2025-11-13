#pragma once

#include <string>
#include <string_view>

enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

enum class LogOutput {
    Console,
    File,
    Both
};

class ILogger {
public:
    virtual ~ILogger() = default;

    virtual void Log(LogLevel level, std::string_view message) = 0;

    virtual void Trace(std::string_view message) = 0;
    virtual void Debug(std::string_view message) = 0;
    virtual void Info(std::string_view message) = 0;
    virtual void Warning(std::string_view message) = 0;
    virtual void Error(std::string_view message) = 0;
    virtual void Critical(std::string_view message) = 0;

    virtual void SetLogLevel(LogLevel level) = 0;
    virtual LogLevel GetLogLevel() const = 0;

    virtual void SetOutput(LogOutput output) = 0;
    virtual LogOutput GetOutput() const = 0;

    virtual void SetLogFile(const std::string& filePath) = 0;
    virtual const std::string& GetLogFile() const = 0;

    virtual void CleanupOldLogs(const std::string& folderPath, size_t maxLogs = 10);
};
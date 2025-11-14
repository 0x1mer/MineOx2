#include "Logger.h"

#include <filesystem>

#include "DebugColors.h"

Logger::Logger(LogLevel level, LogOutput output)
    : m_logLevel(level), m_output(output) {}

Logger::~Logger() {
    if (m_file.is_open())
        m_file.close();
}

void Logger::SetLogFile(const std::string& folderPath) {
    std::lock_guard<std::mutex> lock(m_mutex);

    using namespace std::chrono;
    auto now = system_clock::now();
    auto time = system_clock::to_time_t(now);

    std::tm tm {};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif

    // Формируем имя файла: log_YYYY-MM-DD_HH-MM-SS.txt
    std::ostringstream name;
    name << "log_"
         << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S")
         << ".txt";

    // Полный путь: folderPath + имя файла
    m_filePath = folderPath;
    if (!m_filePath.empty() && (m_filePath.back() != '/' && m_filePath.back() != '\\'))
        m_filePath += "\\";

    m_filePath += name.str();

    if (m_file.is_open())
        m_file.close();

    m_file.open(m_filePath, std::ios::app);
    if (!m_file)
        std::cerr << "[Logger] Failed to open log file: " << m_filePath << std::endl;
    else
        std::cout << "[Logger] Logging to file: " << m_filePath << std::endl;
}

void Logger::Log(LogLevel level, std::string_view message) {
    if (level < m_logLevel)
        return;

    std::lock_guard<std::mutex> lock(m_mutex);

    std::string timestamp = GetTimeStamp();
    std::string levelStr = LevelToString(level);
    const char* color = LevelColor(level);

    std::string formatted = "[" + timestamp + "] [" + levelStr + "] " + std::string(message);

    // Console output
    if (m_output == LogOutput::Console || m_output == LogOutput::Both) {
        std::cout << color << formatted << DebugColors::Reset << std::endl;
    }

    // File output
    if ((m_output == LogOutput::File || m_output == LogOutput::Both) && m_file.is_open()) {
        m_file << formatted << std::endl;
    }
}

std::string Logger::GetTimeStamp() const {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto time = system_clock::to_time_t(now);

    std::tm tm {};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif
    std::ostringstream ss;
    ss << std::put_time(&tm, "%H:%M:%S");
    return ss.str();
}

std::string Logger::LevelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace:    return "TRACE";
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO";
        case LogLevel::Warning:  return "WARN";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default:                 return "UNKNOWN";
    }
}

const char* Logger::LevelColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace:    return DebugColors::White;     // white
        case LogLevel::Debug:    return DebugColors::Cyan;      // cyan
        case LogLevel::Info:     return DebugColors::Green;     // green
        case LogLevel::Warning:  return DebugColors::Yellow;    // yellow
        case LogLevel::Error:    return DebugColors::Red;       // red
        case LogLevel::Critical: return DebugColors::BrightRed; // bright red
        default:                 return DebugColors::White;
    }
}

void Logger::CleanupOldLogs(const std::string& folderPath, size_t maxLogs) {
    try {
        if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath))
            return;

        std::vector<std::pair<std::filesystem::path, std::filesystem::file_time_type>> logs;

        for (auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                const auto& path = entry.path();
                if (path.extension() == ".txt" || path.extension() == ".log") {
                    logs.emplace_back(path, std::filesystem::last_write_time(path));
                }
            }
        }

        if (logs.size() <= maxLogs)
            return;

        std::sort(logs.begin(), logs.end(),
            [](auto& a, auto& b) {
                return a.second < b.second;
            });

        for (size_t i = 0; i < logs.size() - maxLogs; ++i) {
            try {
                std::filesystem::remove(logs[i].first);
                std::cout << "[Logger] Removed old log: " << logs[i].first << std::endl;
            } catch (...) {
                std::cerr << "[Logger] Failed to remove old log: " << logs[i].first << std::endl;
            }
        }
    } catch (...) {
        std::cerr << "[Logger] CleanupOldLogs failed\n";
    }
}
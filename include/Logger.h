#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <filesystem>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <fstream>
#include <thread>

enum class LogLevel
{
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  CRITICAL
};

struct LogLine
{
  LogLevel level;
  long long threadID;
  std::string message;
  std::string timestamp;
};

std::string logLevelToString(LogLevel level);
const long long getThreadID();

class Logger
{
  // The Logger class is responsible for managing log messages and writing them to a file.
  // It includes methods for logging messages at different levels and writing them to a file.
  // The logger can be configured with a name, log file path, and default logging level.
public:
  Logger(const std::string &name, const std::filesystem::path &logFilePath);
  ~Logger();
  const LogLevel &getMinimumLogLevel() const;
  const std::filesystem::path getLogFilePath() const;
  const std::string &getName() const;
  void debug(const std::string &message);
  void info(const std::string &message);
  void warning(const std::string &message);
  void error(const std::string &message);
  void critical(const std::string &message);

private:
  void processLogQueue_();
  void writeLine_(const LogLine &logLine);
  const std::string name_;
  std::ofstream logFile_;
  const LogLevel minimumLogLevel_{LogLevel::DEBUG};
  void log_(const std::string &message, LogLevel level);

  // These variables are used for thread-safe logging.
  bool loggingDone_{false};
  std::queue<LogLine> logQueue_;
  std::mutex logQueueMutex_;
  std::condition_variable cv_;
  std::thread workerThread_;
};

#endif // LOGGER_H
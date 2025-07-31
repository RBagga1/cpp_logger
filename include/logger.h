#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <filesystem>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <fstream>
#include <sstream>
#include <thread>
#include "logutil.h"

struct LogLine
{
  LogLevel level;
  long long threadID;
  std::string message;
  std::string timestamp;
};

class Logger
{
  // The Logger class is responsible for managing log messages and writing them to a file.
  // It includes methods for logging messages at different levels and writing them to a file.
  // The logger can be configured with various options.
public:
  Logger(const std::string &name,
         const std::filesystem::path &logFilePath,
         bool printToConsole = false,
         bool logThreadIDs = false,
         bool logSelfName = false,
         LogLevel minimumLogLevel = LogLevel::DEBUG);
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
  const std::string name_;
  const bool printToConsole_; // Whether to print logs to the console
  const bool logThreadIDs_;   // Whether to log thread IDs in each log line
  const bool logSelfName_;    // Whether to log the logger's name in each log line
  std::ofstream logFile_;
  const LogLevel minimumLogLevel_;

  // Methods to log messages and write them to the specified stream.
  void processLogQueue_();
  void writeLine_(const LogLine &logLine);
  void log_(const std::string &message, LogLevel level);
  const std::string getFormattedLogLine(const LogLine &logLine) const;

  // These variables are used for thread-safe logging.
  bool loggingDone_{false};
  std::queue<LogLine> logQueue_;
  std::mutex logQueueMutex_;
  std::condition_variable cv_;
  std::thread workerThread_;
};

class LoggerBuilder
{
  // The Builder class is used to create a Logger instance with a fluent interface.
  // It allows customizing various options when creating a Logger object.
public:
  LoggerBuilder();
  ~LoggerBuilder();
  LoggerBuilder &setName(const std::string &name);
  LoggerBuilder &setPrintToConsole(bool enabled);
  LoggerBuilder &setLogThreadIDs(bool enabled);
  LoggerBuilder &setLogSelfName(bool enabled);
  LoggerBuilder &setLogFilePath(const std::filesystem::path &logFilePath);
  LoggerBuilder &setMinimumLogLevel(LogLevel level);
  Logger build() const;

private:
  std::string name_;
  bool printToConsole_{false};
  bool logThreadIDs_{false};
  bool logSelfName_{false};
  std::filesystem::path logFilePath_;
  LogLevel minimumLogLevel_{LogLevel::DEBUG};
};

#endif // LOGGER_H
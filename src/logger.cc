#include <iostream>
#include "logger.h"

// Logger class implementation
Logger::Logger(
    const std::string &name,
    const std::filesystem::path &logFilePath,
    bool printToConsole,
    bool logThreadIDs,
    bool logSelfName,
    LogLevel minimumLogLevel)
    : name_(name),
      logFile_(logFilePath, std::ios::app),
      printToConsole_(printToConsole),
      logThreadIDs_(logThreadIDs),
      logSelfName_(logSelfName),
      minimumLogLevel_(minimumLogLevel)
{
  workerThread_ = std::thread(&Logger::processLogQueue_, this);
}

Logger::~Logger()
{
  {
    std::lock_guard<std::mutex> lock(logQueueMutex_);
    loggingDone_ = true;
  }
  cv_.notify_one();
  workerThread_.join();
}

const LogLevel &Logger::getMinimumLogLevel() const
{
  return minimumLogLevel_;
}

const std::string &Logger::getName() const
{
  return name_;
}

void Logger::processLogQueue_()
{
  while (true)
  {
    std::unique_lock<std::mutex> lock(logQueueMutex_);
    cv_.wait(lock, [this]
             { return !logQueue_.empty() || loggingDone_; });

    if (loggingDone_ && logQueue_.empty())
    {
      break;
    }

    // Retrieve the log line, then unlock since we don't need the lock while writing to the file.
    LogLine logLine = logQueue_.front();
    logQueue_.pop();

    lock.unlock();

    writeLine_(logLine);
  }
}

void Logger::writeLine_(const LogLine &logLine)
{
  std::string formattedLogMessage = getFormattedLogLine(logLine);
  if (printToConsole_)
  {
    std::cout << formattedLogMessage;
    return;
  }

  if (logFile_.is_open())
  {
    logFile_ << formattedLogMessage;
  }
  else
  {
    throw std::runtime_error("Log file is not open.");
  }
}

void Logger::debug(const std::string &message)
{
  log_(message, LogLevel::DEBUG);
}

void Logger::info(const std::string &message)
{
  log_(message, LogLevel::INFO);
}

void Logger::warning(const std::string &message)
{
  log_(message, LogLevel::WARNING);
}

void Logger::error(const std::string &message)
{
  log_(message, LogLevel::ERROR);
}

void Logger::critical(const std::string &message)
{
  log_(message, LogLevel::CRITICAL);
}

void Logger::log_(const std::string &message, LogLevel level)
{
  if (level < minimumLogLevel_)
  {
    return; // Skip if below min logging level
  }

  long long threadID = getThreadID();

  // Creating the timestamp
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");

  LogLine logLine{level, threadID, message, ss.str()};

  {
    std::lock_guard<std::mutex> lock(logQueueMutex_);
    logQueue_.push(logLine);
  }

  cv_.notify_one();
}

const std::string Logger::getFormattedLogLine(const LogLine &logLine) const
{
  std::stringstream ss;
  ss << logLine.timestamp;
  logThreadIDs_ ? ss << " [thread: " << logLine.threadID << "]" : ss;
  logSelfName_ ? ss << " [<" + getName() + "> " + logLevelToString(logLine.level) + "] - "
               : ss << " [" + logLevelToString(logLine.level) + "] - ";
  ss << logLine.message;
  ss << "\n";
  return ss.str();
}

// LoggerBuilder implementation
LoggerBuilder::LoggerBuilder() {};
LoggerBuilder::~LoggerBuilder() {};

/** Set the name of the logger. */
LoggerBuilder &LoggerBuilder::setName(const std::string &name)
{
  name_ = name;
  return *this;
}

/**
 * Set whether to print log messages to the console.
 * @param enabled True to enable console output, false by default.
 */
LoggerBuilder &LoggerBuilder::setPrintToConsole(bool enabled)
{
  printToConsole_ = enabled;
  return *this;
}

/**
 * Set whether to log thread IDs in each log line.
 * @param enabled True to enable thread ID logging, false by default.
 */
LoggerBuilder &LoggerBuilder::setLogThreadIDs(bool enabled)
{
  logThreadIDs_ = enabled;
  return *this;
}

/**
 * Set whether to log the logger's name in each log line.
 * @param enabled True to enable logger name logging, false by default.
 */
LoggerBuilder &LoggerBuilder::setLogSelfName(bool enabled)
{
  logSelfName_ = enabled;
  return *this;
}

/**
 * Set the file path for the log file.
 * @param logFilePath The path to the log file.
 */
LoggerBuilder &LoggerBuilder::setLogFilePath(const std::filesystem::path &logFilePath)
{
  logFilePath_ = logFilePath;
  return *this;
}

/**
 * Set the minimum log level for the logger.
 * @param level The minimum log level to log messages. Default is LogLevel::DEBUG.
 */
LoggerBuilder &LoggerBuilder::setMinimumLogLevel(LogLevel level)
{
  minimumLogLevel_ = level;
  return *this;
}

Logger LoggerBuilder::build() const
{
  if (name_.empty())
  {
    throw std::runtime_error("Logger name cannot be empty.");
  }
  if (logFilePath_.empty() && !printToConsole_)
  {
    throw std::runtime_error("Log file path cannot be empty.");
  }

  return Logger(name_, logFilePath_, printToConsole_, logThreadIDs_, logSelfName_, minimumLogLevel_);
}

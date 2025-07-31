#include <iostream>
#include "logger.h"

// Logger class implementation
Logger::Logger(const std::string &name, const std::filesystem::path &logFilePath, bool printToConsole, LogLevel minimumLogLevel)
    : name_(name), logFile_(logFilePath, std::ios::app), printToConsole_(printToConsole), minimumLogLevel_(minimumLogLevel)
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
  if (printToConsole_)
  {
    std::cout << logLine.timestamp
              << " [thread: " << logLine.threadID << "]"
              << " [<" << getName() << "> " << logLevelToString(logLine.level) << "] - "
              << logLine.message << "\n";
    return;
  }

  if (logFile_.is_open())
  {
    logFile_ << logLine.timestamp
             << " [thread: " << logLine.threadID << "]"
             << " [<" << getName() << "> " << logLevelToString(logLine.level) << "] - "
             << logLine.message << "\n";
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

// LoggerBuilder implementation
LoggerBuilder::LoggerBuilder() {};
LoggerBuilder::~LoggerBuilder() {};

LoggerBuilder &LoggerBuilder::setName(const std::string &name)
{
  name_ = name;
  return *this;
}

LoggerBuilder &LoggerBuilder::setPrintToConsole(bool enabled)
{
  printToConsole_ = enabled;
  return *this;
}

LoggerBuilder &LoggerBuilder::setLogFilePath(const std::filesystem::path &logFilePath)
{
  logFilePath_ = logFilePath;
  return *this;
}

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

  return Logger(name_, logFilePath_, printToConsole_, minimumLogLevel_);
}

#include "Logger.h"

std::string logLevelToString(LogLevel level)
{
  switch (level)
  {
  case LogLevel::DEBUG:
    return "DEBUG";
  case LogLevel::INFO:
    return "INFO";
  case LogLevel::WARNING:
    return "WARNING";
  case LogLevel::ERROR:
    return "ERROR";
  case LogLevel::CRITICAL:
    return "CRITICAL";
  default:
    return "UNKNOWN";
  }
}

const long long getThreadID()
{
  std::stringstream ss;
  ss << std::this_thread::get_id();
  return std::stoll(ss.str());
}

Logger::Logger(const std::string &name, const std::filesystem::path &logFilePath)
    : name_(name), logFile_(logFilePath, std::ios::app)
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
  if (logFile_.is_open())
  {
    logFile_ << logLine.timestamp
             << " [ thread: " << logLine.threadID << "]"
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

// TODO - Check if the log level is below the minimum log level before logging
void Logger::log_(const std::string &message, LogLevel level)
{
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

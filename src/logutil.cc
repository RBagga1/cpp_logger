#include <sstream>
#include <thread>
#include "logutil.h"

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
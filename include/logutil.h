#ifndef LOGUTIL_H
#define LOGUTIL_H

#include <string>

enum class LogLevel
{
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  CRITICAL
};

std::string logLevelToString(LogLevel level);
const long long getThreadID();

#endif // LOGUTIL_H
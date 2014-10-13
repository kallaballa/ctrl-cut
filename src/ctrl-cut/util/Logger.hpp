/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <assert.h>
#include "tri_logger/tri_logger.hpp"

using std::string;

enum LogLevel {
  CC_UNINITIALiZED = -1,
  CC_FATAL = 0,
  CC_ERROR = 1,
  CC_WARNING = 2,
  CC_INFO = 3,
  CC_DEBUG = 4,
  CC_ULTRA = 5
};

class Logger {
public:
  LogLevel level;

  static void init(const LogLevel l);
  static Logger& getInstance();
  static LogLevel getLevel();
private:
  Logger(const LogLevel l) : level(l) {}
  static Logger* instance;
};

#define LOG_ULTRA_STR(x) if(Logger::getLevel()>= CC_ULTRA) TRI_LOG_LEVEL_STR("ULTRA", str)
#define LOG_DEBUG_STR(str) if(Logger::getLevel()>= CC_DEBUG) TRI_LOG_LEVEL_STR("DEBUG", str)
#define LOG_INFO_STR(str) if(Logger::getLevel()>= CC_INFO) TRI_LOG_LEVEL_STR("INFO", str)
#define LOG_WARN_STR(str) if(Logger::getLevel()>= CC_WARNING) TRI_LOG_LEVEL_STR("WARN", str)
#define LOG_ERR_STR(str) if(Logger::getLevel()>= CC_ERROR) TRI_LOG_LEVEL_STR("ERROR", str)
#define LOG_FATAL_STR(str) if(Logger::getLevel()>= CC_FATAL) TRI_LOG_LEVEL_STR("FATAL", str)
#define LOG_ULTRA_MSG(msg,x) if(Logger::getLevel()>= CC_ULTRA) TRI_LOG_LEVEL_MSG("ULTRA", msg, x)
#define LOG_DEBUG_MSG(msg,x) if(Logger::getLevel()>= CC_DEBUG) TRI_LOG_LEVEL_MSG("DEBUG", msg, x)
#define LOG_INFO_MSG(msg,x) if(Logger::getLevel()>= CC_INFO) TRI_LOG_LEVEL_MSG("INFO", msg, x)
#define LOG_WARN_MSG(msg,x) if(Logger::getLevel()>= CC_WARNING) TRI_LOG_LEVEL_MSG("WARN", msg, x)
#define LOG_ERR_MSG(msg,x) if(Logger::getLevel()>= CC_ERROR) TRI_LOG_LEVEL_MSG("ERROR", msg, x)
#define LOG_FATAL_MSG(msg,x) if(Logger::getLevel()>= CC_FATAL) TRI_LOG_LEVEL_MSG("FATAL", msg, x)
#define LOG_ULTRA(x) if(Logger::getLevel()>= CC_ULTRA) TRI_LOG_LEVEL("ULTRA", x)
#define LOG_DEBUG(x) if(Logger::getLevel()>= CC_DEBUG) TRI_LOG_LEVEL("DEBUG", x)
#define LOG_INFO(x) if(Logger::getLevel()>= CC_INFO) TRI_LOG_LEVEL("INFO", x)
#define LOG_WARN(x) if(Logger::getLevel()>= CC_WARNING) TRI_LOG_LEVEL("WARN", x)
#define LOG_ERR(x) if(Logger::getLevel()>= CC_ERROR) TRI_LOG_LEVEL("ERROR", x)
#define LOG_FATAL(x) if(Logger::getLevel()>= CC_FATAL) TRI_LOG_LEVEL("FATAL", x)

#endif /* LOGGER_H_ */

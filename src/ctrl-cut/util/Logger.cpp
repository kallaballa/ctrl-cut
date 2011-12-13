/*
 * Logger.cpp
 *
 *  Created on: Dec 13, 2011
 *      Author: elchaschab
 */

#include "Logger.h"

Logger* Logger::instance = NULL;

void Logger::init(const LogLevel l) {
  Logger::instance = new Logger(l);
}

Logger& Logger::getInstance() {
  assert(Logger::instance != NULL);
  if (Logger::instance != NULL)
    return *Logger::instance;

  return *new Logger(CC_UNINITIALiZED);
}

LogLevel Logger::getLevel() {
  return Logger::getInstance().level;
}

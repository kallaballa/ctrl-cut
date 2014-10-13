/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include "Logger.hpp"

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

/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "tri_logger/tri_logger.hpp"

enum LogLevel {
  CC_FATAL = 0,
  CC_ERROR = 1,
  CC_WARNING = 2,
  CC_INFO = 3,
  CC_DEBUG = 4,
  CC_ULTRA = 5
};

extern LogLevel cc_loglevel;

#define LOG_ULTRA_STR(x) if(cc_loglevel>= CC_ULTRA) TRI_LOG_LEVEL_STR("ULTRA", str)
#define LOG_DEBUG_STR(str) if(cc_loglevel>= CC_DEBUG) TRI_LOG_LEVEL_STR("DEBUG", str)
#define LOG_INFO_STR(str) if(cc_loglevel>= CC_INFO) TRI_LOG_LEVEL_STR("INFO", str)
#define LOG_WARN_STR(str) if(cc_loglevel>= CC_WARNING) TRI_LOG_LEVEL_STR("WARN", str)
#define LOG_ERR_STR(str) if(cc_loglevel>= CC_ERROR) TRI_LOG_LEVEL_STR("ERROR", str)
#define LOG_FATAL_STR(str) if(cc_loglevel>= CC_FATAL) TRI_LOG_LEVEL_STR("FATAL", str)

#define LOG_ULTRA_MSG(msg,x) if(cc_loglevel>= CC_ULTRA) TRI_LOG_LEVEL_MSG("ULTRA", msg, x)
#define LOG_DEBUG_MSG(msg,x) if(cc_loglevel>= CC_DEBUG) TRI_LOG_LEVEL_MSG("DEBUG", msg, x)
#define LOG_INFO_MSG(msg,x) if(cc_loglevel>= CC_INFO) TRI_LOG_LEVEL_MSG("INFO", msg, x)
#define LOG_WARN_MSG(msg,x) if(cc_loglevel>= CC_WARNING) TRI_LOG_LEVEL_MSG("WARN", msg, x)
#define LOG_ERR_MSG(msg,x) if(cc_loglevel>= CC_ERROR) TRI_LOG_LEVEL_MSG("ERROR", msg, x)
#define LOG_FATAL_MSG(msg,x) if(cc_loglevel>= CC_FATAL) TRI_LOG_LEVEL_MSG("FATAL", msg, x)

#define LOG_ULTRA(x) if(cc_loglevel>= CC_ULTRA) TRI_LOG_LEVEL("ULTRA", x)
#define LOG_DEBUG(x) if(cc_loglevel>= CC_DEBUG) TRI_LOG_LEVEL("DEBUG", x)
#define LOG_INFO(x) if(cc_loglevel>= CC_INFO) TRI_LOG_LEVEL("INFO", x)
#define LOG_WARN(x) if(cc_loglevel>= CC_WARNING) TRI_LOG_LEVEL("WARN", x)
#define LOG_ERR(x) if(cc_loglevel>= CC_ERROR) TRI_LOG_LEVEL("ERROR", x)
#define LOG_FATAL(x) if(cc_loglevel>= CC_FATAL) TRI_LOG_LEVEL("FATAL", x)

#endif /* LOGGER_H_ */

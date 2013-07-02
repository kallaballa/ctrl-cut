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

#ifndef CTRLCUTEXCEPTION_H_
#define CTRLCUTEXCEPTION_H_

#include "boost/filesystem.hpp"
#include "boost/exception/all.hpp"
#include "boost/system/error_code.hpp"

namespace bfs = boost::filesystem3;
namespace berrc = boost::system::errc;

class CtrlCutException {
  class setting_not_found : public std::exception
  {
  public:
    const string keyid;
    setting_not_found(const string keyid) : keyid(keyid) {}
    virtual ~setting_not_found() throw () {}

  public:
      virtual const char * what() const throw() {
          return keyid.c_str();
      }
  };

  class general_error : public std::exception
  {
  public:
    const string msg;
    general_error(const string msg) : msg(msg) {}
    virtual ~general_error() throw () {}

  public:
      virtual const char * what() const throw() {
          return msg.c_str();
      }
  };

  class malformed_document_error: public std::exception {
  public:
    const string msg;
    malformed_document_error(const string& msg) :
        msg(msg) {
    }
    virtual ~malformed_document_error() throw () {
    }

  public:
    virtual const char * what() const throw () {
      return msg.c_str();
    }
  };

public:
  static void fileNotFoundException(const string& path) {
    boost::throw_exception(bfs::filesystem_error(path,berrc::make_error_code(berrc::no_such_file_or_directory)));
  }

  static void generalError(const string& message) {
    boost::throw_exception(general_error(message));
  }

  static void malformedDocument(const string& message) {
    boost::throw_exception(malformed_document_error(message));
  }
};

#endif /* CTRLCUTEXCEPTION_H_ */

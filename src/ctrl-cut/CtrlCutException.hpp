/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef CTRLCUTEXCEPTION_H_
#define CTRLCUTEXCEPTION_H_

#include "boost/filesystem.hpp"
#include "boost/exception/all.hpp"
#include "boost/system/error_code.hpp"

namespace bfs = boost::filesystem;
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

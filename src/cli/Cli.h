/*
 * Cli.h
 *
 *  Created on: Dec 6, 2011
 *      Author: elchaschab
 */

#ifndef CLI_H_
#define CLI_H_

#include <typeinfo>
#include <map>

class Cli {
public:
  Cli();
  virtual ~Cli();
};
/*
 * factory
 * transformator
 *
 *
 */
class Lookup {
public:
  Lookup();
  virtual ~Lookup();

  void registerContainer(std::type_info typeinfo) {
    containers.insert(std::make_pair(typeinfo.name(), typeinfo));
  }
  std::function<std::string(std::string&)>;
  void registerFunctions(std::function<) {
    typeinfo.
    functions.insert(std::make_pair(typeinfo.name(), typeinfo));
  }
private:
  std::map<const char *, std::type_info> containers;
  std::map<const char *, std::type_info> functions;
};

#endif /* CLI_H_ */

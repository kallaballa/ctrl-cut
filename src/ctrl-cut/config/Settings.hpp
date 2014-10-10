#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <util/Logger.hpp>

#include <stdint.h>
#include <iostream>
#include <string>

#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <functional>

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include "util/Measurement.hpp"
#include "cut/geom/Geometry.hpp"


using std::string;

typedef uint64_t CtrlCutID_t;
class Settings
{
public:
  class setting_not_found : public std::exception
  {
  public:
    const string keyid;
    setting_not_found(const string keyid) : keyid(keyid) {}
    virtual ~setting_not_found() throw () {}

  public:
      virtual const char * what() const throw() {
          return ("setting_not_found: " + keyid).c_str();
      }
  };

  struct KeyBase {
    string name;

    KeyBase():
      name("")
    {};

    KeyBase(const char* name):
      name(name)
    {};

    KeyBase(const string& name):
       name(name)
     {};

    KeyBase(const KeyBase& other):
       name(other.name)
     {};

    bool operator<(const KeyBase& other) const{
      return this->name < other.name;
    }

    void operator==(const KeyBase& other){
      this->name = other.name;
    }

    operator std::string() const{
      return this->name;
    }
  };

  template<typename T>
  class Key : public KeyBase {
  public:
    Key(const char* name): KeyBase(name) {};
    Key(const string& name): KeyBase(name) {};
  };

  class Trigger {
  private:
    std::function<void (const KeyBase&)> func;
  public:
    Trigger()  {}
    Trigger(std::function<void (const KeyBase&)> func) : func(func) {}
    Trigger(const Trigger& other) : func(other.func) {}

    void operator()(KeyBase key) {
      if(func)
        func(key);
    }
  };

  typedef boost::any Value;
  typedef std::map<KeyBase,Value>  SettingsMap;
  typedef SettingsMap::iterator iterator;
  typedef SettingsMap::const_iterator const_iterator;
  typedef SettingsMap::reference reference;
  typedef SettingsMap::const_reference const_reference;

  Settings(Settings& parent) : parent(&parent) {};
  Settings(const Settings& other) : parent(NULL) {
    this->properties = other.properties;
  };

  Settings() : parent(NULL), updateTrigger(NULL)  {};


  string key(const_iterator it) const {
    return (*it).first;
  }

  string value(const_iterator it) const {
    boost::any prop = (*it).second;

    if(prop.type() == typeid(Distance)) {
      return boost::lexical_cast<string>(boost::any_cast<Distance>(prop));
    } else if(prop.type() == typeid(Point)) {
      return boost::lexical_cast<string>(boost::any_cast<Point>(prop));
    } else if(prop.type() == typeid(string)){
      return boost::lexical_cast<string>(boost::any_cast<string>(prop));
    } else if(prop.type() == typeid(float)){
      return boost::lexical_cast<string>(boost::any_cast<float>(prop));
    } else if(prop.type() == typeid(bool)){
      return boost::lexical_cast<string>(boost::any_cast<bool>(prop));
    } else if(prop.type() == typeid(uint16_t)){
      return boost::lexical_cast<string>(boost::any_cast<uint16_t>(prop));
    } else if(prop.type() == typeid(uint32_t)){
      return boost::lexical_cast<string>(boost::any_cast<uint32_t>(prop));
    }
    return "unknown type: " + (*it).first.name;
  }

  string json(const_iterator it) const {
    std::stringstream ss;
    ss << "\"" << (*it).first.name << "\":";
    boost::any prop = (*it).second;

    if(prop.type() == typeid(Distance)) {
      ss << boost::any_cast<Distance>(prop).in(PX);
    } else if(prop.type() == typeid(Point)) {
      boost::any_cast<Point>(prop).toJson(ss);
    } else if(prop.type() == typeid(string)){
      ss << "\"" << boost::any_cast<string>(prop) << "\"";
    } else if(prop.type() == typeid(float)){
      ss << "\"" << boost::any_cast<float>(prop) << "\"";
    } else if(prop.type() == typeid(bool)){
      ss << "\"" << boost::any_cast<bool>(prop) << "\"";
    } else if(prop.type() == typeid(uint16_t)){
      ss << "\"" << boost::any_cast<uint16_t>(prop) << "\"";
    } else if(prop.type() == typeid(uint32_t)){
      ss << "\"" << boost::any_cast<uint32_t>(prop) << "\"";
    } else {
      ss << "\"\"";
    }

    return ss.str();
  }

  void set(iterator it, const string& val) {
    KeyBase key = (*it).first;
    boost::any prop = (*it).second;
    if(prop.type() == typeid(Distance)) {
      this->put(Key<Distance>(key.name), boost::lexical_cast<Distance>(val));
    } else if(prop.type() == typeid(Point)) {
      this->put(Key<Point>(key.name), boost::lexical_cast<Point>(val));
    } else if(prop.type() == typeid(string)){
      this->put(Key<string>(key.name), val);
    } else if(prop.type() == typeid(float)){
      this->put(Key<float>(key.name), boost::lexical_cast<float>(val));
    } else if(prop.type() == typeid(bool)){
      this->put(Key<bool>(key.name), boost::lexical_cast<bool>(val));
    } else if(prop.type() == typeid(uint16_t)){
      this->put(Key<uint16_t>(key.name), boost::lexical_cast<uint16_t>(val));
    } else if(prop.type() == typeid(uint32_t)){
      this->put(Key<uint32_t>(key.name), boost::lexical_cast<uint32_t>(val));
    }
    std::cerr << "unkown type for: " << key.name << std::endl;
  }


  string key(int i) const {
    const_iterator it;
    for(it = properties.begin(); i > 0; --i) {
      it++;
    }
    return this->key(it);
  }

  string value(int i) const {
    const_iterator it;
    for(it = properties.begin(); i > 0; --i) {
      it++;
    }
    return this->value(it);
  }

  void set(int i, const string& val) {
    iterator it;
    for(it = properties.begin(); i > 0; --i) {
      it++;
    }
    set(it,val);
  }

  string value(const KeyBase& key) const {
    const_iterator it = this->properties.find(key);
    if (it != this->properties.end()) {
      return this->value(it);
    }

    boost::throw_exception(setting_not_found(key));
  }

  void clear() { return this->properties.clear(); }
  size_t size() const { return this->properties.size(); }

  template<typename T, typename V>
  void put(const Settings::Key<T>& key, V value) {
    properties[key] = boost::any(static_cast<T>(value));
    this->updateTrigger(key);
  }

  template<typename T>
  const T get(const Settings::Key<T>& key) const {
    const_iterator it = this->properties.find(key);
    if (it != this->properties.end()) {
      return boost::any_cast<T>((*it).second);
    } else if(parent != NULL) {
      return parent->get(key);
    }

    boost::throw_exception(setting_not_found(key));
  }

  void setUpdateTrigger(std::function<void (const KeyBase&)> triggerFunc) {
    this->updateTrigger = Trigger(triggerFunc);
  }

  void deleteUpdateTrigger() {
    this->updateTrigger = Trigger();
  }

  bool hasParent() const {
    return this->parent != NULL;
  }

  Settings& getParent() const {
    assert(this->hasParent());
    return *this->parent;
  }

  void setParent(Settings* p) {
    this->parent = p;
  }

  void operator=(const Settings& other) {
    this->properties = other.properties;
  }

protected:
  Settings* parent;
  SettingsMap properties;
  Trigger updateTrigger;
};

#endif /* SETTINGS_H_ */

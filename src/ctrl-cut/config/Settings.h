#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <map>
#include <string>
#include <algorithm>
#include <boost/any.hpp>
#include "util/Logger.h"

using std::string;

class Settings
{
public:
  typedef const string Key;
  typedef boost::any Value;
  typedef std::map<Key, Value> SettingsMap;
  typedef SettingsMap::iterator iterator;
  typedef SettingsMap::const_iterator const_iterator;

  Settings(Settings& parent, bool setDefaults=true) : parent(&parent)  {
    if(setDefaults)
      resetToDefaults();
  };

  Settings(bool setDefaults=true) : parent(NULL)  {
    if(setDefaults)
      resetToDefaults();
  };

  ~Settings() {}

  iterator begin() { return this->settings.begin(); }
  const_iterator begin() const  { return this->settings.begin(); }
  iterator end() { return this->settings.end(); }
  const_iterator end() const  { return this->settings.end(); }
  size_t size() const { return this->settings.size(); }
  bool empty() const { return this->settings.empty(); }

  virtual Value& operator[](const string& key) {
    SettingsMap::iterator it=settings.find(key);

    if(it == settings.end()) {
      if(parent != NULL) {
        return (*parent)[key];
      } else {
        std::pair<SettingsMap::iterator, bool> insertee = settings.insert(std::make_pair(key, Value()));
        return (*(insertee.first)).second;
      }
    } else {
      return (*it).second;
    }
  }

  template<typename T>
  const T get(const string& key) {
    return boost::any_cast<T>((*this)[key]);
  }

  template<typename T>
  void clip(const Key& key, T lowerBound, T higherBound) {
    T value = this->get<T>(key);

    if(value < lowerBound) {
      LOG_WARN_MSG("Clipping setting" + key, lowerBound);
      (*this)[key] = lowerBound;
    } else if (value > higherBound) {
      LOG_WARN_MSG("Clipping setting" + key, higherBound);
      (*this)[key] = higherBound;
    }
  }

  virtual void resetToDefaults() {
    settings.clear();
  };

  virtual void rangeCheck() = 0;
  virtual void log();

private:
  SettingsMap settings;
  Settings* parent;
};

#endif /* SETTINGS_H_ */

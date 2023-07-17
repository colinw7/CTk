#ifndef CStrUniqueMatch_H
#define CStrUniqueMatch_H

#include <set>
#include <map>

template<typename T>
class CStrUniqueMatchValues {
 public:
  using NameValue = std::pair<std::string, T>;

 public:
  CStrUniqueMatchValues() { }

  CStrUniqueMatchValues(std::initializer_list<NameValue> values) {
    addValues(values);
  }

  void addValues(std::initializer_list<NameValue> values) {
    for (const auto &v : values)
      addValue(v);
  }

  void addValue(const NameValue &nameValue) {
    addValue(nameValue.first, nameValue.second);
  }

  void addValue(const std::string &str, const T &value) {
    // name must match unique value but value can have many names
    strValues_[str] = value;

    valueStrs_[value].push_back(str);
  }

  int numValues() const { return int(strValues_.size()); }

  bool match(const std::string &str, T &value) {
    std::set<T> partialValues;

    for (auto &v : strValues_) {
      if (v.first == str) {
        value = v.second;
        return true;
      }

      if (v.first.find(str) == 0)
        partialValues.insert(v.second);
    }

    if (partialValues.size() != 1)
      return false;

    value = *partialValues.begin();

    return true;
  }

  const std::string &lookup(const T &value, const std::string &no_str="<no_value>") {
    static std::string s_no_str;

    auto p = valueStrs_.find(value);

    if (p != valueStrs_.end()) {
      auto &strs = (*p).second;

      return longestString(strs);
    }
    else {
      s_no_str = no_str;

      return s_no_str;
    }
  }

  void values(std::vector<std::string> &values) {
    for (auto &v : strValues_)
      values.push_back(v.first);
  }

  static const std::string &longestString(const std::vector<std::string> &strs) {
    static std::string str;
    size_t             len = 0;

    str = "";

    for (const auto &s : strs) {
      if (s.size() > len) {
        len = s.size();
        str = s;
      }
    }

    return str;
  }

 private:
  typedef std::map<std::string,T>  StringValueMap;
  typedef std::vector<std::string> Strings;
  typedef std::map<T,Strings>      ValueStringsMap;

  StringValueMap  strValues_;
  ValueStringsMap valueStrs_;
};

class CStrUniqueMatchInds : public CStrUniqueMatchValues<int> {
 public:
  CStrUniqueMatchInds(const std::vector<std::string> &strs) {
    int i = 0;

    for (const auto &str : strs)
      addValue(str, i++);
  }
};

//---

namespace CStrUniqueMatch {
  template<typename T>
  inline void initNameValues(CStrUniqueMatchValues<T> &) { }

  template<typename T>
  inline CStrUniqueMatchValues<T> &getNameValues() {
    static CStrUniqueMatchValues<T> nameValues;

    if (! nameValues.numValues())
      initNameValues<T>(nameValues);

    return nameValues;
  }

  template<typename T>
  inline bool stringToValue(const std::string &str, T &value) {
    return getNameValues<T>().match(str, value);
  }

  template<typename T>
  inline bool stringToValueWithErr(const std::string &str, T &value, const std::string &err) {
    if (! stringToValue(str, value)) {
      std::cerr << "Invalid " << err << " '" << str << "'" << std::endl;
      return false;
    }
    return true;
  }

  template<typename T>
  inline std::string valueToString(const T &value) {
    return getNameValues<T>().lookup(value);
  }

  template<typename T>
  inline void values(std::vector<std::string> &strs) {
    getNameValues<T>().values(strs);
  }

  template<typename T>
  inline std::string valueStrings() {
    std::vector<std::string> strs;

    getNameValues<T>().values(strs);

    std::string cstr;

    for (const auto &str : strs) {
      if (! cstr.empty()) cstr += ", ";

      cstr += "'" + str + "'";
    }

    return cstr;
  }
}

#endif

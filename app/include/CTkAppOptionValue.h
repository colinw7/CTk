#ifndef CTkAppOptionValue_H
#define CTkAppOptionValue_H

#include <string>
#include <map>

//---

enum class CTkOptionType {
  None,
  Flag,
  String,
  Int,
  Real
};

//---

struct CTkOptionValue {
  long        i { 0 };
  double      r { 0.0 };
  std::string s;
};

using CTkOptionValueMap = std::map<std::string, CTkOptionValue>;

//---

struct CTkOption {
  const char*   name  { nullptr };
  CTkOptionType type  { CTkOptionType::None };
  const char*   alias { nullptr };
};

//---

struct CTkOpt {
  const char* name  { nullptr }; // option name
  const char* dname { nullptr }; // database name
  const char* cname { nullptr }; // class name
  const char* def   { nullptr }; // default value
};

#endif

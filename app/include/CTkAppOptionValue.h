#ifndef CTkAppOptionValue_H
#define CTkAppOptionValue_H

#include <string>
#include <map>

//---

enum class CTkAppOptionType {
  None,
  Flag,
  String,
  Int,
  Real
};

//---

struct CTkAppOptionValue {
  long        i { 0 };
  double      r { 0.0 };
  std::string s;
};

using CTkAppOptionValueMap = std::map<std::string, CTkAppOptionValue>;

//---

struct CTkAppOption {
  const char*      name  { nullptr };
  CTkAppOptionType type  { CTkAppOptionType::None };
  const char*      alias { nullptr };
};

//---

struct CTkAppOpt {
  const char* name  { nullptr }; // option name
  const char* dname { nullptr }; // database name
  const char* cname { nullptr }; // class name
  const char* def   { nullptr }; // default value
};

#endif

#ifndef CTkOptData_H
#define CTkOptData_H

#include <CTkAppOptionValue.h>

#include <tcl.h>

class CTkApp;
class CTkOpt;

//---

class CTkOptData {
 public:
  CTkOptData(CTkApp *tk);

  void init(const CTkOpt *opts) {
   opts_ = opts;
  }

  Tcl_Obj *getOpts() const;
  Tcl_Obj *getOpt(const std::string &name) const;

  bool getOptValue(const std::string &name, std::string &value);

  bool setOptValue(const std::string &name, const std::string &value, const CTkOpt **opt);

 private:
  CTkApp*           tk_   { nullptr };
  const CTkOpt*     opts_ { nullptr };
  CTkOptionValueMap values_;
};

#endif

#ifndef CTkAppOptData_H
#define CTkAppOptData_H

#include <CTkAppOptionValue.h>

#include <tcl.h>

class CTkApp;
class CTkAppOpt;

//---

class CTkAppOptData {
 public:
  CTkAppOptData(CTkApp *tk);

  void init(const CTkAppOpt *opts) {
   opts_ = opts;
  }

  Tcl_Obj *getOpts() const;
  Tcl_Obj *getOpt(const std::string &name) const;

  bool getOptValue(const std::string &name, std::string &value);

  bool setOptValue(const std::string &name, const std::string &value, const CTkAppOpt **opt);

 private:
  CTkApp*              tk_   { nullptr };
  const CTkAppOpt*     opts_ { nullptr };
  CTkAppOptionValueMap values_;
};

#endif

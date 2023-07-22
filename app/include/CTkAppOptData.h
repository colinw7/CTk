#ifndef CTkAppOptData_H
#define CTkAppOptData_H

#include <CTkAppOptionValue.h>

#include <tcl.h>

#include <vector>

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

  void getNames(std::vector<std::string> &names) const;

  bool getOptValue(const std::string &name, std::string &value) const;

  bool getDefValue(const std::string &optName, const std::string &optClass,
                   std::string &value) const;

  bool setOptValue(const std::string &name, const std::string &value, const CTkAppOpt **opt);

 private:
  CTkApp*              tk_   { nullptr };
  const CTkAppOpt*     opts_ { nullptr };
  CTkAppOptionValueMap values_;
};

#endif

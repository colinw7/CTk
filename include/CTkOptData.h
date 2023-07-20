#ifndef CTkOptData_H
#define CTkOptData_H

#include <CTkOptionValue.h>
#include <CTcl.h>

class CTk;
class CTkOpt;

//---

class CTkOptData {
 public:
  CTkOptData(CTk *tk);

  void init(const CTkOpt *opts) {
   opts_ = opts;
  }

  CTclValueRef getOpts();

  bool getOptValue(const std::string &name, std::string &value);

  bool setOptValue(const std::string &name, const std::string &value, const CTkOpt **opt);

 private:
  CTk*              tk_   { nullptr };
  const CTkOpt*     opts_ { nullptr };
  CTkOptionValueMap values_;
};

#endif

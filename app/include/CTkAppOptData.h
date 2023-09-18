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
  explicit CTkAppOptData(CTkApp *tk);

  void init(const CTkAppOpt *opts) {
    opts_ = opts;
  }

  const CTkAppOpt *opts() { return opts_; }

  const CTkAppOpt *opt(const QString &name) const;

  QVariant getOptsVar() const;
  QVariant getOptVar(const QString &name) const;

  void getNames(std::vector<QString> &names, bool alias=true) const;

  bool getOptValue(const QString &name, QVariant &value) const;
  bool getOptValue(const CTkAppOpt *opt, QVariant &value) const;

  bool getDefValue(const QString &optName, const QString &optClass, QVariant &value) const;

  bool setOptValue(const QString &name, const QVariant &value, const CTkAppOpt **opt);

 private:
  CTkApp*              tk_   { nullptr };
  const CTkAppOpt*     opts_ { nullptr };
  CTkAppOptionValueMap values_;
};

#endif

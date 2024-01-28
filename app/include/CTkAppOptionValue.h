#ifndef CTkAppOptionValue_H
#define CTkAppOptionValue_H

#include <QVariant>

#include <string>
#include <map>

//---

enum class CTkAppOptionType {
  None,
  Flag,
  String,
  Int,
  Real,
  Boolean
};

//---

class CTkAppOptionValue {
 public:
  CTkAppOptionValue() { }

  bool getBool() const { return var_.toBool(); }
  void setBool(bool b) { var_ = QVariant(b); }

  long getInteger() const {
    if (var_.type() == QVariant::Int     ) return var_.value<int>();
    if (var_.type() == QVariant::LongLong) return var_.value<qlonglong>();
    if (var_.type() == QVariant::Double  ) return long(var_.value<double>());
    return var_.toString().toInt();
  }

  void setInteger(long i) { var_ = QVariant(static_cast<qlonglong>(i)); }

  double getReal() const {
    if (var_.type() == QVariant::Int     ) return double(var_.value<int>());
    if (var_.type() == QVariant::LongLong) return double(var_.value<qlonglong>());
    if (var_.type() == QVariant::Double  ) return var_.value<double>();
    return var_.toString().toDouble();
  }

  void setReal(double r) { var_ = QVariant(r); }

  QString getString() const { return var_.toString(); }

  const QVariant &getValue() const { return var_; }
  void setValue(const QVariant &var) { var_ = var; }

 private:
  QVariant var_;
};

using CTkAppOptionValueMap = std::map<QString, CTkAppOptionValue>;

//---

struct CTkAppOption {
  const char*      name  { nullptr };
  CTkAppOptionType type  { CTkAppOptionType::None };
  const char*      alias { nullptr };
};

//---

class CTkAppOpt {
 public:
  CTkAppOpt(const char* name, const char* dname, const char* cname,
            const char* def, const QVariant::Type &type=QVariant::Invalid) :
   name_(name), dname_(dname), cname_(cname), def_(def), type_(type) {
  }

  const char *name () const { return name_; }
  const char *dname() const { return dname_; }
  const char *cname() const { return cname_; }
  const char *def  () const { return def_; }

  const QVariant::Type &type  () const { return type_; }

  bool isAlias() const { return ! cname_; }

  bool isColor() const {
    if (! cname_) return false;

    auto len = strlen(cname_);
    if (len >= 10 && (strcmp(&cname_[len - 10], "Foreground") == 0 ||
                      strcmp(&cname_[len - 10], "Background") == 0))
      return true;

    return false;
  }

  bool isEOF() const {
    return (name_ == nullptr);
  }

  using AppOpt = CTkAppOpt;

  static AppOpt optBool(const char* name, const char* dname, const char* cname, bool def) {
    return AppOpt { name, dname, cname, (def ? "1" : "0"), QVariant::Bool };
  }
  static AppOpt optInt(const char* name, const char* dname, const char* cname, const char *def) {
    return AppOpt { name, dname, cname, def, QVariant::LongLong };
  }
  static AppOpt optReal(const char* name, const char* dname, const char* cname, const char *def) {
    return AppOpt { name, dname, cname, def, QVariant::Double };
  }
  static AppOpt optString(const char* name, const char* dname, const char* cname, const char *def) {
    return AppOpt { name, dname, cname, def, QVariant::String };
  }
  static AppOpt optColor(const char* name, const char* dname, const char* cname, const char *def) {
    return AppOpt { name, dname, cname, def, QVariant::Color };
  }
  static AppOpt optFont(const char* name, const char* dname, const char* cname, const char *def) {
    return AppOpt { name, dname, cname, def, QVariant::Font };
  }
  static AppOpt optCursor(const char* name, const char* dname, const char* cname, const char *def) {
    return AppOpt { name, dname, cname, def, QVariant::Cursor };
  }
  static AppOpt optSynonym(const char* name, const char* dname) {
    return AppOpt { name, dname, nullptr, nullptr, QVariant::Invalid };
  }
  static AppOpt optEnd() {
    return AppOpt { nullptr, nullptr, nullptr, nullptr, QVariant::Invalid };
  }

 private:
  const char*    name_  { nullptr }; // option name
  const char*    dname_ { nullptr }; // database name (or alias name)
  const char*    cname_ { nullptr }; // class name    (null if alias)
  const char*    def_   { nullptr }; // default value (null if alias)
  QVariant::Type type_  { QVariant::Invalid };
};

#endif

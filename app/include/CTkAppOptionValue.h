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

struct CTkAppOpt {
  bool isAlias() const { return ! cname; }

  bool isColor() const {
    if (! cname) return false;
    auto len = strlen(cname);
    if (len >= 10 && (strcmp(&cname[len - 10], "Foreground") == 0 ||
                      strcmp(&cname[len - 10], "Background") == 0))
      return true;
    return false;
  }

  const char* name  { nullptr }; // option name
  const char* dname { nullptr }; // database name (or alias name)
  const char* cname { nullptr }; // class name    (null if alias)
  const char* def   { nullptr }; // default value (null if alias)
};

#endif

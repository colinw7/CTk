#include <CTkAppOptData.h>
#include <CTkAppUtil.h>
#include <CTkApp.h>

#include <cstring>

CTkAppOptData::
CTkAppOptData(CTkApp *tk) :
 tk_(tk)
{
}

const CTkAppOpt *
CTkAppOptData::
opt(const QString &name) const
{
  for (uint i = 0; ! opts_[i].isEOF(); ++i) {
    const auto &opt = opts_[i];

    if (opt.name() == name)
      return &opt;
  }

  return nullptr;
}

QVariant
CTkAppOptData::
getOptsVar() const
{
  std::map<QString, QVariant> vars;

  for (uint i = 0; ! opts_[i].isEOF(); ++i) {
    const auto &opt = opts_[i];

    QVariantList vars1;

    vars1.push_back(opt.name());
    vars1.push_back(opt.dname());

    if (! opt.isAlias()) {
      vars1.push_back(opt.cname());
      vars1.push_back(opt.def());

      auto p = values_.find(opt.name());

      if (p != values_.end()) {
        auto s = (*p).second.getString();

        vars1.push_back(s);
      }
      else
        vars1.push_back(opt.def());
    }

    vars[opt.name()] = vars1;
  }

  QVariantList vars1;

  for (const auto &pv : vars)
    vars1.push_back(pv.second);

  return vars1;
}

bool
CTkAppOptData::
getOptVar(const QString &name, QVariant &var) const
{
  QString name1;
  if (! lookupName(name, name1))
    return false;

  QVariantList vars;

  for (uint i = 0; ! opts_[i].isEOF(); ++i) {
    const auto &opt = opts_[i];

    auto optName = QString(opt.name());
    if (optName != name1) continue;

    if (opt.isAlias())
      return getOptVar(opt.dname(), var);

    vars.push_back(opt.name());
    vars.push_back(opt.dname());

    vars.push_back(opt.cname());
    vars.push_back(opt.def());

    auto p = values_.find(optName);

    if (p != values_.end()) {
      auto s = (*p).second.getString();

      vars.push_back(s);
    }
    else
      vars.push_back(opt.def());

    var = QVariant(vars);

    return true;
  }

  return false;
}

void
CTkAppOptData::
getNames(std::vector<QString> &names, bool alias) const
{
  for (uint i = 0; ! opts_[i].isEOF(); ++i) {
    const auto &opt = opts_[i];

    if (! alias && opt.isAlias())
      continue;

    auto optName = QString(opt.name());

    names.push_back(optName);
  }
}

bool
CTkAppOptData::
getOptValue(const QString &name, QVariant &value) const
{
  QString name1;
  if (! lookupName(name, name1))
    return false;

  for (uint i = 0; ! opts_[i].isEOF(); ++i) {
    const auto &opt = opts_[i];

    auto optName = QString(opt.name());
    if (optName != name1) continue;

    if (opt.isAlias())
      return getOptValue(opt.dname(), value);

    auto p = values_.find(name1);

    if (p != values_.end())
      value = (*p).second.getValue();
    else
      value = QString(opt.def());

    return true;
  }

  return false;
}

bool
CTkAppOptData::
getOptValue(const CTkAppOpt *opt, QVariant &value) const
{
  auto optName = QString(opt->name());

  auto p = values_.find(opt->name());

  if (p != values_.end()) {
    value = (*p).second.getValue();
    return true;
  }

  return false;
}

bool
CTkAppOptData::
getDefValue(const QString &optName, const QString &optClass, QVariant &value) const
{
  for (uint i = 0; ! opts_[i].isEOF(); ++i) {
    const auto &opt = opts_[i];

    if (opt.dname() == optName.toStdString()) {
      value = QString(opt.def());
      return true;
    }
  }

  for (uint i = 0; ! opts_[i].isEOF(); ++i) {
    const auto &opt = opts_[i];

    if (! opt.isAlias() && opt.cname() == optClass.toStdString()) {
      value = QString(opt.def());
      return true;
    }
  }

  return false;
}

bool
CTkAppOptData::
setOptValue(const QString &name, const QVariant &value, const CTkAppOpt **opt)
{
  QString name1;
  if (! lookupName(name, name1))
    return false;

  // exact match
  for (uint i = 0; ! opts_[i].isEOF(); ++i) {
    const auto &opt1 = opts_[i];

    if (opt1.name() != name1) continue;

    if (opt1.isAlias())
      return setOptValue(opt1.dname(), value, opt);

    *opt = &opt1;

    auto pv = values_.find(name1);

    if (pv == values_.end())
      pv = values_.emplace_hint(values_.end(), name1, CTkAppOptionValue());

    if (value.type() == QVariant::String) {
      if      (opt1.type() == QVariant::Int) {
        long i;
        if (CTkAppUtil::stringToInt(value.toString(), i))
          (*pv).second.setInteger(i);
      }
      else if (opt1.type() == QVariant::Bool) {
        bool b;
        if (CTkAppUtil::stringToBool(value.toString(), b))
          (*pv).second.setBool(b);
      }
      else
        (*pv).second.setValue(value);
    }
    else
      (*pv).second.setValue(value);

    return true;
  }

  // abbrev
  *opt = nullptr;

  for (uint i = 0; ! opts_[i].isEOF(); ++i) {
    const auto &opt1 = opts_[i];

    auto pos = QString(opt1.name()).indexOf(name1);
    if (pos != 0) continue;

    if (opt)
      *opt = &opt1;
    else
      return false;
  }

  if (! *opt)
    return false;

  values_[name1].setValue(value);

  return true;
}

bool
CTkAppOptData::
lookupName(const QString &name, QString &name1) const
{
  if (names_.empty()) {
    for (uint i = 0; ! opts_[i].isEOF(); ++i) {
      const auto &opt = opts_[i];

      names_.push_back(opt.name());
    }
  }

  int nmatch;
  if (! CTkAppUtil::uniqueMatch(names_, name, name1, nmatch))
    return false;

  if (nmatch != 1)
    return false;

  return true;
}

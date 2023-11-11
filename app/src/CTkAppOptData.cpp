#include <CTkAppOptData.h>
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
  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt = opts_[i];

    if (opt.name == name)
      return &opt;
  }

  return nullptr;
}

QVariant
CTkAppOptData::
getOptsVar() const
{
  QVariantList vars;

  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt = opts_[i];

    QVariantList vars1;

    vars1.push_back(opt.name);
    vars1.push_back(opt.dname);

    if (! opt.isAlias()) {
      vars1.push_back(opt.cname);
      vars1.push_back(opt.def);

      auto p = values_.find(opt.name);

      if (p != values_.end()) {
        auto s = (*p).second.getString();

        vars1.push_back(s);
      }
      else
        vars1.push_back(opt.def);
    }

    vars.push_back(vars1);
  }

  return vars;
}

QVariant
CTkAppOptData::
getOptVar(const QString &name) const
{
  QVariantList vars;

  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt = opts_[i];

    auto optName = QString(opt.name);
    if (optName != name) continue;

    vars.push_back(opt.name);
    vars.push_back(opt.dname);

    if (! opt.isAlias()) {
      vars.push_back(opt.cname);
      vars.push_back(opt.def);

      auto p = values_.find(optName);

      if (p != values_.end()) {
        auto s = (*p).second.getString();

        vars.push_back(s);
      }
      else
        vars.push_back(opt.def);
    }
    else
      return getOptVar(opt.dname); 
  }

  return vars;
}


void
CTkAppOptData::
getNames(std::vector<QString> &names, bool alias) const
{
  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt = opts_[i];

    if (! alias && opt.isAlias())
      continue;

    auto optName = QString(opt.name);

    names.push_back(optName);
  }
}

bool
CTkAppOptData::
getOptValue(const QString &name, QVariant &value) const
{
  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt = opts_[i];

    auto optName = QString(opt.name);

    if (optName != name) continue;

    if (opt.isAlias())
      return getOptValue(opt.dname, value);

    auto p = values_.find(name);

    if (p != values_.end())
      value = (*p).second.getValue();
    else
      value = QString(opt.def);

    return true;
  }

  return false;
}

bool
CTkAppOptData::
getOptValue(const CTkAppOpt *opt, QVariant &value) const
{
  auto optName = QString(opt->name);

  auto p = values_.find(opt->name);

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
  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt = opts_[i];

    if (opt.dname == optName.toStdString()) {
      value = QString(opt.def);
      return true;
    }
  }

  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt = opts_[i];

    if (! opt.isAlias() && opt.cname == optClass.toStdString()) {
      value = QString(opt.def);
      return true;
    }
  }

  return false;
}

bool
CTkAppOptData::
setOptValue(const QString &name, const QVariant &value, const CTkAppOpt **opt)
{
  // exact match
  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt1 = opts_[i];

    if (opt1.name != name) continue;

    if (opt1.isAlias())
      return setOptValue(opt1.dname, value, opt);

    *opt = &opt1;

    values_[name].setValue(value);

    return true;
  }

  // abbrev
  *opt = nullptr;

  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt1 = opts_[i];

    auto pos = QString(opt1.name).indexOf(name);
    if (pos != 0) continue;

    if (opt)
      *opt = &opt1;
    else
      return false;
  }

  if (! *opt)
    return false;

  values_[name].setValue(value);

  return true;
}

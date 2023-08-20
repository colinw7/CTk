#include <CTkAppOptData.h>
#include <CTkApp.h>

#include <cstring>

CTkAppOptData::
CTkAppOptData(CTkApp *tk) :
 tk_(tk)
{
}

Tcl_Obj *
CTkAppOptData::
getOpts() const
{
  auto *list = Tcl_NewListObj(0, nullptr);

  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt = opts_[i];

    auto *list1 = Tcl_NewListObj(0, nullptr);

    auto *sobj1 = Tcl_NewStringObj(opt.name , strlen(opt.name ));
    auto *sobj2 = Tcl_NewStringObj(opt.dname, strlen(opt.dname));

    Tcl_ListObjAppendElement(tk_->getInterp(), list1, sobj1);
    Tcl_ListObjAppendElement(tk_->getInterp(), list1, sobj2);

    if (opt.cname != nullptr) {
      auto *sobj3 = Tcl_NewStringObj(opt.cname, strlen(opt.cname));
      auto *sobj4 = Tcl_NewStringObj(opt.def  , strlen(opt.def  ));

      Tcl_ListObjAppendElement(tk_->getInterp(), list1, sobj3);
      Tcl_ListObjAppendElement(tk_->getInterp(), list1, sobj4);

      auto p = values_.find(opt.name);

      if (p != values_.end()) {
        auto s = (*p).second.getString();

        auto *sobj5 = Tcl_NewStringObj(s.toLatin1().constData(), -1);

        Tcl_ListObjAppendElement(tk_->getInterp(), list1, sobj5);
      }
      else {
        auto *sobj5 = Tcl_NewStringObj(opt.def, strlen(opt.def));

        Tcl_ListObjAppendElement(tk_->getInterp(), list1, sobj5);
      }
    }

    Tcl_ListObjAppendElement(tk_->getInterp(), list, list1);
  }

  return list;
}

Tcl_Obj *
CTkAppOptData::
getOpt(const QString &name) const
{
  auto *list = Tcl_NewListObj(0, nullptr);

  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt = opts_[i];

    auto optName = QString(opt.name);

    if (optName != name) continue;

    auto *sobj1 = Tcl_NewStringObj(opt.name , strlen(opt.name ));
    auto *sobj2 = Tcl_NewStringObj(opt.dname, strlen(opt.dname));

    Tcl_ListObjAppendElement(tk_->getInterp(), list, sobj1);
    Tcl_ListObjAppendElement(tk_->getInterp(), list, sobj2);

    if (opt.cname != nullptr) {
      auto *sobj3 = Tcl_NewStringObj(opt.cname, strlen(opt.cname));
      auto *sobj4 = Tcl_NewStringObj(opt.def  , strlen(opt.def  ));

      Tcl_ListObjAppendElement(tk_->getInterp(), list, sobj3);
      Tcl_ListObjAppendElement(tk_->getInterp(), list, sobj4);

      auto p = values_.find(optName);

      if (p != values_.end()) {
        auto s = (*p).second.getString();

        auto *sobj5 = Tcl_NewStringObj(s.toLatin1().constData(), -1);

        Tcl_ListObjAppendElement(tk_->getInterp(), list, sobj5);
      }
      else {
        auto *sobj5 = Tcl_NewStringObj(opt.def, strlen(opt.def));

        Tcl_ListObjAppendElement(tk_->getInterp(), list, sobj5);
      }
    }

    break;
  }

  return list;
}

void
CTkAppOptData::
getNames(std::vector<QString> &names) const
{
  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt = opts_[i];

    auto optName = QString(opt.name);

    names.push_back(optName);
  }
}

bool
CTkAppOptData::
getOptValue(const QString &name, QString &value) const
{
  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt = opts_[i];

    auto optName = QString(opt.name);

    if (optName != name) continue;

    if (opt.cname == nullptr)
      return getOptValue(opt.dname, value);

    auto p = values_.find(name);

    if (p != values_.end()) {
      auto s = (*p).second.getString();

      value = s;
    }
    else
      value = QString(opt.def);

    return true;
  }

  return false;
}

bool
CTkAppOptData::
getDefValue(const QString &optName, const QString &optClass, QString &value) const
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

    if (opt.cname && opt.cname == optClass.toStdString()) {
      value = QString(opt.def);
      return true;
    }
  }

  return false;
}

bool
CTkAppOptData::
setOptValue(const QString &name, const QString &value, const CTkAppOpt **opt)
{
  // exact match
  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt1 = opts_[i];

    if (opt1.name != name) continue;

    if (opt1.cname == nullptr)
      return setOptValue(opt1.dname, value, opt);

    *opt = &opt1;

    values_[name].setString(value);

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

  values_[name].setString(value);

  return true;
}

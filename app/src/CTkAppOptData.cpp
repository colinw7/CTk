#include <CTkAppOptData.h>
#include <CTkApp.h>

#include <cstring>

CTkOptData::
CTkOptData(CTkApp *tk) :
 tk_(tk)
{
}

Tcl_Obj *
CTkOptData::
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
        auto *sobj5 = Tcl_NewStringObj((*p).second.s.c_str(), int((*p).second.s.size()));

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
CTkOptData::
getOpt(const std::string &name) const
{
  auto *list = Tcl_NewListObj(0, nullptr);

  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt = opts_[i];
    if (opt.name != name) continue;

    auto *sobj1 = Tcl_NewStringObj(opt.name , strlen(opt.name ));
    auto *sobj2 = Tcl_NewStringObj(opt.dname, strlen(opt.dname));

    Tcl_ListObjAppendElement(tk_->getInterp(), list, sobj1);
    Tcl_ListObjAppendElement(tk_->getInterp(), list, sobj2);

    if (opt.cname != nullptr) {
      auto *sobj3 = Tcl_NewStringObj(opt.cname, strlen(opt.cname));
      auto *sobj4 = Tcl_NewStringObj(opt.def  , strlen(opt.def  ));

      Tcl_ListObjAppendElement(tk_->getInterp(), list, sobj3);
      Tcl_ListObjAppendElement(tk_->getInterp(), list, sobj4);

      auto p = values_.find(opt.name);

      if (p != values_.end()) {
        auto *sobj5 = Tcl_NewStringObj((*p).second.s.c_str(), int((*p).second.s.size()));

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

bool
CTkOptData::
getOptValue(const std::string &name, std::string &value)
{
  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt = opts_[i];

    if (opt.name != name) continue;

    if (opt.cname == nullptr)
      return getOptValue(opt.dname, value);

    auto p = values_.find(name);

    if (p != values_.end())
      value = (*p).second.s;
    else
      value = opts_[i].def;

    return true;
  }

  return false;
}

bool
CTkOptData::
setOptValue(const std::string &name, const std::string &value, const CTkOpt **opt)
{
  // exact match
  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt1 = opts_[i];

    if (opt1.name != name) continue;

    if (opt1.cname == nullptr)
      return setOptValue(opt1.dname, value, opt);

    *opt = &opt1;

    values_[name].s = value;

    return true;
  }

  // abbrev
  *opt = nullptr;

  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt1 = opts_[i];

    auto pos = std::string(opt1.name).find(name);

    if (pos != 0) continue;

    if (opt)
      *opt = &opt1;
    else
      return false;
  }

  if (! *opt)
    return false;

  values_[name].s = value;

  return true;
}

#include <CTkOptData.h>
#include <CTk.h>

CTkOptData::
CTkOptData(CTk *tk) :
 tk_(tk)
{
}

CTclValueRef
CTkOptData::
getOpts()
{
  CTcl *tcl = tk_->getTcl();

  auto *list = new CTclList;

  for (uint i = 0; opts_[i].name != nullptr; ++i) {
    const auto &opt = opts_[i];

    auto *list1 = new CTclList;

    list1->addValue(tcl->createValue(opt.name ));
    list1->addValue(tcl->createValue(opt.dname));

    if (opt.cname != nullptr) {
      list1->addValue(tcl->createValue(opt.cname));
      list1->addValue(tcl->createValue(opt.def  ));

      auto p = values_.find(opt.name);

      if (p != values_.end())
        list1->addValue(tcl->createValue((*p).second.s));
      else
        list1->addValue(tcl->createValue(opt.def));
    }

    list->addValue(CTclValueRef(list1));
  }

  return CTclValueRef(list);
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

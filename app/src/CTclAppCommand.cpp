#include <CTclAppCommand.h>
#include <CTclApp.h>

#include <CStrUtil.h>

#include <tcl.h>

CTclAppCommand::
CTclAppCommand(CTclApp *app, const std::string &name) :
 app_(app), name_(name)
{
  Tcl_CreateCommand(app_->getInterp(), name_.c_str(),
                    static_cast<Tcl_CmdProc *>(&CTclAppCommand::commandProc),
                    static_cast<ClientData>(this), nullptr);
}

CTclAppCommand::
~CTclAppCommand()
{
  Tcl_DeleteCommand(app_->getInterp(), name_.c_str());
}

int
CTclAppCommand::
commandProc(ClientData clientData, Tcl_Interp *, int argc, const char **argv)
{
  auto *command = static_cast<CTclAppCommand *>(clientData);

  if (! command->proc(argc, argv))
    return TCL_ERROR;

  return TCL_OK;
}

//---

void
CTclAppCommand::
setIntegerResult(int value)
{
  setStringResult(CStrUtil::toString(value));
}

void
CTclAppCommand::
setRealResult(double value)
{
  setStringResult(CStrUtil::toString(value));
}

void
CTclAppCommand::
setStringResult(const std::string &value)
{
  auto *sobj = Tcl_NewStringObj(value.c_str(), int(value.size()));

  setObjResult(sobj);
}

void
CTclAppCommand::
setIntegerArrayResult(int *values, int num_values)
{
  std::vector<std::string> strs;

  for (int i = 0; i < num_values; ++i)
    strs.push_back(CStrUtil::toString(values[i]));

  setStringArrayResult(strs);
}

void
CTclAppCommand::
setIntegerArrayResult(const std::vector<int> &values)
{
  std::vector<std::string> strs;

  int num_values = values.size();

  for (int i = 0; i < num_values; ++i)
    strs.push_back(CStrUtil::toString(values[i]));

  setStringArrayResult(strs);
}

void
CTclAppCommand::
setRealArrayResult(double *values, int num_values)
{
  std::vector<std::string> strs;

  for (int i = 0; i < num_values; ++i)
    strs.push_back(CStrUtil::toString(values[i]));

  setStringArrayResult(strs);
}

void
CTclAppCommand::
setRealArrayResult(const std::vector<double> &values)
{
  std::vector<std::string> strs;

  int num_values = values.size();

  for (int i = 0; i < num_values; ++i)
    strs.push_back(CStrUtil::toString(values[i]));

  setStringArrayResult(strs);
}

void
CTclAppCommand::
setStringArrayResult(char **values, int num_values)
{
  std::vector<std::string> strs;

  for (int i = 0; i < num_values; ++i)
    strs.push_back(values[i]);

  setStringArrayResult(strs);
}

void
CTclAppCommand::
setStringArrayResult(const std::vector<std::string> &values)
{
  auto *obj = Tcl_NewListObj(0, nullptr);

  int num_values = values.size();

  for (int i = 0; i < num_values; ++i) {
    auto *sobj = Tcl_NewStringObj(values[i].c_str(), int(values[i].size()));

    Tcl_ListObjAppendElement(app_->getInterp(), obj, sobj);
  }

  setObjResult(obj);
}

void
CTclAppCommand::
setObjResult(Tcl_Obj *obj)
{
  app_->setObjResult(obj);
}

//---

void
CTclAppCommand::
setIntegerVar(const std::string &name, int value)
{
  app_->setIntegerGlobalVar(name, value);
}

void
CTclAppCommand::
setRealVar(const std::string &name, double value)
{
  app_->setRealGlobalVar(name, value);
}

void
CTclAppCommand::
setStringVar(const std::string &name, const std::string &value)
{
  app_->setStringGlobalVar(name, value);
}

void
CTclAppCommand::
setBoolVar(const std::string &name, bool b)
{
  app_->setBoolGlobalVar(name, b);
}

//---

int
CTclAppCommand::
getIntegerVar(const std::string &name) const
{
  return app_->getIntegerGlobalVar(name);
}

double
CTclAppCommand::
getRealVar(const std::string &name) const
{
  return app_->getRealGlobalVar(name);
}

std::string
CTclAppCommand::
getStringVar(const std::string &name) const
{
  return app_->getStringGlobalVar(name);
}

bool
CTclAppCommand::
getBoolVar(const std::string &name) const
{
  return app_->getBoolGlobalVar(name);
}

bool
CTclAppCommand::
getStringArrayVar(const std::string &name, std::vector<std::string> &strs) const
{
  return app_->getStringArrayGlobalVar(name, strs);
}

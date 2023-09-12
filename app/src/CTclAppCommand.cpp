#include <CTclAppCommand.h>
#include <CTclApp.h>

#include <tcl.h>

CTclAppCommand::
CTclAppCommand(CTclApp *app, const QString &name) :
 app_(app), name_(name)
{
  Tcl_CreateObjCommand(app_->getInterp(), name_.toLatin1().constData(),
                       static_cast<Tcl_ObjCmdProc *>(&CTclAppCommand::commandProc),
                       static_cast<ClientData>(this), nullptr);
}

CTclAppCommand::
~CTclAppCommand()
{
  Tcl_DeleteCommand(app_->getInterp(), name_.toLatin1().constData());
}

int
CTclAppCommand::
commandProc(ClientData clientData, Tcl_Interp *, int objc, Tcl_Obj * const *objv)
{
  auto *command = static_cast<CTclAppCommand *>(clientData);

  if (! command->proc(objc, objv))
    return TCL_ERROR;

  return TCL_OK;
}

//---

void
CTclAppCommand::
setIntegerResult(int value)
{
  app_->setIntegerResult(value);
}

void
CTclAppCommand::
setRealResult(double value)
{
  app_->setRealResult(value);
}

void
CTclAppCommand::
setStringResult(const QString &value)
{
  app_->setStringResult(value);
}

void
CTclAppCommand::
setBoolResult(bool b)
{
  app_->setBoolResult(b);
}

void
CTclAppCommand::
setResult(const QVariant &var)
{
  app_->setResult(var);
}

void
CTclAppCommand::
setIntegerArrayResult(int *values, int num_values)
{
  app_->setIntegerArrayResult(values, num_values);
}

void
CTclAppCommand::
setIntegerArrayResult(const std::vector<int> &values)
{
  app_->setIntegerArrayResult(values);
}

void
CTclAppCommand::
setRealArrayResult(double *values, int num_values)
{
  app_->setRealArrayResult(values, num_values);
}

void
CTclAppCommand::
setRealArrayResult(const std::vector<double> &values)
{
  app_->setRealArrayResult(values);
}

void
CTclAppCommand::
setStringArrayResult(const std::vector<QString> &values)
{
  app_->setStringArrayResult(values);
}

//---

void
CTclAppCommand::
setIntegerVar(const QString &name, int value)
{
  app_->setIntegerGlobalVar(name, value);
}

void
CTclAppCommand::
setRealVar(const QString &name, double value)
{
  app_->setRealGlobalVar(name, value);
}

void
CTclAppCommand::
setStringVar(const QString &name, const QString &value)
{
  app_->setStringGlobalVar(name, value);
}

void
CTclAppCommand::
setBoolVar(const QString &name, bool b)
{
  app_->setBoolGlobalVar(name, b);
}

//---

int
CTclAppCommand::
getIntegerVar(const QString &name) const
{
  return app_->getIntegerGlobalVar(name);
}

double
CTclAppCommand::
getRealVar(const QString &name) const
{
  return app_->getRealGlobalVar(name);
}

QString
CTclAppCommand::
getStringVar(const QString &name) const
{
  return app_->getStringGlobalVar(name);
}

bool
CTclAppCommand::
getBoolVar(const QString &name) const
{
  return app_->getBoolGlobalVar(name);
}

bool
CTclAppCommand::
getStringArrayVar(const QString &name, std::vector<QString> &strs) const
{
  return app_->getStringArrayGlobalVar(name, strs);
}

//---

#if 0
QString
CTclAppCommand::
stringFromObj(const Tcl_Obj *obj)
{
  int len = 0;

  char *str = Tcl_GetStringFromObj(const_cast<Tcl_Obj *>(obj), &len);

  return QString(str, size_t(len));
}
#endif

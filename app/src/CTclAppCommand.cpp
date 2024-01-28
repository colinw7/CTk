#include <CTclAppCommand.h>
#include <CTclApp.h>

#include <tcl.h>

#include <iostream>

CTclAppCommand::
CTclAppCommand(CTclApp *app, const QString &name) :
 app_(app), name_(name)
{
  if (getenv("CTK_APP_DEBUG_CMDS"))
    std::cerr << "Create command '" << name.toStdString() << "'\n";

  Tcl_CreateObjCommand(app_->getInterp(), name_.toLatin1().constData(),
                       static_cast<Tcl_ObjCmdProc *>(&CTclAppCommand::commandProc),
                       static_cast<ClientData>(this), nullptr);
}

CTclAppCommand::
~CTclAppCommand()
{
  deleteCommand();
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

void
CTclAppCommand::
deleteCommand()
{
  if (! cmdDeleted_) {
    if (getenv("CTK_APP_DEBUG_CMDS"))
      std::cerr << "Delete command '" << name_.toStdString() << "'\n";

    Tcl_DeleteCommand(app_->getInterp(), name_.toLatin1().constData());

    cmdDeleted_ = true;
  }
}

//---

void
CTclAppCommand::
setIntegerResult(long value)
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
setIntegerArrayResult(long *values, uint num_values)
{
  app_->setIntegerArrayResult(values, num_values);
}

void
CTclAppCommand::
setIntegerArrayResult(const std::vector<long> &values)
{
  app_->setIntegerArrayResult(values);
}

void
CTclAppCommand::
setRealArrayResult(double *values, uint num_values)
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
setIntegerVar(const QString &name, long value)
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

long
CTclAppCommand::
getIntegerVar(const QString &name) const
{
  long i;
  (void) app_->getIntegerGlobalVar(name, i);
  return i;
}

double
CTclAppCommand::
getRealVar(const QString &name) const
{
  double r;
  (void) app_->getRealGlobalVar(name, r);
  return r;
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

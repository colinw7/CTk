#include <CTclApp.h>
#include <CTkAppUtil.h>
#include <CTkAppCmdNotify.h>
#include <CQTclUtil.h>

#ifdef CTCL_APP_READLINE
#include <CReadLine.h>
#endif

#include <iostream>
#include <cstring>
#include <cassert>

extern "C" {
#include <tcl.h>
}

namespace {

int
CTclAppTraceProc(ClientData clientData, Tcl_Interp *, int level, const char *command,
                 Tcl_Command /*commandToken*/ , int /*objc*/ , Tcl_Obj *const /*objv*/ [])
{
  auto *th = static_cast<CTclApp *>(clientData);
  assert(th);

  std::cerr << "Trace: " << command << " " << level << "\n";

  return TCL_OK;
}

void
CTclAppTraceDeleteProc(ClientData clientData)
{
  auto *th = static_cast<CTclApp *>(clientData);
  assert(th);
}

void
CTclAppCommandTraceProc(ClientData clientData, Tcl_Interp *, const char *oldName,
                        const char *newName, int flags)
{
  auto *notify = static_cast<CTkAppCmdNotify *>(clientData);
  assert(notify);

  //std::string desc;
  //if (flags & TCL_TRACE_RENAME   ) desc += " rename";
  //if (flags & TCL_TRACE_DELETE   ) desc += " delete";
  //if (flags & TCL_TRACE_DESTROYED) desc += " destroyed";

  //std::cerr << "TraceCmd: " << oldName << " " << newName << " " << desc << "\n";

  if      (flags & TCL_TRACE_RENAME)
    notify->handleRename(oldName, newName);
  else if (flags & TCL_TRACE_DELETE)
    notify->handleDelete(oldName);
}

}

//---

CTclApp *CTclApp::app_ = nullptr;

CTclApp::
CTclApp(Tcl_Interp *interp) :
 interp_(interp)
{
}

CTclApp::
CTclApp(int argc, const char **argv) :
 argc_(argc), argv_(argv)
{
  char *p = strrchr(const_cast<char *>(argv[0]), '/');

  if (p)
    appName_ = p + 1;
  else
    appName_ = argv[0];
}

void
CTclApp::
setAppName(const QString &appName)
{
  appName_ = appName;
}

bool
CTclApp::
tclInit()
{
  initInterp();

  app_ = this;

  if (Tcl_Init(app_->interp_) == TCL_ERROR)
    return false;

  return true;
}

void
CTclApp::
init()
{
  app_ = this;

  initInterp();

  initCommands();

  initInteractive();
}

void
CTclApp::
initInterp()
{
  if (! interp_)
    interp_ = Tcl_CreateInterp();
}

void
CTclApp::
initCommands()
{
  addCommands();
}

void
CTclApp::
initInteractive()
{
  app_->startup();

  app_->mainLoop();
}

void
CTclApp::
addTrace()
{
  int level = 1;
  int flags = TCL_ALLOW_INLINE_COMPILATION;

  Tcl_CreateObjTrace(interp_, level, flags, &CTclAppTraceProc, static_cast<ClientData>(this),
                     &CTclAppTraceDeleteProc);
}

void
CTclApp::
addCommandTrace(const char *command, class CTkAppCmdNotify *notify)
{
  int flags = TCL_TRACE_RENAME | TCL_TRACE_DELETE;

  Tcl_TraceCommand(interp_, command, flags, &CTclAppCommandTraceProc,
                   static_cast<ClientData>(notify));
}

void
CTclApp::
removeCommandTrace(const char *command, class CTkAppCmdNotify *notify)
{
  int flags = TCL_TRACE_RENAME | TCL_TRACE_DELETE;

  Tcl_UntraceCommand(interp_, command, flags, &CTclAppCommandTraceProc,
                     static_cast<ClientData>(notify));
}

void
CTclApp::
mainLoop()
{
  if (isInteractive())
    interactiveMainLoop();
}

void
CTclApp::
interactiveMainLoop()
{
#ifdef CTCL_APP_READLINE
  CReadLine readline;

  readline.setAutoHistory(true);

  std::string line;

  while (true) {
    while (Tcl_DoOneEvent(TCL_DONT_WAIT));

    readline.setPrompt("> ");

    line = readline.readLine();

    (void) eval(line);
  }
#else
  assert(false);
#endif
}

//---

void
CTclApp::
setIntegerResult(long value)
{
  CQTclUtil::setResult(app_->interp_, QVariant(static_cast<qlonglong>(value)));
}

void
CTclApp::
setRealResult(double value)
{
  long ivalue = long(value);

  if (ivalue == value)
    setIntegerResult(ivalue);
  else
    CQTclUtil::setResult(app_->interp_, QVariant(value));
}

void
CTclApp::
setStringResult(const QString &value)
{
  CQTclUtil::setResult(app_->interp_, value);
}

void
CTclApp::
setBytesResult(const QByteArray &ba)
{
  CQTclUtil::setBytesResult(app_->interp_, ba);
}

void
CTclApp::
setBoolResult(bool b)
{
  CQTclUtil::setResult(app_->interp_, b);
}

void
CTclApp::
setColorResult(const QColor &c)
{
  CQTclUtil::setResult(app_->interp_, c.name());
}

void
CTclApp::
setIntegerArrayResult(long *values, uint num_values)
{
  QVariantList vars;

  for (uint i = 0; i < num_values; ++i)
    vars.push_back(QVariant(static_cast<qlonglong>(values[i])));

  setVariantListResult(vars);
}

void
CTclApp::
setIntegerArrayResult(const std::vector<long> &values)
{
  QVariantList vars;

  for (const auto &value : values)
    vars.push_back(static_cast<qlonglong>(value));

  setVariantListResult(vars);
}

void
CTclApp::
setIntegerArrayResult(std::initializer_list<long> l)
{
  std::vector<long> il;

  for (const auto &i : l)
    il.push_back(i);

  setIntegerArrayResult(il);
}

void
CTclApp::
setRealArrayResult(double *values, uint num_values)
{
  QVariantList vars;

  for (uint i = 0; i < num_values; ++i)
    vars.push_back(values[i]);

  setVariantListResult(vars);
}

void
CTclApp::
setRealArrayResult(const std::vector<double> &values)
{
  QVariantList vars;

  for (const auto &value : values)
    vars.push_back(value);

  setVariantListResult(vars);
}

void
CTclApp::
setStringArrayResult(const std::vector<QString> &values)
{
  QVariantList vars;

  for (const auto &value : values)
    vars.push_back(value);

  setVariantListResult(vars);
}

void
CTclApp::
setVariantArrayResult(const std::vector<QVariant> &vars)
{
  QVariantList vars1;

  for (const auto &var : vars)
    vars1.push_back(var);

  setVariantListResult(vars1);
}

void
CTclApp::
setVariantListResult(const QVariantList &vars)
{
  CQTclUtil::setResult(app_->interp_, vars);
}

void
CTclApp::
setResult(const QVariant &var)
{
  CQTclUtil::setResult(app_->interp_, var);
}

//---

bool
CTclApp::
getStringResult(QString &res) const
{
  QVariant var;
  if (! getResult(var))
    return false;

  res = CTkAppUtil::variantToString(const_cast<CTclApp *>(this), var);

  return true;
}

bool
CTclApp::
getResult(QVariant &res) const
{
  auto var = CQTclUtil::getResult(app_->getInterp());
  if (! var.isValid()) return false;

  res = var;

  return true;
}

//---

void
CTclApp::
setIntegerGlobalVar(const QString &name, long value)
{
  setGlobalVar(name, QVariant(static_cast<qlonglong>(value)));
}

void
CTclApp::
setRealGlobalVar(const QString &name, double value)
{
  setGlobalVar(name, QVariant(value));
}

void
CTclApp::
setBoolGlobalVar(const QString &name, bool b)
{
  setGlobalVar(name, QVariant(b ? "1" : "0"));
}

void
CTclApp::
setStringGlobalVar(const QString &name, const QString &value)
{
  setGlobalVar(name, QVariant(value));
}

void
CTclApp::
setGlobalVar(const QString &name, const QVariant &value)
{
  CQTclUtil::createGlobalVar(app_->interp_, name, value);
}

//---

void
CTclApp::
setStringLocalVar(const QString &name, const QString &value)
{
  setLocalVar(name, QVariant(value));
}

void
CTclApp::
setLocalVar(const QString &name, const QVariant &value)
{
  CQTclUtil::createLocalVar(app_->interp_, name, value);
}

//---

void
CTclApp::
setStringArrayGlobalVar(const QString &name, const std::vector<QString> &strs)
{
  setStringGlobalVar(name, mergeList(strs));
}

void
CTclApp::
setIntegerArrayGlobalVar(const QString &name, const std::vector<long> &values)
{
  std::vector<QString> strs;

  auto num_values = values.size();

  for (uint i = 0; i < num_values; ++i)
    strs.push_back(QString::number(values[i]));

  setStringArrayGlobalVar(name, strs);
}

void
CTclApp::
setStringArrayLocalVar(const QString &name, const std::vector<QString> &strs)
{
  setStringLocalVar(name, mergeList(strs));
}

void
CTclApp::
setIntegerArrayLocalVar(const QString &name, const std::vector<long> &values)
{
  std::vector<QString> strs;

  auto num_values = values.size();

  for (uint i = 0; i < num_values; ++i)
    strs.push_back(QString::number(values[i]));

  setStringArrayLocalVar(name, strs);
}

//---

bool
CTclApp::
hasGlobalVar(const QString &name) const
{
  return CQTclUtil::hasGlobalVar(app_->interp_, name);
}

bool
CTclApp::
getIntegerGlobalVar(const QString &name, long &i) const
{
  auto var = getGlobalVar(name);

  i = 0;
  if (! CTkAppUtil::variantToInt(app_, var, i))
    return false;

  return true;
}

bool
CTclApp::
getRealGlobalVar(const QString &name, double &r) const
{
  auto var = getGlobalVar(name);

  r = 0.0;
  if (! CTkAppUtil::variantToReal(app_, var, r))
    return false;

  return true;
}

bool
CTclApp::
getBoolGlobalVar(const QString &name) const
{
  auto var = getGlobalVar(name);

  bool b = false;
  (void) CTkAppUtil::variantToBool(app_, var, b);

  return b;
}

QString
CTclApp::
getStringGlobalVar(const QString &name) const
{
  auto var = getGlobalVar(name);

  return CTkAppUtil::variantToString(app_, var);
}

QVariant
CTclApp::
getGlobalVar(const QString &name) const
{
  return CQTclUtil::getGlobalVar(app_->interp_, name);
}

bool
CTclApp::
getStringArrayGlobalVar(const QString &name, std::vector<QString> &strs) const
{
  auto str = getStringGlobalVar(name);

  auto rc = CQTclUtil::splitList(str, strs);

  if (rc != TCL_OK) {
    lastErrorMsg_ = errorInfo_(lastError_);
    std::cerr << lastErrorMsg_.toStdString() << "\n";
    return false;
  }

  return true;
}

//---

bool
CTclApp::
splitList(const QVariant &var, std::vector<QString> &strs) const
{
  // TODO: handle list variant
  auto str = CTkAppUtil::variantToString(app_, var);
  return splitList(str, strs);
}

bool
CTclApp::
splitList(const QString &str, std::vector<QString> &strs) const
{
  auto rc = CQTclUtil::splitList(str, strs);
  if (rc != TCL_OK) return false;

  return true;
}

QString
CTclApp::
mergeList(const std::vector<QString> &strs) const
{
  auto str = CQTclUtil::mergeList(strs);

  return str;
}

//---

void
CTclApp::
traceVar(const QString &name, CTclTraceProc *proc)
{
  auto name1 = name;

  auto pos = name1.indexOf('(');

  if (pos >= 0)
    name1 = name1.mid(0, pos);

  //---

  auto pn = traceProcs_.find(name1);

  if (pn != traceProcs_.end()) {
    auto pp = (*pn).second.find(proc);

    if (pp != (*pn).second.end()) {
      std::cerr << "Multiple traces on same var '" << name1.toStdString() << "'\n";
      return;
    }
  }

  traceProcs_[name1].insert(proc);

  //---

  int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS;

  auto data = Tcl_VarTraceInfo(interp_, name1.toLatin1().constData(), flags, &traceProc, nullptr);

  if (! data)
    Tcl_TraceVar(interp_, name1.toLatin1().constData(), flags,
                 &traceProc, static_cast<ClientData>(this));
}

void
CTclApp::
traceGlobalVar(const QString &name, CTclTraceProc *proc)
{
  auto name1 = name;

  auto pos = name1.indexOf('(');

  if (pos >= 0)
    name1 = name1.mid(0, pos);

  //---

  auto pn = traceProcs_.find(name1);

  if (pn != traceProcs_.end()) {
    auto pp = (*pn).second.find(proc);

    if (pp != (*pn).second.end()) {
      std::cerr << "Multiple traces on same var '" << name1.toStdString() << "'\n";
      return;
    }
  }

  traceProcs_[name1].insert(proc);

  //---

  int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

  auto data = Tcl_VarTraceInfo(interp_, name1.toLatin1().constData(), flags, &traceProc, nullptr);

  if (! data)
    Tcl_TraceVar(interp_, name1.toLatin1().constData(), flags,
                 &traceProc, static_cast<ClientData>(this));
}

void
CTclApp::
traceGlobalVar(const QString &name)
{
  auto name1 = name;

  auto pos = name1.indexOf('(');

  if (pos >= 0)
    name1 = name1.mid(0, pos);

  //---

  auto pn = traces_.find(name1);

  if (pn != traces_.end()) {
    std::cerr << "Multiple traces on same var '" << name1.toStdString() << "'\n";
    return;
  }

  traces_.insert(name1);

  //---

  int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

  auto data = Tcl_VarTraceInfo(interp_, name1.toLatin1().constData(), flags,
                               &genTraceProc, nullptr);

  if (! data)
    Tcl_TraceVar(interp_, name1.toLatin1().constData(), flags,
                 &genTraceProc, static_cast<ClientData>(this));
}

void
CTclApp::
untraceVar(const QString &name, CTclTraceProc *proc)
{
  auto name1 = name;

  auto pos = name1.indexOf('(');

  if (pos >= 0)
    name1 = name1.mid(0, pos);

  //---

  auto pn = traceProcs_.find(name1);

  if (pn == traceProcs_.end()) {
    std::cerr << "No trace on var '" << name1.toStdString() << "'\n";
    return;
  }

  auto pp = (*pn).second.find(proc);

  if (pp == (*pn).second.end()) {
    std::cerr << "No trace on var '" << name1.toStdString() << "'\n";
    return;
  }

  (*pn).second.erase(pp);

  //---

  if ((*pn).second.empty()) {
    int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS;

    Tcl_UntraceVar(interp_, name1.toLatin1().constData(), flags,
                   &traceProc, static_cast<ClientData>(this));
  }
}

void
CTclApp::
untraceGlobalVar(const QString &name, CTclTraceProc *proc)
{
  auto name1 = name;

  auto pos = name1.indexOf('(');

  if (pos >= 0)
    name1 = name1.mid(0, pos);

  //---

  auto pn = traceProcs_.find(name1);

  if (pn == traceProcs_.end()) {
    std::cerr << "No trace on var '" << name1.toStdString() << "'\n";
    return;
  }

  auto pp = (*pn).second.find(proc);

  if (pp == (*pn).second.end()) {
    std::cerr << "No trace on var '" << name1.toStdString() << "'\n";
    return;
  }

  (*pn).second.erase(pp);

  //---

  if ((*pn).second.empty()) {
    int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

    Tcl_UntraceVar(interp_, name1.toLatin1().constData(), flags,
                   &traceProc, static_cast<ClientData>(this));
  }
}

void
CTclApp::
untraceGlobalVar(const QString &name)
{
  auto name1 = name;

  auto pos = name1.indexOf('(');

  if (pos >= 0)
    name1 = name1.mid(0, pos);

  //---

  auto pn = traceProcs_.find(name1);

  if (pn == traceProcs_.end()) {
    std::cerr << "No trace on var '" << name1.toStdString() << "'\n";
    return;
  }

  traces_.erase(name1);

  //---

  int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

  Tcl_UntraceVar(interp_, name1.toLatin1().constData(), flags,
                 &genTraceProc, static_cast<ClientData>(this));
}

void
CTclApp::
handleTrace(const char *name, int flags)
{
  // ignore unset called on trace destruction
//if (flags & TCL_TRACE_UNSETS)
//  return;

  bool handled = false;

  if (flags & TCL_TRACE_READS ) { handleRead (name); handled = true; }
  if (flags & TCL_TRACE_WRITES) { handleWrite(name); handled = true; }
  if (flags & TCL_TRACE_UNSETS) { handleUnset(name); handled = true; }

  assert(handled);
}

char *
CTclApp::
traceProc(ClientData data, Tcl_Interp *, const char *name1, const char *, int flags)
{
  // ignore unset called on trace destruction
//if (flags & TCL_TRACE_UNSETS)
//  return nullptr;

  auto *th = static_cast<CTclApp *>(data);
  assert(th);

  auto pn = th->traceProcs_.find(name1);

  if (name1[0] == ':' && name1[1] == ':')
    pn = th->traceProcs_.find(&name1[2]);

  //assert(pn != th->traceProcs_.end());
  if (pn == th->traceProcs_.end()) {
    std::cerr << "Failed to find trace proc '" << name1 << "'\n";
    return nullptr;
  }

  for (auto *proc : (*pn).second) {
    if (! proc->isEnabled())
      continue;

    bool handled = false;

    if (flags & TCL_TRACE_READS ) { proc->handleRead (name1); handled = true; }
    if (flags & TCL_TRACE_WRITES) { proc->handleWrite(name1); handled = true; }
    if (flags & TCL_TRACE_UNSETS) { proc->handleUnset(name1); handled = true; }

    assert(handled);
  }

  return nullptr;
}

char *
CTclApp::
genTraceProc(ClientData data, Tcl_Interp *, const char *name1, const char *, int flags)
{
  auto *th = static_cast<CTclApp *>(data);
  assert(th);

  th->handleTrace(name1, flags);

  return nullptr;
}

void
CTclApp::
handleRead(const char *name)
{
  std::cerr << "CTclApp::handleRead " << name << "\n";
}

void
CTclApp::
handleWrite(const char *name)
{
  std::cerr << "CTclApp::handleWrite " << name << "\n";
}

void
CTclApp::
handleUnset(const char *name)
{
  std::cerr << "CTcl::handleUnset " << name << "\n";
}

//---

int
CTclApp::
createAlias(const QString &newName, const QString &oldName)
{
  return Tcl_CreateAlias(interp_, newName.toLatin1().constData(),
                         interp_, oldName.toLatin1().constData(), 0, nullptr);
}

//---

bool
CTclApp::
evalFile(const QString &filename) const
{
#if 0
  auto cmd = "source \"" + filename + "\"";

  return eval(cmd);
#endif

  lastError_ = Tcl_EvalFile(interp_, filename.toLatin1().constData());

  if (lastError_ != TCL_OK) {
    lastErrorMsg_ = errorInfo_(lastError_);
    std::cerr << lastErrorMsg_.toStdString() << "\n";
  }

  return (lastError_ == TCL_OK);
}

bool
CTclApp::
eval(const QString &str) const
{
  lastError_ = Tcl_Eval(interp_, str.toLatin1().constData());

  if (lastError_ != TCL_OK) {
    lastErrorMsg_ = errorInfo_(lastError_);
    std::cerr << lastErrorMsg_.toStdString() << "\n";
  }

  return (lastError_ == TCL_OK);
}

bool
CTclApp::
evalNoExcept(const QString &str) const
{
  Tcl_AllowExceptions(interp_);

  int rc = Tcl_Eval(interp_, str.toLatin1().constData());

  return (rc == TCL_OK);
}

QString
CTclApp::
errorInfo_(int rc) const
{
  return QString::fromStdString(CTclUtil::errorInfo(interp_, rc));
}

//---

QString
CTclApp::
lastErrorInfo() const
{
  return lastErrorMsg_;
}

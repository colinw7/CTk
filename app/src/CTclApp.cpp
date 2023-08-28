#include <CTclApp.h>
#include <CTkAppUtil.h>
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
setIntegerResult(int value)
{
  CQTclUtil::setResult(app_->interp_, QVariant(value));
}

void
CTclApp::
setRealResult(double value)
{
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
setBoolResult(bool b)
{
  CQTclUtil::setResult(app_->interp_, b);
}

void
CTclApp::
setIntegerArrayResult(int *values, int num_values)
{
  QVariantList vars;

  for (int i = 0; i < num_values; ++i)
    vars.push_back(values[i]);

  CQTclUtil::setResult(app_->interp_, vars);
}

void
CTclApp::
setIntegerArrayResult(const std::vector<int> &values)
{
  QVariantList vars;

  for (const auto &value : values)
    vars.push_back(value);

  CQTclUtil::setResult(app_->interp_, vars);
}

void
CTclApp::
setIntegerArrayResult(std::initializer_list<int> l)
{
  std::vector<int> il;

  for (const auto &i : l)
    il.push_back(i);

  setIntegerArrayResult(il);
}

void
CTclApp::
setRealArrayResult(double *values, int num_values)
{
  QVariantList vars;

  for (int i = 0; i < num_values; ++i)
    vars.push_back(values[i]);

  CQTclUtil::setResult(app_->interp_, vars);
}

void
CTclApp::
setRealArrayResult(const std::vector<double> &values)
{
  QVariantList vars;

  for (const auto &value : values)
    vars.push_back(value);

  CQTclUtil::setResult(app_->interp_, vars);
}

void
CTclApp::
setStringArrayResult(const std::vector<QString> &values)
{
  QVariantList vars;

  for (const auto &value : values)
    vars.push_back(value);

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
  auto var = CQTclUtil::getResult(app_->getInterp());
  if (! var.isValid()) return false;

  res = var.toString();

  return true;
}

//---

void
CTclApp::
setIntegerGlobalVar(const QString &var, int value)
{
  setStringGlobalVar(var, QString::number(value));
}

void
CTclApp::
setRealGlobalVar(const QString &var, double value)
{
  setStringGlobalVar(var, QString::number(value));
}

void
CTclApp::
setStringGlobalVar(const QString &var, const QString &value)
{
  CQTclUtil::createGlobalVar(app_->interp_, var, value);
}

void
CTclApp::
setStringVar(const QString &var, const QString &value)
{
  CQTclUtil::createLocalVar(app_->interp_, var, value);
}

void
CTclApp::
setBoolGlobalVar(const QString &name, bool b)
{
  setStringGlobalVar(name, QString(b ? "1" : "0"));
}

void
CTclApp::
setStringArrayGlobalVar(const QString &var, const std::vector<QString> &strs)
{
  setStringGlobalVar(var, mergeList(strs));
}

void
CTclApp::
setStringArrayVar(const QString &var, const std::vector<QString> &strs)
{
  setStringVar(var, mergeList(strs));
}

void
CTclApp::
setIntegerArrayGlobalVar(const QString &var, const std::vector<int> &values)
{
  std::vector<QString> strs;

  auto num_values = values.size();

  for (uint i = 0; i < num_values; ++i)
    strs.push_back(QString::number(values[i]));

  setStringArrayGlobalVar(var, strs);
}

void
CTclApp::
setIntegerArrayVar(const QString &var, const std::vector<int> &values)
{
  std::vector<QString> strs;

  auto num_values = values.size();

  for (uint i = 0; i < num_values; ++i)
    strs.push_back(QString::number(values[i]));

  setStringArrayVar(var, strs);
}

//---

bool
CTclApp::
hasGlobalVar(const QString &var) const
{
  return CQTclUtil::hasGlobalVar(app_->interp_, var);
}

int
CTclApp::
getIntegerGlobalVar(const QString &var) const
{
  auto str = getStringGlobalVar(var);

  long i = 0;
  (void) CTkAppUtil::stringToInt(str, i);

  return i;
}

double
CTclApp::
getRealGlobalVar(const QString &var) const
{
  auto str = getStringGlobalVar(var);

  double r = 0.0;
  (void) CTkAppUtil::stringToReal(str, r);

  return r;
}

QString
CTclApp::
getStringGlobalVar(const QString &var) const
{
  return CQTclUtil::getGlobalVar(app_->interp_, var).toString();
}

bool
CTclApp::
getBoolGlobalVar(const QString &var) const
{
  auto str = getStringGlobalVar(var).toLower();

  bool b = false;
  (void) CTkAppUtil::stringToBool(str, b);

  return b;
}

bool
CTclApp::
getStringArrayGlobalVar(const QString &var, std::vector<QString> &strs) const
{
  auto str = CQTclUtil::getGlobalVar(app_->interp_, var).toString();

  auto rc = CQTclUtil::splitList(str, strs);

  if (rc != TCL_OK) {
    std::cerr << errorInfo_(rc).toStdString() << "\n";
    return false;
  }

  return true;
}

//---

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
  if (flags & TCL_TRACE_UNSETS) return;

  bool handled = false;

  if (flags & TCL_TRACE_READS ) { handleRead (name); handled = true; }
  if (flags & TCL_TRACE_WRITES) { handleWrite(name); handled = true; }
//if (flags & TCL_TRACE_UNSETS) { handleUnset(name); handled = true; }

  assert(handled);
}

char *
CTclApp::
traceProc(ClientData data, Tcl_Interp *, const char *name1, const char *, int flags)
{
  // ignore unset called on trace destruction
  if (flags & TCL_TRACE_UNSETS) return nullptr;

  auto *th = static_cast<CTclApp *>(data);
  assert(th);

  auto pn = th->traceProcs_.find(name1);
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
  //if (flags & TCL_TRACE_UNSETS) { proc->handleUnset(name1); handled = true; }

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

#if 0
void
CTclApp::
handleUnset(const char *name)
{
  std::cerr << "CTcl::handleUnset " << name << "\n";
}
#endif

//---

bool
CTclApp::
evalFile(const QString &filename) const
{
#if 0
  auto cmd = "source \"" + filename + "\"";

  return eval(cmd);
#endif

  auto rc = Tcl_EvalFile(interp_, filename.toLatin1().constData());

  if (rc != TCL_OK)
    std::cerr << errorInfo_(rc).toStdString() << "\n";

  return (rc == TCL_OK);
}

bool
CTclApp::
eval(const QString &str) const
{
  auto rc = Tcl_Eval(interp_, str.toLatin1().constData());

  if (rc != TCL_OK)
    std::cerr << errorInfo_(rc).toStdString() << "\n";

  return (rc == TCL_OK);
}

//---

QString
CTclApp::
errorInfo_(int rc) const
{
  return QString::fromStdString(CTclUtil::errorInfo(interp_, rc));
}

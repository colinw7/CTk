#include <CTclApp.h>
#include <CTkAppUtil.h>

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
  setStringResult(QString::number(value));
}

void
CTclApp::
setRealResult(double value)
{
  setStringResult(QString::number(value));
}

void
CTclApp::
setStringResult(const QString &value)
{
  auto *sobj = Tcl_NewStringObj(value.toLatin1().constData(), int(value.length()));

  setObjResult(sobj);
}

void
CTclApp::
setBoolResult(bool b)
{
  setStringResult(b ? "1" : "0");
}

void
CTclApp::
setIntegerArrayResult(int *values, int num_values)
{
  std::vector<QString> strs;

  for (int i = 0; i < num_values; ++i)
    strs.push_back(QString::number(values[i]));

  setStringArrayResult(strs);
}

void
CTclApp::
setIntegerArrayResult(const std::vector<int> &values)
{
  std::vector<QString> strs;

  auto num_values = values.size();

  for (uint i = 0; i < num_values; ++i)
    strs.push_back(QString::number(values[i]));

  setStringArrayResult(strs);
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
  std::vector<QString> strs;

  for (int i = 0; i < num_values; ++i)
    strs.push_back(QString::number(values[i]));

  setStringArrayResult(strs);
}

void
CTclApp::
setRealArrayResult(const std::vector<double> &values)
{
  std::vector<QString> strs;

  int num_values = values.size();

  for (int i = 0; i < num_values; ++i)
    strs.push_back(QString::number(values[i]));

  setStringArrayResult(strs);
}

void
CTclApp::
setStringArrayResult(const std::vector<QString> &values)
{
  auto *obj = Tcl_NewListObj(0, nullptr);

  int num_values = values.size();

  for (int i = 0; i < num_values; ++i) {
    auto *sobj = Tcl_NewStringObj(values[i].toLatin1().constData(), int(values[i].length()));

    Tcl_ListObjAppendElement(app_->getInterp(), obj, sobj);
  }

  setObjResult(obj);
}

void
CTclApp::
setObjResult(Tcl_Obj *obj)
{
  if (obj) {
    resultSet_ = true;

    Tcl_SetObjResult(app_->getInterp(), obj);
  }
  else
    resultSet_ = false;
}

//---

bool
CTclApp::
getStringResult(QString &res) const
{
  auto *resPtr = Tcl_GetObjResult(app_->getInterp());
  Tcl_IncrRefCount(resPtr);
  int length;
  res = QString(Tcl_GetStringFromObj(resPtr, &length));
  Tcl_DecrRefCount(resPtr);
  return (length > 0);
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
  Tcl_SetVar(app_->interp_, var.toLatin1().constData(),
             value.toLatin1().constData(), TCL_GLOBAL_ONLY);
}

void
CTclApp::
setStringVar(const QString &var, const QString &value)
{
  Tcl_SetVar(app_->interp_, var.toLatin1().constData(), value.toLatin1().constData(), 0);
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
  const char *value = Tcl_GetVar(app_->interp_, var.toLatin1().constData(), TCL_GLOBAL_ONLY);

  return value;
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
  const char *value = Tcl_GetVar(app_->interp_, var.toLatin1().constData(), TCL_GLOBAL_ONLY);
  if (! value) return "";

  return value;
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
  const char *value = Tcl_GetVar(app_->interp_, var.toLatin1().constData(), TCL_GLOBAL_ONLY);
  if (! value) return false;

  int    argc;
  char **argv;

  int rc = Tcl_SplitList(app_->interp_, value, &argc, const_cast<const char ***>(&argv));

  if (rc != TCL_OK) {
    std::cerr << errorInfo_(rc).toStdString() << "\n";
    return false;
  }

  for (int i = 0; i < argc; ++i)
    strs.push_back(QString(argv[i]));

  Tcl_Free(reinterpret_cast<char *>(argv));

  return true;
}

//---

bool
CTclApp::
splitList(const QString &str, std::vector<QString> &strs) const
{
  int    argc;
  char **argv;

  auto cstr = str.toStdString();

  int rc = Tcl_SplitList(app_->interp_, cstr.c_str(), &argc, const_cast<const char ***>(&argv));
  if (rc != TCL_OK) return false;

  for (int i = 0; i < argc; ++i)
    strs.push_back(QString(argv[i]));

  Tcl_Free(reinterpret_cast<char *>(argv));

  return true;
}

QString
CTclApp::
mergeList(const std::vector<QString> &strs) const
{
  auto argc = size_t(strs.size());
  if (argc == 0) return QString();

  std::vector<char *> argv;

  argv.resize(argc);

  for (size_t i = 0; i < argc; ++i)
    argv[i] = strdup(strs[int(i)].toLatin1().constData());

  char *res = Tcl_Merge(int(argc), &argv[0]);

  QString str(res);

  for (size_t i = 0; i < argc; ++i)
    free(argv[i]);

  Tcl_Free(res);

  return str;
}

//---

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
  auto getStringFromObj = [](Tcl_Obj *obj) {
    int len = 0;

    char *str = Tcl_GetStringFromObj(obj, &len);

    std::string cstr(str, size_t(len));

    return QString::fromStdString(cstr);
  };

  auto *options = Tcl_GetReturnOptions(interp_, rc);

  auto *key1 = Tcl_NewStringObj("-errorcode", -1);
  auto *key2 = Tcl_NewStringObj("-errorinfo", -1);

  Tcl_Obj *errorMsg;
  Tcl_IncrRefCount(key1);
  Tcl_DictObjGet(nullptr, options, key1, &errorMsg);
  Tcl_DecrRefCount(key1);

  auto msg = getStringFromObj(errorMsg);

  Tcl_Obj *stackTrace;
  Tcl_IncrRefCount(key2);
  Tcl_DictObjGet(nullptr, options, key2, &stackTrace);
  Tcl_DecrRefCount(key2);

  auto trace = getStringFromObj(stackTrace);

  Tcl_DecrRefCount(options);

  return msg + "\n" + trace;
}

#include <CTclApp.h>

#include <CStrUtil.h>

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
setAppName(const std::string &appName)
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
  setStringResult(CStrUtil::toString(value));
}

void
CTclApp::
setRealResult(double value)
{
  setStringResult(CStrUtil::toString(value));
}

void
CTclApp::
setStringResult(const std::string &value)
{
  auto *sobj = Tcl_NewStringObj(value.c_str(), int(value.size()));

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
  std::vector<std::string> strs;

  for (int i = 0; i < num_values; ++i)
    strs.push_back(CStrUtil::toString(values[i]));

  setStringArrayResult(strs);
}

void
CTclApp::
setIntegerArrayResult(const std::vector<int> &values)
{
  std::vector<std::string> strs;

  auto num_values = values.size();

  for (uint i = 0; i < num_values; ++i)
    strs.push_back(CStrUtil::toString(values[i]));

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
  std::vector<std::string> strs;

  for (int i = 0; i < num_values; ++i)
    strs.push_back(CStrUtil::toString(values[i]));

  setStringArrayResult(strs);
}

void
CTclApp::
setRealArrayResult(const std::vector<double> &values)
{
  std::vector<std::string> strs;

  int num_values = values.size();

  for (int i = 0; i < num_values; ++i)
    strs.push_back(CStrUtil::toString(values[i]));

  setStringArrayResult(strs);
}

void
CTclApp::
setStringArrayResult(char **values, int num_values)
{
  std::vector<std::string> strs;

  for (int i = 0; i < num_values; ++i)
    strs.push_back(values[i]);

  setStringArrayResult(strs);
}

void
CTclApp::
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
getStringResult(std::string &res) const
{
  auto *resPtr = Tcl_GetObjResult(app_->getInterp());
  Tcl_IncrRefCount(resPtr);
  int length;
  res = std::string(Tcl_GetStringFromObj(resPtr, &length));
  Tcl_DecrRefCount(resPtr);
  return (length > 0);
}

//---

void
CTclApp::
setIntegerGlobalVar(const std::string &var, int value)
{
  setStringGlobalVar(var, CStrUtil::toString(value));
}

void
CTclApp::
setRealGlobalVar(const std::string &var, double value)
{
  setStringGlobalVar(var, CStrUtil::toString(value));
}

void
CTclApp::
setStringGlobalVar(const std::string &var, const std::string &value)
{
  Tcl_SetVar(app_->interp_, var.c_str(), value.c_str(), TCL_GLOBAL_ONLY);
}

void
CTclApp::
setStringVar(const std::string &var, const std::string &value)
{
  Tcl_SetVar(app_->interp_, var.c_str(), value.c_str(), 0);
}

void
CTclApp::
setBoolGlobalVar(const std::string &name, bool b)
{
  setStringGlobalVar(name, b ? "1" : "0");
}

void
CTclApp::
setStringArrayGlobalVar(const std::string &var, const std::vector<std::string> &strs)
{
  setStringGlobalVar(var, mergeList(strs));
}

void
CTclApp::
setStringArrayVar(const std::string &var, const std::vector<std::string> &strs)
{
  setStringVar(var, mergeList(strs));
}

void
CTclApp::
setIntegerArrayGlobalVar(const std::string &var, const std::vector<int> &values)
{
  std::vector<std::string> strs;

  auto num_values = values.size();

  for (uint i = 0; i < num_values; ++i)
    strs.push_back(CStrUtil::toString(values[i]));

  setStringArrayGlobalVar(var, strs);
}

void
CTclApp::
setIntegerArrayVar(const std::string &var, const std::vector<int> &values)
{
  std::vector<std::string> strs;

  auto num_values = values.size();

  for (uint i = 0; i < num_values; ++i)
    strs.push_back(CStrUtil::toString(values[i]));

  setStringArrayVar(var, strs);
}

//---

bool
CTclApp::
hasGlobalVar(const std::string &var) const
{
  const char *value = Tcl_GetVar(app_->interp_, var.c_str(), TCL_GLOBAL_ONLY);

  return value;
}

int
CTclApp::
getIntegerGlobalVar(const std::string &var) const
{
  return CStrUtil::toInteger(getStringGlobalVar(var));
}

double
CTclApp::
getRealGlobalVar(const std::string &var) const
{
  return CStrUtil::toReal(getStringGlobalVar(var));
}

std::string
CTclApp::
getStringGlobalVar(const std::string &var) const
{
  const char *value = Tcl_GetVar(app_->interp_, var.c_str(), TCL_GLOBAL_ONLY);
  if (! value) return "";

  return value;
}

bool
CTclApp::
getBoolGlobalVar(const std::string &var) const
{
  bool b;

  if (! CStrUtil::toBool(getStringGlobalVar(var), &b))
    b = false;

  return b;
}

bool
CTclApp::
getStringArrayGlobalVar(const std::string &var, std::vector<std::string> &strs) const
{
  const char *value = Tcl_GetVar(app_->interp_, var.c_str(), TCL_GLOBAL_ONLY);
  if (! value) return false;

  int    argc;
  char **argv;

  int rc = Tcl_SplitList(app_->interp_, value, &argc, const_cast<const char ***>(&argv));

  if (rc != TCL_OK) {
    std::cerr << errorInfo_(rc) << "\n";
    return false;
  }

  for (int i = 0; i < argc; ++i)
    strs.push_back(std::string(argv[i]));

  Tcl_Free(reinterpret_cast<char *>(argv));

  return true;
}

//---

bool
CTclApp::
splitList(const std::string &str, std::vector<std::string> &strs) const
{
  int    argc;
  char **argv;

  int rc = Tcl_SplitList(app_->interp_, str.c_str(), &argc, const_cast<const char ***>(&argv));
  if (rc != TCL_OK) return false;

  for (int i = 0; i < argc; ++i)
    strs.push_back(std::string(argv[i]));

  Tcl_Free(reinterpret_cast<char *>(argv));

  return true;
}

std::string
CTclApp::
mergeList(const std::vector<std::string> &strs) const
{
  auto argc = strs.size();

  std::vector<char *> argv;

  argv.resize(argc);

  for (size_t i = 0; i < argc; ++i)
    argv[i] = strdup(strs[i].c_str());

  char *res = Tcl_Merge(int(argc), &argv[0]);

  std::string str(res);

  for (size_t i = 0; i < argc; ++i)
    free(argv[i]);

  Tcl_Free(res);

  return str;
}

//---

void
CTclApp::
traceGlobalVar(const std::string &name, CTclTraceProc *proc)
{
  auto name1 = name;

  auto pos = name1.find('(');

  if (pos != std::string::npos)
    name1 = name1.substr(0, pos);

  //---

  auto pn = traceProcs_.find(name1);

  if (pn != traceProcs_.end()) {
    auto pp = (*pn).second.find(proc);

    if (pp != (*pn).second.end()) {
      std::cerr << "Multiple traces on same var '" << name1 << "'\n";
      return;
    }
  }

  traceProcs_[name1].insert(proc);

  //---

  int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

  auto data = Tcl_VarTraceInfo(interp_, name1.c_str(), flags, &traceProc, nullptr);

  if (! data)
    Tcl_TraceVar(interp_, name1.c_str(), flags, &traceProc, static_cast<ClientData>(this));
}

void
CTclApp::
traceGlobalVar(const std::string &name)
{
  auto name1 = name;

  auto pos = name1.find('(');

  if (pos != std::string::npos)
    name1 = name1.substr(0, pos);

  //---

  auto pn = traces_.find(name1);

  if (pn != traces_.end()) {
    std::cerr << "Multiple traces on same var '" << name1 << "'\n";
    return;
  }

  traces_.insert(name1);

  //---

  int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

  auto data = Tcl_VarTraceInfo(interp_, name1.c_str(), flags, &genTraceProc, nullptr);

  if (! data)
    Tcl_TraceVar(interp_, name1.c_str(), flags, &genTraceProc, static_cast<ClientData>(this));
}

void
CTclApp::
untraceGlobalVar(const std::string &name, CTclTraceProc *proc)
{
  auto name1 = name;

  auto pos = name1.find('(');

  if (pos != std::string::npos)
    name1 = name1.substr(0, pos);

  //---

  auto pn = traceProcs_.find(name1);

  if (pn == traceProcs_.end()) {
    std::cerr << "No trace on var '" << name1 << "'\n";
    return;
  }

  auto pp = (*pn).second.find(proc);

  if (pp == (*pn).second.end()) {
    std::cerr << "No trace on var '" << name1 << "'\n";
    return;
  }

  (*pn).second.erase(pp);

  //---

  if ((*pn).second.empty()) {
    int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

    Tcl_UntraceVar(interp_, name1.c_str(), flags, &traceProc, static_cast<ClientData>(this));
  }
}

void
CTclApp::
untraceGlobalVar(const std::string &name)
{
  auto name1 = name;

  auto pos = name1.find('(');

  if (pos != std::string::npos)
    name1 = name1.substr(0, pos);

  //---

  auto pn = traceProcs_.find(name1);

  if (pn == traceProcs_.end()) {
    std::cerr << "No trace on var '" << name1 << "'\n";
    return;
  }

  traces_.erase(name1);

  //---

  int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

  Tcl_UntraceVar(interp_, name1.c_str(), flags, &genTraceProc, static_cast<ClientData>(this));
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
evalFile(const std::string &filename) const
{
#if 0
  std::string cmd = "source \"" + filename + "\"";

  return eval(cmd);
#endif

  auto rc = Tcl_EvalFile(interp_, filename.c_str());

  if (rc != TCL_OK)
    std::cerr << errorInfo_(rc) << "\n";

  return (rc == TCL_OK);
}

bool
CTclApp::
eval(const std::string &str) const
{
  auto rc = Tcl_Eval(interp_, str.c_str());

  if (rc != TCL_OK)
    std::cerr << errorInfo_(rc) << "\n";

  return (rc == TCL_OK);
}

bool
CTclApp::
eval(const char *format, ...) const
{
  va_list vargs;

  va_start(vargs, format);

  std::string str;

  CStrUtil::vsprintf(str, format, &vargs);

  auto rc = eval(str);

  va_end(vargs);

  return rc;
}

//---

std::string
CTclApp::
errorInfo_(int rc) const
{
  auto stringFromObj = [](Tcl_Obj *obj) {
    int len = 0;

    char *str = Tcl_GetStringFromObj(obj, &len);

    return std::string(str, size_t(len));
  };

  auto *options = Tcl_GetReturnOptions(interp_, rc);

  auto *key1 = Tcl_NewStringObj("-errorcode", -1);
  auto *key2 = Tcl_NewStringObj("-errorinfo", -1);

  Tcl_Obj *errorMsg;
  Tcl_IncrRefCount(key1);
  Tcl_DictObjGet(nullptr, options, key1, &errorMsg);
  Tcl_DecrRefCount(key1);

  auto msg = stringFromObj(errorMsg);

  Tcl_Obj *stackTrace;
  Tcl_IncrRefCount(key2);
  Tcl_DictObjGet(nullptr, options, key2, &stackTrace);
  Tcl_DecrRefCount(key2);

  auto trace = stringFromObj(stackTrace);

  Tcl_DecrRefCount(options);

  return msg + "\n" + trace;
}

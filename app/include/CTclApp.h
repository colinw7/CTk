#ifndef CTCL_APP_H
#define CTCL_APP_H

#include <QString>
#include <QVariant>

#include <string>
#include <vector>
#include <set>
#include <map>

using ClientData = void *;

struct Tcl_Interp;
struct Tcl_Obj;

#if 0
struct Display;
#else
typedef struct _XDisplay Display;
#endif

using CTclNonConstProc = int (*)(ClientData clientData, Tcl_Interp *interp,
                                 int argc, char **argv);
using CTclConstProc    = int (*)(ClientData clientData, Tcl_Interp *interp,
                                 int argc, const char **argv);

//---

class CTclApp;

class CTclTraceProc {
 public:
  explicit CTclTraceProc(CTclApp *app) : app_(app) { }

  virtual ~CTclTraceProc() = default;

  virtual void handleRead (const char *) { }
  virtual void handleWrite(const char *) { }

  bool isEnabled() const { return enabled_; }
  void setEnabled(bool b) { enabled_ = b; }

 protected:
  CTclApp *app_     { nullptr };
  bool     enabled_ { true };
};

//---

class CTclApp {
 public:
  Tcl_Interp *getInterp() { return interp_; }
  void setInterp(Tcl_Interp *interp) { interp_ = interp; }

  Display *getDisplay();

  bool isInteractive() const { return interactive_; }
  void setInteractive(bool interactive) { interactive_ = interactive; }

  virtual void mainLoop();

  virtual void idle() { }

  void interactiveMainLoop();

  //---

  bool hasResult() const { return resultSet_; }

  void setIntegerResult(int value);
  void setRealResult   (double value);
  void setStringResult (const QString &value);
  void setBoolResult   (bool b);
  void setResult       (const QVariant &var);

  void setIntegerArrayResult(int *values, int num_values);
  void setIntegerArrayResult(const std::vector<int> &values);
  void setIntegerArrayResult(std::initializer_list<int> l);
  void setRealArrayResult   (double *values, int num_values);
  void setRealArrayResult   (const std::vector<double> &values);
  void setStringArrayResult (const std::vector<QString> &strs);

  //---

  bool getStringResult(QString &res) const;

  //---

  void setIntegerGlobalVar(const QString &var, int value);
  void setRealGlobalVar   (const QString &var, double value);
  void setStringGlobalVar (const QString &var, const QString &value);
  void setBoolGlobalVar   (const QString &var, bool value);

  void setStringArrayGlobalVar (const QString &var, const std::vector<QString> &strs);
  void setIntegerArrayGlobalVar(const QString &var, const std::vector<int> &strs);

  void setStringVar(const QString &var, const QString &value);

  void setStringArrayVar (const QString &var, const std::vector<QString> &strs);
  void setIntegerArrayVar(const QString &var, const std::vector<int> &strs);

  //---

  bool hasGlobalVar(const QString &var) const;

  int     getIntegerGlobalVar(const QString &var) const;
  double  getRealGlobalVar   (const QString &var) const;
  QString getStringGlobalVar (const QString &var) const;
  bool    getBoolGlobalVar   (const QString &var) const;

  bool getStringArrayGlobalVar(const QString &var, std::vector<QString> &strs) const;

  //---

  bool splitList(const QString &str, std::vector<QString> &strs) const;

  QString mergeList(const std::vector<QString> &strs) const;

  //---

  void traceGlobalVar  (const QString &name);
  void traceGlobalVar  (const QString &name, CTclTraceProc *proc);
  void untraceGlobalVar(const QString &name);
  void untraceGlobalVar(const QString &name, CTclTraceProc *proc);

  void handleTrace(const char *name, int flags);

  static char *traceProc(ClientData data, Tcl_Interp *,
                         const char *name1, const char *, int flags);
  static char *genTraceProc(ClientData data, Tcl_Interp *,
                            const char *name1, const char *, int flags);

  virtual void handleRead(const char *name);
  virtual void handleWrite(const char *name);
#if 0
  virtual void handleUnset(const char *name);
#endif

  //---

  bool evalFile(const QString &filename) const;

  bool eval(const QString &str) const;

 protected:
  explicit CTclApp(Tcl_Interp *interp);
  explicit CTclApp(int argc, const char **argv);

  virtual ~CTclApp() { }

  void setAppName(const QString &appName);

  bool tclInit();

  void init();

  void initInterp();

  virtual void addCommands() = 0;

  virtual void startup() { }

 private:
  void initCommands();
  void initInteractive();

  QString errorInfo_(int rc) const;

 private:
  using TraceNames     = std::set<QString>;
  using TraceProcs     = std::set<CTclTraceProc *>;
  using NameTraceProcs = std::map<QString, TraceProcs>;

  static CTclApp *app_;

  int          argc_        { 0 };
  const char** argv_        { nullptr };
  Tcl_Interp*  interp_      { nullptr };
  QString      appName_;
  bool         interactive_ { true };

  bool resultSet_ { false };

  TraceNames     traces_;
  NameTraceProcs traceProcs_;
};

#endif

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
  virtual void handleUnset(const char *) { }

  bool isEnabled() const { return enabled_; }
  void setEnabled(bool b) { enabled_ = b; }

 protected:
  CTclApp *app_     { nullptr };
  bool     enabled_ { true };
};

//---

class CTkAppCmdNotify;

class CTclApp {
 public:
  Tcl_Interp *getInterp() const { return interp_; }
  void setInterp(Tcl_Interp *interp) { interp_ = interp; }

  Display *getDisplay();

  bool isInteractive() const { return interactive_; }
  void setInteractive(bool interactive) { interactive_ = interactive; }

  virtual void mainLoop();

  virtual void idle() { }

  void interactiveMainLoop();

  //---

  void addTrace();

  void addCommandTrace   (const char *command, CTkAppCmdNotify *notify);
  void removeCommandTrace(const char *command, CTkAppCmdNotify *notify);

  //---

  bool hasResult() const { return resultSet_; }

  void setIntegerResult(long value);
  void setRealResult   (double value);
  void setStringResult (const QString &value);
  void setBytesResult  (const QByteArray &value);
  void setBoolResult   (bool b);
  void setColorResult  (const QColor &c);
  void setResult       (const QVariant &var);

  void setIntegerArrayResult(long *values, uint num_values);
  void setIntegerArrayResult(const std::vector<long> &values);
  void setIntegerArrayResult(std::initializer_list<long> l);
  void setRealArrayResult   (double *values, uint num_values);
  void setRealArrayResult   (const std::vector<double> &values);
  void setStringArrayResult (const std::vector<QString> &strs);
  void setVariantArrayResult(const std::vector<QVariant> &vars);

  void setVariantListResult(const QVariantList &vars);

  //---

  bool getStringResult(QString &str) const;
  bool getResult      (QVariant &res) const;

  //---

  void setIntegerGlobalVar(const QString &name, long value);
  void setRealGlobalVar   (const QString &name, double value);
  void setBoolGlobalVar   (const QString &name, bool value);
  void setStringGlobalVar (const QString &name, const QString &value);
  void setGlobalVar       (const QString &name, const QVariant &var);

  void setStringLocalVar(const QString &name, const QString &value);
  void setLocalVar      (const QString &name, const QVariant &var);

  void setStringArrayGlobalVar (const QString &name, const std::vector<QString> &strs);
  void setIntegerArrayGlobalVar(const QString &name, const std::vector<long> &values);

  void setStringArrayLocalVar (const QString &name, const std::vector<QString> &strs);
  void setIntegerArrayLocalVar(const QString &name, const std::vector<long> &values);

  //---

  bool hasGlobalVar(const QString &name) const;

  bool     getIntegerGlobalVar(const QString &name, long &i) const;
  bool     getRealGlobalVar   (const QString &name, double &r) const;
  QString  getStringGlobalVar (const QString &name) const;
  bool     getBoolGlobalVar   (const QString &name) const;
  QVariant getGlobalVar       (const QString &name) const;

  bool getStringArrayGlobalVar(const QString &var, std::vector<QString> &strs) const;

  //---

  bool splitList(const QVariant &var, std::vector<QString> &strs) const;
  bool splitList(const QString &str, std::vector<QString> &strs) const;

  QString mergeList(const std::vector<QString> &strs) const;

  //---

  void traceVar        (const QString &name, CTclTraceProc *proc);
  void traceGlobalVar  (const QString &name);
  void traceGlobalVar  (const QString &name, CTclTraceProc *proc);
  void untraceVar      (const QString &name, CTclTraceProc *proc);
  void untraceGlobalVar(const QString &name);
  void untraceGlobalVar(const QString &name, CTclTraceProc *proc);

  void handleTrace(const char *name, int flags);

  static char *traceProc(ClientData data, Tcl_Interp *,
                         const char *name1, const char *, int flags);
  static char *genTraceProc(ClientData data, Tcl_Interp *,
                            const char *name1, const char *, int flags);

  virtual void handleRead (const char *name);
  virtual void handleWrite(const char *name);
  virtual void handleUnset(const char *name);

  int createAlias(const QString &newName, const QString &oldName);

  //---

  bool evalFile(const QString &filename) const;

  bool eval(const QString &str) const;

  bool evalNoExcept(const QString &str) const;

  QString lastErrorInfo() const;

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

  mutable int     lastError_ { 0 };
  mutable QString lastErrorMsg_;
};

#endif

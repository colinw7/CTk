#ifndef CTclAppCommand_H
#define CTclAppCommand_H

class CTclApp;

#include <QString>
#include <QVariant>

#include <vector>
#include <string>

using ClientData = void *;

struct Tcl_Interp;
struct Tcl_Obj;

class CTclAppCommand {
 protected:
  explicit CTclAppCommand(CTclApp *app, const QString &name);

  virtual ~CTclAppCommand();

  const QString &getName() const { return name_; }

  virtual bool proc(int objc, Tcl_Obj* const *objv) = 0;

  void deleteCommand();

  //---

  void setIntegerResult(int value);
  void setRealResult   (double value);
  void setStringResult (const QString &value);
  void setBoolResult   (bool b);
  void setResult       (const QVariant &var);

  void setIntegerArrayResult(int *values, int num_values);
  void setIntegerArrayResult(const std::vector<int> &values);
  void setRealArrayResult   (double *values, int num_values);
  void setRealArrayResult   (const std::vector<double> &values);
  void setStringArrayResult (const std::vector<QString> &strs);

  //---

  void setIntegerVar(const QString &var, int value);
  void setRealVar   (const QString &var, double value);
  void setStringVar (const QString &var, const QString &value);
  void setBoolVar   (const QString &var, bool value);

  long    getIntegerVar(const QString &var) const;
  double  getRealVar   (const QString &var) const;
  QString getStringVar (const QString &var) const;
  bool    getBoolVar   (const QString &var) const;

  bool getStringArrayVar(const QString &var, std::vector<QString> &strs) const;

 private:
  static int commandProc(ClientData clientData, Tcl_Interp *interp,
                         int objc, Tcl_Obj* const *objv);

 protected:
  CTclApp* app_ { nullptr };
  QString  name_;
  bool     cmdDeleted_ { false };
};

#endif

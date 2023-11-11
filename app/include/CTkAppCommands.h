#ifndef CTkAppCommands_H
#define CTkAppCommands_H

#include <CTclAppCommand.h>
#include <CTkAppOptData.h>

class CTkAppRoot;
class CTkAppWidget;

namespace CTkAppCommands {

void addCommands(CTkApp *tk);

}

//---

class CTkAppCommand : public CTclAppCommand {
 public:
  using Args = std::vector<QVariant>;

 public:
  explicit CTkAppCommand(CTkApp *tk, const QString &name);

  CTkApp *getTk() const { return tk_; }

  CTkAppRoot *root() const;

  const QString &getName() const { return CTclAppCommand::getName(); }

  void deleteCommand() const { return
    const_cast<CTkAppCommand *>(this)->CTclAppCommand::deleteCommand(); }

 protected:
  bool proc(int objc, Tcl_Obj * const *objv) override;

  virtual bool run(const Args &args) = 0;

 protected:
  CTkApp *tk_ { nullptr };

  mutable QString arg0_;
};

//---

class CTkAppOptsCommand :  public CTkAppCommand {
 public:
  CTkAppOptsCommand(CTkApp *tk, const QString &name);

  const CTkAppOptData &getOpts() const { return opts_; }

  bool execCGet(const Args &args);

  bool execConfigure(const Args &args);

  bool getOptValue(const QString &name, QVariant &value);

  virtual bool setOptValue(const QString &name, const QVariant &value);

 protected:
  CTkAppOptData opts_;
};

//---

class CTkAppRootCommand :  public CTkAppOptsCommand {
 public:
  CTkAppRootCommand(CTkApp *tk);

  bool run(const Args &args) override;

  bool setOptValue(const QString &name, const QVariant &value) override;
};

//---

class CTkAppWidgetCommand : public CTkAppOptsCommand {
 public:
  using Args = std::vector<QVariant>;

 public:
  explicit CTkAppWidgetCommand(CTkAppCommand *command, const QString &name,
                               CTkAppWidget *w, const CTkAppOpt *opts);
  explicit CTkAppWidgetCommand(CTkApp *app, const QString &name,
                               CTkAppWidget *w, const CTkAppOpt *opts);

 ~CTkAppWidgetCommand();

  bool run(const Args &args) override;

  bool processArgs(const Args &args);

  bool setOptValue(const QString &name, const QVariant &value) override;

 private:
  CTkAppCommand* command_ { nullptr };
  CTkAppWidget*  w_       { nullptr };
};

//---

class CTkAppImageCommand : public CTkAppCommand {
 public:
  explicit CTkAppImageCommand(CTkApp *app, const QString &name, const QString &type);

 ~CTkAppImageCommand();

  bool run(const Args &args) override;

 private:
  CTkApp* app_ { nullptr };
  QString type_;
};

//---

class QEventLoop;

class CTkWindowDestroyEventLoop : public QObject {
  Q_OBJECT

 public:
  explicit CTkWindowDestroyEventLoop(CTclApp *app, QWidget *w);

 ~CTkWindowDestroyEventLoop();

  void startLoop();

 private Q_SLOTS:
  void destroySlot();

 protected:
  CTclApp*    app_       { nullptr };
  QWidget*    w_         { nullptr };
  QEventLoop* eventLoop_ { nullptr };
};

//---

class QEventLoop;

class CTkWindowVisibleEventLoop : public QObject {
  Q_OBJECT

 public:
  explicit CTkWindowVisibleEventLoop(CTclApp *app, QWidget *w);

 ~CTkWindowVisibleEventLoop();

  void startLoop();

 protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

 protected:
  CTclApp*    app_       { nullptr };
  QWidget*    w_         { nullptr };
  QEventLoop* eventLoop_ { nullptr };
};

#endif

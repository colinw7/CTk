#ifndef CTkAppCommands_H
#define CTkAppCommands_H

#include <CTkAppCommand.h>
#include <CTkAppOptData.h>

#include <QDialog>

class CTkAppRoot;
class CTkAppWidget;

namespace CTkAppCommands {

void addCommands(CTkApp *tk);

}

//---

class CTkAppOptsCommand :  public CTkAppCommand {
 public:
  CTkAppOptsCommand(CTkApp *tk, const QString &name);

  const CTkAppOptData &getOpts() const { return opts_; }

  bool getNameValue(const Args &args, QString &name, QVariant &value);

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

  bool setNameValue(const QString &name, const QVariant &value);

 private:
  CTkAppCommand* command_ { nullptr };
  CTkAppWidget*  w_       { nullptr };
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

//---

class CTkDialog : public QDialog {
  Q_OBJECT

 public:
  CTkDialog(QWidget *parent);

 private Q_SLOTS:
  void buttonSlot();
};

#endif

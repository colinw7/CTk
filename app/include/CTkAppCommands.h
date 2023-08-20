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

 protected:
  bool proc(int objc, Tcl_Obj * const *objv) override;

  virtual bool run(const Args &args) = 0;

  CTkAppRoot *root() const;

 protected:
  CTkApp *tk_ { nullptr };

  mutable QString arg0_;
};

//---

class CTkAppWidgetCommand : public CTkAppCommand {
 public:
  explicit CTkAppWidgetCommand(CTkAppCommand *command, const QString &name,
                               CTkAppWidget *w, const CTkAppOpt *opts = nullptr);
  explicit CTkAppWidgetCommand(CTkApp *app, const QString &name,
                               CTkAppWidget *w, const CTkAppOpt *opts = nullptr);

 ~CTkAppWidgetCommand();

  bool run(const Args &args) override;

  const CTkAppOptData &getOpts() const { return opts_; }

 public:
  using Args = std::vector<QVariant>;

  bool processArgs(const Args &args);

  bool getOptValue(const QString &name, QString &value);
  bool setOptValue(const QString &name, const QString &value);

 private:
  CTkAppCommand* command_ { nullptr };
  CTkAppWidget*  w_       { nullptr };
  CTkAppOptData  opts_;
};

//---

class CTkAppImageCommand : public CTkAppCommand {
 public:
  explicit CTkAppImageCommand(CTkApp *app, const QString &name);

 ~CTkAppImageCommand();

  bool run(const Args &args) override;

 private:
  CTkApp* app_ { nullptr };
};

#endif

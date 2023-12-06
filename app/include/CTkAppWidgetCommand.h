#ifndef CTkAppWidgetCommand_H
#define CTkAppWidgetCommand_H

#include <CTkAppCommands.h>

class CTkAppCommand;

class CTkAppWidgetCommand : public CTkAppOptsCommand {
 public:
  using Args = std::vector<QVariant>;

 public:
  explicit CTkAppWidgetCommand(CTkAppCommand *command, const QString &name,
                               CTkAppWidget *w, const CTkAppOpt *opts);
  explicit CTkAppWidgetCommand(CTkApp *app, const QString &name,
                               CTkAppWidget *w, const CTkAppOpt *opts);

 ~CTkAppWidgetCommand();

  bool isInCreate() const { return inCreate_; }
  void setInCreate(bool b) { inCreate_ = b; }

  bool run(const Args &args) override;

  bool processArgs(const Args &args, bool inCreate);

  bool setOptValue(const QString &name, const QVariant &value) override;

  bool setNameValue(const QString &name, const QVariant &value);

 private:
  CTkAppCommand* command_  { nullptr };
  CTkAppWidget*  w_        { nullptr };
  bool           inCreate_ { false };
};

#endif

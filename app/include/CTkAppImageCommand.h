#ifndef CTkAppImageCommand_H
#define CTkAppImageCommand_H

#include <CTkAppCommand.h>

class CTkAppImageCommand : public CTkAppCommand {
 public:
  explicit CTkAppImageCommand(CTkApp *app, const QString &name, const QString &type);

 ~CTkAppImageCommand();

  bool run(const Args &args) override;

 private:
  CTkApp* app_ { nullptr };
  QString type_;
};

#endif

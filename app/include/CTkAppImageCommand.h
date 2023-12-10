#ifndef CTkAppImageCommand_H
#define CTkAppImageCommand_H

#include <CTkAppCommand.h>
#include <CTkApp.h>

class CTkAppImageCommand : public CTkAppCommand {
 public:
  explicit CTkAppImageCommand(CTkApp *app, const QString &name, const QString &type);

 ~CTkAppImageCommand();

  bool run(const Args &args) override;

  static bool processOption(CTkApp *tk, CTkAppImageRef &image, const QString &type,
                            const QString &name, const QVariant &var);

  static bool lookupFormatName(CTkApp *app, const QString &format, QString &format1);

 private:
  CTkApp* app_ { nullptr };
  QString type_;
};

#endif

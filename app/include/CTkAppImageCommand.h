#ifndef CTkAppImageCommand_H
#define CTkAppImageCommand_H

#include <CTkAppCommand.h>
#include <CTkApp.h>
#include <CTkAppCmdNotify.h>

class CTkAppImageCommand : public CTkAppCommand, public CTkAppCmdNotify {
 public:
  explicit CTkAppImageCommand(CTkApp *app, const QString &name, const QString &type);

 ~CTkAppImageCommand();

  uint imageId() const { return imageId_; }
  void setImageId(uint i) { imageId_ = i; }

  bool run(const Args &args) override;

  static bool processOption(CTkApp *tk, CTkAppImageRef &image, const QString &type,
                            const QString &name, const QVariant &var);

  void handleRename(const char *oldName, const char *newName) override;
  void handleDelete(const char *name) override;

 private:
  CTkApp* app_ { nullptr };
  QString type_;
  uint    imageId_ { 0 };
};

#endif

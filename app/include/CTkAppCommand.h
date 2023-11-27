#ifndef CTkAppCommand_H
#define CTkAppCommand_H

#include <CTclAppCommand.h>

class CTkApp;
class CTkAppRoot;

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

#endif

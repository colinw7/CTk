#ifndef CTkAppSpinBox_H
#define CTkAppSpinBox_H

#include <CTkAppWidget.h>

class CTkAppSpinBoxWidget;
class CTkAppSpinBoxVarProc;
class CTkAppSpinBoxValidator;

class CTkAppSpinBox : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName        READ varName           WRITE setVarName)
  Q_PROPERTY(QString invalidCommand READ getInvalidCommand WRITE setInvalidCommand)

 public:
  explicit CTkAppSpinBox(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "SpinBox"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  const QString &getInvalidCommand() const { return invalidCommand_; }
  void setInvalidCommand(const QString &command) { invalidCommand_ = command; }

  //---

  void setText(const QString &text) override;

  bool validate(const QString &) const;

  void updateFromVar();

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void valueChangedSlot();

 private:
  enum class ValidateMode {
    NONE,
    FOCUS,
    FOCUSIN,
    FOCUSOUT,
    KEY,
    ALL
  };

  CTkAppSpinBoxWidget*    qspin_ { nullptr };
  QString                 varName_;
  CTkAppSpinBoxVarProc*   varProc_;
  ValidateMode            validateMode_ { ValidateMode::NONE };
  QString                 validateCmd_;
  CTkAppSpinBoxValidator* validateProc_ { nullptr };
  QString                 invalidCommand_;
};

#endif

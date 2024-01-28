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

  const char *getClassName() const override { return "Spinbox"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  const QString &getInvalidCommand() const { return invalidCommand_; }
  void setInvalidCommand(const QString &command) { invalidCommand_ = command; }

  bool isExportSelection() const { return exportSelection_; }
  void setExportSelection(bool b) { exportSelection_ = b; }

  const QColor &readOnlyBackground() const { return readOnlyBackground_; }
  void setReadOnlyBackground(const QColor &c) { readOnlyBackground_ = c; }

  const QColor &buttonBackground() const { return buttonBackground_; }
  void setButtonBackground(const QColor &c) { buttonBackground_ = c; }

  const CTkAppCursorData &buttonCursor() const { return buttonCursor_; }
  void setButtonCursor(const CTkAppCursorData &c) { buttonCursor_ = c; }

  const QColor &disabledBackground() const { return disabledBackground_; }
  void setDisabledBackground(const QColor &c) { disabledBackground_ = c; }

  const QColor &disabledForeground() const { return disabledForeground_; }
  void setDisabledForeground(const QColor &c) { disabledForeground_ = c; }

  const QString &format() const { return format_; }
  void setFormat(const QString &s) { format_ = s; }

  bool isWrap() const { return wrap_; }
  void setWrap(bool b) { wrap_ = b; }

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

  CTkAppSpinBoxWidget* qspin_ { nullptr };

  QString               varName_;
  CTkAppSpinBoxVarProc* varProc_;

  ValidateMode            validateMode_ { ValidateMode::NONE };
  QString                 validateCmd_;
  CTkAppSpinBoxValidator* validateProc_ { nullptr };

  QString invalidCommand_;

  bool exportSelection_ { true };

  QColor readOnlyBackground_;

  QColor           buttonBackground_;
  CTkAppCursorData buttonCursor_;

  QColor disabledBackground_;
  QColor disabledForeground_;

  QString format_;

  bool wrap_ { false };
};

#endif

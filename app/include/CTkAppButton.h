#ifndef CTkAppButton_H
#define CTkAppButton_H

#include <CTkAppWidget.h>

class CTkAppButtonWidget;
class CTkAppButtonVarProc;

class CTkAppButton : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(bool overRaised READ isOverRaised WRITE setOverRaised)

 public:
  explicit CTkAppButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  ~CTkAppButton() override;

  const char *getClassName() const override { return "Button"; }

  //---

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  bool isOverRaised() const { return overRaised_; }
  void setOverRaised(bool b) { overRaised_ = b; }

  //---

  void setText(const QString &s) override;

  void updateFromVar();

  void setImageRef(const CTkAppImageRef &i) override;

  void setJustify(const Qt::Alignment &a) override;

  void flash();

  void setPadX(int i) override;
  void setPadY(int i) override;

  void appearanceChanged() override;

 private:
  void connectSlots(bool b);

 private Q_SLOTS:
  void clickSlot();

 private:
  CTkAppButtonWidget*  qbutton_    { nullptr };
  QString              varName_;
  CTkAppButtonVarProc* varProc_    { nullptr };
  bool                 overRaised_ { false };
};

#endif

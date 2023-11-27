#ifndef CTkAppMenuButton_H
#define CTkAppMenuButton_H

#include <CTkAppWidget.h>

class CTkAppMenuButtonWidget;
class CTkAppMenuButtonVarProc;
class CTkAppMenu;

class CTkAppMenuButton : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName       READ varName         WRITE setVarName)
  Q_PROPERTY(bool    showIndicator READ isShowIndicator WRITE setShowIndicator)

 public:
  explicit CTkAppMenuButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "MenuButton"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &menuName() const { return menuName_; }
  void setMenuName(const QString &s) { menuName_ = s; }

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  bool isShowIndicator() const { return showIndicator_; }
  void setShowIndicator(bool b) { showIndicator_ = b; }

  //---

  void setText(const QString &text) override;

  void setImageRef(const CTkAppImageRef &i) override;

  void updateMenu();

  void setMenu(CTkAppMenu *menu);

  void updateFromVar();

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void clickSlot();

 private:
  CTkAppMenuButtonWidget*  qbutton_ { nullptr };
  QString                  menuName_;
  QString                  varName_;
  CTkAppMenuButtonVarProc* varProc_ { nullptr };
  bool                     showIndicator_ { true };
};

//---

class CTkAppMenuButtonWidget : public QToolButton {
  Q_OBJECT

 public:
  explicit CTkAppMenuButtonWidget(CTkAppMenuButton *button);

  bool isReadOnly() const { return readOnly_; }
  void setReadOnly(bool b) { readOnly_ = b; }

 private:
  CTkAppMenuButton *button_ { nullptr };

  bool readOnly_ { false };
};

#endif

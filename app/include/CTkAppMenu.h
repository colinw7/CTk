#ifndef CTkAppMenu_H
#define CTkAppMenu_H

#include <CTkAppWidget.h>

#include <QRadioButton>

class CTkAppCheckAction : public QWidgetAction {
  Q_OBJECT

  Q_PROPERTY(QString text    READ text    WRITE setText)
  Q_PROPERTY(QString varName READ varName WRITE setVarName)

 public:
  explicit CTkAppCheckAction(CTkApp* tk) :
   QWidgetAction(nullptr), tk_(tk) {
  }

  QWidget *createWidget(QWidget *parent) override {
    check_ = new QCheckBox(parent);

    if (getText() != "")
      check_->setText(getText());

    connect(check_, SIGNAL(stateChanged(int)), this, SIGNAL(stateChanged(int)));

    return check_;
  }

  //---

  const QString &getText() const { return text_; }
  void setText(const QString &s) { text_ = s; }

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  //---

  void updateFromVar();

 Q_SIGNALS:
  void stateChanged(int);

 private:
  CTkApp*    tk_ { nullptr };
  QCheckBox* check_ { nullptr };
  QString    text_;
  QString    varName_;
};

//---

class CTkAppRadioAction : public QWidgetAction {
  Q_OBJECT

  Q_PROPERTY(QString text    READ text    WRITE setText)
  Q_PROPERTY(QString value   READ value   WRITE setValue)
  Q_PROPERTY(QString varName READ varName WRITE setVarName)

 public:
  explicit CTkAppRadioAction(CTkApp* tk) :
   QWidgetAction(nullptr), tk_(tk) {
  }

  QWidget *createWidget(QWidget *parent) override {
    radio_ = new QRadioButton(parent);

    if (getText() != "")
      radio_->setText(getText());

    connect(radio_, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));

    return radio_;
  }

  //---

  const QString &getText() const { return text_; }
  void setText(const QString &s) { text_ = s; }

  const QString &value() const { return value_; }
  void setValue(const QString &s) { value_ = s; }

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  //---

  void updateFromVar();

 Q_SIGNALS:
  void toggled(bool);

 private:
  CTkApp*       tk_ { nullptr };
  QRadioButton* radio_ { nullptr };
  QString       text_;
  QString       value_;
  QString       varName_;
};

//---

class CTkAppSubMenu : public QMenu {
  Q_OBJECT

 public:
  explicit CTkAppSubMenu(CTkApp* tk);

  const QString &menu() const { return menu_; }
  void setMenu(const QString &s) { menu_ = s; }

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void showSlot();
  void hideSlot();

 private:
  CTkApp* tk_ { nullptr };
  QString menu_;
};

class CTkAppMenu : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppMenu(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Menu"; }

  QMenu *qmenu() { return qmenu_; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void actionPressedSlot();

  void checkChangedSlot(int);

  void radioToggledSlot(bool);

 private:
  QMenu* qmenu_   { nullptr };
  bool   tearOff_ { true };
};

#endif

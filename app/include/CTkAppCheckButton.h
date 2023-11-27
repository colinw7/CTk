#ifndef CTkAppCheckButton_H
#define CTkAppCheckButton_H

#include <CTkAppWidget.h>

class CTkAppCheckButtonWidget;
class CTkAppCheckButtonVarProc;

class CTkAppCheckButton : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName       READ varName         WRITE setVarName)
  Q_PROPERTY(QString onValue       READ onValueStr      WRITE setOnValueStr)
  Q_PROPERTY(QString offValue      READ offValueStr     WRITE setOffValueStr)
  Q_PROPERTY(QColor  selectColor   READ selectColor     WRITE setSelectColor)
  Q_PROPERTY(bool    showIndicator READ isShowIndicator WRITE setShowIndicator)
  Q_PROPERTY(bool    overRaised    READ isOverRaised    WRITE setOverRaised)
  Q_PROPERTY(int     underlinePos  READ underlinePos    WRITE setUnderlinePos)

 public:
  using OptString = std::optional<QString>;

 public:
  explicit CTkAppCheckButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Checkbutton"; }

  //---

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  const OptString &onValue() const { return onValue_; }
  void setOnValue(const OptString &s) { onValue_ = s; }

  QString onValueStr() const { return onValue().value_or(""); }
  void setOnValueStr(const QString &s) { setOnValue(s); }

  const OptString &offValue() const { return offValue_; }
  void setOffValue(const OptString &s) { offValue_ = s; }

  QString offValueStr() const { return offValue().value_or(""); }
  void setOffValueStr(const QString &s) { setOffValue(s); }

  const QColor &selectColor() const { return selectColor_; }
  void setSelectColor(const QColor &c) { selectColor_ = c; }

  bool isShowIndicator() const { return showIndicator_; }
  void setShowIndicator(bool b) { showIndicator_ = b; }

  bool isOverRaised() const { return overRaised_; }
  void setOverRaised(bool b) { overRaised_ = b; }

  int underlinePos() const { return underlinePos_; }
  void setUnderlinePos(int i) { underlinePos_ = i; }

  //---

  void setText(const QString &text) override;

  void setChecked(bool b);

  void updateFromVar();
  void updateToVar();

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void stateChangedSlot(int);

 private:
  CTkAppCheckButtonWidget*  qcheck_  { nullptr };
  QString                   varName_;
  CTkAppCheckButtonVarProc* varProc_ { nullptr };
  OptString                 onValue_;
  OptString                 offValue_;
  QColor                    selectColor_;
  bool                      showIndicator_ { true };
  bool                      overRaised_ { false };
  int                       underlinePos_ { -1 };
};

class CTkAppCheckButtonWidget : public QCheckBox {
  Q_OBJECT

 public:
  explicit CTkAppCheckButtonWidget(CTkAppCheckButton *check);

  const QString &getLabel() const { return text_; }
  void setLabel(const QString &s) { text_ = s; updateText(); }

  bool isReadOnly() const { return readOnly_; }
  void setReadOnly(bool b) { readOnly_ = b; }

  double wrapLength() const { return wrapLength_; }
  void setWrapLength(double r) { wrapLength_ = r; updateText(); }

 private:
  void updateText();

 private:
  CTkAppCheckButton *check_ { nullptr };

  QString text_;
  bool    readOnly_   { false };
  double  wrapLength_ { -1 };
};

#endif

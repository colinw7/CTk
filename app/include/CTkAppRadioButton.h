#ifndef CTkAppRadioButton_H
#define CTkAppRadioButton_H

#include <CTkAppWidget.h>

#include <QRadioButton>

class CTkAppRadioButtonWidget;
class CTkAppRadioButtonVarProc;

class CTkAppRadioButton : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName       READ varName         WRITE setVarName)
  Q_PROPERTY(QString value         READ value           WRITE setValue)
  Q_PROPERTY(QColor  selectColor   READ selectColor     WRITE setSelectColor)
  Q_PROPERTY(bool    showIndicator READ isShowIndicator WRITE setShowIndicator)
  Q_PROPERTY(bool    overRaised    READ isOverRaised    WRITE setOverRaised)
  Q_PROPERTY(double  userWidth     READ userWidth       WRITE setUserWidth)
  Q_PROPERTY(double  userHeight    READ userHeight      WRITE setUserHeight)

 public:
  struct RadioGroupData {
    QButtonGroup*                 group { nullptr };
    std::set<CTkAppRadioButton *> buttons;
  };

  using RadioGroups = std::map<QString, RadioGroupData>;

 public:
  static void addRadioGroup   (CTkAppRadioButton *radio, const QString &varName);
  static void removeRadioGroup(CTkAppRadioButton *radio, const QString &varName);

 public:
  explicit CTkAppRadioButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");
 ~CTkAppRadioButton();

  const char *getClassName() const override { return "Radiobutton"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  CTkAppRadioButtonWidget *qradio() const { return qradio_; }

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s);

  const QString &value() const { return value_; }
  void setValue(const QString &value);

  const QString &triStateValue() const { return triStateValue_; }
  void setTriStateValue(const QString &value) { triStateValue_ = value; }

  const QColor &selectColor() const { return selectColor_; }
  void setSelectColor(const QColor &c) { selectColor_ = c; }

  bool isShowIndicator() const { return showIndicator_; }
  void setShowIndicator(bool b) { showIndicator_ = b; }

  bool isOverRaised() const { return overRaised_; }
  void setOverRaised(bool b) { overRaised_ = b; }

  int userWidth() const { return userWidth_; }
  void setUserWidth(int i) { userWidth_ = i; }

  int userHeight() const { return userHeight_; }
  void setUserHeight(int i) { userHeight_ = i; }

  //---

  void setText(const QString &text) override;

  void setImageRef(const CTkAppImageRef &i) override;

  void setChecked(bool b);

  void updateFromVar();
  void updateToVar();
  void updateToVar(bool checked);

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void toggleSlot(bool);

 private:
  static RadioGroups s_radioGroups;

  CTkAppRadioButtonWidget*  qradio_ { nullptr };
  QString                   varName_;
  QString                   value_;
  CTkAppRadioButtonVarProc* varProc_ { nullptr };
  QString                   triStateValue_;
  QColor                    selectColor_;
  bool                      showIndicator_ { true };
  bool                      overRaised_ { false };
  double                    userWidth_  { -1 };
  double                    userHeight_ { -1 };
};

//---

class CTkAppRadioButtonWidget : public QRadioButton {
  Q_OBJECT

 public:
  explicit CTkAppRadioButtonWidget(CTkAppRadioButton *radio);

  const QString &getText() const { return text_; }
  void setText(const QString &s) { text_ = s; updateText(); }

  //! get/set image
  const QImage &image() const { return image_; }
  void setImage(const QImage &i) { image_ = i; update(); }

  bool isReadOnly() const { return readOnly_; }
  void setReadOnly(bool b) { readOnly_ = b; }

  double wrapLength() const { return wrapLength_; }
  void setWrapLength(double r) { wrapLength_ = r; updateText(); }

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override;

 private:
  void updateText();

 private:
  CTkAppRadioButton *radio_ { nullptr };

  QString text_;
  QImage  image_;
  bool    readOnly_ { false };

  double wrapLength_ { -1 };
};

#endif

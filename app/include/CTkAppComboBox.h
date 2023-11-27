#ifndef CTkAppComboBox_H
#define CTkAppComboBox_H

#include <CTkAppWidget.h>

class CTkAppComboBoxWidget;
class CTkAppComboBoxVarProc;

class CTkAppComboBox : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName READ varName WRITE setVarName)

 public:
  explicit CTkAppComboBox(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "ComboBox"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  //---

  void updateFromVar();

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void indexChangedSlot(int);

 private:
  CTkAppComboBoxWidget*  qcombo_ { nullptr };
  QString                varName_;
  CTkAppComboBoxVarProc* varProc_ { nullptr };
};

class CTkAppComboBoxWidget : public QComboBox {
  Q_OBJECT

 public:
  CTkAppComboBoxWidget(CTkAppComboBox *combo);

  int width() const { return width_; }
  void setWidth(int i) { width_ = i; }

  bool isReadOnly() const { return readOnly_; }
  void setReadOnly(bool b) { readOnly_ = b; }

  QSize sizeHint() const override;

 private:
  CTkAppComboBox* combo_ { nullptr };

  int  width_    { -1 };
  bool readOnly_ { false };
};

#endif

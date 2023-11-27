#ifndef CTkAppScale_H
#define CTkAppScale_H

#include <CTkAppWidget.h>

class CTkAppScaleVarProc;

class CTkAppScale : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName READ varName WRITE setVarName)

 public:
  explicit CTkAppScale(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Scale"; }

  CQRealSlider *qscale() const { return qscale_; }

  const QColor &troughColor() const { return troughColor_; }
  void setTroughColor(const QColor &c) { troughColor_ = c; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  //---

  void updateFromVar();
  void updateToVar();

 private:
  void connectSlots(bool);

  void updateSize();

 private Q_SLOTS:
  void valueSlot(double);

 private:
  CQRealSlider* qscale_ { nullptr };

  QString             varName_;
  CTkAppScaleVarProc* varProc_ { nullptr };
  int                 length_  { 0 };
  int                 width_   { 0 };
  QColor              troughColor_;
};

#endif

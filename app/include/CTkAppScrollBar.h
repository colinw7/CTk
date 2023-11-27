#ifndef CTkAppScrollBar_H
#define CTkAppScrollBar_H

#include <CTkAppWidget.h>

class CTkAppScrollBar;

class CTkAppScrollBarWidget : public QScrollBar {
  Q_OBJECT

 public:
  explicit CTkAppScrollBarWidget(CTkAppScrollBar *scrollbar);

  const QColor &troughColor() const { return troughColor_; }
  void setTroughColor(const QColor &c) { troughColor_ = c; }

  double getRValue() const;
  void setRValue(double r);

  void setBounds(double pos1, double pos2);

  double getSliderPos() const;

 private:
  CTkAppScrollBar* scrollbar_ { nullptr };

  QColor troughColor_;
  int    resolution_ { 1000 };
};

class CTkAppScrollBar : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppScrollBar(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  ~CTkAppScrollBar() override;

  const char *getClassName() const override { return "Scrollbar"; }

  const Relief &activeRelief() const { return activeRelief_; }
  void setActiveRelief(const Relief &v) { activeRelief_ = v; }

  double elementBorderWidth() const { return borderWidth_; }
  void setElementBorderWidth(double width) { elementBorderWidth_ = width; }

  bool isJump() const { return jump_; }
  void setJump(bool b) { jump_ = b; }

  const QString &activeElement() const { return activeElement_; }
  void setActiveElement(const QString &s) { activeElement_ = s; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void scrollValueSlot(int);
  void scrollActionSlot(int);

 private:
  CTkAppScrollBarWidget* qscrollbar_   { nullptr };
  Relief                 activeRelief_ { Relief::NONE };
  double                 elementBorderWidth_ { 0 };
  bool                   jump_ { false };
  QString                activeElement_;
};

#endif

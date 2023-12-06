#ifndef CTkAppLabelFrame_H
#define CTkAppLabelFrame_H

#include <CTkAppWidget.h>

class CTkAppLabelFrame : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppLabelFrame(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "LabelFrame"; }

  void setText(const QString &text) override;

  const Qt::Alignment &labelAnchor() const { return labelAnchor_; }
  void setLabelAnchor(const Qt::Alignment &v) { labelAnchor_ = v; }

  const AppWidgetP &labelWidget() const { return labelWidget_; }
  void setLabelWidget(const AppWidgetP &v) { labelWidget_ = v; }

  bool isContainer() const { return container_; }
  void setContainer(bool b) { container_ = b; }

  //---

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

 private:
  QGroupBox* qframe_ { nullptr };

  Qt::Alignment labelAnchor_ {  Qt::AlignCenter };
  AppWidgetP    labelWidget_;
  bool          container_ { false };
};

#endif

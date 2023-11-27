#ifndef CTkAppLabelFrame_H
#define CTkAppLabelFrame_H

#include <CTkAppWidget.h>

class CTkAppLabelFrame : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppLabelFrame(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "LabelFrame"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  void setText(const QString &text) override;

 private:
  QGroupBox* qframe_ { nullptr };
};

#endif

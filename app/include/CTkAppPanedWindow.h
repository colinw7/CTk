#ifndef CTkAppPanedWindow_H
#define CTkAppPanedWindow_H

#include <CTkAppWidget.h>

class CTkAppPanedWindow : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppPanedWindow(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "PanedWindow"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

 private:
  QSplitter* qpane_ { nullptr };
};

#endif

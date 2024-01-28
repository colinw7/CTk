#ifndef CTkAppRoot_H
#define CTkAppRoot_H

#include <CTkAppWidget.h>

class CTkAppRootWidget;

class CTkAppRoot : public CTkAppWidgetRoot {
  Q_OBJECT

 public:
  explicit CTkAppRoot(CTkApp *tk);

  void setRoot(QFrame *qroot);

  const char *getClassName() const override { return "Root"; }

  bool isRoot() const override { return true; }

  bool show() override;

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  bool decodeWidgetName(const QVariant &name, CTkAppWidget **parent, QString &childName) const;

 private:
  QFrame* qroot_ { nullptr };

  bool    needsShow_ { false };
  QString menuName_;
};

//---

class CTkAppRootWidget : public QFrame {
  Q_OBJECT

 public:
  explicit CTkAppRootWidget(CTkAppRoot *root);

  void setMenu(QMenu *menu);

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override;

 private:
  CTkAppRoot* root_    { nullptr };
  QMenu*      menu_    { nullptr };
  QMenuBar*   menuBar_ { nullptr };
};

#endif

#ifndef CTkAppTopLevel_H
#define CTkAppTopLevel_H

#include <CTkAppWidget.h>

class CTkAppTopLevelWidget;

class CTkAppTopLevel : public CTkAppWidgetRoot {
  Q_OBJECT

  Q_PROPERTY(bool iconWindow READ isIconWindow WRITE setIconWindow)

 public:
  explicit CTkAppTopLevel(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

 ~CTkAppTopLevel();

  const char *getClassName() const override { return "TopLevel"; }

  bool isContainer() const { return container_; }
  void setContainer(bool b) { container_ = b; }

  //---

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  void setTopLevelWidget(CTkAppTopLevelWidget *qtoplevel);

  bool show() override;
  void hide() override;

 private:
  CTkAppTopLevelWidget* qtoplevel_ { nullptr };

  bool container_ { false };
};

//---

class CTkAppTopLevelWidget : public QFrame {
  Q_OBJECT

 public:
  explicit CTkAppTopLevelWidget(CTkAppTopLevel *toplevel);

  QSize sizeHint() const override;

 private:
  CTkAppTopLevel* toplevel_ { nullptr };
};

#endif

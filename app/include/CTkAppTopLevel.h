#ifndef CTkAppTopLevel_H
#define CTkAppTopLevel_H

#include <CTkAppWidget.h>

class CTkAppTopLevelWidget;

class CTkAppTopLevel : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(bool iconWindow READ isIconWindow WRITE setIconWindow)

 public:
  using TopLevelP = QPointer<CTkAppTopLevel>;

 public:
  explicit CTkAppTopLevel(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

 ~CTkAppTopLevel();

  const char *getClassName() const override { return "TopLevel"; }

  bool isTopLevel() const override { return true; }

  bool isNeedsShow() const { return needsShow_; }
  void setNeedsShow(bool b) override { needsShow_ = b; }

  const QString &menuName() const { return menuName_; }
  void setMenuName(const QString &s) { menuName_ = s; }

  bool isIconWindow() const { return iconWindow_; }
  void setIconWindow(bool b) { iconWindow_ = b; }

  const TopLevelP iconWindowP() const { return iconWindowP_; }
  void setIconWindowP(const TopLevelP &w) { iconWindowP_ = w; }

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

  bool      needsShow_  { false };
  QString   menuName_;
  bool      iconWindow_ { false };
  TopLevelP iconWindowP_;
  bool      container_  { false };
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

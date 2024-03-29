#ifndef CTkAppListBox_H
#define CTkAppListBox_H

#include <CTkAppWidget.h>

class CTkAppListBoxWidget;
class CTkAppListBoxVarProc;

class CTkAppListBox : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName READ varName WRITE setVarName)

 public:
  explicit CTkAppListBox(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Listbox"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  //---

  bool isExportSelection() const { return exportSelection_; }
  void setExportSelection(bool b) { exportSelection_ = b; }

  int activeIndex() const { return activeIndex_; }
  void setActiveIndex(int i) { activeIndex_ = i; }

  int selectionIndex() const { return selectionIndex_; }
  void setSelectionIndex(int i) { selectionIndex_ = i; }

  const QString &activeStyle() const { return activeStyle_; }
  void setActiveStyle(const QString &s) { activeStyle_ = s; }

  const QString &selectMode() const { return selectMode_; }
  void setSelectMode(const QString &s) { selectMode_ = s; }

  //---

  void updateFromVar();

 private:
  void connectSlots(bool b);

 private Q_SLOTS:
  void selectionSlot();

  void vscrollSlot(int value);
  void hscrollSlot(int value);

 private:
  CTkAppListBoxWidget* qlist_ { nullptr };

  QString               varName_;
  CTkAppListBoxVarProc* varProc_ { nullptr };

  bool    exportSelection_ { true };
  int     activeIndex_     { -1 };
  QString activeStyle_;
  int     selectionIndex_  { -1 };
  QString selectMode_;
};

//---

class CTkAppListBoxWidget : public QListWidget {
  Q_OBJECT

 public:
  using OptReal = std::optional<double>;

 public:
  CTkAppListBoxWidget(CTkAppListBox *listbox);

  const OptReal &width() const { return width_; }
  void setWidth(const OptReal &w);

  const OptReal &height() const { return height_; }
  void setHeight(const OptReal &h);

  bool isReadOnly() const { return readOnly_; }
  void setReadOnly(bool b) { readOnly_ = b; }

  QSize sizeHint() const override;

 private:
  CTkAppListBox* listbox_ { nullptr };

  OptReal width_;
  OptReal height_;
  bool    readOnly_ { false };
};

#endif

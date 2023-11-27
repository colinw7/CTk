#ifndef CTkAppEntry_H
#define CTkAppEntry_H

#include <CTkAppWidget.h>

class CTkAppEntryWidget;
class CTkAppEntryVarProc;
class CTkAppEntryValidator;

class CTkAppEntry : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName READ varName WRITE setVarName)

 public:
  explicit CTkAppEntry(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Entry"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  //---

  void setText(const QString &text) override;

  bool validate(const QString &) const;

  void updateFromVar();

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void valueChangedSlot();

 private:
  enum class ValidateMode {
    NONE,
    FOCUS,
    FOCUSIN,
    FOCUSOUT,
    KEY,
    ALL
  };

  CTkAppEntryWidget*    qedit_ { nullptr };
  QString               varName_;
  CTkAppEntryVarProc*   varProc_ { nullptr };
  ValidateMode          validateMode_ { ValidateMode::NONE };
  QString               validateCmd_;
  CTkAppEntryValidator* validateProc_ { nullptr };
};

class CTkAppEntryWidget : public QLineEdit {
  Q_OBJECT

 public:
  CTkAppEntryWidget(CTkAppEntry *entry);

  int width() const { return width_; }
  void setWidth(int i) { width_ = i; }

  QSize sizeHint() const override;

 private:
  CTkAppEntry* entry_ { nullptr };

  int width_ { -1 };
};

#endif

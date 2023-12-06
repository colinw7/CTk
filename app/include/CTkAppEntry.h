#ifndef CTkAppEntry_H
#define CTkAppEntry_H

#include <CTkAppWidget.h>

class CTkAppEntryWidget;
class CTkAppEntryVarProc;
class CTkAppEntryValidator;

class CTkAppEntry : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName        READ varName           WRITE setVarName)
  Q_PROPERTY(QString invalidCommand READ getInvalidCommand WRITE setInvalidCommand)

 public:
  explicit CTkAppEntry(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Entry"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  const QString &getInvalidCommand() const { return invalidCommand_; }
  void setInvalidCommand(const QString &command) { invalidCommand_ = command; }

  bool isExportSelection() const { return exportSelection_; }
  void setExportSelection(bool b) { exportSelection_ = b; }

  const QString &placeHolder() const { return placeHolder_; }
  void setPlaceHolder(const QString &s) { placeHolder_ = s; }

  const QColor &placeHolderForeground() const { return placeHolderForeground_; }
  void setPlaceHolderForeground(const QColor &c) { placeHolderForeground_ = c; }

  const QColor &readOnlyBackground() const { return readOnlyBackground_; }
  void setReadOnlyBackground(const QColor &c) { readOnlyBackground_ = c; }

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

  CTkAppEntryWidget* qedit_ { nullptr };

  QString             varName_;
  CTkAppEntryVarProc* varProc_ { nullptr };

  ValidateMode          validateMode_ { ValidateMode::NONE };
  QString               validateCmd_;
  CTkAppEntryValidator* validateProc_{ nullptr };

  QString invalidCommand_;

  bool exportSelection_ { true };

  QString placeHolder_;
  QColor  placeHolderForeground_;

  QColor readOnlyBackground_;
};

//---

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

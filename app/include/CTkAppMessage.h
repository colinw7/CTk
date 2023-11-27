#ifndef CTkAppMessage_H
#define CTkAppMessage_H

#include <CTkAppWidget.h>

class CTkAppMessageVarProc;
class QTextEdit;

class CTkAppMessage : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName READ varName WRITE setVarName)

 public:
  explicit CTkAppMessage(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Message"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  //---

  void setText(const QString &text) override;

  void updateFromVar();

 private:
  QTextEdit*            qedit_ { nullptr };
  QString               varName_;
  CTkAppMessageVarProc* varProc_ { nullptr };
};

#endif

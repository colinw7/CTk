#ifndef CTkAppLabel_H
#define CTkAppLabel_H

#include <CTkAppWidget.h>

class CTkAppLabelWidget;
class CTkAppLabelVarProc;

class CTkAppLabel : public CTkAppWidget {
  Q_OBJECT

  Q_PROPERTY(QString varName READ varName WRITE setVarName)

 public:
  explicit CTkAppLabel(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Label"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  const QString &varName() const { return varName_; }
  void setVarName(const QString &s) { varName_ = s; }

  //---

  void setText(const QString &text) override;

  void setImageRef(const CTkAppImageRef &i) override;

  void setJustify(const Qt::Alignment &a) override;

  void updateFromVar();

 private:
  CTkAppLabelWidget*  qlabel_ { nullptr };
  QString             varName_;
  CTkAppLabelVarProc* varProc_ { nullptr };
};

//---

class CTkAppLabelWidget : public CQLabelImage {
  Q_OBJECT

 public:
  using OptReal = std::optional<double>;

 public:
  explicit CTkAppLabelWidget(CTkAppLabel *label);

  const QString &getLabel() const { return text_; }
  void setLabel(const QString &s) { text_ = s; updateText(); }

  const OptReal &width() const { return width_; }
  void setWidth(const OptReal &w) { width_ = w; updateText(); }

  const OptReal &height() const { return height_; }
  void setHeight(const OptReal &h) { height_ = h; updateText(); }

  double wrapLength() const { return wrapLength_; }
  void setWrapLength(double r) { wrapLength_ = r; updateText(); }

  QSize sizeHint() const override;

 private:
  void updateText();

 private:
  CTkAppLabel* label_ { nullptr };

  QString text_;

  OptReal width_;
  OptReal height_;

  double wrapLength_ { -1 };
};

#endif

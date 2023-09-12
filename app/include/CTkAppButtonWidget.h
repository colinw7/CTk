#ifndef CTkAppButtonWidget_H
#define CTkAppButtonWidget_H

#include <CQButtonImage.h>

class CTkAppButton;

class CTkAppButtonWidget : public CQButtonImage {
  Q_OBJECT

 public:
  explicit CTkAppButtonWidget(CTkAppButton *button);

  const QString &getText() const { return text_; }
  void setText(const QString &s);

  double wrapLength() const { return wrapLength_; }
  void setWrapLength(double r);

  bool isReadOnly() const { return readOnly_; }
  void setReadOnly(bool b) { readOnly_ = b; }

 private:
  void updateText();

 private:
  CTkAppButton *button_     { nullptr };
  double        wrapLength_ { -1 };
  QString       text_;
  bool          readOnly_   { false };
};

#endif

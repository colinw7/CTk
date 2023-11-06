#ifndef CTkAppButtonWidget_H
#define CTkAppButtonWidget_H

#include <CQButtonImage.h>

class CTkAppButton;

class CTkAppButtonWidget : public CQButtonImage {
  Q_OBJECT

 public:
  explicit CTkAppButtonWidget(CTkAppButton *button);

  const QString &getText() const { return text_; }
  void setText(const QString &s) { text_ = s; updateText(); update(); }

  double wrapLength() const { return wrapLength_; }
  void setWrapLength(double r) { wrapLength_ = r; updateText(); update(); }

  bool isReadOnly() const { return readOnly_; }
  void setReadOnly(bool b) { readOnly_ = b; update(); }

  const Qt::Alignment &anchor() const { return anchor_; }
  void setAnchor(const Qt::Alignment &a) { anchor_ = a; update(); }

 private:
  void updateText();

 private:
  CTkAppButton *button_ { nullptr };

  QString       text_;
  double        wrapLength_ { -1 };
  bool          readOnly_   { false };
  Qt::Alignment anchor_     { Qt::AlignCenter };
};

#endif

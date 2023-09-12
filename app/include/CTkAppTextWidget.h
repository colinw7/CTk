#ifndef CTkAppTextWidget_H
#define CTkAppTextWidget_H

#include <CQTextWidget.h>

class CTkAppText;

class CTkAppTextWidget : public CQTextWidget {
  Q_OBJECT

 public:
  CTkAppTextWidget(CTkAppText *text);

  int width() const { return width_; }
  void setWidth(int i) { width_ = i; }

  int height() const { return height_; }
  void setHeight(int i) { height_ = i; }

  QSize sizeHint() const override;

 private:
  CTkAppText *text_   { nullptr };
  int         width_  { -1 };
  int         height_ { -1 };
};

#endif

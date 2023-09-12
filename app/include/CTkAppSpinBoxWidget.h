#ifndef CTkAppSpinBoxWidget_H
#define CTkAppSpinBoxWidget_H

#include <CQSpinList.h>

class CTkAppSpinBox;

class CTkAppSpinBoxWidget : public CQSpinList {
  Q_OBJECT

 public:
  explicit CTkAppSpinBoxWidget(CTkAppSpinBox *spin);

  int width() const { return width_; }
  void setWidth(int i) { width_ = i; }

  QSize sizeHint() const override;

 private:
  CTkAppSpinBox* spin_  { nullptr };
  int            width_ { -1 };
};

#endif

#ifndef CTkAppFrame_H
#define CTkAppFrame_H

#include <CTkAppWidget.h>

class CTkAppFrameWidget;

class CTkAppFrame : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppFrame(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Frame"; }

  bool isContainer() const { return container_; }
  void setContainer(bool b) { container_ = b; }

  void setBackgroundImage(const CTkAppImageRef &i) { backgroundImage_ = i; }
  const CTkAppImageRef &backgroundImage() const { return backgroundImage_; }

  bool isTile() const { return tile_; }
  void setTile(bool b) { tile_ = b; }

  //---

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

 private:
  CTkAppFrameWidget* qframe_ { nullptr };

  bool           container_ { false };
  CTkAppImageRef backgroundImage_;
  bool           tile_ { false };
};

//---

class CTkAppFrameWidget : public QFrame {
 public:
  explicit CTkAppFrameWidget(CTkAppFrame *frame);

  QSize sizeHint() const override;

 private:
  CTkAppFrame *frame_ { nullptr };
};

#endif

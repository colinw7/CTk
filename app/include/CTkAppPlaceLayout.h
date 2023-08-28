#ifndef CTkAppPlaceLayout_H
#define CTkAppPlaceLayout_H

#include <CTkAppLayout.h>
#include <CTkAppLayoutWidget.h>

#include <optional>

class CTkAppPlaceLayout;
class CTkAppWidget;

//---

class CTkAppPlaceLayoutInfo {
 public:
  enum class BorderMode {
    None,
    Inside,
    Outside,
    Ignore
  };

 public:
  using OptReal = std::optional<double>;

 public:
  explicit CTkAppPlaceLayoutInfo() { }

  const Qt::Alignment &anchor() const { return anchor_; }
  void setAnchor(const Qt::Alignment &v) { anchor_ = v; }

  const BorderMode &borderMode() const { return borderMode_; }
  void setBorderMode(const BorderMode &v) { borderMode_ = v; }

  bool   isXValid() const { return bool(x_); }
  double getX    () const { return x_.value_or(0.0); }
  void   setX    (double x) { x_ = x; }
  void   unsetX  () { x_ = OptReal(); }

  bool   isYValid() const { return bool(y_); }
  double getY    () const { return y_.value_or(0.0); }
  void   setY    (double y) { y_ = y; }
  void   unsetY  () { y_ = OptReal(); }

  bool   isWidthValid() const { return bool(width_); }
  double getWidth    () const { return width_.value_or(0.0); }
  void   setWidth    (double width) { width_ = width; }
  void   unsetWidth  () { width_ = OptReal(); }

  bool   isHeightValid() const { return bool(height_); }
  double getHeight    () const { return height_.value_or(0.0); }
  void   setHeight    (double height) { height_ = height; }
  void   unsetHeight  () { height_ = OptReal(); }

  bool   isRelXValid() const { return bool(relX_); }
  double getRelX    () const { return relX_.value_or(0.0); }
  void   setRelX    (double relX) { relX_ = relX; }
  void   unsetRelX  () { relX_ = OptReal(); }

  bool   isRelYValid() const { return bool(relY_); }
  double getRelY    () const { return relY_.value_or(0.0); }
  void   setRelY    (double relY) { relY_ = relY; }
  void   unsetRelY  () { relY_ = OptReal(); }

  bool   isRelWidthValid() const { return bool(relWidth_); }
  double getRelWidth    () const { return relWidth_.value_or(0.0); }
  void   setRelWidth    (double relWidth) { relWidth_ = relWidth; }
  void   unsetRelWidth  () { relWidth_ = OptReal(); }

  bool   isRelHeightValid() const { return bool(relHeight_); }
  double getRelHeight    () const { return relHeight_.value_or(0.0); }
  void   setRelHeight    (double relHeight) { relHeight_ = relHeight; }
  void   unsetRelHeight  () { relHeight_ = OptReal(); }

  const QString &in() const { return in_; }
  void setIn(const QString &s) { in_ = s; }

  void update(const CTkAppPlaceLayoutInfo &info) {
    if (info.relX_) relX_ = info.relX_;
    if (info.relY_) relY_ = info.relY_;
  }

 private:
  Qt::Alignment anchor_ { };
  BorderMode    borderMode_ { BorderMode::None };
  OptReal       x_, y_;
  OptReal       width_, height_;
  OptReal       relX_, relY_;
  OptReal       relWidth_, relHeight_;
  QString       in_;
};

//---

class CTkAppPlaceLayoutWidget : public CTkAppLayoutWidget {
 public:
  using Info = CTkAppPlaceLayoutInfo;

 public:
  explicit CTkAppPlaceLayoutWidget(CTkAppPlaceLayout *place, TkWidget *widget, const Info &info);
  explicit CTkAppPlaceLayoutWidget(CTkAppPlaceLayout *place, QLayout *layout, const Info &info);

  const Info &info() const { return info_; }
  Info &info() { return info_; }

 private:
  CTkAppPlaceLayout* place_ { nullptr };
  Info               info_;
};

//---

class CTkAppPlaceLayout : public CTkAppLayout {
  Q_OBJECT

 public:
  using Info       = CTkAppPlaceLayoutInfo;
  using BorderMode = Info::BorderMode;

 public:
  explicit CTkAppPlaceLayout(CTkAppWidget *parent, int margin = 0, int spacing = -1);
  explicit CTkAppPlaceLayout(int spacing = -1);

 ~CTkAppPlaceLayout();

  QString name() const override { return "place"; }

  void addItem(QLayoutItem *item) override;

  void addWidgets(const std::vector<CTkAppWidget *> &widgets, const Info &info);
  void addWidget(CTkAppWidget *widget, const Info &info);

  QLayoutItem *getItem(CTkAppWidget *widget) const;

  bool getChildInfo(CTkAppWidget *widget, Info &info);

  std::vector<CTkAppLayoutWidget *> getLayoutWidgets(bool flat=false) const override;

  //---

  Qt::Orientations expandingDirections() const override;

  bool hasHeightForWidth() const override;

  int count() const override;

  QLayoutItem *itemAt(int index) const override;

  QSize minimumSize() const override;

  void setGeometry(const QRect &rect) override;

  QSize sizeHint() const override;

  QLayoutItem *takeAt(int index) override;

  void add(QLayoutItem *item);

 private:
  int getXPos(const Info &info, const QRect &rect) const;
  int getYPos(const Info &info, const QRect &rect) const;

  int getWidth (const Info &info, const QRect &rect, CTkAppLayoutWidget *item,
                const SizeType &sizeType=SizeType::SizeHint) const;
  int getHeight(const Info &info, const QRect &rect, CTkAppLayoutWidget *item,
                const SizeType &sizeType=SizeType::SizeHint) const;

 private:
  QSize calculateSize(SizeType sizeType) const;

  QList<QLayoutItem *> list_;
};

#endif

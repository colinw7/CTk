#ifndef CTkAppPlaceLayout_H
#define CTkAppPlaceLayout_H

#include <CTkAppLayout.h>
#include <optional>

class CTkAppWidget;

class CTkAppPlaceLayout : public CTkAppLayout {
  Q_OBJECT

 public:
  enum BorderMode {
    None,
    Inside,
    Outside,
    Ignore
  };

  class Info {
   private:
    Qt::Alignment         anchor_ { };
    BorderMode            borderMode_ { BorderMode::None };
    std::optional<double> x_, y_;
    std::optional<double> width_, height_;
    std::optional<double> relx_, rely_;
    std::optional<double> relWidth_, relHeight_;
    QString               in_;

   public:
    explicit Info() { }

    const Qt::Alignment &anchor() const { return anchor_; }
    void setAnchor(const Qt::Alignment &v) { anchor_ = v; }

    const BorderMode &borderMode() const { return borderMode_; }
    void setBorderMode(const BorderMode &v) { borderMode_ = v; }

    bool   isXValid() const { return bool(x_); }
    double getX    () const { return x_.value_or(0.0); }
    void   setX    (double x) { x_ = x; }

    bool   isYValid() const { return bool(y_); }
    double getY    () const { return y_.value_or(0.0); }
    void   setY    (double y) { y_ = y; }

    bool   isWidthValid() const { return bool(width_); }
    double getWidth    () const { return width_.value_or(0.0); }
    void   setWidth    (double width) { width_ = width; }

    bool   isHeightValid() const { return bool(height_); }
    double getHeight    () const { return height_.value_or(0.0); }
    void   setHeight    (double height) { height_ = height; }

    bool   isRelXValid() const { return bool(relx_); }
    double getRelX    () const { return relx_.value_or(0.0); }
    void   setRelX    (double relx) { relx_ = relx; }

    bool   isRelYValid() const { return bool(rely_); }
    double getRelY    () const { return rely_.value_or(0.0); }
    void   setRelY    (double rely) { rely_ = rely; }

    bool   isRelWidthValid() const { return bool(relWidth_); }
    double getRelWidth    () const { return relWidth_.value_or(0.0); }
    void   setRelWidth    (double relWidth) { relWidth_ = relWidth; }

    bool   isRelHeightValid() const { return bool(relHeight_); }
    double getRelHeight    () const { return relHeight_.value_or(0.0); }
    void   setRelHeight    (double relHeight) { relHeight_ = relHeight; }

    const QString &in() const { return in_; }
    void setIn(const QString &s) { in_ = s; }

    void update(const Info &info) {
      if (info.relx_) relx_ = info.relx_;
      if (info.rely_) rely_ = info.rely_;
    }
  };

 private:
  struct ItemWrapper {
    ItemWrapper(QLayoutItem *i, const Info &inf) {
      item = i;
      info = inf;
    }

    QLayoutItem *item;
    Info         info;
  };

 public:
  explicit CTkAppPlaceLayout(QWidget *parent, int margin = 0, int spacing = -1);
  explicit CTkAppPlaceLayout(int spacing = -1);

 ~CTkAppPlaceLayout();

  QString name() const override { return "place"; }

  void addItem(QLayoutItem *item) override;

  void addWidgets(const std::vector<CTkAppWidget *> &widgets, const Info &info);
  void addWidget(CTkAppWidget *widget, const Info &info);

  ItemWrapper *getItem(CTkAppWidget *widget) const;

  bool getChildInfo(CTkAppWidget *widget, Info &info);

  std::vector<CTkAppLayoutWidget *> getLayoutWidgets() const override;

  //---

  Qt::Orientations expandingDirections() const override;

  bool hasHeightForWidth() const override;

  int count() const override;

  QLayoutItem *itemAt(int index) const override;

  QSize minimumSize() const override;

  void setGeometry(const QRect &rect) override;

  QSize sizeHint() const override;

  QLayoutItem *takeAt(int index) override;

  void add(QLayoutItem *item, const Info &info);

 private:
  int getXPos(const Info &info, const QRect &rect) const;
  int getYPos(const Info &info, const QRect &rect) const;

  int getWidth (const Info &info, const QRect &rect, CTkAppLayoutWidget *item,
                const SizeType &sizeType=SizeType::SizeHint) const;
  int getHeight(const Info &info, const QRect &rect, CTkAppLayoutWidget *item,
                const SizeType &sizeType=SizeType::SizeHint) const;

 private:
  QSize calculateSize(SizeType sizeType) const;

  QList<ItemWrapper *> list_;
};

#endif

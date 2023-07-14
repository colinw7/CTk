#ifndef CTK_PLACE_LAYOUT_H
#define CTK_PLACE_LAYOUT_H

#include <COptVal.h>

#include <QLayout>

class CTkAppWidget;

class CTkAppPlaceLayout : public QLayout {
  Q_OBJECT

 public:
  class Info {
   private:
    COptValT<double> relx_, rely_;

   public:
    Info() { }

    bool   isRelXValid() const { return relx_.isValid(); }
    double getRelX    () const { return relx_.getValue(0.0); }
    void   setRelX    (double relx) { relx_.setValue(relx); }

    bool   isRelYValid() const { return rely_.isValid(); }
    double getRelY    () const { return rely_.getValue(0.0); }
    void   setRelY    (double rely) { rely_.setValue(rely); }

    void update(const Info &info) {
      if (info.relx_.isValid()) relx_ = info.relx_;
      if (info.rely_.isValid()) rely_ = info.rely_;
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
  CTkAppPlaceLayout(QWidget *parent, int margin = 0, int spacing = -1);
  CTkAppPlaceLayout(int spacing = -1);

 ~CTkAppPlaceLayout();

  void addItem(QLayoutItem *item) override;

  void addWidgets(const std::vector<CTkAppWidget *> &widgets, const Info &info);
  void addWidget(CTkAppWidget *widget, const Info &info);

  ItemWrapper *getItem(CTkAppWidget *widget) const;

  bool getChildInfo(CTkAppWidget *widget, Info &info);

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
  enum SizeType { MinimumSize, SizeHint };

  QSize calculateSize(SizeType sizeType) const;

  QList<ItemWrapper *> list_;
};

#endif

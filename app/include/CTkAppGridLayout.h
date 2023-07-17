#ifndef CTkAppGridLayout_H
#define CTkAppGridLayout_H

#include <COptVal.h>

#include <QLayout>

class CTkAppWidget;

class CTkAppGridLayout : public QLayout {
  Q_OBJECT

 public:
  enum StickySide {
    STICKY_NONE = 0,
    STICKY_ALL  = ~0,
    STICKY_N    = (1<<0),
    STICKY_S    = (1<<1),
    STICKY_E    = (1<<2),
    STICKY_W    = (1<<3)
  };

  class Info {
   public:
    Info() { }

    bool isRowValid() const { return row_.isValid(); }
    int  getRow    () const { return row_.getValue(-1); }
    void setRow    (int row) { row_.setValue(row); }

    bool isColValid() const { return col_.isValid(); }
    int  getCol    () const { return col_.getValue(-1); }
    void setCol    (int col) { col_.setValue(col); }

    void setSticky(const std::string &sticky) { sticky_.setValue(sticky); }

    uint getStickySides() const {
      if (sticky_.isValid())
        sticky_sides_ = decodeStickySides();
      else
        sticky_sides_ = STICKY_NONE;

      return sticky_sides_;
    }

    bool isPadXValid() const { return padx_.isValid(); }
    int  getPadX    () const { return padx_.getValue(0); }
    void setPadX    (int padx) { padx_.setValue(padx); }

    bool isPadYValid() const { return pady_.isValid(); }
    int  getPadY    () const { return pady_.getValue(0); }
    void setPadY    (int pady) { pady_.setValue(pady); }

    bool isIPadXValid() const { return ipadx_.isValid(); }
    int  getIPadX    () const { return ipadx_.getValue(0); }
    void setIPadX    (int ipadx) { ipadx_.setValue(ipadx); }

    bool isIadYValid() const { return ipady_.isValid(); }
    int  getIPadY   () const { return ipady_.getValue(0); }
    void setIPadY   (int ipady) { ipady_.setValue(ipady); }

    void update(const Info &info) {
      if (info.row_   .isValid()) row_    = info.row_   ;
      if (info.col_   .isValid()) col_    = info.col_   ;
      if (info.sticky_.isValid()) sticky_ = info.sticky_;
      if (info.padx_  .isValid()) padx_   = info.padx_  ;
      if (info.pady_  .isValid()) pady_   = info.pady_  ;
      if (info.ipadx_ .isValid()) ipadx_  = info.ipadx_  ;
      if (info.ipady_ .isValid()) ipady_  = info.ipady_  ;
    }

   private:
    uint decodeStickySides() const {
      uint sides = 0;

      std::string sticky = sticky_.getValue("");

      auto len = sticky.size();

      for (size_t i = 0; i < len; ++i) {
        switch (sticky[i]) {
          case 'n': case 'N': sides |= STICKY_N; break;
          case 's': case 'S': sides |= STICKY_S; break;
          case 'e': case 'E': sides |= STICKY_E; break;
          case 'w': case 'W': sides |= STICKY_W; break;
          default:                               break;
        }
      }

      return sides;
    }

   private:
    COptValT<int>         row_, col_;
    COptValT<std::string> sticky_;
    COptValT<int>         padx_, pady_;
    COptValT<int>         ipadx_, ipady_;

    mutable uint sticky_sides_;
  };

 private:
  struct ItemWrapper {
    ItemWrapper(QLayoutItem *i, const Info &inf) {
      item   = i;
      info   = inf;
      weight = 0;
    }

    QLayoutItem *item;
    Info         info;
    int          weight;
  };

 public:
  CTkAppGridLayout(QWidget *parent, int margin = 0, int spacing = -1);
  CTkAppGridLayout(int spacing = -1);

 ~CTkAppGridLayout();

  void addItem(QLayoutItem *item) override;

  void addWidgets(const std::vector<CTkAppWidget *> &widgets, const Info &info);
  void addWidget(CTkAppWidget *widget, const Info &info);

  ItemWrapper *getItem(CTkAppWidget *widget) const;

  bool getChildInfo(CTkAppWidget *widget, Info &info);

  bool setChildWeight(CTkAppWidget *widget, int weight);

  void setColumnWeight(int col, int weight);
  void setRowWeight   (int row, int weight);

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

 private:
  using ColWeights = std::map<int, int>;
  using RowWeights = std::map<int, int>;

  QList<ItemWrapper *> list_;
  int                  row_ { 0 };
  int                  col_ { 0 };
  ColWeights           colWeights_;
  RowWeights           rowWeights_;
};

#endif
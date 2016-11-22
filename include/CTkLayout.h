#ifndef CTK_LAYOUT_H
#define CTK_LAYOUT_H

#include <COptVal.h>

#include <QLayout>
#include <QRect>
#include <QWidgetItem>

#include <vector>

class CTkWidget;

class CTkLayoutWidget : public QLayoutItem {
 public:
  CTkLayoutWidget(CTkWidget *widget) :
   widget_(widget) {
  }

  CTkWidget *getTkWidget() const { return widget_; }

  Qt::Orientations expandingDirections() const;
  QRect            geometry() const;
  bool             hasHeightForWidth() const;
  int              heightForWidth(int w) const;
  bool             isEmpty() const;
  QSize            maximumSize() const;
  QSize            minimumSize() const;
  void             setGeometry(const QRect &rect);
  QSize            sizeHint() const;
  QWidget*         widget();

 private:
  QWidget*         getQWidget() const;

 private:
  CTkWidget *widget_;
};

class CTkPackLayout : public QLayout {
  Q_OBJECT

 public:
  enum Side { SIDE_LEFT, SIDE_RIGHT, SIDE_BOTTOM, SIDE_TOP, SIDE_NONE };
  enum Fill { FILL_X, FILL_Y, FILL_NONE, FILL_BOTH };

  struct Info {
    Side side;
    Fill fill;
    bool expand;
    int  padx, pady;
    int  ipadx, ipady;

    Info(Side tside=SIDE_TOP, Fill tfill=FILL_NONE, bool texpand=false, int tpadx=0, int tpady=0,
         int tipadx=0, int tipady=0) :
     side(tside), fill(tfill), expand(texpand), padx(tpadx), pady(tpady),
     ipadx(tipadx), ipady(tipady) {
    }

    const char *getSideStr() const {
      switch (side) {
        case SIDE_LEFT  : return "left";
        case SIDE_RIGHT : return "right";
        case SIDE_BOTTOM: return "bottom";
        case SIDE_TOP   : return "top";
        default         : return "none";
      }
    }

    const char *getFillStr() const {
      switch (side) {
        case FILL_X   : return "x";
        case FILL_Y   : return "y";
        case FILL_BOTH: return "both";
        default       : return "none";
      }
    }
  };

 private:
  struct ItemWrapper {
    ItemWrapper(QLayoutItem *i, const Info &f) {
      item = i;
      info = f;
    }

    QLayoutItem * item;
    Info          info;
  };

 public:
  CTkPackLayout(QWidget *parent, int margin=0, int spacing=0);
  CTkPackLayout(int spacing = -1);

 ~CTkPackLayout();

  void addItem(QLayoutItem *item);

  void addWidgets(const std::vector<CTkWidget *> &widgets, const Info &info);
  void addWidget(CTkWidget *widget, const Info &info);

  ItemWrapper *getItem(CTkWidget *widget) const;

  bool getChildInfo(CTkWidget *widget, Info &info);

  Qt::Orientations expandingDirections() const;

  bool hasHeightForWidth() const;

  int count() const;

  QLayoutItem *itemAt(int index) const;

  QSize minimumSize() const;

  void setGeometry(const QRect &rect);

  QSize sizeHint() const;

  QLayoutItem *takeAt(int index);

  void add(QLayoutItem *item, const Info &info);

 private:
  void setItemGeometry(QLayoutItem *item, const QRect &r);

 private:
  enum SizeType { MinimumSize, SizeHint };

  QSize calculateSize(SizeType sizeType) const;

  QList<ItemWrapper *> list_;
};

class CTkGridLayout : public QLayout {
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
   private:
    COptValT<int>         row_, col_;
    COptValT<std::string> sticky_;
    COptValT<int>         padx_, pady_;
    COptValT<int>         ipadx_, ipady_;

    mutable uint sticky_sides_;

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
    uint getPadX    () const { return padx_.getValue(0); }
    void setPadX    (int padx) { padx_.setValue(padx); }

    bool isPadYValid() const { return pady_.isValid(); }
    uint getPadY    () const { return pady_.getValue(0); }
    void setPadY    (int pady) { pady_.setValue(pady); }

    bool isIPadXValid() const { return ipadx_.isValid(); }
    uint getIPadX    () const { return ipadx_.getValue(0); }
    void setIPadX    (int ipadx) { ipadx_.setValue(ipadx); }

    bool isIadYValid() const { return ipady_.isValid(); }
    uint getIPadY   () const { return ipady_.getValue(0); }
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

      uint len = sticky.size();

      for (uint i = 0; i < len; ++i) {
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
  CTkGridLayout(QWidget *parent, int margin = 0, int spacing = -1);
  CTkGridLayout(int spacing = -1);

 ~CTkGridLayout();

  void addItem(QLayoutItem *item);

  void addWidgets(const std::vector<CTkWidget *> &widgets, const Info &info);
  void addWidget(CTkWidget *widget, const Info &info);

  ItemWrapper *getItem(CTkWidget *widget) const;

  bool getChildInfo(CTkWidget *widget, Info &info);

  bool setChildWeight(CTkWidget *widget, int weight);

  Qt::Orientations expandingDirections() const;

  bool hasHeightForWidth() const;

  int count() const;

  QLayoutItem *itemAt(int index) const;

  QSize minimumSize() const;

  void setGeometry(const QRect &rect);

  QSize sizeHint() const;

  QLayoutItem *takeAt(int index);

  void add(QLayoutItem *item, const Info &info);

 private:
  enum SizeType { MinimumSize, SizeHint };

  QSize calculateSize(SizeType sizeType) const;

  QList<ItemWrapper *> list_;
  int                  row_, col_;
};

class CTkPlaceLayout : public QLayout {
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
  CTkPlaceLayout(QWidget *parent, int margin = 0, int spacing = -1);
  CTkPlaceLayout(int spacing = -1);

 ~CTkPlaceLayout();

  void addItem(QLayoutItem *item);

  void addWidgets(const std::vector<CTkWidget *> &widgets, const Info &info);
  void addWidget(CTkWidget *widget, const Info &info);

  ItemWrapper *getItem(CTkWidget *widget) const;

  bool getChildInfo(CTkWidget *widget, Info &info);

  Qt::Orientations expandingDirections() const;

  bool hasHeightForWidth() const;

  int count() const;

  QLayoutItem *itemAt(int index) const;

  QSize minimumSize() const;

  void setGeometry(const QRect &rect);

  QSize sizeHint() const;

  QLayoutItem *takeAt(int index);

  void add(QLayoutItem *item, const Info &info);

 private:
  enum SizeType { MinimumSize, SizeHint };

  QSize calculateSize(SizeType sizeType) const;

  QList<ItemWrapper *> list_;
};

#endif

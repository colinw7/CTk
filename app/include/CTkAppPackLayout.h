#ifndef CTkAppPackLayout_H
#define CTkAppPackLayout_H

#include <QLayout>

class CTkAppWidget;

class QPainter;

class CTkAppPackLayout : public QLayout {
  Q_OBJECT

 public:
  enum Side { SIDE_LEFT, SIDE_RIGHT, SIDE_BOTTOM, SIDE_TOP, SIDE_NONE };
  enum Fill { FILL_X, FILL_Y, FILL_NONE, FILL_BOTH };

  struct Info {
    Side side   { SIDE_TOP };
    Fill fill   { FILL_NONE };
    bool expand { false };
    int  padx   { 0 };
    int  pady   { 0 };
    int  ipadx  { 0 };
    int  ipady  { 0 };

    Info(Side side=SIDE_TOP, Fill fill=FILL_NONE, bool expand=false, int padx=0, int pady=0,
         int ipadx=0, int ipady=0) :
     side(side), fill(fill), expand(expand), padx(padx), pady(pady), ipadx(ipadx), ipady(ipady) {
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
      switch (fill) {
        case FILL_X   : return "x";
        case FILL_Y   : return "y";
        case FILL_BOTH: return "both";
        default       : return "none";
      }
    }
  };

 private:
  struct ItemWrapper {
    ItemWrapper() = default;

    ItemWrapper(QLayoutItem *i, const Info &f) {
      item = i;
      info = f;
    }

    QLayoutItem *item { nullptr };
    Info         info;
  };

 public:
  CTkAppPackLayout(QWidget *parent, int margin=0, int spacing=0);
  CTkAppPackLayout(int spacing = -1);

 ~CTkAppPackLayout();

  bool isPropagate() const { return propagate_; }
  void setPropagate(bool b) { propagate_ = b; }

  void addItem(QLayoutItem *item) override;

  void addWidgets(const std::vector<CTkAppWidget *> &widgets, const Info &info);
  void addWidget(CTkAppWidget *widget, const Info &info);

  void removeWidget(CTkAppWidget *widget);

  std::vector<CTkAppWidget *> getWidgets() const;

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
  void setItemGeometry(QLayoutItem *item, const QRect &r);

 private:
  enum SizeType { MinimumSize, SizeHint };

  QSize calculateSize(SizeType sizeType) const;

  QList<ItemWrapper *> list_;
  bool                 propagate_ { true };
};

#endif

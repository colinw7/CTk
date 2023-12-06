#ifndef CTkAppGridLayout_H
#define CTkAppGridLayout_H

#include <CTkAppLayout.h>
#include <CTkAppLayoutWidget.h>
#include <optional>

class CTkAppGridLayout;
class CTkAppWidget;

class QPainter;

//---

class CTkAppGridLayoutInfo {
 public:
  enum class StickySide {
    NONE = 0,
    ALL  = ~0,
    N    = (1<<0),
    S    = (1<<1),
    E    = (1<<2),
    W    = (1<<3)
  };

  using OptInt    = std::optional<int>;
  using OptString = std::optional<QString>;

 public:
  explicit CTkAppGridLayoutInfo() { }

  //---

  bool isRowValid() const { return bool(row_); }
  int  getRow    () const { return row_.value_or(-1); }
  void setRow    (int row) { row_ = row; }
  void unsetRow  () { row_ = OptInt(); }

  bool isColValid() const { return bool(col_); }
  int  getCol    () const { return col_.value_or(-1); }
  void setCol    (int col) { col_ = col; }
  void unsetCol  () { col_ = OptInt(); }

  bool isRowSpanValid() const { return bool(rowSpan_); }
  int  getRowSpan    () const { return rowSpan_.value_or(1); }
  void setRowSpan    (int rowSpan) { rowSpan_ = rowSpan; }
  void unsetRowSpan  () { rowSpan_ = OptInt(); }

  bool isColSpanValid() const { return bool(colSpan_); }
  int  getColSpan    () const { return colSpan_.value_or(1); }
  void setColSpan    (int colSpan) { colSpan_ = colSpan; }
  void unsetColSpan  () { colSpan_ = OptInt(); }

  QString getSticky() const { return sticky_.value_or(""); }
  void setSticky(const QString &sticky) { sticky_ = sticky; }

  uint getStickySides(bool &valid) const {
    valid = true;

    if (sticky_)
      sticky_sides_ = decodeStickySides(valid);
    else
      sticky_sides_ = uint(StickySide::NONE);

    return sticky_sides_;
  }

  bool isPadXValid() const { return bool(padl_); }
  int  getPadX    () const { return padl_.value_or(0); }
  void setPadX    (int padl, int padr) { padl_ = padl; padr_ = padr; }

  bool isPadYValid() const { return bool(padb_); }
  int  getPadY    () const { return padb_.value_or(0); }
  void setPadY    (int padb, int pady) { padb_ = padb; padt_ = pady; }

  bool isIPadXValid() const { return bool(ipadx_); }
  int  getIPadX    () const { return ipadx_.value_or(0); }
  void setIPadX    (int ipadx) { ipadx_ = ipadx; }

  bool isIPadYValid() const { return bool(ipady_); }
  int  getIPadY    () const { return ipady_.value_or(0); }
  void setIPadY    (int ipady) { ipady_ = ipady; }

  bool isInValid() const { return bool(in_); }
  QString getIn() const { return in_.value_or(""); }
  void setIn(const QString &s) { in_ = s; }

  int isWeightValue() const { return bool(weight_); }
  int weight() const { return weight_.value_or(0); }
  void setWeight(int i) { weight_ = i; }

  void update(const CTkAppGridLayoutInfo &info) {
    if (info.row_    ) row_     = info.row_    ;
    if (info.col_    ) col_     = info.col_    ;
    if (info.rowSpan_) rowSpan_ = info.rowSpan_;
    if (info.colSpan_) colSpan_ = info.colSpan_;
    if (info.sticky_ ) sticky_  = info.sticky_ ;
    if (info.padl_   ) padl_    = info.padl_   ;
    if (info.padb_   ) padb_    = info.padb_   ;
    if (info.ipadx_  ) ipadx_   = info.ipadx_  ;
    if (info.ipady_  ) ipady_   = info.ipady_  ;
    if (info.in_     ) in_      = info.in_     ;
    if (info.weight_ ) weight_  = info.weight_ ;
  }

 private:
  uint decodeStickySides(bool &valid) const {
    uint sides = 0;

    auto sticky = sticky_.value_or("");

    auto len = sticky.size();

    valid = true;

    for (int i = 0; i < len; ++i) {
      switch (sticky[i].toLatin1()) {
        case 'n': case 'N': sides |= uint(StickySide::N); break;
        case 's': case 'S': sides |= uint(StickySide::S); break;
        case 'e': case 'E': sides |= uint(StickySide::E); break;
        case 'w': case 'W': sides |= uint(StickySide::W); break;
        case ' ': case '\t': break;
        case ',': break;
        default: valid = false; break;
      }
    }

    return sides;
  }

 private:
  OptInt    row_;
  OptInt    col_;
  OptInt    rowSpan_;
  OptInt    colSpan_;
  OptString sticky_;
  OptInt    padl_, padr_;
  OptInt    padb_, padt_;
  OptInt    ipadx_;
  OptInt    ipady_;
  OptString in_;
  OptInt    weight_;

  mutable uint sticky_sides_;
};

//---

class CTkAppGridLayoutWidget : public CTkAppLayoutWidget {
 public:
  using Info = CTkAppGridLayoutInfo;

 public:
  explicit CTkAppGridLayoutWidget(CTkAppGridLayout *grid, TkWidget *widget, const Info &info);
  explicit CTkAppGridLayoutWidget(CTkAppGridLayout *grid, QLayout *layout, const Info &info);

  const Info &info() const { return info_; }
  Info &info() { return info_; }

 private:
  CTkAppGridLayout* grid_ { nullptr };
  Info              info_;
};

//---

class CTkAppGridLayout : public CTkAppLayout {
  Q_OBJECT

 public:
  using Info = CTkAppGridLayoutInfo;

 public:
  enum class WidgetType {
    NONE,
    WIDGET,
    COL_SPAN,
    ROW_SPAN,
    EMPTY
  };

  struct WidgetData {
    WidgetType    type   { WidgetType::NONE };
    CTkAppWidget* widget { nullptr };

    WidgetData() { }

    WidgetData(CTkAppWidget *w) :
     type(WidgetType::WIDGET), widget(w) {
    }

    WidgetData(const WidgetType &t) :
     type(t), widget(nullptr) {
    }
  };

  using WidgetDatas = std::vector<WidgetData>;

 public:
  explicit CTkAppGridLayout(CTkAppWidget *parent, int margin = 0, int spacing = -1);
  explicit CTkAppGridLayout(int spacing = -1);

 ~CTkAppGridLayout();

  QString name() const override { return "grid"; }

  void addItem(QLayoutItem *item) override;

  void addWidgets(const WidgetDatas &widgets, const Info &info);
  void addWidget(const WidgetData &widget, const Info &info);

  bool removeWidget(CTkAppWidget *widget);

  std::vector<CTkAppWidget *> getWidgets() const;

  std::vector<CTkAppLayoutWidget *> getLayoutWidgets(bool flat=false) const override;

  QLayoutItem *getItem(CTkAppWidget *widget) const;

  bool getChildInfo(CTkAppWidget *widget, Info &info);

  bool setChildWeight(CTkAppWidget *widget, int weight);

  //---

  void setColumnWeight(int col, int weight);
  int getColumnWeight(int col) const;

  void setColumnUniform(int col, const QString &name);
  QString getColumnUniform(int col) const;

  void setColumnMinSize(int col, double size);
  double getColumnMinSize(int col) const;

  void setColumnPad(int col, int pad);
  int getColumnPad(int col) const;

  //---

  void setRowWeight(int row, int weight);
  int  getRowWeight(int row) const;

  void setRowUniform(int col, const QString &name);
  QString getRowUniform(int col) const;

  void setRowMinSize(int col, double size);
  double getRowMinSize(int col) const;

  void setRowPad(int col, int pad);
  int getRowPad(int col) const;

  //---

  Qt::Orientations expandingDirections() const override;

  bool hasHeightForWidth() const override;

  int count() const override;

  bool isRowUsed   (int) const;
  bool isColumnUsed(int) const;

  uint numRows() const { return numRows_; }
  uint numCols() const { return numCols_; }

  QLayoutItem *itemAt(int index) const override;

  QSize minimumSize() const override;

  //---

  void setGeometry(const QRect &rect) override;

  int colX(uint c) const;
  int rowY(uint r) const;

  QSize sizeHint() const override;

  //---

  QLayoutItem *takeAt(int index) override;

  void add(QLayoutItem *item);

  //---

  void getBBox(QRect &r) const;
  void getCellBBox(QRect &r, int row1, int col1, int row2, int col2) const;

  bool getCellAt(double x, double y, int &row, int &col) const;

  //---

  void draw(QPainter *p) const;

 private:
  QSize calculateSize(SizeType sizeType) const;

  void calculateDims(uint &num_rows, uint &num_cols) const;

  void calcPrefSizes(SizeType sizeType, std::vector<int> &prefColWidths,
                     std::vector<int> &prefRowHeights, int &prefWidth, int &prefHeight) const;

  QLayoutItem *gridItem(int row, int col) const;

 private:
  using RowColWeights = std::map<int, int>;
  using RowColUniform = std::map<int, QString>;
  using RowColMinSize = std::map<int, double>;
  using RowColPad     = std::map<int, int>;
  using RowWeights    = std::map<int, int>;

  QList<QLayoutItem *> list_;
  int                  row_ { 0 };
  int                  col_ { 0 };
  int                  rowSpan_ { 1 };
  int                  colSpan_ { 1 };

  RowColWeights colWeights_;
  RowColUniform colUniform_;
  RowColMinSize colMinSize_;
  RowColPad     colPad_;

  RowColWeights rowWeights_;
  RowColUniform rowUniform_;
  RowColMinSize rowMinSize_;
  RowColPad     rowPad_;

  QRect            rect_;
  uint             numRows_ { 0 };
  uint             numCols_ { 0 };
  std::vector<int> colWidths_;
  std::vector<int> rowHeights_;
};

#endif

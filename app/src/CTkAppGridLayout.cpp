#include <CTkAppGridLayout.h>
#include <CTkAppLayoutWidget.h>
#include <CTkAppWidget.h>

#include <QApplication>
#include <QWidget>
#include <QPainter>

#include <set>

CTkAppGridLayout::
CTkAppGridLayout(QWidget *parent, int margin, int spacing) :
 CTkAppLayout(parent)
{
  setObjectName("grid");

  setMargin(margin);
  setSpacing(spacing);
}

CTkAppGridLayout::
CTkAppGridLayout(int spacing) :
 CTkAppLayout(nullptr)
{
  setObjectName("grid");

  setSpacing(spacing);
}

CTkAppGridLayout::
~CTkAppGridLayout()
{
  QLayoutItem *l;

  while ((l = takeAt(0)))
    delete l;
}

void
CTkAppGridLayout::
addItem(QLayoutItem *item)
{
  add(item, Info());
}

void
CTkAppGridLayout::
addWidgets(const std::vector<WidgetData> &widgets, const Info &info)
{
  Info info1 = info;

  if (info1.isRowValid()) row_ = info1.getRow();
  if (info1.isColValid()) col_ = info1.getCol();

  uint num_added = 0;

  auto num = widgets.size();

  int rowSpan = 1;

  for (size_t i = 0; i < num; ++i) {
    const auto &widgetData = widgets[i];

    if      (widgetData.type == WidgetType::WIDGET) {
      auto *wrapper = getItem(widgetData.widget);

      if (! wrapper) {
        info1.setRow(row_);
        info1.setCol(col_);

        add(new CTkAppLayoutWidget(widgetData.widget), info1);

        col_ += info1.getColSpan();

        rowSpan = std::max(rowSpan, info1.getRowSpan());

        ++num_added;
      }
      else
        wrapper->info.update(info1);
    }
    else if (widgetData.type == WidgetType::EMPTY) {
      ++col_;

      ++num_added;
    }
    else if (widgetData.type == WidgetType::COL_SPAN) {
      for (int i = col_ - 1; i >= 0; --i) {
        auto *wrapper = gridItem(row_, i);
        if (wrapper) {
          wrapper->info.setColSpan(wrapper->info.getColSpan() + 1);
          break;
        }
      }

      ++col_;

      ++num_added;
    }
    else if (widgetData.type == WidgetType::ROW_SPAN) {
      for (int i = row_ - 1; i >= 0; --i) {
        auto *wrapper = gridItem(i, col_);
        if (wrapper) {
          wrapper->info.setRowSpan(wrapper->info.getRowSpan() + 1);
          break;
        }
      }

      ++num_added;
    }
    else {
      assert(false);
    }
  }

  if (num_added > 0) {
    row_ += rowSpan;
    col_  = 0;
  }
}

void
CTkAppGridLayout::
addWidget(const WidgetData &widgetData, const Info &info)
{
  if (widgetData.type == WidgetType::WIDGET) {
    auto *wrapper = getItem(widgetData.widget);

    if (! wrapper)
      add(new CTkAppLayoutWidget(widgetData.widget), info);
    else
      wrapper->info.update(info);
  }
}

bool
CTkAppGridLayout::
removeWidget(CTkAppWidget *widget)
{
  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    auto *lw = dynamic_cast<CTkAppLayoutWidget *>(wrapper->item);

    if (lw->widget() == widget->qwidget()) {
      auto *l = takeAt(i);

      delete l;

      widget->qwidget()->hide();

      return true;
    }
  }

  return false;
}

CTkAppGridLayout::ItemWrapper *
CTkAppGridLayout::
getItem(CTkAppWidget *widget) const
{
  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    auto *w = dynamic_cast<CTkAppLayoutWidget *>(wrapper->item);
    if (! w) continue;

    auto *widget1 = w->getTkWidget();

    if (widget == widget1)
      return wrapper;
  }

  return nullptr;
}

bool
CTkAppGridLayout::
getChildInfo(CTkAppWidget *widget, Info &info)
{
  auto *wrapper = getItem(widget);
  if (! wrapper) return false;

  info = wrapper->info;

  return true;
}

bool
CTkAppGridLayout::
setChildWeight(CTkAppWidget *widget, int weight)
{
  auto *wrapper = getItem(widget);
  if (! wrapper) return false;

  wrapper->weight = weight;

  return true;
}

//---

void
CTkAppGridLayout::
setColumnWeight(int col, int weight)
{
  colWeights_[col] = std::max(weight, 0);
}

int
CTkAppGridLayout::
getColumnWeight(int col) const
{
  auto p = colWeights_.find(col);

  return (p != colWeights_.end() ? (*p).second : 0);
}

void
CTkAppGridLayout::
setColumnUniform(int col, const std::string &name)
{
  colUniform_[col] = name;
}

std::string
CTkAppGridLayout::
getColumnUniform(int col) const
{
  auto p = colUniform_.find(col);

  return (p != colUniform_.end() ? (*p).second : "");
}

void
CTkAppGridLayout::
setColumnMinSize(int col, double size)
{
  colMinSize_[col] = size;
}

double
CTkAppGridLayout::
getColumnMinSize(int col) const
{
  auto p = colMinSize_.find(col);

  return (p != colMinSize_.end() ? (*p).second : 0.0);
}

void
CTkAppGridLayout::
setColumnPad(int col, int pad)
{
  colPad_[col] = pad;
}

int
CTkAppGridLayout::
getColumnPad(int col) const
{
  auto p = colPad_.find(col);

  return (p != colPad_.end() ? (*p).second : 0);
}

//---

void
CTkAppGridLayout::
setRowWeight(int row, int weight)
{
  rowWeights_[row] = std::max(weight, 0);
}

int
CTkAppGridLayout::
getRowWeight(int row) const
{
  auto p = rowWeights_.find(row);

  return (p != rowWeights_.end() ? (*p).second : 0);
}

void
CTkAppGridLayout::
setRowUniform(int row, const std::string &name)
{
  rowUniform_[row] = name;
}

std::string
CTkAppGridLayout::
getRowUniform(int row) const
{
  auto p = rowUniform_.find(row);

  return (p != rowUniform_.end() ? (*p).second : "");
}

void
CTkAppGridLayout::
setRowMinSize(int row, double size)
{
  rowMinSize_[row] = size;
}

double
CTkAppGridLayout::
getRowMinSize(int row) const
{
  auto p = rowMinSize_.find(row);

  return (p != rowMinSize_.end() ? (*p).second : 0.0);
}

void
CTkAppGridLayout::
setRowPad(int row, int pad)
{
  rowPad_[row] = pad;
}

int
CTkAppGridLayout::
getRowPad(int row) const
{
  auto p = rowPad_.find(row);

  return (p != rowPad_.end() ? (*p).second : 0);
}

//---

Qt::Orientations
CTkAppGridLayout::
expandingDirections() const
{
  return Qt::Horizontal | Qt::Vertical;
}

bool
CTkAppGridLayout::
hasHeightForWidth() const
{
  // preferred height depends on width
  return false;
}

int
CTkAppGridLayout::
count() const
{
  return list_.size();
}

QLayoutItem *
CTkAppGridLayout::
itemAt(int index) const
{
  auto *wrapper = list_.value(index);
  if (! wrapper) return nullptr;

  return wrapper->item;
}

CTkAppGridLayout::ItemWrapper *
CTkAppGridLayout::
gridItem(int row, int col) const
{
  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    int row1 = wrapper->info.getRow();
    int col1 = wrapper->info.getCol();

    if (row == row1 && col == col1)
      return wrapper;
  }

  return nullptr;
}

QSize
CTkAppGridLayout::
minimumSize() const
{
  return calculateSize(MinimumSize);
}

void
CTkAppGridLayout::
setGeometry(const QRect &rect)
{
  CTkAppLayout::setGeometry(rect);

  //---

  // get num rows and columns
  uint num_rows, num_cols;

  calculateDims(num_rows, num_cols);

  std::vector<int> prefColWidths, prefRowHeights;
  int              prefWidth, prefHeight;

  calcPrefSizes(SizeHint, prefColWidths, prefRowHeights, prefWidth, prefHeight);

  //---

  std::vector<int> weights;

  weights.resize(std::max(num_rows, num_cols));

  //---

  std::vector<int> colWidths, rowHeights;

  colWidths .resize(size_t(num_cols));
  rowHeights.resize(size_t(num_rows));

  for (uint i = 0; i < num_cols; ++i)
    colWidths[i] = prefColWidths[i];

  for (uint i = 0; i < num_rows; ++i)
    rowHeights[i] = prefRowHeights[i];

  int width  = prefWidth;
  int height = prefHeight;

  //---

  // resize columns, rows
  if (rect.width() > width) {
    //int maxWeight = 0;

    for (uint j = 0; j < num_cols; ++j) {
      auto w = getColumnWeight(j);

      weights[j] = w;

      //maxWeight = std::max(maxWeight, w);
    }

    int sumWeights = 0;

    for (uint j = 0; j < num_cols; ++j) {
      //if (maxWeight == 0)
      //  weights[j] = 1;

      sumWeights += weights[j];
    }

    if (sumWeights > 0) {
      int dw = rect.width() - width;

      int dw1 = dw/sumWeights;
      int dw2 = dw - dw1*sumWeights;

      for (uint j = 0; j < num_cols; ++j) {
        if (weights[j] == 0)
          continue;

        int dr = (dw2 > 0 ? 1 : 0); --dw2;

        colWidths[j] += (dw1 + dr)*weights[j];
      }
    }
  }

  if (rect.height() > height) {
    //int maxWeight = 0;

    for (uint j = 0; j < num_rows; ++j) {
      auto w = getRowWeight(j);

      weights[j] = w;

      //maxWeight = std::max(maxWeight, w);
    }

    int sumWeights = 0;

    for (uint j = 0; j < num_rows; ++j) {
      //if (maxWeight == 0)
      //  weights[j] = 1;

      sumWeights += weights[j];
    }

    if (sumWeights > 0) {
      int dh = rect.height() - height;

      int dh1 = dh/sumWeights;
      int dh2 = dh - dh1*sumWeights;

      for (uint j = 0; j < num_rows; ++j) {
        if (weights[j] == 0)
          continue;

        int dr = (dh2 > 0 ? 1 : 0); --dh2;

        rowHeights[j] += (dh1 + dr)*weights[j];
      }
    }
  }

  //---

  auto colX = [&](int c) {
    int x = rect.x();

    for (int i = 0; i < c; ++i)
      x += colWidths[i];

    return x;
  };

  auto rowX = [&](int r) {
    int y = rect.y();

    for (int i = 0; i < r; ++i)
      y += rowHeights[i];

    return y;
  };

  //---

  // place widgets in grid
  auto placeWidgets = [&]() {
    bool changed = false;

    for (int i = 0; i < list_.size(); ++i) {
      auto *wrapper = list_.at(i);

      auto *item = dynamic_cast<CTkAppLayoutWidget *>(wrapper->item);
      assert(item);

      int rowSpan = wrapper->info.getRowSpan();
      int colSpan = wrapper->info.getColSpan();

      int row1 = wrapper->info.getRow();
      int row2 = row1 + rowSpan;
      int col1 = wrapper->info.getCol();
      int col2 = col1 + colSpan;

      uint sides = wrapper->info.getStickySides();

      int padx = wrapper->info.getPadX();
      int pady = wrapper->info.getPadY();

      int ipadx = wrapper->info.getIPadX();
      int ipady = wrapper->info.getIPadY();

      auto itemSize = item->sizeHint();

      itemSize = itemSize.expandedTo(QApplication::globalStrut());

      int iw = itemSize.width () + 2*ipadx;
      int ih = itemSize.height() + 2*ipady;

      int colWidth1 = 0;
      for (int col = col1; col < col2; ++col)
        colWidth1 += colWidths[size_t(col)];

      int colHeight1 = 0;
      for (int row = row1; row < row2; ++row)
        colHeight1 += rowHeights[size_t(row)];

      int x1 = colX(col1);
      int y1 = rowX(row1);

      int dx = 0, dy = 0;

      if      ((sides & STICKY_W) && (sides & STICKY_E)) dx = 0;
      else if ((sides & STICKY_W)                      ) dx = 0;
      else if ((sides & STICKY_E)                      ) dx = colWidth1  - iw - 2*padx;
      else                                               dx = (colWidth1 - iw - 2*padx)/2;

      if      ((sides & STICKY_N) && (sides & STICKY_S)) dy = 0;
      else if ((sides & STICKY_N)                      ) dy = 0;
      else if ((sides & STICKY_S)                      ) dy = colHeight1  - ih - 2*pady;
      else                                               dy = (colHeight1 - ih - 2*pady)/2;

      int w = ((sides & STICKY_W) && (sides & STICKY_E) ? colWidth1  - 2*padx: iw);
      int h = ((sides & STICKY_N) && (sides & STICKY_S) ? colHeight1 - 2*pady: ih);

      QRect r(x1 + dx + padx, y1 + dy + pady, w, h);

      item->setGeometry(r);

      item->show();
    }

    return changed;
  };

  placeWidgets();
}

QSize
CTkAppGridLayout::
sizeHint() const
{
  return calculateSize(SizeHint);
}

QLayoutItem *
CTkAppGridLayout::
takeAt(int index)
{
  if (index >= 0 && index < list_.size()) {
    ItemWrapper *layoutStruct = list_.takeAt(index);

    return layoutStruct->item;
  }

  return nullptr;
}

void
CTkAppGridLayout::
add(QLayoutItem *item, const Info &info)
{
  list_.append(new ItemWrapper(item, info));
}

QSize
CTkAppGridLayout::
calculateSize(SizeType sizeType) const
{
  std::vector<int> prefColWidths, prefRowHeights;
  int              prefWidth, prefHeight;

  calcPrefSizes(sizeType, prefColWidths, prefRowHeights, prefWidth, prefHeight);

  return QSize(prefWidth, prefHeight);
}

void
CTkAppGridLayout::
calculateDims(uint &num_rows, uint &num_cols) const
{
  // get num rows and columns
  num_rows = 0;
  num_cols = 0;

  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    int row = wrapper->info.getRow();
    int col = wrapper->info.getCol();

    int rowSpan = wrapper->info.getRowSpan();
    int colSpan = wrapper->info.getColSpan();

    num_rows = std::max(num_rows, uint(row + rowSpan));
    num_cols = std::max(num_cols, uint(col + colSpan));
  }
}

void
CTkAppGridLayout::
calcPrefSizes(SizeType sizeType, std::vector<int> &prefColWidths,
              std::vector<int> &prefRowHeights, int &prefWidth, int &prefHeight) const
{
  // get num rows and columns
  uint num_rows, num_cols;

  calculateDims(num_rows, num_cols);

  //---

  std::vector<int> weights;

  weights.resize(std::max(num_rows, num_cols));

  //---

  // calculate preferred (min) size for rows/cols
  prefColWidths .resize(size_t(num_cols));
  prefRowHeights.resize(size_t(num_rows));

  for (uint i = 0; i < num_cols; ++i)
    prefColWidths[i] = 0;

  for (uint i = 0; i < num_rows; ++i)
    prefRowHeights[i] = 0;

  //---

  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    auto *item = dynamic_cast<CTkAppLayoutWidget *>(wrapper->item);
    assert(item);

    int rowSpan = wrapper->info.getRowSpan();
    int colSpan = wrapper->info.getColSpan();

    if (rowSpan != 1 || colSpan != 1)
      continue;

    int row = wrapper->info.getRow();
    int col = wrapper->info.getCol();

    QSize itemSize;

    if (sizeType == MinimumSize)
      itemSize = item->minimumSize();
    else // (sizeType == SizeHint)
      itemSize = item->sizeHint();

    itemSize = itemSize.expandedTo(QApplication::globalStrut());

    int padx = wrapper->info.getPadX();
    int pady = wrapper->info.getPadY();

    int ipadx = wrapper->info.getIPadX();
    int ipady = wrapper->info.getIPadY();

    int iw = itemSize.width () + 2*ipadx + 2*padx;
    int ih = itemSize.height() + 2*ipady + 2*pady;

    prefColWidths [col] = std::max(prefColWidths [col], iw);
    prefRowHeights[row] = std::max(prefRowHeights[row], ih);
  }

  //---

  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    auto *item = dynamic_cast<CTkAppLayoutWidget *>(wrapper->item);
    assert(item);

    int rowSpan = wrapper->info.getRowSpan();
    int colSpan = wrapper->info.getColSpan();

    if (rowSpan == 1 && colSpan == 1)
      continue;

    int row1 = wrapper->info.getRow();
    int row2 = row1 + rowSpan;
    int col1 = wrapper->info.getCol();
    int col2 = col1 + colSpan;

    QSize itemSize;

    if (sizeType == MinimumSize)
      itemSize = item->minimumSize();
    else // (sizeType == SizeHint)
      itemSize = item->sizeHint();

    itemSize = itemSize.expandedTo(QApplication::globalStrut());

    int padx = wrapper->info.getPadX();
    int pady = wrapper->info.getPadY();

    int ipadx = wrapper->info.getIPadX();
    int ipady = wrapper->info.getIPadY();

    int iw = itemSize.width () + 2*ipadx + 2*padx;
    int ih = itemSize.height() + 2*ipady + 2*pady;

    int sw = 0, sh = 0;

    for (int j = col1; j < col2; ++j)
      sw += prefColWidths[j];

    for (int j = row1; j < row2; ++j)
      sh += prefRowHeights[j];

    if (iw > sw) {
      int maxWeight = 0;

      for (int j = col1; j < col2; ++j) {
        auto w = getColumnWeight(j);

        weights[j] = w;

        maxWeight = std::max(maxWeight, w);
      }

      int sumWeights = 0;

      for (int j = col1; j < col2; ++j) {
        if (maxWeight == 0)
          weights[j] = 1;

        sumWeights += weights[j];
      }

      if (sumWeights > 0) {
        int dw = iw - sw;

        int dw1 = dw/sumWeights;
        int dw2 = dw - dw1*sumWeights;

        for (int j = col1; j < col2; ++j) {
          if (weights[j] == 0)
            continue;

          int dr = (dw2 > 0 ? 1 : 0); --dw2;

          prefColWidths[j] += (dw1 + dr)*weights[j];
        }
      }
    }

    if (ih > sh) {
      int maxWeight = 0;

      for (int j = row1; j < row2; ++j) {
        auto w = getRowWeight(j);

        weights[j] = w;

        maxWeight = std::max(maxWeight, w);
      }

      int sumWeights = 0;

      for (int j = row1; j < row2; ++j) {
        if (maxWeight == 0)
          weights[j] = 1;

        sumWeights += weights[j];
      }

      if (sumWeights > 0) {
        int dh = ih - sh;

        int dh1 = dh/sumWeights;
        int dh2 = dh - dh1*sumWeights;

        for (int j = row1; j < row2; ++j) {
          if (weights[j] == 0)
            continue;

          int dr = (dh2 > 0 ? 1 : 0); --dh2;

          prefRowHeights[j] += (dh1 + dr)*weights[j];
        }
      }
    }
  }

  //---

  // calculate preferred total width/height
  prefWidth = 0;
  prefHeight = 0;

  for (uint i = 0; i < num_cols; ++i)
    prefWidth += prefColWidths[i];

  for (uint i = 0; i < num_rows; ++i)
    prefHeight += prefRowHeights[i];
}

void
CTkAppGridLayout::
draw(QPainter *p) const
{
  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    auto *item = dynamic_cast<CTkAppLayoutWidget *>(wrapper->item);
    assert(item);

    auto r = item->geometry();

    p->setPen(Qt::red);

    p->drawRect(r);
  }
}

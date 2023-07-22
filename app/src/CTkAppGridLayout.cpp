#include <CTkAppGridLayout.h>
#include <CTkAppLayoutWidget.h>

#include <QWidget>

#include <set>

CTkAppGridLayout::
CTkAppGridLayout(QWidget *parent, int margin, int spacing) :
 QLayout(parent)
{
  setMargin(margin);
  setSpacing(spacing);
}

CTkAppGridLayout::
CTkAppGridLayout(int spacing) :
 QLayout(nullptr)
{
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
addWidgets(const std::vector<CTkAppWidget *> &widgets, const Info &info)
{
  Info info1 = info;

  if (info1.isRowValid()) row_ = info1.getRow();
  if (info1.isColValid()) col_ = info1.getCol();

  uint num_added = 0;

  auto num = widgets.size();

  for (size_t i = 0; i < num; ++i) {
    auto *wrapper = getItem(widgets[i]);

    if (! wrapper) {
      info1.setRow(row_);
      info1.setCol(col_);

      add(new CTkAppLayoutWidget(widgets[i]), info1);

      ++col_;

      ++num_added;
    }
    else
      wrapper->info.update(info1);
  }

  if (num_added > 0) {
    row_ += 1;
    col_  = 0;
  }
}

void
CTkAppGridLayout::
addWidget(CTkAppWidget *widget, const Info &info)
{
  auto *wrapper = getItem(widget);

  if (! wrapper)
    add(new CTkAppLayoutWidget(widget), info);
  else
    wrapper->info.update(info);
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

void
CTkAppGridLayout::
setColumnWeight(int col, int weight)
{
  colWeights_[col] = weight;
}

void
CTkAppGridLayout::
setRowWeight(int col, int weight)
{
  rowWeights_[col] = weight;
}

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
  uint num_rows = 0;
  uint num_cols = 0;

  QLayout::setGeometry(rect);

  // get num rows and columns
  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    int row = wrapper->info.getRow();
    int col = wrapper->info.getCol();

    num_rows = std::max(num_rows, uint(row + 1));
    num_cols = std::max(num_cols, uint(col + 1));
  }

  // create column and row grids and sizes
  std::vector<int> colWidths, rowHeights, prefColWidths, prefRowHeights;
  std::set<int>    resizeColumns, resizeRows;

  colWidths     .resize(size_t(num_cols));
  rowHeights    .resize(size_t(num_rows));
  prefColWidths .resize(size_t(num_cols));
  prefRowHeights.resize(size_t(num_rows));

  //---

  // default to equal sized across all rows/columns
  int initColWidth  = rect.width ()/num_cols;
  int initRowHeight = rect.height()/num_rows;

  for (uint i = 0; i < num_cols; ++i)
    colWidths[i] = initColWidth;

  for (uint i = 0; i < num_rows; ++i)
    rowHeights[i] = initRowHeight;

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

    for (uint i = 0; i < num_cols; ++i)
      prefColWidths[i] = 0;

    for (uint i = 0; i < num_rows; ++i)
      prefRowHeights[i] = 0;

    resizeColumns.clear();
    resizeRows   .clear();

    for (int i = 0; i < list_.size(); ++i) {
      auto *wrapper = list_.at(i);

      auto *item = dynamic_cast<CTkAppLayoutWidget *>(wrapper->item);
      assert(item);

      int  row   = wrapper->info.getRow();
      int  col   = wrapper->info.getCol();
      uint sides = wrapper->info.getStickySides();

      int padx = wrapper->info.getPadX();
      int pady = wrapper->info.getPadY();

      int ipadx = wrapper->info.getIPadX();
      int ipady = wrapper->info.getIPadY();

      auto itemSize = item->sizeHint();

      int iw = itemSize.width () + 2*ipadx;
      int ih = itemSize.height() + 2*ipady;

      prefColWidths [col] = std::max(prefColWidths [col], iw);
      prefRowHeights[row] = std::max(prefRowHeights[row], ih);

      int colWidth1  = colWidths [size_t(col)];
      int colHeight1 = rowHeights[size_t(row)];

      int x1 = colX(col);
      int y1 = rowX(row);

      int dx = 0, dy = 0;

      if      ((sides & STICKY_W) && (sides & STICKY_E)) { resizeColumns.insert(col); }
      else if ((sides & STICKY_W)                      ) dx = 0;
      else if ((sides & STICKY_E)                      ) dx = colWidth1  - iw - 2*padx;
      else                                               dx = (colWidth1 - iw - 2*padx)/2;

      if      ((sides & STICKY_N) && (sides & STICKY_S)) { resizeRows.insert(row); }
      else if ((sides & STICKY_N)                      ) dy = 0;
      else if ((sides & STICKY_S)                      ) dy = colHeight1  - ih - 2*pady;
      else                                               dy = (colHeight1 - ih - 2*pady)/2;

      int w = ((sides & STICKY_W) && (sides & STICKY_E) ? colWidth1  - 2*padx: iw);
      int h = ((sides & STICKY_N) && (sides & STICKY_S) ? colHeight1 - 2*pady: ih);

      QRect r(x1 + dx + padx, y1 + dy + pady, w, h);

      if (r != item->geometry()) {
        item->setGeometry(r);
        changed = true;
      }

      item->show();
    }

    return changed;
  };

  placeWidgets();

  auto spreadColumns = [&]() {
    std::set<int> extraColumns, clippedColumns;

    int extraWidth = 0;

    for (uint i = 0; i < num_cols; ++i) {
      if (resizeColumns.find(i) != resizeColumns.end())
        prefColWidths[i] = INT_MAX;

      if      (colWidths[i] > prefColWidths[i]) {
        extraColumns.insert(i);

        extraWidth += colWidths[i] - prefColWidths[i];
      }
      else if (colWidths[i] < prefColWidths[i]) {
        clippedColumns.insert(i);
      }
    }

    if (extraWidth > 0 && ! clippedColumns.empty()) {
      int numClipped = clippedColumns.size();

      int adjustWidth = extraWidth/numClipped;

      for (auto i : clippedColumns)
        colWidths[i] += std::min(prefColWidths[i] - colWidths[i], adjustWidth);

      for (auto i : extraColumns)
        colWidths[i] -= std::min(colWidths[i] - prefColWidths[i], adjustWidth);

      return placeWidgets();
    }

    return false;
  };

  auto spreadRows = [&]() {
    std::set<int> extraRows, clippedRows;

    int extraHeight = 0;

    for (uint i = 0; i < num_rows; ++i) {
      if (resizeRows.find(i) != resizeRows.end())
        prefRowHeights[i] = INT_MAX;

      if      (rowHeights[i] > prefRowHeights[i]) {
        extraRows.insert(i);

        extraHeight += rowHeights[i] - prefRowHeights[i];
      }
      else if (rowHeights[i] < prefRowHeights[i]) {
        clippedRows.insert(i);
      }
    }

    if (extraHeight > 0 && ! clippedRows.empty()) {
      int numClipped = clippedRows.size();

      int adjustHeight = extraHeight/numClipped;

      for (auto i : clippedRows)
        rowHeights[i] += std::min(prefRowHeights[i] - rowHeights[i], adjustHeight);

      for (auto i : extraRows)
        rowHeights[i] -= std::min(rowHeights[i] - prefRowHeights[i], adjustHeight);

      return placeWidgets();
    }

    return false;
  };

  spreadColumns();
  spreadRows();
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
#if 0
  auto fixSize = [](QLayoutItem *item, int &s) {
    int s1 = s;
    s = std::min(std::max(s1, 1), 2048);
    if (s != s1) {
      std::cerr << "Bad Size: " << s1 << "\n";
      auto *w = item->widget();
      if (w) std::cerr << "  Widget:" << w->objectName().toStdString() << "\n";
    }
  };
#endif

  using SizeMap = std::map<int, int>;

  SizeMap rowHeight, colWidth;
  int     num_rows = 0, num_cols = 0;

  for (int i = 0; i < list_.size(); ++i) {
    ItemWrapper *wrapper = list_.at(i);

    auto *item = wrapper->item;

    int row = wrapper->info.getRow();
    int col = wrapper->info.getCol();

    int padx = wrapper->info.getPadX();
    int pady = wrapper->info.getPadY();

    int ipadx = wrapper->info.getIPadX();
    int ipady = wrapper->info.getIPadY();

    num_rows = std::max(num_rows, row + 1);
    num_cols = std::max(num_cols, col + 1);

    QSize itemSize;

    if (sizeType == MinimumSize)
      itemSize = item->minimumSize();
    else // (sizeType == SizeHint)
      itemSize = item->sizeHint();

    int iw = itemSize.width () + 2*padx + 2*ipadx;
    int ih = itemSize.height() + 2*pady + 2*ipady;

    //fixSize(item, iw);
    //fixSize(item, ih);

    if (rowHeight.find(row) == rowHeight.end())
      rowHeight[row] = ih;
    else
      rowHeight[row] = std::max(rowHeight[row], ih);

    if (colWidth.find(row) == colWidth.end())
      colWidth[col] = iw;
    else
      colWidth[col] = std::max(colWidth[col], iw);
  }

  int w = 0, h = 0;

  for (int row = 0; row < num_rows; ++row)
    if (rowHeight.find(row) != rowHeight.end())
      h += rowHeight[row];

  for (int col = 0; col < num_cols; ++col)
    if (colWidth.find(col) != colWidth.end())
      w += colWidth[col];

  if (w <= 0) w = 1;
  if (h <= 0) h = 1;

  return QSize(w, h);
}

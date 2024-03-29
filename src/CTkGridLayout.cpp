#include <CTkLayout.h>

CTkGridLayout::
CTkGridLayout(QWidget *parent, int margin, int spacing) :
 QLayout(parent)
{
  setMargin(margin);
  setSpacing(spacing);
}

CTkGridLayout::
CTkGridLayout(int spacing) :
 QLayout(nullptr)
{
  setSpacing(spacing);
}

CTkGridLayout::
~CTkGridLayout()
{
  QLayoutItem *l;

  while ((l = takeAt(0)))
    delete l;
}

void
CTkGridLayout::
addItem(QLayoutItem *item)
{
  add(item, Info());
}

void
CTkGridLayout::
addWidgets(const std::vector<CTkWidget *> &widgets, const Info &info)
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

      add(new CTkLayoutWidget(widgets[i]), info1);

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
CTkGridLayout::
addWidget(CTkWidget *widget, const Info &info)
{
  auto *wrapper = getItem(widget);

  if (! wrapper)
    add(new CTkLayoutWidget(widget), info);
  else
    wrapper->info.update(info);
}

CTkGridLayout::ItemWrapper *
CTkGridLayout::
getItem(CTkWidget *widget) const
{
  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    auto *w = dynamic_cast<CTkLayoutWidget *>(wrapper->item);
    if (! w) continue;

    auto *widget1 = w->getTkWidget();

    if (widget == widget1)
      return wrapper;
  }

  return nullptr;
}

bool
CTkGridLayout::
getChildInfo(CTkWidget *widget, Info &info)
{
  auto *wrapper = getItem(widget);
  if (! wrapper) return false;

  info = wrapper->info;

  return true;
}

bool
CTkGridLayout::
setChildWeight(CTkWidget *widget, int weight)
{
  auto *wrapper = getItem(widget);
  if (! wrapper) return false;

  wrapper->weight = weight;

  return true;
}

void
CTkGridLayout::
setColumnWeight(int col, int weight)
{
  colWeights_[col] = weight;
}

Qt::Orientations
CTkGridLayout::
expandingDirections() const
{
  return Qt::Horizontal | Qt::Vertical;
}

bool
CTkGridLayout::
hasHeightForWidth() const
{
  // preferred height depends on width
  return false;
}

int
CTkGridLayout::
count() const
{
  return list_.size();
}

QLayoutItem *
CTkGridLayout::itemAt(int index) const
{
  auto *wrapper = list_.value(index);
  if (! wrapper) return nullptr;

  return wrapper->item;
}

QSize
CTkGridLayout::
minimumSize() const
{
  return calculateSize(MinimumSize);
}

void
CTkGridLayout::
setGeometry(const QRect &rect)
{
  int num_rows = 0;
  int num_cols = 0;

  QLayout::setGeometry(rect);

  // get num rows and columns
  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    int row = wrapper->info.getRow();
    int col = wrapper->info.getCol();

    num_rows = std::max(num_rows, row + 1);
    num_cols = std::max(num_cols, col + 1);
  }

  // create column and row grids and sizes
  std::vector<int> cws, rhs;

  cws.resize(size_t(num_cols));
  rhs.resize(size_t(num_rows));

  int cw = rect.width ()/num_cols;
  int rh = rect.height()/num_rows;

  for (int i = 0; i < num_cols; ++i)
    cws[size_t(i)] = cw;

  for (int i = 0; i < num_rows; ++i)
    rhs[size_t(i)] = rh;

  // place widgets in grid
  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    auto *item  = wrapper->item;
    int   row   = wrapper->info.getRow();
    int   col   = wrapper->info.getCol();
    uint  sides = wrapper->info.getStickySides();

    int padx = wrapper->info.getPadX();
    int pady = wrapper->info.getPadY();

    int ipadx = wrapper->info.getIPadX();
    int ipady = wrapper->info.getIPadY();

    int wh = item->sizeHint().width () + 2*ipadx;
    int hh = item->sizeHint().height() + 2*ipady;

    int cw = cws[size_t(col)];
    int rh = rhs[size_t(row)];

    int x1 = rect.x() + col*cw;
    int y1 = rect.y() + row*rh;

    int dx = 0, dy = 0;

    if      (sides & STICKY_W && sides & STICKY_E) dx = 0;
    else if (sides & STICKY_W                    ) dx = 0;
    else if (sides & STICKY_E                    ) dx = cw - wh - 2*padx;
    else                                           dx = (cw - wh - 2*padx)/2;

    if      (sides & STICKY_N && sides & STICKY_S) dy = 0;
    else if (sides & STICKY_N                    ) dy = 0;
    else if (sides & STICKY_S                    ) dy = rh - hh - 2*pady;
    else                                           dy = (rh - hh - 2*pady)/2;

    int w = (sides & STICKY_W && sides & STICKY_E ? cw - 2*padx: wh);
    int h = (sides & STICKY_N && sides & STICKY_S ? rh - 2*pady: hh);

    item->setGeometry(QRect(x1 + dx + padx, y1 + dy + pady, w, h));
  }
}

QSize
CTkGridLayout::
sizeHint() const
{
  return calculateSize(SizeHint);
}

QLayoutItem *
CTkGridLayout::
takeAt(int index)
{
  if (index >= 0 && index < list_.size()) {
    ItemWrapper *layoutStruct = list_.takeAt(index);

    return layoutStruct->item;
  }

  return nullptr;
}

void
CTkGridLayout::
add(QLayoutItem *item, const Info &info)
{
  list_.append(new ItemWrapper(item, info));
}

QSize
CTkGridLayout::
calculateSize(SizeType sizeType) const
{
  typedef std::map<int,int> SizeMap;

  SizeMap rowHeight, colWidth;
  int     num_rows = 0, num_cols = 0;

  for (int i = 0; i < list_.size(); ++i) {
    ItemWrapper *wrapper = list_.at(i);

    QLayoutItem *item = wrapper->item;
    int          row  = wrapper->info.getRow();
    int          col  = wrapper->info.getCol();

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

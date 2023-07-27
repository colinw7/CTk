#include <CTkAppPackLayout.h>
#include <CTkAppLayoutWidget.h>
#include <CTkAppWidget.h>
#include <CTkApp.h>

#include <QWidget>
#include <QPainter>

CTkAppPackLayout::
CTkAppPackLayout(QWidget *parent, int margin, int spacing) :
 QLayout(parent)
{
  setObjectName("pack");

  setMargin(margin);
  setSpacing(spacing);
}

CTkAppPackLayout::
CTkAppPackLayout(int spacing)
{
  setObjectName("pack");

  setSpacing(spacing);
}

CTkAppPackLayout::
~CTkAppPackLayout()
{
  QLayoutItem *l;

  while ((l = takeAt(0)))
    delete l;
}

void
CTkAppPackLayout::
addItem(QLayoutItem *item)
{
  add(item, Info(SIDE_TOP, FILL_NONE, false, 0, 0, 0, 0));
}

void
CTkAppPackLayout::
addWidgets(const std::vector<CTkAppWidget *> &widgets, const Info &info)
{
  auto *parent = parentWidget();

  for (auto *widget : widgets) {
    if (widget->parentWidget() != parent)
      widget->setParentWidget(parent);
  }

  auto *layout = new CTkAppPackLayout;

  for (auto *widget : widgets)
    layout->add(new CTkAppLayoutWidget(widget), info);

  add(new CTkAppLayoutWidget(layout), info);
}

void
CTkAppPackLayout::
addWidget(CTkAppWidget *widget, const Info &info)
{
  auto *parent = parentWidget();

  if (widget->parentWidget() != parent)
    widget->setParentWidget(parent);

  add(new CTkAppLayoutWidget(widget), info);
}

void
CTkAppPackLayout::
removeWidget(CTkAppWidget *widget)
{
  int ind = -1;

  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    auto *w = dynamic_cast<CTkAppLayoutWidget *>(wrapper->item);
    if (! w) continue;

    auto *widget1 = w->getTkWidget();
    auto *layout1 = dynamic_cast<CTkAppPackLayout *>(w->getLayout());

    if      (widget1 == widget) {
      ind = i;
      break;
    }
    else if (layout1) {
      layout1->removeWidget(widget);
    }
  }

  if (ind != -1) {
    auto *item = takeAt(ind);

    delete item;
  }
}

std::vector<CTkAppWidget *>
CTkAppPackLayout::
getWidgets() const
{
  std::vector<CTkAppWidget *> widgets;

  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    auto *w = dynamic_cast<CTkAppLayoutWidget *>(wrapper->item);
    if (! w) continue;

    auto *widget1 = w->getTkWidget();
    auto *layout1 = dynamic_cast<CTkAppPackLayout *>(w->getLayout());

    if      (widget1)
      widgets.push_back(widget1);
    else if (layout1)
      layout1->getWidgets();
  }

  return widgets;
}

CTkAppPackLayout::ItemWrapper *
CTkAppPackLayout::
getItem(CTkAppWidget *widget) const
{
  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    auto *w = dynamic_cast<CTkAppLayoutWidget *>(wrapper->item);
    if (! w) continue;

    auto *widget1 = w->getTkWidget();
    auto *layout1 = dynamic_cast<CTkAppPackLayout *>(w->getLayout());

    if (widget1) {
      if (widget == widget1)
        return wrapper;
    }
    else if (layout1) {
      auto *item1 = layout1->getItem(widget);

      if (item1)
        return item1;
    }
  }

  return nullptr;
}

bool
CTkAppPackLayout::
getChildInfo(CTkAppWidget *widget, Info &info)
{
  auto *wrapper = getItem(widget);

  if (! wrapper) return false;

  info = wrapper->info;

  return true;
}

Qt::Orientations
CTkAppPackLayout::
expandingDirections() const
{
  return Qt::Horizontal | Qt::Vertical;
}

bool
CTkAppPackLayout::
hasHeightForWidth() const
{
  // preferred height depends on width
  return false;
}

int
CTkAppPackLayout::
count() const
{
  return list_.size();
}

QLayoutItem *
CTkAppPackLayout::
itemAt(int index) const
{
  auto *wrapper = list_.value(index);

  if (wrapper)
    return wrapper->item;

  return nullptr;
}

QSize
CTkAppPackLayout::
minimumSize() const
{
  return calculateSize(MinimumSize);
}

void
CTkAppPackLayout::
setGeometry(const QRect &rect)
{
  QLayout::setGeometry(rect);

  auto placeRect = rect;

  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    auto *item = dynamic_cast<CTkAppLayoutWidget *>(wrapper->item);
    assert(item);

    const Info &info = wrapper->info;

    int iw = item->sizeHint().width ();
    int ih = item->sizeHint().height();

    int rw = placeRect.width ();
    int rh = placeRect.height();

    int x1 = placeRect.x();
    int y1 = placeRect.y();
    int x2 = x1 + rw;
    int y2 = y1 + rh;

    int w = iw + 2*info.ipadx;
    int h = ih + 2*info.ipady;

    if      (info.side == SIDE_LEFT) {
      if (info.fill == FILL_Y || info.fill == FILL_BOTH)
        h = rh;

      int dy = (rh - h - 2*info.ipady)/2;

      auto r = QRect(x1 + info.padx, y1 + info.pady + dy, w + 2*info.padx, h + 2*info.pady);

      item->setGeometry(r);

      placeRect.setLeft(placeRect.left() + r.width());
    }
    else if (info.side == SIDE_RIGHT) {
      if (info.fill == FILL_Y || info.fill == FILL_BOTH)
        h = rh;

      int dy = (rh - h - 2*info.ipady)/2;

      auto r = QRect(x2 - w - info.padx, y1 + info.pady + dy, w + 2*info.padx, h + 2*info.pady);

      item->setGeometry(r);

      placeRect.setRight(placeRect.right() - r.width());
    }
    else if (info.side == SIDE_TOP) {
      if (info.fill == FILL_X || info.fill == FILL_BOTH)
        w = rw;

      int dx = (rw - w - 2*info.ipadx)/2;

      auto r = QRect(x1 + info.padx + dx, y1 + info.pady, w + 2*info.ipadx, h + 2*info.ipady);

      item->setGeometry(r);

      placeRect.setTop(placeRect.top() + r.height());
    }
    else if (info.side == SIDE_BOTTOM) {
      if (info.fill == FILL_X || info.fill == FILL_BOTH)
        w = rw;

      int dx = (rw - w - 2*info.ipadx)/2;

      auto r = QRect(x1 + info.padx + dx, y2 - h - info.padx, w + 2*info.ipadx, h + 2*info.ipady);

      item->setGeometry(r);

      placeRect.setBottom(placeRect.bottom() - r.height());
    }
    else {
      auto r = QRect(x1 + info.padx, y1 + info.pady, w + 2*info.ipadx, h + 2*info.ipady);

      item->setGeometry(r);
    }

    item->show();
  }
}

QSize
CTkAppPackLayout::
sizeHint() const
{
  return calculateSize(SizeHint);
}

QLayoutItem *
CTkAppPackLayout::
takeAt(int index)
{
  if (index >= 0 && index < list_.size()) {
    auto *layoutStruct = list_.takeAt(index);

    return layoutStruct->item;
  }

  return nullptr;
}

void
CTkAppPackLayout::
add(QLayoutItem *item, const Info &info)
{
  list_.append(new ItemWrapper(item, info));
}

QSize
CTkAppPackLayout::
calculateSize(SizeType sizeType) const
{
  QSize totalSize(0, 0);

  int topHeight    = 0;
  int bottomHeight = 0;
  int leftWidth    = 0;
  int rightWidth   = 0;

  int maxWidth  = 0;
  int maxHeight = 0;

  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    const Info &info = wrapper->info;

    QSize itemSize;

    if (sizeType == MinimumSize)
      itemSize = wrapper->item->minimumSize();
    else // if (sizeType == SizeHint)
      itemSize = wrapper->item->sizeHint();

    itemSize += QSize(2*info.padx , 2*info.pady );
    itemSize += QSize(2*info.ipadx, 2*info.ipady);

    if      (info.side == SIDE_TOP) {
      topHeight += itemSize.height();

      maxWidth = std::max(maxWidth, leftWidth + rightWidth + itemSize.width());
    }
    else if (info.side == SIDE_LEFT) {
      leftWidth += itemSize.width();

      maxHeight = std::max(maxHeight, bottomHeight + topHeight + itemSize.height());
    }
    else if (info.side == SIDE_BOTTOM) {
      bottomHeight += itemSize.height();

      maxWidth = std::max(maxWidth, leftWidth + rightWidth + itemSize.width());
    }
    else if (info.side == SIDE_RIGHT) {
      rightWidth += itemSize.width();

      maxHeight = std::max(maxHeight, bottomHeight + topHeight + itemSize.height());
    }
    else {
      leftWidth += itemSize.width ();
      topHeight += itemSize.height();

      maxWidth  = std::max(maxWidth , leftWidth + rightWidth + itemSize.width());
      maxHeight = std::max(maxHeight, bottomHeight + topHeight + itemSize.height());
    }
  }

  totalSize.setWidth (std::max(std::max(leftWidth    + rightWidth, maxWidth ), 10));
  totalSize.setHeight(std::max(std::max(bottomHeight + topHeight , maxHeight), 10));

  return totalSize;
}

#if 0
void
CTkAppPackLayout::
draw(QPainter *p) const
{
  auto nx = xyvals_.getNumXVals();
  auto ny = xyvals_.getNumYVals();

  for (int iy = 0; iy < ny - 1; ++iy) {
    auto y1 = xyvals_.yval(iy);
    auto y2 = xyvals_.yval(iy + 1);

    for (int ix = 0; ix < nx - 1; ++ix) {
      auto x1 = xyvals_.xval(ix);
      auto x2 = xyvals_.xval(ix + 1);

      if (xyvals_.insideVal(ix, iy) == 0) {
        p->fillRect(QRect(x1, y1, x2 - x1, y2 - y1), Qt::red);
      }
    }
  }
}
#endif

#include <CTkLayout.h>
#include <CTk.h>
#include <QWidget>

CTkPackLayout::
CTkPackLayout(QWidget *parent, int margin, int spacing) :
 QLayout(parent)
{
  setMargin(margin);
  setSpacing(spacing);
}

CTkPackLayout::
CTkPackLayout(int spacing)
{
  setSpacing(spacing);
}

CTkPackLayout::
~CTkPackLayout()
{
  QLayoutItem *l;

  while ((l = takeAt(0)))
    delete l;
}

void
CTkPackLayout::
addItem(QLayoutItem *item)
{
  add(item, Info(SIDE_TOP, FILL_NONE, false, 0, 0, 0, 0));
}

void
CTkPackLayout::
addWidgets(const std::vector<CTkWidget *> &widgets, const Info &info)
{
  auto num = widgets.size();

  for (size_t i = 0; i < num; ++i)
    addWidget(widgets[i], info);
}

void
CTkPackLayout::
addWidget(CTkWidget *widget, const Info &info)
{
  add(new CTkLayoutWidget(widget), info);
}

CTkPackLayout::ItemWrapper *
CTkPackLayout::
getItem(CTkWidget *widget) const
{
  for (int i = 0; i < list_.size(); ++i) {
    ItemWrapper *wrapper = list_.at(i);

    CTkLayoutWidget *w = dynamic_cast<CTkLayoutWidget *>(wrapper->item);

    if (! w) continue;

    CTkWidget *widget1 = w->getTkWidget();

    if (widget == widget1)
      return wrapper;
  }

  return NULL;
}

bool
CTkPackLayout::
getChildInfo(CTkWidget *widget, Info &info)
{
  ItemWrapper *wrapper = getItem(widget);

  if (! wrapper) return false;

  info = wrapper->info;

  return true;
}

Qt::Orientations
CTkPackLayout::
expandingDirections() const
{
  return Qt::Horizontal | Qt::Vertical;
}

bool
CTkPackLayout::
hasHeightForWidth() const
{
  // preferred height depends on width
  return false;
}

int
CTkPackLayout::
count() const
{
  return list_.size();
}

QLayoutItem *
CTkPackLayout::
itemAt(int index) const
{
  auto *wrapper = list_.value(index);

  if (wrapper)
    return wrapper->item;
  else
    return NULL;
}

QSize
CTkPackLayout::
minimumSize() const
{
  return calculateSize(MinimumSize);
}

void
CTkPackLayout::
setGeometry(const QRect &rect)
{
  int topHeight    = 0;
  int bottomHeight = 0;
  int leftWidth    = 0;
  int rightWidth   = 0;

  QLayout::setGeometry(rect);

  for (int i = 0; i < list_.size(); ++i) {
    ItemWrapper *wrapper = list_.at(i);

    QLayoutItem * item = wrapper->item;
    const Info &  info = wrapper->info;

    int wh = item->sizeHint().width ();
    int hh = item->sizeHint().height();

    int rh = rect.height() - topHeight - bottomHeight;
    int rw = rect.width () - leftWidth - rightWidth;
    int x1 = rect.x() + leftWidth;
    int y1 = rect.y() + topHeight;

    if      (info.side == SIDE_TOP) {
      if (info.fill == FILL_X || info.fill == FILL_BOTH)
        item->setGeometry(QRect(x1 + info.padx, y1 + info.pady,
                                rw - 2*info.padx, hh + 2*info.ipady));
      else {
        int dx = (rw - wh - 2*info.padx - 2*info.ipadx)/2;

        item->setGeometry(QRect(x1 + info.padx + dx, y1 + info.pady,
                                wh + 2*info.ipadx, hh + 2*info.ipady));
      }

      QRect g = item->geometry();

      topHeight += g.height() + 2*info.pady;
    }
    else if (info.side == SIDE_LEFT) {
      if (info.fill == FILL_Y || info.fill == FILL_BOTH)
        item->setGeometry(QRect(x1 + info.padx, y1 + info.pady,
                                wh + 2*info.ipadx, rh - 2*info.pady));
      else {
        int dy = (rh - hh - 2*info.pady - 2*info.ipady)/2;

        item->setGeometry(QRect(x1 + info.padx, y1 + info.pady + dy,
                                wh + 2*info.ipadx, hh + 2*info.ipady));
      }

      QRect g = item->geometry();

      leftWidth += g.width() + 2*info.padx;
    }
    else if (info.side == SIDE_BOTTOM) {
      int dx = 0;

      // resize
      QRect g = item->geometry();

      if (info.fill == FILL_X || info.fill == FILL_BOTH)
        item->setGeometry(QRect(g.x(), g.y(), rw, hh + 2*info.ipady));
      else {
        dx = (rw - wh - 2*info.ipadx)/2;

        item->setGeometry(QRect(g.x(), g.y(), wh + 2*info.ipadx, hh + 2*info.ipady));
      }

      // position
      g = item->geometry();

      bottomHeight += g.height();

      int y2 = rect.y() + rect.height() - bottomHeight;

      item->setGeometry(QRect(x1 + dx, y2, g.width(), g.height()));
    }
    else if (info.side == SIDE_RIGHT) {
      int dy = 0;

      // resize
      QRect g = item->geometry();

      if (info.fill == FILL_Y || info.fill == FILL_BOTH)
        item->setGeometry(QRect(g.x(), g.y(), wh + 2*info.ipadx, rh));
      else {
        dy = (rh - hh - 2*info.ipady)/2;

        item->setGeometry(QRect(g.x(), g.y(), wh + 2*info.ipadx, hh + 2*info.ipady));
      }

      // position
      g = item->geometry();

      rightWidth += g.width();

      int x2 = rect.x() + rect.width() - rightWidth;

      item->setGeometry(QRect(x2, y1 + dy, g.width(), g.height()));
    }
  }

  for (int i = 0; i < list_.size(); ++i) {
    ItemWrapper *wrapper = list_.at(i);

    QLayoutItem * item = wrapper->item;
    const Info &  info = wrapper->info;

//  int wh = item->sizeHint().width ();
//  int hh = item->sizeHint().height();

    int rh = rect.height() - topHeight - bottomHeight;
    int rw = rect.width () - leftWidth - rightWidth;
    int x1 = rect.x() + leftWidth;
    int y1 = rect.y() + topHeight;
  //int x2 = rect.x() + rect.width () - rightWidth;
  //int y2 = rect.y() + rect.height() - bottomHeight;

    if      (info.side == SIDE_NONE)
      item->setGeometry(QRect(x1, y1, rw, rh));
  }
}

QSize
CTkPackLayout::
sizeHint() const
{
  return calculateSize(SizeHint);
}

QLayoutItem *
CTkPackLayout::
takeAt(int index)
{
  if (index >= 0 && index < list_.size()) {
    ItemWrapper *layoutStruct = list_.takeAt(index);

    return layoutStruct->item;
  }

  return nullptr;
}

void
CTkPackLayout::
add(QLayoutItem *item, const Info &info)
{
  list_.append(new ItemWrapper(item, info));
}

QSize
CTkPackLayout::
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
    ItemWrapper *wrapper = list_.at(i);

    const Info &info = wrapper->info;

    QSize itemSize;

    if (sizeType == MinimumSize)
      itemSize = wrapper->item->minimumSize();
    else // (sizeType == SizeHint)
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
  }

  totalSize.setWidth (std::max(std::max(leftWidth    + rightWidth, maxWidth ), 10));
  totalSize.setHeight(std::max(std::max(bottomHeight + topHeight , maxHeight), 10));

  return totalSize;
}

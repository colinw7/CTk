#include <CTkAppPackLayout.h>
#include <CTkAppLayout.h>
#include <CTkAppWidget.h>
#include <CTkApp.h>
#include <QWidget>

CTkAppPackLayout::
CTkAppPackLayout(QWidget *parent, int margin, int spacing) :
 QLayout(parent)
{
  setMargin(margin);
  setSpacing(spacing);
}

CTkAppPackLayout::
CTkAppPackLayout(int spacing)
{
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
addWidgets(const std::vector<CTkWidget *> &widgets, const Info &info)
{
  auto num = widgets.size();

  for (size_t i = 0; i < num; ++i)
    addWidget(widgets[i], info);
}

void
CTkAppPackLayout::
addWidget(CTkWidget *widget, const Info &info)
{
  if (widget->getQWidget()->parentWidget() != parentWidget())
    widget->getQWidget()->setParent(parentWidget());

  add(new CTkLayoutWidget(widget), info);
}

CTkAppPackLayout::ItemWrapper *
CTkAppPackLayout::
getItem(CTkWidget *widget) const
{
  for (int i = 0; i < list_.size(); ++i) {
    ItemWrapper *wrapper = list_.at(i);

    auto *w = dynamic_cast<CTkLayoutWidget *>(wrapper->item);
    if (! w) continue;

    auto *widget1 = w->getTkWidget();

    if (widget == widget1)
      return wrapper;
  }

  return nullptr;
}

bool
CTkAppPackLayout::
getChildInfo(CTkWidget *widget, Info &info)
{
  ItemWrapper *wrapper = getItem(widget);

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
  int topHeight    = 0;
  int bottomHeight = 0;
  int leftWidth    = 0;
  int rightWidth   = 0;

  QLayout::setGeometry(rect);

  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    auto *item = dynamic_cast<CTkLayoutWidget *>(wrapper->item);

    const Info &info = wrapper->info;

    int iw = item->sizeHint().width ();
    int ih = item->sizeHint().height();

    int rh = rect.height() - topHeight - bottomHeight;
    int rw = rect.width () - leftWidth - rightWidth;
    int x1 = rect.x() + leftWidth;
    int y1 = rect.y() + topHeight;

    if (info.side == SIDE_LEFT) {
      if      (info.fill == FILL_X)
        item->setGeometry(QRect(x1 + info.padx, y1 + info.pady,
                                rw - 2*info.padx, ih + 2*info.ipady));
      else if (info.fill == FILL_Y)
        item->setGeometry(QRect(x1 + info.padx, y1 + info.pady,
                                iw + 2*info.ipadx, rh - 2*info.pady));
      else if (info.fill == FILL_BOTH)
        item->setGeometry(QRect(x1 + info.padx, y1 + info.pady,
                                rw - 2*info.padx, rh - 2*info.pady));
      else {
        int dy = (rh - ih - 2*info.pady - 2*info.ipady)/2;

        item->setGeometry(QRect(x1 + info.padx, y1 + info.pady + dy,
                                iw + 2*info.ipadx, ih + 2*info.ipady));
      }

      auto g = item->geometry();

      leftWidth += g.width() + 2*info.padx;
    }
    else if (info.side == SIDE_RIGHT) {
      int dy = 0;

      // resize
      auto g = item->geometry();

      if      (info.fill == FILL_X)
        item->setGeometry(QRect(g.x(), g.y(), rw - 2*info.padx, ih + 2*info.ipady));
      else if (info.fill == FILL_Y)
        item->setGeometry(QRect(g.x(), g.y(), iw + 2*info.ipadx, rh - 2*info.pady));
      else if (info.fill == FILL_BOTH)
        item->setGeometry(QRect(g.x(), g.y(), rw - 2*info.padx, rh - 2*info.pady));
      else {
        dy = (rh - ih - 2*info.ipady)/2;

        item->setGeometry(QRect(g.x(), g.y(), iw + 2*info.ipadx, ih + 2*info.ipady));
      }

      // position
      g = item->geometry();

      rightWidth += g.width() + 2*info.padx;

      int x2 = rect.x() + rect.width() - rightWidth;

      item->setGeometry(QRect(x2, y1 + dy, g.width(), g.height()));
    }
    else if (info.side == SIDE_TOP) {
      if      (info.fill == FILL_X)
        item->setGeometry(QRect(x1 + info.padx, y1 + info.pady,
                                rw - 2*info.padx, ih + 2*info.ipady));
      else if (info.fill == FILL_Y)
        item->setGeometry(QRect(x1 + info.padx, y1 + info.pady,
                                iw + 2*info.ipadx, rh - 2*info.pady));
      else if (info.fill == FILL_BOTH)
        item->setGeometry(QRect(x1 + info.padx, y1 + info.pady,
                                rw - 2*info.padx, rh - 2*info.pady));
      else {
        int dx = (rw - iw - 2*info.padx - 2*info.ipadx)/2;

        item->setGeometry(QRect(x1 + info.padx + dx, y1 + info.pady,
                                iw + 2*info.ipadx, ih + 2*info.ipady));
      }

      QRect g = item->geometry();

      topHeight += g.height() + 2*info.pady;
    }
    else if (info.side == SIDE_BOTTOM) {
      int dx = 0;

      // resize
      auto g = item->geometry();

      if      (info.fill == FILL_X)
        item->setGeometry(QRect(g.x(), g.y(), rw - 2*info.padx, ih + 2*info.ipady));
      else if (info.fill == FILL_Y)
        item->setGeometry(QRect(g.x(), g.y(), iw + 2*info.ipadx, rh - 2*info.pady));
      else if (info.fill == FILL_BOTH)
        item->setGeometry(QRect(g.x(), g.y(), rw - 2*info.padx, rh - 2*info.pady));
      else {
        dx = (rw - iw - 2*info.ipadx)/2;

        item->setGeometry(QRect(g.x(), g.y(), iw + 2*info.ipadx, ih + 2*info.ipady));
      }

      // position
      g = item->geometry();

      bottomHeight += g.height() + 2*info.padx;

      int y2 = rect.y() + rect.height() - bottomHeight;

      item->setGeometry(QRect(x1 + dx, y2, g.width(), g.height()));
    }
  }

  for (int i = 0; i < list_.size(); ++i) {
    ItemWrapper *wrapper = list_.at(i);

    auto *item = dynamic_cast<CTkLayoutWidget *>(wrapper->item);

    const Info &info = wrapper->info;

//  int iw = item->sizeHint().width ();
//  int ih = item->sizeHint().height();

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
    ItemWrapper *layoutStruct = list_.takeAt(index);

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

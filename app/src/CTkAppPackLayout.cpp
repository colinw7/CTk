#include <CTkAppPackLayout.h>
#include <CTkAppWidget.h>

#include <QWidget>
#include <QPainter>

CTkAppPackLayout::
CTkAppPackLayout(CTkAppWidget *parent, int margin, int spacing) :
 CTkAppLayout(parent, Type::PACK)
{
  setObjectName("pack");

  setMargin(margin);
  setSpacing(spacing);
}

CTkAppPackLayout::
CTkAppPackLayout(int spacing) :
 CTkAppLayout(Type::PACK)
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
  add(item);
}

void
CTkAppPackLayout::
addWidgets(const std::vector<CTkAppWidget *> &widgets, const Info &info)
{
  if (widgets.size() == 1)
    return addWidget(widgets[0], info);

  auto *parent = parentWidget();

  for (auto *widget : widgets) {
    if (widget->parentWidget() != parent)
      widget->setParentWidget(parent);
  }

  auto *layout = new CTkAppPackLayout;

  auto *parentLayoutWidget = new CTkAppPackLayoutWidget(this, layout, info);

  for (auto *widget : widgets)
    layout->add(new CTkAppPackLayoutWidget(this, parentLayoutWidget, widget, info));

  add(parentLayoutWidget);
}

void
CTkAppPackLayout::
addWidget(CTkAppWidget *widget, const Info &info)
{
  auto *parent = parentWidget();

  if (widget->parentWidget() != parent)
    widget->setParentWidget(parent);

  add(new CTkAppPackLayoutWidget(this, widget, info));
}

void
CTkAppPackLayout::
removeWidget(CTkAppWidget *widget)
{
  int ind = -1;

  for (int i = 0; i < list_.size(); ++i) {
    auto *item = list_.at(i);

    auto *w = dynamic_cast<CTkAppPackLayoutWidget *>(item);
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

  widget->hide();

  invalidate();
}

std::vector<CTkAppLayoutWidget *>
CTkAppPackLayout::
getLayoutWidgets(bool flat) const
{
  std::vector<CTkAppLayoutWidget *> widgets;

  for (int i = 0; i < list_.size(); ++i) {
    auto *item = list_.at(i);

    auto *w = dynamic_cast<CTkAppLayoutWidget *>(item);
    if (! w) continue;

    if (flat && w->getLayout()) {
      auto n = w->getLayout()->count();

      for (int j = 0; j < n; ++j) {
        auto *item1 = w->getLayout()->itemAt(j);

        auto *w1 = dynamic_cast<CTkAppLayoutWidget *>(item1);
        if (! w1) continue;

        widgets.push_back(w1);
      }
    }
    else
      widgets.push_back(w);
  }

  return widgets;
}

std::vector<CTkAppWidget *>
CTkAppPackLayout::
getWidgets() const
{
  std::vector<CTkAppWidget *> widgets;

  for (int i = 0; i < list_.size(); ++i) {
    auto *item = list_.at(i);

    auto *w = dynamic_cast<CTkAppPackLayoutWidget *>(item);
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

QLayoutItem *
CTkAppPackLayout::
getItem(CTkAppWidget *widget) const
{
  for (int i = 0; i < list_.size(); ++i) {
    auto *item = list_.at(i);

    auto *w = dynamic_cast<CTkAppPackLayoutWidget *>(item);
    if (! w) continue;

    auto *widget1 = w->getTkWidget();
    auto *layout1 = dynamic_cast<CTkAppPackLayout *>(w->getLayout());

    if (widget1) {
      if (widget == widget1)
        return item;
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
  auto *item = getItem(widget);
  if (! item) return false;

  auto *w = dynamic_cast<CTkAppPackLayoutWidget *>(item);
  info = w->info();

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
  auto *item = list_.value(index);

  return item;
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
  CTkAppLayout::setGeometry(rect);

  auto placeRect = rect;

  using WidgetList = std::vector<CTkAppPackLayoutWidget *>;

  WidgetList expandHWidgets, expandVWidgets;

  for (int i = 0; i < list_.size(); ++i) {
    auto *item = list_.at(i);

    auto *widget = dynamic_cast<CTkAppPackLayoutWidget *>(item);
    assert(widget);

    const Info &info = widget->info();

    auto itemSize = widgetSizeHint(widget);

    int iw = itemSize.width ();
    int ih = itemSize.height();

    int rw = placeRect.width ();
    int rh = placeRect.height();

    int x1 = placeRect.x();
    int y1 = placeRect.y();
    int x2 = x1 + rw;
    int y2 = y1 + rh;

    int w = iw + 2*info.ipadX();
    int h = ih + 2*info.ipadY();

    if      (info.side() == Info::Side::LEFT) {
      if (info.fill() == Info::Fill::Y || info.fill() == Info::Fill::BOTH)
        h = rh;

      if (info.isExpand()) {
        if (widget->getLayout())
          w = rw;
        else
          expandHWidgets.push_back(widget);
      }

      int dy = (rh - h - 2*info.padY())/2;

      auto r = QRect(x1 + info.padX(), y1 + info.padY() + dy, w, h);

      widget->setGeometry(r);

      placeRect.setLeft(placeRect.left() + r.width() + info.padX());
    }
    else if (info.side() == Info::Side::RIGHT) {
      if (info.fill() == Info::Fill::Y || info.fill() == Info::Fill::BOTH)
        h = rh;

      if (info.isExpand()) {
        if (widget->getLayout())
          w = rw;
        else
          expandHWidgets.push_back(widget);
      }

      int dy = (rh - h - 2*info.padY())/2;

      auto r = QRect(x2 - w - info.padX(), y1 + info.padY() + dy, w, h);

      widget->setGeometry(r);

      placeRect.setRight(placeRect.right() - r.width() - info.padX());
    }
    else if (info.side() == Info::Side::TOP) {
      if (info.fill() == Info::Fill::X || info.fill() == Info::Fill::BOTH)
        w = rw;

      if (info.isExpand()) {
        if (widget->getLayout())
          h = rh;
        else
          expandVWidgets.push_back(widget);
      }

      int dx = (rw - w - 2*info.padX())/2;

      auto r = QRect(x1 + info.padX() + dx, y1 + info.padY(), w, h);

      widget->setGeometry(r);

      placeRect.setTop(placeRect.top() + r.height() + info.padY());
    }
    else if (info.side() == Info::Side::BOTTOM) {
      if (info.fill() == Info::Fill::X || info.fill() == Info::Fill::BOTH)
        w = rw;

      if (info.isExpand()) {
        if (widget->getLayout())
          h = rh;
        else
          expandVWidgets.push_back(widget);
      }

      int dx = (rw - w - 2*info.padX())/2;

      auto r = QRect(x1 + info.padX() + dx, y2 - h - info.padX(), w, h);

      widget->setGeometry(r);

      placeRect.setBottom(placeRect.bottom() - r.height() - info.padY());
    }
    else {
      auto r = QRect(x1 + info.padX(), y1 + info.padY(),
                     w + 2*info.ipadX(), h + 2*info.ipadY());

      widget->setGeometry(r);
    }

    widget->show();
  }

  if (! expandHWidgets.empty()) {
    using XWidgets = std::map<int, WidgetList>;

    XWidgets xWidgets;

    int x1 = INT_MAX, x2 = 0;

    for (auto *w : expandHWidgets) {
      auto r = w->geometry();

      x1 = std::min(x1, r.left ());
      x2 = std::max(x2, r.right());

      xWidgets[x1].push_back(w);
    }

    bool placed = false;

    if (expandHWidgets.size() == 1) {
      auto *w = expandHWidgets[0];

      auto r = w->geometry();

      if (placeRect.top() == r.top() && placeRect.bottom() == r.bottom()) {
        x1 = std::min(x1, placeRect.left());
        x2 = std::max(x2, placeRect.right());

        r.setLeft (x1);
        r.setRight(x2);

        w->setGeometry(r);

        placed = true;
      }
    }

    if (! placed) {
      int dx = (x1 + (rect.width() - x2))/(expandHWidgets.size() + 1);

      int d = dx;

      for (const auto &p1 : xWidgets) {
        for (auto *w : p1.second) {
          auto r = w->geometry();

          auto r1 = QRect(r.left() + d, r.top(), r.width(), r.height());

          w->setGeometry(r1);

          d += dx;
        }
      }
    }
  }

  if (! expandVWidgets.empty()) {
    using YWidgets = std::map<int, WidgetList>;

    YWidgets yWidgets;

    int y1 = INT_MAX, y2 = 0;

    for (auto *w : expandVWidgets) {
      auto r = w->geometry();

      y1 = std::min(y1, r.top   ());
      y2 = std::max(y2, r.bottom());

      yWidgets[y1].push_back(w);
    }

    bool placed = false;

    if (expandVWidgets.size() == 1) {
      auto *w = expandVWidgets[0];

      auto r = w->geometry();

      if (placeRect.left() == r.left() && placeRect.right() == r.right()) {
        y1 = std::min(y1, placeRect.top());
        y2 = std::max(y2, placeRect.bottom());

        r.setTop   (y1);
        r.setBottom(y2);

        w->setGeometry(r);

        placed = true;
      }
    }

    if (! placed) {
      int dy = (y1 + (rect.height() - y2))/(expandVWidgets.size() + 1);

      int d = dy;

      for (const auto &p1 : yWidgets) {
        for (auto *w : p1.second) {
          auto r = w->geometry();

          auto r1 = QRect(r.left(), r.top() + dy/2, r.width(), r.height());

          w->setGeometry(r1);

          d += dy;
        }
      }
    }
  }
}

QSize
CTkAppPackLayout::
widgetSizeHint(CTkAppPackLayoutWidget *widget) const
{
  auto s = widget->sizeHint();

  auto m = widget->contentsMargins();

  s.setWidth (s.width () + m.left() + m.right ());
  s.setHeight(s.height() + m.top () + m.bottom());

  return s;
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
    auto *item = list_.takeAt(index);

    invalidate();

    return item;
  }

  return nullptr;
}

void
CTkAppPackLayout::
add(QLayoutItem *item)
{
  list_.append(item);

  invalidate();
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
    auto *item = list_.at(i);

    auto *widget = dynamic_cast<CTkAppPackLayoutWidget *>(item);
    assert(widget);

    const Info &info = widget->info();

    QSize itemSize;

    if (sizeType == MinimumSize)
      itemSize = widget->minimumSize();
    else // if (sizeType == SizeHint)
      itemSize = widgetSizeHint(widget);

    itemSize += QSize(2*info.padX() , 2*info.padY() );
    itemSize += QSize(2*info.ipadX(), 2*info.ipadY());

    if      (info.side() == Info::Side::TOP) {
      topHeight += itemSize.height();

      maxWidth = std::max(maxWidth, leftWidth + rightWidth + itemSize.width());
    }
    else if (info.side() == Info::Side::LEFT) {
      leftWidth += itemSize.width();

      maxHeight = std::max(maxHeight, bottomHeight + topHeight + itemSize.height());
    }
    else if (info.side() == Info::Side::BOTTOM) {
      bottomHeight += itemSize.height();

      maxWidth = std::max(maxWidth, leftWidth + rightWidth + itemSize.width());
    }
    else if (info.side() == Info::Side::RIGHT) {
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

void
CTkAppPackLayout::
invalidate()
{
  CTkAppLayout::invalidate();
}

//---

CTkAppPackLayoutWidget::
CTkAppPackLayoutWidget(CTkAppPackLayout *pack, TkWidget *widget, const Info &info) :
 CTkAppLayoutWidget(pack, widget), pack_(pack), info_(info)
{
}

CTkAppPackLayoutWidget::
CTkAppPackLayoutWidget(CTkAppPackLayout *pack, QLayout *layout, const Info &info) :
 CTkAppLayoutWidget(pack, layout), pack_(pack), info_(info)
{
}

CTkAppPackLayoutWidget::
CTkAppPackLayoutWidget(CTkAppPackLayout *pack, CTkAppPackLayoutWidget *parent,
                       TkWidget *widget, const Info &info) :
 CTkAppLayoutWidget(pack, widget), pack_(pack), parent_(parent), info_(info)
{
}

void
CTkAppPackLayoutWidget::
setPadX(int i)
{
  info_.setPadX(i); pack_->invalidate();
}

void
CTkAppPackLayoutWidget::
setPadY(int i)
{
  info_.setPadY(i); pack_->invalidate();
}

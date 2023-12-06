#include <CTkAppPlaceLayout.h>
#include <CTkAppLayoutWidget.h>
#include <CTkAppWidget.h>

CTkAppPlaceLayout::
CTkAppPlaceLayout(CTkAppWidget *parent, int margin, int spacing) :
 CTkAppLayout(parent, Type::PLACE)
{
  setObjectName("place");

  setMargin(margin);
  setSpacing(spacing);
}

CTkAppPlaceLayout::
CTkAppPlaceLayout(int spacing) :
 CTkAppLayout(Type::PLACE)
{
  setObjectName("place");

  setSpacing(spacing);
}

CTkAppPlaceLayout::
~CTkAppPlaceLayout()
{
  QLayoutItem *l;

  while ((l = takeAt(0)))
    delete l;
}

void
CTkAppPlaceLayout::
addItem(QLayoutItem *item)
{
  add(item);
}

void
CTkAppPlaceLayout::
addWidgets(const std::vector<CTkAppWidget *> &widgets, const Info &info)
{
  auto num = widgets.size();

  for (size_t i = 0; i < num; ++i)
    addWidget(widgets[i], info);
}

void
CTkAppPlaceLayout::
addWidget(CTkAppWidget *widget, const Info &info)
{
  auto *item = getItem(widget);

  if (! item) {
    auto *parent = parentWidget();

    if (widget->parentWidget() != parent)
      widget->setParentWidget(parent);

    add(new CTkAppPlaceLayoutWidget(this, widget, info));
  }
  else {
    auto *w = dynamic_cast<CTkAppPlaceLayoutWidget *>(item);
    if (! w) return;

    w->info().update(info);
  }
}

void
CTkAppPlaceLayout::
removeWidget(CTkAppWidget *widget)
{
  int ind = -1;

  for (int i = 0; i < list_.size(); ++i) {
    auto *item = list_.at(i);

    auto *w = dynamic_cast<CTkAppPlaceLayoutWidget *>(item);
    if (! w) continue;

    auto *widget1 = w->getTkWidget();
    auto *layout1 = dynamic_cast<CTkAppPlaceLayout *>(w->getLayout());

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

QLayoutItem *
CTkAppPlaceLayout::
getItem(CTkAppWidget *widget) const
{
  for (int i = 0; i < list_.size(); ++i) {
    auto *item = list_.at(i);

    auto *w = dynamic_cast<CTkAppPlaceLayoutWidget *>(item);
    if (! w) continue;

    auto *widget1 = w->getTkWidget();

    if (widget == widget1)
      return item;
  }

  return nullptr;
}

bool
CTkAppPlaceLayout::
getChildInfo(CTkAppWidget *widget, Info &info)
{
  auto *item = getItem(widget);
  if (! item) return false;

  auto *w = dynamic_cast<CTkAppPlaceLayoutWidget *>(item);
  info = w->info();

  return true;
}

std::vector<CTkAppLayoutWidget *>
CTkAppPlaceLayout::
getLayoutWidgets(bool /*flat*/) const
{
  std::vector<CTkAppLayoutWidget *> widgets;

  for (int i = 0; i < list_.size(); ++i) {
    auto *item = list_.at(i);

    auto *w = dynamic_cast<CTkAppPlaceLayoutWidget *>(item);
    if (! w) continue;

    widgets.push_back(w);
  }

  return widgets;
}

Qt::Orientations
CTkAppPlaceLayout::
expandingDirections() const
{
  return Qt::Horizontal | Qt::Vertical;
}

bool
CTkAppPlaceLayout::
hasHeightForWidth() const
{
  // preferred height depends on width
  return false;
}

int
CTkAppPlaceLayout::
count() const
{
  return list_.size();
}

QLayoutItem *
CTkAppPlaceLayout::
itemAt(int index) const
{
  auto *item = list_.value(index);

  return item;
}

QSize
CTkAppPlaceLayout::
minimumSize() const
{
  return calculateSize(MinimumSize);
}

void
CTkAppPlaceLayout::
setGeometry(const QRect &rect)
{
  CTkAppLayout::setGeometry(rect);

  for (int i = 0; i < list_.size(); ++i) {
    auto *item = list_.at(i);

    auto *widget = dynamic_cast<CTkAppPlaceLayoutWidget *>(item);
    assert(widget);

    int x1 = getXPos(widget->info(), rect);
    int y1 = getYPos(widget->info(), rect);

    int wh = getWidth (widget->info(), rect, widget);
    int hh = getHeight(widget->info(), rect, widget);

    widget->setGeometry(QRect(x1, y1, wh, hh));

    widget->show();
  }
}

int
CTkAppPlaceLayout::
getXPos(const Info &info, const QRect &rect) const
{
  if (info.isXValid())
    return info.getX();

  if (info.isRelXValid() && ! rect.isNull()) {
    double r = info.getRelX();

    return int(rect.x() + r*rect.width());
  }

  return 0;
}

int
CTkAppPlaceLayout::
getYPos(const Info &info, const QRect &rect) const
{
  if (info.isYValid())
    return info.getY();

  if (info.isRelYValid() && ! rect.isNull()) {
    double r = info.getRelY();

    return int(rect.y() + r*rect.height());
  }

  return 0;
}

int
CTkAppPlaceLayout::
getWidth(const Info &info, const QRect &rect,
         CTkAppLayoutWidget *item, const SizeType &sizeType) const
{
  if (info.isWidthValid())
    return info.getWidth();

  if (info.isRelWidthValid() && ! rect.isNull()) {
    double r = info.getRelWidth();

    return int(r*rect.width());
  }

  if (sizeType == SizeType::SizeHint)
    return item->sizeHint().width();
  else
    return item->minimumSizeHint().width();
}

int
CTkAppPlaceLayout::
getHeight(const Info &info, const QRect &rect,
          CTkAppLayoutWidget *item, const SizeType &sizeType) const
{
  if (info.isHeightValid())
    return info.getHeight();

  if (info.isRelHeightValid() && ! rect.isNull()) {
    double r = info.getRelHeight();

    return int(r*rect.height());
  }

  if (sizeType == SizeType::SizeHint)
    return item->sizeHint().height();
  else
    return item->minimumSizeHint().width();
}

QSize
CTkAppPlaceLayout::
sizeHint() const
{
  return calculateSize(SizeHint);
}

QLayoutItem *
CTkAppPlaceLayout::
takeAt(int index)
{
  if (index >= 0 && index < list_.size()) {
    auto *item = list_.takeAt(index);

    return item;
  }

  return nullptr;
}

void
CTkAppPlaceLayout::
add(QLayoutItem *item)
{
  list_.append(item);

  invalidate();
}

QSize
CTkAppPlaceLayout::
calculateSize(SizeType sizeType) const
{
  int w = 0;
  int h = 0;

  QSize itemSize(1, 1);

  for (int i = 0; i < list_.size(); ++i) {
    auto *item = list_.at(i);

    auto *widget = dynamic_cast<CTkAppPlaceLayoutWidget *>(item);
    assert(widget);

    int x1 = getXPos(widget->info(), QRect());
    int y1 = getYPos(widget->info(), QRect());

    int wh = getWidth (widget->info(), QRect(), widget, sizeType);
    int hh = getHeight(widget->info(), QRect(), widget, sizeType);

    w = std::max(x1 + wh, itemSize.width ());
    h = std::max(y1 + hh, itemSize.height());
  }

  return QSize(w, h);
}

//---

CTkAppPlaceLayoutWidget::
CTkAppPlaceLayoutWidget(CTkAppPlaceLayout *place, TkWidget *widget, const Info &info) :
 CTkAppLayoutWidget(place, widget), place_(place), info_(info)
{
}

CTkAppPlaceLayoutWidget::
CTkAppPlaceLayoutWidget(CTkAppPlaceLayout *place, QLayout *layout, const Info &info) :
 CTkAppLayoutWidget(place, layout), place_(place), info_(info)
{
}

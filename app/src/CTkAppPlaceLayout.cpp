#include <CTkAppPlaceLayout.h>
#include <CTkAppLayoutWidget.h>
#include <CTkAppWidget.h>

CTkAppPlaceLayout::
CTkAppPlaceLayout(QWidget *parent, int margin, int spacing) :
 CTkAppLayout(parent, Type::PLACE)
{
  setObjectName("place");

  setMargin(margin);
  setSpacing(spacing);
}

CTkAppPlaceLayout::
CTkAppPlaceLayout(int spacing) :
 CTkAppLayout(nullptr, Type::PLACE)
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
  add(item, Info());
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
  auto *wrapper = getItem(widget);

  if (! wrapper) {
    auto *parent = parentWidget();

    if (widget->parentWidget() != parent)
      widget->setParentWidget(parent);

    add(new CTkAppLayoutWidget(widget), info);
  }
  else
    wrapper->info.update(info);
}

CTkAppPlaceLayout::ItemWrapper *
CTkAppPlaceLayout::
getItem(CTkAppWidget *widget) const
{
  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    auto *w = dynamic_cast<CTkAppLayoutWidget *>(wrapper->item);

    if (! w) continue;

    CTkAppWidget *widget1 = w->getTkWidget();

    if (widget == widget1)
      return wrapper;
  }

  return nullptr;
}

bool
CTkAppPlaceLayout::
getChildInfo(CTkAppWidget *widget, Info &info)
{
  ItemWrapper *wrapper = getItem(widget);

  if (! wrapper) return false;

  info = wrapper->info;

  return true;
}

std::vector<CTkAppLayoutWidget *>
CTkAppPlaceLayout::
getLayoutWidgets() const
{
  std::vector<CTkAppLayoutWidget *> widgets;

  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    auto *w = dynamic_cast<CTkAppLayoutWidget *>(wrapper->item);
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
CTkAppPlaceLayout::itemAt(int index) const
{
  ItemWrapper *wrapper = list_.value(index);

  if (wrapper)
    return wrapper->item;

  return nullptr;
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
    auto *wrapper = list_.at(i);

    auto *item = dynamic_cast<CTkAppLayoutWidget *>(wrapper->item);
    assert(item);

    int x1 = getXPos(wrapper->info, rect);
    int y1 = getYPos(wrapper->info, rect);

    int wh = getWidth (wrapper->info, rect, item);
    int hh = getHeight(wrapper->info, rect, item);

    item->setGeometry(QRect(x1, y1, wh, hh));

    item->show();
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
    auto *layoutStruct = list_.takeAt(index);

    return layoutStruct->item;
  }

  return nullptr;
}

void
CTkAppPlaceLayout::
add(QLayoutItem *item, const Info &info)
{
  list_.append(new ItemWrapper(item, info));
}

QSize
CTkAppPlaceLayout::
calculateSize(SizeType sizeType) const
{
  int w = 0;
  int h = 0;

  QSize itemSize(1, 1);

  for (int i = 0; i < list_.size(); ++i) {
    auto *wrapper = list_.at(i);

    auto *item = dynamic_cast<CTkAppLayoutWidget *>(wrapper->item);
    assert(item);

    int x1 = getXPos(wrapper->info, QRect());
    int y1 = getYPos(wrapper->info, QRect());

    int wh = getWidth (wrapper->info, QRect(), item, sizeType);
    int hh = getHeight(wrapper->info, QRect(), item, sizeType);

    w = std::max(x1 + wh, itemSize.width ());
    h = std::max(y1 + hh, itemSize.height());
  }

  return QSize(w, h);
}

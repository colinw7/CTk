#include <CTkAppPlaceLayout.h>
#include <CTkAppLayoutWidget.h>

CTkAppPlaceLayout::
CTkAppPlaceLayout(QWidget *parent, int margin, int spacing) :
 CTkAppLayout(parent)
{
  setObjectName("place");

  setMargin(margin);
  setSpacing(spacing);
}

CTkAppPlaceLayout::
CTkAppPlaceLayout(int spacing) :
 CTkAppLayout(nullptr)
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
  ItemWrapper *wrapper = getItem(widget);

  if (! wrapper)
    add(new CTkAppLayoutWidget(widget), info);
  else
    wrapper->info.update(info);
}

CTkAppPlaceLayout::ItemWrapper *
CTkAppPlaceLayout::
getItem(CTkAppWidget *widget) const
{
  for (int i = 0; i < list_.size(); ++i) {
    ItemWrapper *wrapper = list_.at(i);

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
    ItemWrapper *wrapper = list_.at(i);

    auto *item = dynamic_cast<CTkAppLayoutWidget *>(wrapper->item);
    assert(item);

    double rx = wrapper->info.getRelX();
    double ry = wrapper->info.getRelY();

    int wh = item->sizeHint().width ();
    int hh = item->sizeHint().height();

    int x1 = int(rect.x() + rx*rect.width ());
    int y1 = int(rect.y() + ry*rect.height());

    item->setGeometry(QRect(x1, y1, wh, hh));

    item->show();
  }
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
    ItemWrapper *layoutStruct = list_.takeAt(index);

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

  for (int i = 0; i < list_.size(); ++i) {
    ItemWrapper *wrapper = list_.at(i);

    QLayoutItem *item = wrapper->item;

    QSize itemSize;

    if (sizeType == MinimumSize)
      itemSize = item->minimumSize();
    else // (sizeType == SizeHint)
      itemSize = item->sizeHint();

    w = std::max(w, itemSize.width ());
    h = std::max(h, itemSize.height());
  }

  if (w <= 0) w = 1;
  if (h <= 0) h = 1;

  return QSize(w, h);
}

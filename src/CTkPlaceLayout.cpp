#include <CTkLayout.h>

CTkPlaceLayout::
CTkPlaceLayout(QWidget *parent, int margin, int spacing) :
 QLayout(parent)
{
  setMargin(margin);
  setSpacing(spacing);
}

CTkPlaceLayout::
CTkPlaceLayout(int spacing) :
 QLayout(NULL)
{
  setSpacing(spacing);
}

CTkPlaceLayout::
~CTkPlaceLayout()
{
  QLayoutItem *l;

  while ((l = takeAt(0)))
    delete l;
}

void
CTkPlaceLayout::
addItem(QLayoutItem *item)
{
  add(item, Info());
}

void
CTkPlaceLayout::
addWidgets(const std::vector<CTkWidget *> &widgets, const Info &info)
{
  auto num = widgets.size();

  for (size_t i = 0; i < num; ++i)
    addWidget(widgets[i], info);
}

void
CTkPlaceLayout::
addWidget(CTkWidget *widget, const Info &info)
{
  ItemWrapper *wrapper = getItem(widget);

  if (! wrapper)
    add(new CTkLayoutWidget(widget), info);
  else
    wrapper->info.update(info);
}

CTkPlaceLayout::ItemWrapper *
CTkPlaceLayout::
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
CTkPlaceLayout::
getChildInfo(CTkWidget *widget, Info &info)
{
  ItemWrapper *wrapper = getItem(widget);

  if (! wrapper) return false;

  info = wrapper->info;

  return true;
}

Qt::Orientations
CTkPlaceLayout::
expandingDirections() const
{
  return Qt::Horizontal | Qt::Vertical;
}

bool
CTkPlaceLayout::
hasHeightForWidth() const
{
  // preferred height depends on width
  return false;
}

int
CTkPlaceLayout::
count() const
{
  return list_.size();
}

QLayoutItem *
CTkPlaceLayout::itemAt(int index) const
{
  ItemWrapper *wrapper = list_.value(index);

  if (wrapper)
    return wrapper->item;
  else
    return NULL;
}

QSize
CTkPlaceLayout::
minimumSize() const
{
  return calculateSize(MinimumSize);
}

void
CTkPlaceLayout::
setGeometry(const QRect &rect)
{
  QLayout::setGeometry(rect);

  for (int i = 0; i < list_.size(); ++i) {
    ItemWrapper *wrapper = list_.at(i);

    QLayoutItem *item = wrapper->item;

    double rx = wrapper->info.getRelX();
    double ry = wrapper->info.getRelY();

    int wh = item->sizeHint().width ();
    int hh = item->sizeHint().height();

    int x1 = int(rect.x() + rx*rect.width ());
    int y1 = int(rect.y() + ry*rect.height());

    item->setGeometry(QRect(x1, y1, wh, hh));
  }
}

QSize
CTkPlaceLayout::
sizeHint() const
{
  return calculateSize(SizeHint);
}

QLayoutItem *
CTkPlaceLayout::
takeAt(int index)
{
  if (index >= 0 && index < list_.size()) {
    ItemWrapper *layoutStruct = list_.takeAt(index);

    return layoutStruct->item;
  }

  return nullptr;
}

void
CTkPlaceLayout::
add(QLayoutItem *item, const Info &info)
{
  list_.append(new ItemWrapper(item, info));
}

QSize
CTkPlaceLayout::
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

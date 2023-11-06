#include <CQButtonImage.h>

#include <QStylePainter>
#include <QStyleOption>

CQButtonImage::
CQButtonImage(QWidget *parent) :
 QAbstractButton(parent)
{
  setObjectName("qbutton");
}

void
CQButtonImage::
updateLines()
{
  lines_.clear();

  auto label = this->label();

  auto pos = label.indexOf('\n');

  while (pos != -1) {
    auto lhs = label.mid(0, pos);

    lines_.push_back(lhs);

    label = label.mid(pos + 1);

    pos = label.indexOf('\n');
  }

  if (label != "")
    lines_.push_back(label);
}

void
CQButtonImage::
paintEvent(QPaintEvent *)
{
  QStylePainter painter(this);

  QStyleOptionButton opt;

  opt.initFrom(this);

  if (  isDown()) opt.state |= QStyle::State_Sunken;
  if (! isDown()) opt.state |= QStyle::State_Raised;

  painter.drawControl(QStyle::CE_PushButton, opt);

  //---

  QFontMetrics fm(font());

  bool hasLabel = false;;
  int tw = 0, ta = 0, td = 0;

  if (label().length()) {
    hasLabel = true;

    tw = fm.horizontalAdvance(label());
    ta = fm.ascent();
    td = fm.descent();
  }

  bool hasImage = false;;
  int iw = 0, ih = 0;

  if (! image_.isNull()) {
    hasImage = true;

    iw = image_.width();
    ih = image_.height();
  }

  int b = 2;

  //---

  painter.setPen(palette().windowText().color());

  auto rect = contentsRect();

  int x = rect.left() + borderX_;
  int y = rect.top () + borderY_;
  int w = rect.width () - 2*borderX_;
  int h = rect.height() - 2*borderY_;

  int xc = x + w/2;
  int yc = y + h/2;

  if      (! hasImage) {
    int yc = (rect.height() - lines_.size()*(ta + td))/2 + (ta + td)/2;

    for (const auto &line : lines_) {
      auto tw = fm.horizontalAdvance(line);

      if      (justify() & Qt::AlignLeft)
        painter.drawText(x, yc + (ta - td)/2, line);
      else if (justify() & Qt::AlignHCenter)
        painter.drawText(xc - tw/2, yc + (ta - td)/2, line);
      else if (justify() & Qt::AlignRight)
        painter.drawText(x + w - tw, yc + (ta - td)/2, line);
      else
        painter.drawText(x, yc + (ta - td)/2, line);

      yc += ta + td;
    }
  }
  else if (! hasLabel) {
    painter.drawImage(xc - iw/2, y + (h - ih)/2, image());
  }
  else {
    if      (type_ == Type::LEFT) {
      painter.drawImage(x, yc - ih/2, image()); x += iw + b;

      painter.drawText(x, yc + (ta - td)/2, label());
    }
    else if (type_ == Type::RIGHT) {
      painter.drawText(x, yc + (ta - td)/2, label()); x += tw + b;

      painter.drawImage(x, yc - ih/2, image());
    }
    else if (type_ == Type::BOTTOM) {
      painter.drawText(xc - tw/2, y + ta, label()); y += ta + td;

      painter.drawImage(xc - iw/2, y, image());
    }
    else if (type_ == Type::TOP) {
      painter.drawImage(xc - iw/2, y, image()); y += ih + b;

      painter.drawText(xc - tw/2, y + ta, label());
    }
    else if (type_ == Type::CENTER) {
      painter.drawImage(xc - iw/2, yc - ih/2, image());
      painter.drawText (xc - tw/2, yc + (ta - td)/2, label());
    }
    else {
      painter.drawImage(xc - iw/2, yc - ih/2, image());
      painter.drawText (xc - tw/2, yc + (ta - td)/2, label());
    }
  }
}

QSize
CQButtonImage::
sizeHint() const
{
  QFontMetrics fm(font());

  bool hasLabel = false;;
  int tw = 0, th = 0;

  if (label().length()) {
    hasLabel = true;

    tw = fm.horizontalAdvance(label());
    th = fm.height();
  }

  bool hasImage = false;;
  int iw = 0, ih = 0;

  if (! image_.isNull()) {
    hasImage = true;

    iw = image_.width();
    ih = image_.height();
  }

  int b = 2;

  //---

  if      (! hasImage) {
    int tw = 0;

    if (userWidth() >= 0)
      tw = userWidth()*fm.horizontalAdvance("0");
    else {
      for (auto &line : lines_)
        tw = std::max(tw, fm.horizontalAdvance(line));
    }

    return QSize(tw + 2*borderX_, lines_.size()*th + 2*borderY_);
  }
  else if (! hasLabel) {
    if (userWidth() >= 0)
      iw = userWidth();

    if (userHeight() >= 0)
      ih = userHeight();

    return QSize(iw + 2*borderX_, ih + 2*borderY_);
  }
  else {
    int w = 0, h = 0;

    if      (type_ == Type::LEFT || type_ == Type::RIGHT) {
      w = tw + iw + b;
      h = std::max(th, ih);
    }
    else if (type_ == Type::TOP || type_ == Type::BOTTOM) {
      w = std::max(tw, iw);
      h = ih + th + b;
    }
    else if (type_ == Type::CENTER) {
      w = std::max(tw, iw);
      h = std::max(th, ih);
    }
    else {
      w = std::max(tw, iw);
      h = std::max(th, ih);
    }

   if (userWidth() >= 0)
      w = userWidth();

    if (userHeight() >= 0)
      h = userHeight();

    return QSize(w + 2*borderX_, h + 2*borderY_);
  }
}

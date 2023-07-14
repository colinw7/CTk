#include <CQLabelImage.h>

#include <QPainter>

CQLabelImage::
CQLabelImage(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("qlabel");
}

void
CQLabelImage::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.setPen(palette().windowText().color());

  QFontMetrics fm(font());

  int x = 0;
  int y = 0;

  auto tw = fm.horizontalAdvance(label());
  auto ta = fm.ascent();
  auto td = fm.descent();

  auto iw = image_.width();
  auto ih = image_.height();

  int b = 2;

  if      (type_ == Type::LEFT) {
    painter.drawImage(x, y + (height() - ih)/2, image()); x += iw + b;

    painter.drawText(x, y + height()/2 + (ta - td)/2, label());
  }
  else if (type_ == Type::RIGHT) {
    painter.drawText(x, y + height()/2 + (ta - td)/2, label()); x += tw + b;

    painter.drawImage(x, y + (height() - ih)/2, image());
  }
  else if (type_ == Type::BOTTOM) {
    painter.drawText(x + (width() - tw)/2.0, y + ta, label());

    y += ta + td;

    painter.drawImage(x + (width() - iw)/2.0, y, image());
  }
  else if (type_ == Type::TOP) {
    painter.drawImage(x + (width() - iw)/2.0, y, image());

    y += ih + b;

    painter.drawText(x + (width() - tw)/2.0, y + ta, label());
  }
  else if (type_ == Type::CENTER) {
    painter.drawImage(x + (width() - iw)/2.0, y + (height() - ih)/2, image());
    painter.drawText (x + (width() - tw)/2.0, y + height()/2 + (ta - td)/2, label());
  }
  else {
    if (! image().isNull())
      painter.drawImage(x + (width() - iw)/2.0, y + (height() - ih)/2, image());
    else
      painter.drawText (x + (width() - tw)/2.0, y + height()/2 + (ta - td)/2, label());
  }
}

QSize
CQLabelImage::
sizeHint() const
{
  QFontMetrics fm(font());

  auto tw = fm.horizontalAdvance(label());
  auto th = fm.height();

  auto iw = image_.width();
  auto ih = image_.height();

  int b = 2;

  if      (type_ == Type::LEFT || type_ == Type::RIGHT)
    return QSize(tw + iw + b, std::max(th, ih));
  else if (type_ == Type::TOP || type_ == Type::BOTTOM)
    return QSize(std::max(tw, iw), ih + th + b);
  else if (type_ == Type::CENTER)
    return QSize(std::max(tw, iw), std::max(th, ih));
  else
    return QSize(std::max(tw, iw), std::max(th, ih));
}

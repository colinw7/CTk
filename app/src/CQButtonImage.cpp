#include <CQButtonImage.h>

#include <QStylePainter>
#include <QStyleOption>

CQButtonImage::
CQButtonImage(QWidget *parent) :
 QAbstractButton(parent)
{
  setObjectName("qlabel");
}

void
CQButtonImage::
paintEvent(QPaintEvent *)
{
  QStylePainter p(this);

  QStyleOptionButton opt;

  opt.initFrom(this);

  if (  isDown()) opt.state |= QStyle::State_Sunken;
  if (! isDown()) opt.state |= QStyle::State_Raised;

  p.drawControl(QStyle::CE_PushButton, opt);

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

  int b  = 2;
  int bw = 2;

  //---

  p.setPen(palette().windowText().color());

  int x = bw;
  int y = bw;

  if      (! hasImage) {
    p.drawText(x, y + height()/2 + (ta - td)/2, label());
  }
  else if (! hasLabel) {
    p.drawImage(x + (width() - iw)/2.0, y + (height() - ih)/2, image());
  }
  else {
    if      (type_ == Type::LEFT) {
      p.drawImage(x, y + (height() - ih)/2, image()); x += iw + b;

      p.drawText(x, y + height()/2 + (ta - td)/2, label());
    }
    else if (type_ == Type::RIGHT) {
      p.drawText(x, y + height()/2 + (ta - td)/2, label()); x += tw + b;

      p.drawImage(x, y + (height() - ih)/2, image());
    }
    else if (type_ == Type::BOTTOM) {
      p.drawText(x + (width() - tw)/2.0, y + ta, label());

      y += ta + td;

      p.drawImage(x + (width() - iw)/2.0, y, image());
    }
    else if (type_ == Type::TOP) {
      p.drawImage(x + (width() - iw)/2.0, y, image());

      y += ih + b;

      p.drawText(x + (width() - tw)/2.0, y + ta, label());
    }
    else if (type_ == Type::CENTER) {
      p.drawImage(x + (width() - iw)/2.0, y + (height() - ih)/2, image());
      p.drawText (x + (width() - tw)/2.0, y + height()/2 + (ta - td)/2, label());
    }
    else {
      if (! image().isNull())
        p.drawImage(x + (width() - iw)/2.0, y + (height() - ih)/2, image());
      else
        p.drawText (x + (width() - tw)/2.0, y + height()/2 + (ta - td)/2, label());
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

  int b  = 2;
  int bw = 2;

  //---

  if      (! hasImage) {
    if (userWidth() >= 0)
      tw = userWidth()*fm.horizontalAdvance("0");

    return QSize(tw + 2*bw, th + 2*bw);
  }
  else if (! hasLabel) {
    if (userWidth() >= 0)
      iw = userWidth();

    if (userHeight() >= 0)
      ih = userHeight();

    return QSize(iw + 2*bw, ih + 2*bw);
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

    return QSize(w + 2*bw, h + 2*bw);
  }
}

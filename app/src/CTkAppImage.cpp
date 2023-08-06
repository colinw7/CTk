#include <CTkAppImage.h>
#include <CTkApp.h>

#ifdef CTK_CIMAGE
#include <CImageLib.h>
#include <CQImage.h>
#endif

#include <QSvgRenderer>
#include <QPainter>

#include <cassert>

CTkAppImage::
CTkAppImage(CTkApp *tk, const std::string &name, int width, int height) :
 tk_(tk), name_(name), width_(width), height_(height)
{
  if (width_ > 0 && height_ > 0)
    qimage_ = QImage(width_, height_, QImage::Format_ARGB32);
}

CTkAppImage::
~CTkAppImage()
{
}

bool
CTkAppImage::
loadFile(const std::string &filename)
{
  filename_ = filename;
  width_    = 0;
  height_   = 0;

#ifdef CTK_CIMAGE
  CImageFileSrc src(filename);

  auto image = CImageMgrInst->lookupImage(src);

  if (! image.isValid())
    return tk_->throwError("Failed to read image file '" + filename + "'");

  auto *cqimage = image.cast<CQImage>();
  assert(cqimage);

  qimage_ = cqimage->getQImage();
#else
  qimage_ = QImage(QString::fromStdString(filename_));
#endif

  if (qimage_.isNull())
    return false;

  width_  = qimage_.width();
  height_ = qimage_.height();

  return true;
}

bool
CTkAppImage::
loadSVG(const std::string &filename)
{
  QSvgRenderer renderer(QString::fromStdString(filename));

  if (! renderer.isValid())
    return tk_->throwError("Failed to read SVG image file '" + filename + "'");

  width_  = 256;
  height_ = 256;

  qimage_ = QImage(width_, height_, QImage::Format_ARGB32);
  qimage_.fill(0);

  QPainter painter(&qimage_);
  renderer.render(&painter);

  return true;
}

bool
CTkAppImage::
loadData(const std::string &data)
{
  width_  = 0;
  height_ = 0;

#ifdef CTK_CIMAGE
  CImageDataSrc src(data);

  auto image = CImageMgrInst->lookupImage(src);

  if (! image.isValid())
    return tk_->throwError("Failed to read image data");

  auto *cqimage = image.cast<CQImage>();
  assert(cqimage);

  qimage_ = cqimage->getQImage();
#else
  assert(data.size());

  return false;
#endif

  if (qimage_.isNull())
    return false;

  width_  = qimage_.width();
  height_ = qimage_.height();

  return true;
}

QImage
CTkAppImage::
getQImage() const
{
  return qimage_;
}

QPixmap
CTkAppImage::
getQPixmap() const
{
  QPixmap pixmap;

  pixmap.convertFromImage(getQImage());

  return pixmap;
}

int
CTkAppImage::
width() const
{
  if (! qimage_.isNull())
    return qimage_.width();

  return width_;
}

void
CTkAppImage::
setWidth(int w)
{
  width_ = w;

  if      (! qimage_.isNull())
    qimage_ = qimage_.scaledToWidth(width_);
  else if (width_ > 0)
    qimage_ = QImage(width_, height_, QImage::Format_ARGB32);
}

int
CTkAppImage::
height() const
{
  if (! qimage_.isNull())
    return qimage_.height();

  return height_;
}

void
CTkAppImage::
setHeight(int h)
{
  height_ = h;

  if      (! qimage_.isNull())
    qimage_ = qimage_.scaledToHeight(height_);
  else if (width_ > 0)
    qimage_ = QImage(width_, height_, QImage::Format_ARGB32);
}

void
CTkAppImage::
clear()
{
  if (! qimage_.isNull())
    qimage_.fill(Qt::transparent);
}

bool
CTkAppImage::
getPixel(int x, int y, QColor &c) const
{
  if (x < 0 || x >= width() || y < 0 || y >= height())
    return false;

  auto rgb = qimage_.pixel(x, y);

  c = QColor(rgb);

  return true;
}

bool
CTkAppImage::
setPixel(int x, int y, const QColor &c)
{
  if (x < 0 || x >= width() || y < 0 || y >= height())
    return false;

  qimage_.setPixel(x, y, c.rgba());

  return true;
}

bool
CTkAppImage::
setPixels(int x1, int y1, int x2, int y2, const QColor &c)
{
  for (int y = y1; y < y2; ++y)
    for (int x = x1; x < x2; ++x)
      setPixel(x, y, c);

  return true;
}

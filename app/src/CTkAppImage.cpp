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
CTkAppImage(CTkApp *tk, const std::string &name) :
 tk_(tk), name_(name)
{
}

CTkAppImage::
~CTkAppImage()
{
}

#if 0
const CImagePtr &
CTkAppImage::
getImage() const
{
  return image_;
}

void
CTkAppImage::
setImage(const CImagePtr &image)
{
  image_ = image;
}
#endif

bool
CTkAppImage::
loadFile(const std::string &filename)
{
  filename_ = filename;

#ifdef CTK_CIMAGE
  CImageFileSrc src(filename);

  image_ = CImageMgrInst->lookupImage(src);

  if (! image_.isValid())
    return tk_->throwError("Failed to read image file '" + filename + "'");
#else
  qimage_ = QImage(QString::fromStdString(filename_));
#endif

  return true;
}

bool
CTkAppImage::
loadSVG(const std::string &filename)
{
  QSvgRenderer renderer(QString::fromStdString(filename));

  if (! renderer.isValid())
    return tk_->throwError("Failed to read SVG image file '" + filename + "'");

  int w = 256;
  int h = 256;

  qimage_ = QImage(w, h, QImage::Format_ARGB32);
  qimage_.fill(0);

  QPainter painter(&qimage_);
  renderer.render(&painter);

  return true;
}

bool
CTkAppImage::
loadData(const std::string &data)
{
#ifdef CTK_CIMAGE
  CImageDataSrc src(data);

  image_ = CImageMgrInst->lookupImage(src);

  if (! image_.isValid())
    return tk_->throwError("Failed to read image data");

  return image_.isValid();
#else
  assert(data.size());

  return false;
#endif
}

QImage
CTkAppImage::
getQImage() const
{
  if (! qimage_.isNull())
    return qimage_;

#ifdef CTK_CIMAGE
  auto *cqimage = image_.cast<CQImage>();
  if (! cqimage) return QImage();

  return cqimage->getQImage();
#else
  return QImage();
#endif
}

QPixmap
CTkAppImage::
getQPixmap() const
{
  QPixmap pixmap;

  pixmap.convertFromImage(getQImage());

  return pixmap;
}

void
CTkAppImage::
clear()
{
  if (! qimage_.isNull())
    qimage_.fill(Qt::transparent);
#ifdef CTK_CIMAGE
  else
    image_->setRGBAData(CRGBA(0.0, 0.0, 0.0, 0.0));
#endif
}

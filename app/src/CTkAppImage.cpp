#include <CTkAppImage.h>
#include <CTkApp.h>

#include <CImageLib.h>
#include <CQImage.h>

#include <QSvgRenderer>
#include <QPainter>

CTkAppImage::
CTkAppImage(CTkApp *tk, const std::string &name) :
 tk_(tk), name_(name)
{
}

CTkAppImage::
~CTkAppImage()
{
}

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

bool
CTkAppImage::
loadFile(const std::string &filename)
{
  filename_ = filename;

  CImageFileSrc src(filename);

  image_ = CImageMgrInst->lookupImage(src);

  if (! image_.isValid())
    return tk_->throwError("Failed to read image file '" + filename + "'");

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
  CImageDataSrc src(data);

  image_ = CImageMgrInst->lookupImage(src);

  if (! image_.isValid())
    return tk_->throwError("Failed to read image data");

  return image_.isValid();
}

QImage
CTkAppImage::
getQImage() const
{
  if (! qimage_.isNull())
    return qimage_;

  auto *cqimage = image_.cast<CQImage>();
  if (! cqimage) return QImage();

  return cqimage->getQImage();
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
  else
    image_->setRGBAData(CRGBA(0.0, 0.0, 0.0, 0.0));
}

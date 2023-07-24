#include <CTkAppImage.h>
#include <CImageLib.h>
#include <CQImage.h>

#include <QSvgRenderer>
#include <QPainter>

bool
CTkAppImage::
loadFile(const std::string &filename)
{
  CImageFileSrc src(filename);

  image_ = CImageMgrInst->createImage(src);

  return image_.isValid();
}

bool
CTkAppImage::
loadSVG(const std::string &filename)
{
  QSvgRenderer renderer(QString::fromStdString(filename));
  if (! renderer.isValid()) return false;

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

  image_ = CImageMgrInst->createImage(src);

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

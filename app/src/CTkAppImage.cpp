#include <CTkAppImage.h>
#include <CTkApp.h>

#ifdef CTK_CIMAGE
#include <CImageLib.h>
#include <CQImage.h>
#endif
#include <CXBMImage.h>
#include <CGIFImage.h>
#include <CGenImage.h>
#include <CEncode64.h>

#include <QSvgRenderer>
#include <QPainter>
#include <QBitmap>

#include <cassert>
#include <fstream>
#include <sstream>

CTkAppImage::
CTkAppImage(CTkApp *tk, const QString &name, int width, int height) :
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
loadFile(const QString &filename)
{
  filename_ = filename;
  width_    = 0;
  height_   = 0;

#ifdef CTK_CIMAGE
  CImageFileSrc src(filename.toStdString());

  auto image = CImageMgrInst->lookupImage(src);

  if (! image.isValid())
    return tk_->throwError("Failed to read image file '" + filename + "'");

  auto *cqimage = image.cast<CQImage>();
  assert(cqimage);

  qimage_ = cqimage->getQImage();
#else
  qimage_ = QImage(filename_);
#endif

  if (qimage_.isNull())
    return false;

  width_  = qimage_.width();
  height_ = qimage_.height();

  return true;
}

bool
CTkAppImage::
loadSVG(const QString &filename)
{
  QSvgRenderer renderer(filename);

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
loadXBM(const QString &name, const std::string &data)
{
  auto fs = std::istringstream(data);

  CImageData image;

  if (! CXBMImageInst->read(fs, &image))
    return tk_->throwError("Failed to read XBM image data '" + name + "'");

  width_  = image.getWidth();
  height_ = image.getHeight();

  qimage_ = QImage(width_, height_, QImage::Format_ARGB32);

  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      auto ic = image.getColorIndex(x, y);

      qimage_.setPixelColor(x, y, (ic == 0 ? Qt::black : Qt::white));
    }
  }

  return true;
}

bool
CTkAppImage::
loadMaskXBM(const QString &name, const std::string &data)
{
  auto fs = std::istringstream(data);

  CImageData image;

  if (! CXBMImageInst->read(fs, &image))
    return tk_->throwError("Failed to read XBM image data '" + name + "'");

  maskWidth_  = image.getWidth();
  maskHeight_ = image.getHeight();

  maskQImage_ = QImage(maskWidth_, maskHeight_, QImage::Format_ARGB32);

  for (int y = 0; y < maskHeight_; ++y) {
    for (int x = 0; x < maskWidth_; ++x) {
      auto ic = image.getColorIndex(x, y);

      maskQImage_.setPixelColor(x, y, (ic == 0 ? Qt::black : Qt::white));
    }
  }

  return true;
}

void
CTkAppImage::
loadXBMData(int w, int h, const unsigned char *bits)
{
  width_  = w;
  height_ = h;

  auto bm = QBitmap::fromData(QSize(w, h), bits);

  qimage_ = bm.toImage();
}

bool
CTkAppImage::
loadEncodedData(const QString &name, const QString &format, const std::string &data)
{
  std::string decodeStr;

  for (const auto &c : data) {
    if (std::isspace(c)) continue;
    decodeStr += c;
  }

//std::cerr << decodeStr << "\n";

  auto dataStr = CEncode64Inst->decode(decodeStr); // don't use QString

  return loadData(name, format, dataStr);
}

bool
CTkAppImage::
loadData(const QString & /*name*/, const QString & /*format*/, const std::string &data)
{
#if 0
  auto filename = name + "." + format;

  auto fs = std::ofstream(filename.toLatin1().constData(),
                          std::ofstream::out | std::ofstream::trunc);

  fs << data;
#endif

  //---

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

  if (! qimage_.loadFromData(reinterpret_cast<const uchar *>(data.c_str()), data.size()))
    return tk_->throwError("Failed to read image data");
#endif

  if (qimage_.isNull())
    return false;

  width_  = qimage_.width();
  height_ = qimage_.height();

  return true;
}

QString
CTkAppImage::
getEncodedDataStr() const
{
  if (qimage_.isNull())
    return "";

  auto qimage1 = qimage_.convertToFormat(QImage::Format_Indexed8);

  CImageData image;

  image.setDataSize(width_, height_);

  image.setColormap(true);

  for (int i = 0; i < qimage1.colorCount(); ++i) {
    auto rgba = qimage1.color(i);

    auto c = CGenImage::RGBA(qRed (rgba)/255.0, qGreen(rgba)/255.0,
                             qBlue(rgba)/255.0, qAlpha(rgba)/255.0);

    image.addColor(c);
  }

  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      auto ind = qimage1.pixelIndex(x, y);

      image.setColorIndex(x, y, ind);
    }
  }

  std::stringstream ss;

  CGIFImageInst->write(ss, &image);

  auto str = ss.str();

  //---

  auto *data = reinterpret_cast<const uchar *>(str.c_str());
  auto  len  = str.size();

  auto len1 = CEncode64Inst->calcOutSize(len);

  auto *data1 = new uchar [len1 + 1];

  (void) CEncode64Inst->encode(data, len, data1, len1);

  auto *data2 = reinterpret_cast<char *>(data1);

  auto str2 = std::string(data2, strlen(data2));

  std::string str3;

  int nc = 0;

  for (size_t i = 0; i < str2.size(); ++i, ++nc) {
    if (nc == 60) {
      str3 += "\n";
      nc = 0;
    }

    str3 += str2[i];
  }

  return QString::fromStdString(str3);
}

QString
CTkAppImage::
getBitmapStr() const
{
  if (qimage_.isNull())
    return "";

  CImageData image;

  image.setDataSize(width_, height_);

  image.setColormap(true);

  image.addColor(CImageData::RGBA(0.0, 0.0, 0.0));
  image.addColor(CImageData::RGBA(1.0, 1.0, 1.0));

  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      auto rgba = qimage_.pixel(x, y);

      auto g = qGray(rgba);

      image.setColorIndex(x, y, (g < 0.5 ? 0 : 1));
    }
  }

  std::stringstream ss;

  CXBMImageInst->write(ss, &image);

  return QString::fromStdString(ss.str());
}

QString
CTkAppImage::
getMaskBitmapStr() const
{
  return QString();
}

//---

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

void
CTkAppImage::
setQImage(const QImage &qimage)
{
  qimage_ = qimage;
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
getColorBits(int &r, int &g, int &b) const
{
  r = rbits_;
  g = gbits_;
  b = bbits_;
}

void
CTkAppImage::
setColorBits(int r, int g, int b)
{
  rbits_ = r;
  gbits_ = g;
  bbits_ = b;
}

//---

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
  int w = qimage_.width ();
  int h = qimage_.height();

  bool resize = false;
  if (x2 >= w) { w = x2 + 1; resize = true; }
  if (y2 >= h) { h = y2 + 1; resize = true; }

  if (resize) {
    auto newImage = QImage(w, h, QImage::Format_ARGB32);

    int oldW = qimage_.width ();
    int oldH = qimage_.height();

    for (int y = 0; y < oldH; ++y)
      for (int x = 0; x < oldW; ++x)
        newImage.setPixel(x, y, qimage_.pixel(x, y));

    qimage_ = newImage;
  }

  for (int y = y1; y <= y2; ++y)
    for (int x = x1; x <= x2; ++x)
      qimage_.setPixel(x, y, c.rgba());

  return true;
}

void
CTkAppImage::
addRef(const QString &ref)
{
  refNames_.insert(ref);
}

void
CTkAppImage::
removeRef(const QString &ref)
{
  refNames_.erase(ref);
}

//---

QString
CTkAppImage::
dataString() const
{
  auto hexStr = [&](int i) {
    static const char *hexChars = "0123456789ABCDEF";

    int i1 = i/16;
    int i2 = i - i1*16;

    return QString(QChar(hexChars[i1])) + QChar(hexChars[i2]);
  };

  QString str = "{";

  for (int y = 0; y < height_; ++y) {
    str += " {";

    for (int x = 0; x < width_; ++x) {
      if (x > 0)
        str += " ";

      auto rgba = qimage_.pixel(x, y);

      str += QString("#%1%2%3").
        arg(hexStr(qRed(rgba))).arg(hexStr(qGreen(rgba))).arg(hexStr(qBlue(rgba)));
    }

    str += "}";
  }

  str += "}";

  return str;
}

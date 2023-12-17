#include <CTkAppImage.h>
#include <CTkAppImageCommand.h>
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

uint CTkAppImage::s_nextId = 1;

CTkAppImage::
CTkAppImage(CTkApp *tk, const QString &name, int width, int height) :
 tk_(tk), name_(name), width_(width), height_(height)
{
  id_ = s_nextId++;

  if (isWidthSet() && isHeightSet()) {
    qimage_ = QImage(width_, height_, QImage::Format_ARGB32);
    qimage_.fill(0);
  }

  //std::cerr << "Create Image " << id_ << "\n";
}

CTkAppImage::
~CTkAppImage()
{
  //std::cerr << "Destroy Image " << id_ << "\n";

  delete command_;
}

void
CTkAppImage::
setCommand(CTkAppImageCommand *c)
{
  assert(c->imageId() == 0);

  command_ = c;

  command_->setImageId(id());
}

bool
CTkAppImage::
loadFile(const QString &filename)
{
  if (! loadImageFile(filename, qimage_))
    return false;

  //---

  if (! isWidthSet())
    width_ = qimage_.width();

  if (! isHeightSet())
    height_ = qimage_.height();

  updateImageSize();

  //---

  filename_ = filename;

  Q_EMIT imageChanged();

  return true;
}

bool
CTkAppImage::
loadMaskFile(const QString &filename)
{
  if (! loadImageFile(filename, maskQImage_))
    return false;

  //---

  if (maskWidth_ <= 0)
    maskWidth_ = maskQImage_.width();

  if (maskHeight_ <= 0)
    maskHeight_ = maskQImage_.height();

  updateMaskImageSize();

  //---

  maskFilename_ = filename;

  Q_EMIT maskImageChanged();

  return true;
}

bool
CTkAppImage::
loadImageFile(const QString &filename, QImage &qimage) const
{
  if (! tk_->fileExists(filename))
    return tk_->throwError("couldn't open \"" + filename + "\": no such file or directory");

#ifdef CTK_CIMAGE
  CImageFileSrc src(filename.toStdString());

  auto image = CImageMgrInst->lookupImage(src);

  if (! image.isValid())
    return tk_->throwError("couldn't recognize data in image file \"" + filename + "\"");

  auto *cqimage = image.cast<CQImage>();
  assert(cqimage);

  qimage = cqimage->getQImage();
#else
  qimage = QImage(filename);
#endif

  if (qimage.isNull())
    return tk_->throwError("couldn't recognize data in image file \"" + filename + "\"");

  return true;
}

bool
CTkAppImage::
loadSVG(const QString &filename)
{
  QSvgRenderer renderer(filename);

  if (! renderer.isValid())
    return tk_->throwError("Failed to read SVG image file '" + filename + "'");

  if (! isWidthSet())
    width_ = 256;

  if (! isHeightSet())
    height_ = 256;

  qimage_ = QImage(width_, height_, QImage::Format_ARGB32);
  qimage_.fill(0);

  QPainter painter(&qimage_);
  renderer.render(&painter);

  Q_EMIT imageChanged();

  return true;
}

bool
CTkAppImage::
loadMaskSVG(const QString &filename)
{
  QSvgRenderer renderer(filename);

  if (! renderer.isValid())
    return tk_->throwError("Failed to read SVG image file '" + filename + "'");

  if (maskWidth_ <= 0)
    maskWidth_ = 256;

  if (maskHeight_ <= 0)
    maskHeight_ = 256;

  maskQImage_ = QImage(maskWidth_, maskHeight_, QImage::Format_ARGB32);
  maskQImage_.fill(0);

  QPainter painter(&maskQImage_);
  renderer.render(&painter);

  Q_EMIT maskImageChanged();

  return true;
}

bool
CTkAppImage::
loadXBM(const QString &, const std::string &data)
{
  auto fs = std::istringstream(data);

  CImageData image;

  if (! CXBMImageInst->read(fs, &image))
    return tk_->throwError("format error in bitmap data");

  if (! isWidthSet())
    width_ = image.getWidth();

  if (! isHeightSet())
    height_ = image.getHeight();

  qimage_ = QImage(width_, height_, QImage::Format_ARGB32);

  for (int y = 0; y < height_; ++y) {
    for (int x = 0; x < width_; ++x) {
      auto ic = image.getColorIndex(x, y);

      qimage_.setPixelColor(x, y, (ic == 0 ? Qt::black : Qt::white));
    }
  }

  Q_EMIT imageChanged();

  return true;
}

bool
CTkAppImage::
loadMaskXBM(const QString &, const std::string &data)
{
  auto fs = std::istringstream(data);

  CImageData image;

  if (! CXBMImageInst->read(fs, &image))
    return tk_->throwError("format error in bitmap data");

  if (maskWidth_ <= 0)
    maskWidth_ = image.getWidth();

  if (maskHeight_ <= 0)
    maskHeight_ = image.getHeight();

  maskQImage_ = QImage(maskWidth_, maskHeight_, QImage::Format_ARGB32);

  for (int y = 0; y < maskHeight_; ++y) {
    for (int x = 0; x < maskWidth_; ++x) {
      auto ic = image.getColorIndex(x, y);

      maskQImage_.setPixelColor(x, y, (ic == 0 ? Qt::black : Qt::white));
    }
  }

  Q_EMIT maskImageChanged();

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

  Q_EMIT imageChanged();
}

void
CTkAppImage::
loadMaskXBMData(int w, int h, const unsigned char *bits)
{
  maskWidth_  = w;
  maskHeight_ = h;

  auto bm = QBitmap::fromData(QSize(w, h), bits);

  maskQImage_ = bm.toImage();

  Q_EMIT maskImageChanged();
}

bool
CTkAppImage::
loadEncodedData(const QString &name, const QString &format, const std::string &data)
{
  auto *inst = CEncode64Inst;

  std::string decodeStr;

  for (const auto &c : data) {
    if (std::isspace(c)) continue;

    if (! inst->isChar(c) && c != '=')
      return false;

    decodeStr += c;
  }

//std::cerr << decodeStr << "\n";

  auto dataStr = inst->decode(decodeStr); // don't use QString
  if (dataStr == "") return false;

  if (! loadData(name, format, dataStr))
    return false;

  return true;
}

bool
CTkAppImage::
loadMaskEncodedData(const QString &name, const QString &format, const std::string &data)
{
  auto *inst = CEncode64Inst;

  std::string decodeStr;

  for (const auto &c : data) {
    if (std::isspace(c)) continue;

    if (! inst->isChar(c) && c != '=')
      return false;

    decodeStr += c;
  }

//std::cerr << decodeStr << "\n";

  auto dataStr = inst->decode(decodeStr); // don't use QString
  if (dataStr == "") return false;

  if (! loadMaskData(name, format, dataStr))
    return false;

  return true;
}

bool
CTkAppImage::
loadVarData(const QVariant &var, const QString &format)
{
  std::string data;

  if (var.type() == QVariant::ByteArray) {
    auto ba = var.toByteArray();

    data = std::string(ba.constData(), ba.length());
  }
  else {
    if (format == "" && loadColorList(var))
      return true;

    auto value = tk_->variantToString(var);

    data = value.toStdString();
  }

  if (format == "xbm") {
    return loadXBM("", data);
  }

  if (! loadEncodedData("", format, data)) {
    if (! loadData("", format, data))
      return false;
  }

  return true;
}

bool
CTkAppImage::
loadColorList(const QVariant &var)
{
  using RowColors   = std::vector<QColor>;
  using ImageColors = std::vector<RowColors>;

  ImageColors imageColors;

  bool valid = true;

  auto value = tk_->variantToString(var);

  std::vector<QString> strs;
  if (! tk_->splitList(value, strs))
    return false;

  int nr = strs.size();

  int nc = -1;

  for (int iy = 0; iy < nr; ++iy) {
    std::vector<QString> strs1;
    if (tk_->splitList(strs[iy], strs1)) {
      int nc1 = strs1.size();

      if (nc < 0)
        nc = nc1;

      if (nc != nc1) {
        valid = false;
        break;
      }

      RowColors rowColors;

      for (int ix = 0; ix < nc; ++ix) {
        QColor c;
        if (! tk_->variantToQColor(strs1[ix], c)) {
          valid = false;
          break;
        }

        rowColors.push_back(c);
      }

      if (! valid)
        break;

      imageColors.push_back(rowColors);
    }
  }

  if (! valid)
    return false;

  resize(nc, nr);

  for (int iy = 0; iy < nr; ++iy) {
    for (int ix = 0; ix < nc; ++ix) {
      setPixel(ix, iy, imageColors[iy][ix]);
    }
  }

  return true;
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

  auto *cdata = data.c_str();

  if (data.size() > 3 && strncmp(cdata, "GIF", 3) == 0) {
#if 0
    std::istringstream ss(data);

    CImageData image;
    if (! CGIFImageInst->read(ss, &image))
      return tk_->throwError("Failed to read image data");

    qimage_ = QImage(image.getWidth(), image.getHeight(), QImage::Format_ARGB32);

    if (image.hasColormap()) {
      for (uint iy = 0; iy < image.getHeight(); ++iy) {
        for (uint ix = 0; ix < image.getWidth(); ++ix) {
          auto ind = image.getColorIndex(ix, iy);

          const auto &rgba = image.getColor(ind);

          uint r, g, b, a;
          rgba.getRGBAI(&r, &g, &b, &a);

          qimage_.setPixelColor(ix, iy, QColor(r, g, b, a));
        }
      }
    }
    else {
      for (uint iy = 0; iy < image.getHeight(); ++iy) {
        for (uint ix = 0; ix < image.getWidth(); ++ix) {
          uint pixel = image.getPixel(ix, iy);

          CGenImage::RGBA rgba;
          CGenImage::RGBA::decodeARGB(pixel, rgba);

          uint r, g, b, a;
          rgba.getRGBAI(&r, &g, &b, &a);

          qimage_.setPixelColor(ix, iy, QColor(r, g, b, a));
        }
      }
    }
#else
    if (! qimage_.loadFromData(reinterpret_cast<const uchar *>(data.c_str()), data.size()))
      return tk_->throwError("Failed to read image data");
#endif
  }
  else {
    if (! qimage_.loadFromData(reinterpret_cast<const uchar *>(data.c_str()), data.size()))
      return tk_->throwError("Failed to read image data");
  }
#endif

  if (! isWidthSet())
    width_ = qimage_.width();

  if (! isHeightSet())
    height_ = qimage_.height();

  updateImageSize();

  Q_EMIT imageChanged();

  return true;
}

bool
CTkAppImage::
loadMaskData(const QString & /*name*/, const QString & /*format*/, const std::string &data)
{
#if 0
  auto filename = name + "." + format;

  auto fs = std::ofstream(filename.toLatin1().constData(),
                          std::ofstream::out | std::ofstream::trunc);

  fs << data;
#endif

  //---

#ifdef CTK_CIMAGE
  CImageDataSrc src(data);

  auto image = CImageMgrInst->lookupImage(src);

  if (! image.isValid())
    return tk_->throwError("Failed to read image data");

  auto *cqimage = image.cast<CQImage>();
  assert(cqimage);

  maskQImage_ = cqimage->getQImage();
#else
  assert(data.size());

  if (! maskQImage_.loadFromData(reinterpret_cast<const uchar *>(data.c_str()), data.size()))
    return tk_->throwError("Failed to read image data");
#endif

  if (maskQImage_.isNull())
    return false;

  if (maskWidth_ <= 0)
    maskWidth_ = qimage_.width();

  if (maskHeight_ <= 0)
    maskHeight_ = qimage_.height();

  updateMaskImageSize();

  Q_EMIT maskImageChanged();

  return true;
}

QString
CTkAppImage::
getEncodedDataStr() const
{
  if (qimage_.isNull())
    return "";

  std::stringstream ss;

  if (! writeGIF(ss, qimage_))
    return QString();

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
getMaskEncodedDataStr() const
{
  if (maskQImage_.isNull())
    return "";

  std::stringstream ss;

  if (! writeGIF(ss, maskQImage_))
    return QString();

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

bool
CTkAppImage::
writeImage(const QString &filename, const QImage &qimage, const QString &format)
{
  if (format == "gif") {
    auto fs = std::ofstream(filename.toLatin1().constData(),
                            std::ofstream::out | std::ofstream::trunc);

    return writeGIF(fs, qimage);
  }

  QString qformat;
  if (! mapToQFormat(format, qformat))
    return false;

  if (! qimage.save(filename, qformat.toLatin1().constData()))
    return false;

  return true;
}

bool
CTkAppImage::
writeGIF(std::ostream &os, const QImage &qimage)
{
  auto qimage1 = qimage.convertToFormat(QImage::Format_Indexed8);

  CImageData image;

  image.setDataSize(qimage1.width(), qimage1.height());

  image.setColormap(true);

  for (int i = 0; i < qimage1.colorCount(); ++i) {
    auto rgba = qimage1.color(i);

    auto c = CGenImage::RGBA(qRed (rgba)/255.0, qGreen(rgba)/255.0,
                             qBlue(rgba)/255.0, qAlpha(rgba)/255.0);

    image.addColor(c);
  }

  for (int y = 0; y < qimage1.height(); ++y) {
    for (int x = 0; x < qimage1.width(); ++x) {
      auto ind = qimage1.pixelIndex(x, y);

      image.setColorIndex(x, y, ind);
    }
  }

  return CGIFImageInst->write(os, &image);
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
      auto c = qimage_.pixelColor(x, y);

      auto g = qGray(c.rgba());

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
  if (maskQImage_.isNull())
    return "";

  CImageData image;

  image.setDataSize(maskWidth_, maskHeight_);

  image.setColormap(true);

  image.addColor(CImageData::RGBA(0.0, 0.0, 0.0));
  image.addColor(CImageData::RGBA(1.0, 1.0, 1.0));

  for (int y = 0; y < maskHeight_; ++y) {
    for (int x = 0; x < maskWidth_; ++x) {
      auto c = maskQImage_.pixelColor(x, y);

      auto g = qGray(c.rgba());

      image.setColorIndex(x, y, (g < 0.5 ? 0 : 1));
    }
  }

  std::stringstream ss;

  CXBMImageInst->write(ss, &image);

  return QString::fromStdString(ss.str());
}

//---

QImage
CTkAppImage::
getQImage() const
{
  return qimage_;
}

QImage
CTkAppImage::
getMaskQImage() const
{
  return maskQImage_;
}

QPixmap
CTkAppImage::
getQPixmap() const
{
  QPixmap pixmap;

  pixmap.convertFromImage(getQImage());

  return pixmap;
}

QPixmap
CTkAppImage::
getMaskQPixmap() const
{
  QPixmap pixmap;

  pixmap.convertFromImage(getMaskQImage());

  return pixmap;
}

void
CTkAppImage::
setQImage(const QImage &qimage)
{
  qimage_ = qimage;
}

void
CTkAppImage::
setMaskQImage(const QImage &qimage)
{
  maskQImage_ = qimage;
}

//---

int
CTkAppImage::
width() const
{
  if (! isWidthSet())
    return qimage_.width();

  return width_;
}

void
CTkAppImage::
setWidth(int w)
{
  width_ = w;

  if (isWidthSet() && isHeightSet()) {
    qimage_ = QImage(width_, height_, QImage::Format_ARGB32);
    qimage_.fill(0);
  }
}

int
CTkAppImage::
height() const
{
  if (! isHeightSet())
    return qimage_.height();

  return height_;
}

void
CTkAppImage::
setHeight(int h)
{
  height_ = h;

  if (isWidthSet() && isHeightSet()) {
    qimage_ = QImage(width_, height_, QImage::Format_ARGB32);
    qimage_.fill(0);
  }
}

int
CTkAppImage::
maskWidth() const
{
  if (! maskQImage_.isNull())
    return maskQImage_.width();

  return maskWidth_;
}

void
CTkAppImage::
setMaskWidth(int w)
{
  maskWidth_ = w;

  if (maskHeight_ > 0) {
    maskQImage_ = QImage(maskWidth_, maskHeight_, QImage::Format_ARGB32);
    maskQImage_.fill(0);
  }
}

int
CTkAppImage::
maskHeight() const
{
  if (! maskQImage_.isNull())
    return maskQImage_.height();

  return maskHeight_;
}

void
CTkAppImage::
setMaskHeight(int h)
{
  maskHeight_ = h;

  if (maskHeight_ > 0) {
    maskQImage_ = QImage(maskWidth_, maskHeight_, QImage::Format_ARGB32);
    maskQImage_.fill(0);
  }
}

//---

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

void
CTkAppImage::
resetColorBits()
{
  rbits_ = 8;
  gbits_ = 8;
  bbits_ = 8;
}

//---

void
CTkAppImage::
clear()
{
  if (! qimage_.isNull())
    qimage_.fill(Qt::transparent);
}

void
CTkAppImage::
resize(int w, int h)
{
  auto qimage = QImage(w, h, QImage::Format_ARGB32);

  qimage.fill(Qt::transparent);

  for (int iy = 0; iy < qimage_.height() && iy < h; ++iy) {
    for (int ix = 0; ix < qimage_.width() && ix < w; ++ix) {
      auto c = qimage_.pixelColor(ix, iy);

      qimage.setPixelColor(ix, iy, c);
    }
  }

  std::swap(qimage_, qimage);

  if (width_ > 0)
    width_ = w;

  if (height_ > 0)
    height_ = h;

  Q_EMIT imageChanged();
}

void
CTkAppImage::
copy(int x, int y, const QImage &qimage)
{
  assert(! qimage.isNull());

  if (qimage_.isNull()) {
    qimage_ = QImage(x + qimage.width(), y + qimage.height(), QImage::Format_ARGB32);
    qimage_.fill(0);
  }

  QPainter p(&qimage_);

  p.drawImage(x, y, qimage);

  Q_EMIT imageChanged();
}

bool
CTkAppImage::
getPixel(int x, int y, QColor &c) const
{
  if (x < 0 || x >= width() || y < 0 || y >= height())
    return false;

  if (x < qimage_.width() && y < qimage_.height())
    c = qimage_.pixelColor(x, y);
  else
    c = QColor();

  return true;
}

bool
CTkAppImage::
setPixel(int x, int y, const QColor &c)
{
  if (x < 0 || x >= width() || y < 0 || y >= height())
    return false;

  if (x < qimage_.width() && y < qimage_.height() && c.isValid())
    qimage_.setPixelColor(x, y, c);

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
        newImage.setPixelColor(x, y, qimage_.pixelColor(x, y));

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
dataString(const ColorFormat &colorFormat) const
{
  return dataString(tk_, qimage_, colorFormat);
}

QString
CTkAppImage::
dataString(CTkApp *tk, const QImage &qimage, const ColorFormat &colorFormat)
{
  auto hexStr = [&](int i) {
    static const char *hexChars = "0123456789abcdef";

    int i1 = i/16;
    int i2 = i - i1*16;

    return QString(QChar(hexChars[i1])) + QChar(hexChars[i2]);
  };

  int width  = qimage.width ();
  int height = qimage.height();

  QVariantList vars;

  for (int y = 0; y < height; ++y) {
    QVariantList rowVars;

    for (int x = 0; x < width; ++x) {
      auto c = qimage.pixelColor(x, y);

      QVariant var;

      if      (colorFormat == ColorFormat::RGBA)
        var = QString("#%1%2%3%4").
          arg(hexStr(c.red())).arg(hexStr(c.green())).arg(hexStr(c.blue())).arg(hexStr(c.alpha()));
      else if (colorFormat == ColorFormat::LIST) {
        QVariantList colVars;

        colVars << c.red  ();
        colVars << c.green();
        colVars << c.blue ();
        colVars << c.alpha();

        var = QVariant(colVars);
      }
      else
        var = QString("#%1%2%3").
          arg(hexStr(c.red())).arg(hexStr(c.green())).arg(hexStr(c.blue()));

      rowVars.push_back(var);
    }

    vars.push_back(rowVars);
  }

  return tk->variantToString(vars);
}

//---

void
CTkAppImage::
updateImageSize()
{
  if (! isWidthSet() || ! isHeightSet())
    return;

  if (qimage_.width() != width_ || qimage_.height() != height_)
    qimage_ = qimage_.scaled(width_, height_, Qt::IgnoreAspectRatio, Qt::FastTransformation);
}

void
CTkAppImage::
updateMaskImageSize()
{
  maskQImage_ =
    maskQImage_.scaled(maskWidth_, maskHeight_, Qt::IgnoreAspectRatio, Qt::FastTransformation);
}

bool
CTkAppImage::
lookupFormatName(CTkApp *tk, const QString &format, FormatData &formatData)
{
  std::vector<QString> strs;
  if (! tk->splitList(format, strs))
    strs.push_back(format);

  uint i = 0;

  auto name = strs[i].toLower();

  if (name == "default") {
    formatData.format = "";
  }
  else {
    static auto formatNames = std::vector<QString>({
      "png", "jpg", "jpeg", "bmp", "xpm", "xbm", "gif", "ppm", "svg"});

    if (! tk->lookupOptionName(formatNames, name, formatData.format, /*quiet*/true))
      return false;
  }

  ++i;

  uint numStrs = strs.size();

  while (i < numStrs) {
    if      (strs[i] == "-colorformat") {
      ++i;

      if (i >= numStrs)
        return tk->throwError("missing value for -colorformat");

      if      (strs[i] == "rgb")
        formatData.colorformat = ColorFormat::RGB;
      else if (strs[i] == "rgba")
        formatData.colorformat = ColorFormat::RGBA;
      else if (strs[i] == "list")
        formatData.colorformat = ColorFormat::LIST;
      else
        return tk->throwError("Invalid colorformat \"" + strs[i] + "\"");

      ++i;
    }
    else if (strs[i] == "-index") {
      ++i;

      if (i >= numStrs)
        return tk->throwError("missing value for -index");

      formatData.index = strs[i++];
    }
    else if (strs[i] == "-dpi") {
      ++i;

      if (i >= numStrs)
        return tk->throwError("missing value for -dpi");

      formatData.dpi = strs[i++];
    }
    else if (strs[i] == "-scale") {
      ++i;

      if (i >= numStrs)
        return tk->throwError("missing value for -scale");

      if (! tk->variantToReal(strs[i], formatData.scale))
        return tk->invalidReal(strs[i]);

      if (formatData.scale <= 0)
        return tk->throwError("-scale value must be positive");

      ++i;
    }
    else if (strs[i] == "-scaletowidth") {
      ++i;

      if (i >= numStrs)
        return tk->throwError("missing value for -scaletowidth");

      if (! tk->variantToInt(strs[i], formatData.widthScale))
        return tk->invalidInteger(strs[i]);

      if (formatData.widthScale <= 0)
        return tk->throwError("-scaletowidth value must be positive");

      ++i;
    }
    else if (strs[i] == "-scaletoheight") {
      ++i;

      if (i >= numStrs)
        return tk->throwError("missing value for -scaletoheight");

      if (! tk->variantToInt(strs[i], formatData.heightScale))
        return tk->invalidInteger(strs[i]);

      if (formatData.heightScale <= 0)
        return tk->throwError("-scaletoheight value must be positive");

      ++i;
    }
    else
      return tk->throwError("invalid format option \"" + strs[i] + "\"");
  }

  formatData.set = true;

  return true;
}

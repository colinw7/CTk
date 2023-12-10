#ifndef CTkAppImage_H
#define CTkAppImage_H

#include <QImage>
#include <QObject>

#include <string>
#include <set>

class CTkApp;

class CTkAppImage : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString name     READ name)
  Q_PROPERTY(QString filename READ filename)
  Q_PROPERTY(QString type     READ type   WRITE setType)
  Q_PROPERTY(QString format   READ format WRITE setFormat)
  Q_PROPERTY(double  gamma    READ gamma  WRITE setGamma)
  Q_PROPERTY(int     width    READ width  WRITE setWidth)
  Q_PROPERTY(int     height   READ height WRITE setHeight)

  Q_PROPERTY(QColor background READ background WRITE setBackground)
  Q_PROPERTY(QColor foreground READ foreground WRITE setForeground)

 public:
  explicit CTkAppImage(CTkApp *tk, const QString &name, int width=0, int height=0);

 ~CTkAppImage();

  //---

  const QString &name() const { return name_; }

  //---

  const QString &filename() const { return filename_; }

  int width() const;
  void setWidth(int w);

  int height() const;
  void setHeight(int h);

  const QString &maskFilename() const { return maskFilename_; }

  int maskWidth() const;
  void setMaskWidth(int w);

  int maskHeight() const;
  void setMaskHeight(int h);

  //---

  const QString &type() const { return type_; }
  void setType(const QString &s) { type_ = s; }

  const QString &format() const { return format_; }
  void setFormat(const QString &s) { format_ = s; }

  bool isColor() const { return (type_ == "photo"); }

  double gamma() const { return gamma_; }
  void setGamma(double r) { gamma_ = r; }

  const QColor &background() const { return background_; }
  void setBackground(const QColor &c) { background_ = c; }

  const QColor &foreground() const { return foreground_; }
  void setForeground(const QColor &c) { foreground_ = c; }

  void getColorBits(int &r, int &g, int &b) const;
  void setColorBits(int r, int g, int b);
  void resetColorBits();

  //---

  QImage getQImage() const;
  QPixmap getQPixmap() const;

  void setQImage(const QImage &image);

  QImage getMaskQImage() const;
  QPixmap getMaskQPixmap() const;

  void setMaskQImage(const QImage &image);

  //---

  bool loadFile(const QString &filename);
  bool loadSVG (const QString &filename);

  bool loadMaskFile(const QString &filename);
  bool loadMaskSVG (const QString &filename);

  bool loadXBM    (const QString &name, const std::string &data);
  bool loadMaskXBM(const QString &name, const std::string &data);

  bool loadEncodedData    (const QString &name, const QString &format, const std::string &data);
  bool loadMaskEncodedData(const QString &name, const QString &format, const std::string &data);

  bool loadVarData(const QVariant &var, const QString &format);

  bool loadData    (const QString &name, const QString &format, const std::string &data);
  bool loadMaskData(const QString &name, const QString &format, const std::string &data);

  void loadXBMData    (int w, int h, const unsigned char *bits);
  void loadMaskXBMData(int w, int h, const unsigned char *bits);

  QString getEncodedDataStr    () const;
  QString getMaskEncodedDataStr() const;

  QString getBitmapStr() const;
  QString getMaskBitmapStr() const;

  static bool writeImage(const QString &filename, const QImage &qimage, const QString &format);

  static bool writeGIF(std::ostream &os, const QImage &qimage);

  //---

  void clear();

  void resize(int w, int h);

  bool getPixel(int x, int y, QColor &c) const;
  bool setPixel(int x, int y, const QColor &c);

  bool setPixels(int x1, int y1, int x2, int y2, const QColor &c);

  uint numRefs() const { return uint(refNames_.size()); }

  void addRef(const QString &ref);
  void removeRef(const QString &ref);

  //---

  QString dataString() const;
  static QString dataString(CTkApp *tk, const QImage &image);

  //---

  static bool mapToQFormat(const QString &format, QString &qformat) {
    if      (format == "png"                    ) qformat = "PNG";
    else if (format == "jpg" || format == "jpeg") qformat = "JPEG";
    else if (format == "bmp"                    ) qformat = "BMP";
    else if (format == "xpm"                    ) qformat = "XPM";
    else if (format == "xbm"                    ) qformat = "XBM";
    else if (format == "gif"                    ) qformat = "GIF";
    else if (format == "ppm"                    ) qformat = "PPM";
    else return false;

    return true;
  }

 Q_SIGNALS:
  void imageChanged();
  void maskImageChanged();

 private:
  void updateImageSize();
  void updateMaskImageSize();

 private:
  using RefNames = std::set<QString>;

  CTkApp* tk_ { nullptr };

  QString name_;
  QString type_;

  QImage  qimage_;
  QString filename_;
  int     width_  { 0 };
  int     height_ { 0 };

  QImage  maskQImage_;
  QString maskFilename_;
  int     maskWidth_  { 0 };
  int     maskHeight_ { 0 };

  QString format_;
  double  gamma_  { 1.0 };
  QColor  background_;
  QColor  foreground_;

  int rbits_ { 8 }, gbits_ { 8 }, bbits_ { 8 };

  RefNames refNames_;
};

#endif

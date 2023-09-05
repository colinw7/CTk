#ifndef CTkAppImage_H
#define CTkAppImage_H

#include <QImage>

#include <string>
#include <set>

class CTkApp;

class CTkAppImage {
 public:
  explicit CTkAppImage(CTkApp *tk, const QString &name, int width=-1, int height=-1);

 ~CTkAppImage();

  const QString &name() const { return name_; }

  const QString &filename() const { return filename_; }
  void setFilename(const QString &filename) { filename_ = filename; }

  const QString &type() const { return type_; }
  void setType(const QString &s) { type_ = s; }

  const QString &format() const { return format_; }
  void setFormat(const QString &s) { format_ = s; }

  bool isColor() const { return (type_ == "photo"); }

  double gamma() const { return gamma_; }
  void setGamma(double r) { gamma_ = r; }

  QImage getQImage() const;

  QPixmap getQPixmap() const;

  int width() const;
  void setWidth(int w);

  int height() const;
  void setHeight(int h);

  bool loadFile(const QString &filename);
  bool loadSVG (const QString &filename);
  bool loadXBM (const QString &name, const std::string &data);
  bool loadData(const QString &name, const QString &format, const std::string &data);

  void loadXBMData(int w, int h, const unsigned char *bits);

  void clear();

  bool getPixel(int x, int y, QColor &c) const;
  bool setPixel(int x, int y, const QColor &c);

  bool setPixels(int x1, int y1, int x2, int y2, const QColor &c);

  uint numRefs() const { return uint(refNames_.size()); }

  void addRef(const QString &ref);
  void removeRef(const QString &ref);

 private:
  using RefNames = std::set<QString>;

  CTkApp*  tk_     { nullptr };
  QString  name_;
  QString  type_;
  QImage   qimage_;
  QString  filename_;
  QString  format_;
  int      width_  { 0 };
  int      height_ { 0 };
  double   gamma_  { 1.0 };
  RefNames refNames_;
};

#endif

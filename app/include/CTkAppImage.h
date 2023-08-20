#ifndef CTkAppImage_H
#define CTkAppImage_H

#include <QImage>

#include <string>

class CTkApp;

class CTkAppImage {
 public:
  explicit CTkAppImage(CTkApp *tk, const QString &name, int width=-1, int height=-1);

 ~CTkAppImage();

  const QString &name() const { return name_; }

  const QString &filename() const { return filename_; }

  const QString &type() const { return type_; }
  void setType(const QString &s) { type_ = s; }

  bool isColor() const { return (type_ == "photo"); }

  QImage getQImage() const;

  QPixmap getQPixmap() const;

  int width() const;
  void setWidth(int w);

  int height() const;
  void setHeight(int h);

  bool loadFile(const QString &filename);
  bool loadSVG (const QString &filename);
  bool loadData(const QString &data);

  void clear();

  bool getPixel(int x, int y, QColor &c) const;
  bool setPixel(int x, int y, const QColor &c);

  bool setPixels(int x1, int y1, int x2, int y2, const QColor &c);

 private:
  CTkApp* tk_ { nullptr };
  QString name_;
  QString type_;
  QImage  qimage_;
  QString filename_;
  int     width_ { 0 };
  int     height_ { 0 };
};

#endif

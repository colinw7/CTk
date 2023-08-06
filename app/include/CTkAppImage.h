#ifndef CTkAppImage_H
#define CTkAppImage_H

#include <QImage>

#include <string>

class CTkApp;

class CTkAppImage {
 public:
  explicit CTkAppImage(CTkApp *tk, const std::string &name, int width=-1, int height=-1);

 ~CTkAppImage();

  const std::string &name() const { return name_; }

  const std::string &filename() const { return filename_; }

  const std::string &type() const { return type_; }
  void setType(const std::string &s) { type_ = s; }

  bool isColor() const { return (type_ == "photo"); }

  QImage getQImage() const;

  QPixmap getQPixmap() const;

  int width() const;
  void setWidth(int w);

  int height() const;
  void setHeight(int h);

  bool loadFile(const std::string &filename);
  bool loadSVG (const std::string &filename);
  bool loadData(const std::string &data);

  void clear();

  bool getPixel(int x, int y, QColor &c) const;
  bool setPixel(int x, int y, const QColor &c);

  bool setPixels(int x1, int y1, int x2, int y2, const QColor &c);

 private:
  CTkApp*     tk_ { nullptr };
  std::string name_;
  std::string type_;
  QImage      qimage_;
  std::string filename_;
  int         width_ { 0 };
  int         height_ { 0 };
};

#endif

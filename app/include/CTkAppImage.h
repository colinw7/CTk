#ifndef CTkAppImage_H
#define CTkAppImage_H

#include <CImagePtr.h>

#include <QImage>

#include <string>

class CTkAppImage {
 public:
  explicit CTkAppImage(const std::string &name) :
   name_(name) {
  }

  bool loadFile(const std::string &filename);
  bool loadSVG (const std::string &filename);
  bool loadData(const std::string &data);

  CImagePtr getImage() const { return image_; }

  QImage getQImage() const;

 private:
  std::string name_;
  CImagePtr   image_;
  QImage      qimage_;
};

#endif

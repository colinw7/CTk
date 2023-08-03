#ifndef CTkAppImage_H
#define CTkAppImage_H

#include <CImagePtr.h>

#include <QImage>

#include <string>

class CTkApp;

class CTkAppImage {
 public:
  explicit CTkAppImage(CTkApp *tk, const std::string &name) :
   tk_(tk), name_(name) {
  }

  bool loadFile(const std::string &filename);
  bool loadSVG (const std::string &filename);
  bool loadData(const std::string &data);

  CImagePtr getImage() const { return image_; }

  QImage getQImage() const;

  const std::string &filename() const { return filename_; }

  void clear();

 private:
  CTkApp*     tk_ { nullptr };
  std::string name_;
  CImagePtr   image_;
  QImage      qimage_;
  std::string filename_;
};

#endif

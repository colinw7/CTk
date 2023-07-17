#ifndef CTkAppImage_H
#define CTkAppImage_H

#include <CImagePtr.h>

#include <string>

class CTkAppImage {
 public:
  CTkAppImage(const std::string &name) :
   name_(name) {
  }

  bool loadFile(const std::string &filename);

  CImagePtr getImage() const { return image_; }

 private:
  std::string name_;
  CImagePtr   image_;
};

#endif

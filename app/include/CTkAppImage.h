#ifndef CTK_IMAGE_H
#define CTK_IMAGE_H

#include <CImagePtr.h>

#include <string>

class CTkImage {
 public:
  CTkImage(const std::string &name) :
   name_(name) {
  }

  bool loadFile(const std::string &filename);

  CImagePtr getImage() const { return image_; }

 private:
  std::string name_;
  CImagePtr   image_;
};

#endif

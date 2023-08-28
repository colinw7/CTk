#ifndef CXBM_IMAGE_H
#define CXBM_IMAGE_H

#include <string>
#include <iosfwd>

#define CXBMImageInst CXBMImage::getInstance()

class CGenImage;

class CXBMImage {
 public:
  static CXBMImage *getInstance() {
    static CXBMImage *instance;

    if (! instance)
      instance = new CXBMImage;

    return instance;
  }

 ~CXBMImage() { }

  CXBMImage(const CXBMImage &xpm) = delete;
  CXBMImage &operator=(const CXBMImage &xpm) = delete;

  bool check(std::istream &file) const;

  bool read(std::istream &file, CGenImage *image);

  bool readHeader(std::istream &file, CGenImage *image);

  bool read(const unsigned char *data, CGenImage *image, int width, int height);

  bool write(std::ostream &os, CGenImage *image, const std::string &base="xbm");

 private:
  CXBMImage();

 private:
  bool readBitmap(std::istream &file, unsigned int *width, unsigned int *height,
                  unsigned int **data, int *x_hot, int *y_hot);

  unsigned int *expandBitmapData(const unsigned char *data, int width, int height);

  void skipSpaceAndComment(char *data, unsigned int *i);
};

#endif

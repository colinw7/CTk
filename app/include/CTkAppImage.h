#ifndef CTkAppImage_H
#define CTkAppImage_H

#ifdef CTK_CIMAGE
#include <CImagePtr.h>
#endif

#include <QImage>

#include <string>

class CTkApp;

class CTkAppImage {
 public:
  explicit CTkAppImage(CTkApp *tk, const std::string &name);

 ~CTkAppImage();

  bool loadFile(const std::string &filename);
  bool loadSVG (const std::string &filename);
  bool loadData(const std::string &data);

#if 0
  const CImagePtr &getImage() const;
  void setImage(const CImagePtr &image);
#endif

  QImage getQImage() const;

  QPixmap getQPixmap() const;

  const std::string &filename() const { return filename_; }

  void clear();

 private:
  CTkApp*     tk_ { nullptr };
  std::string name_;
#ifdef CTK_CIMAGE
  CImagePtr   image_;
#endif
  QImage      qimage_;
  std::string filename_;
};

#endif

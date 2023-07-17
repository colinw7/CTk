#include <CTkAppImage.h>
#include <CImageLib.h>

bool
CTkAppImage::
loadFile(const std::string &filename)
{
  CImageFileSrc src(filename);

  image_ = CImageMgrInst->createImage(src);

  return image_.isValid();
}

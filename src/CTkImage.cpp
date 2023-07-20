#include <CTkImage.h>
#include <CImageLib.h>

bool
CTkImage::
loadFile(const std::string &filename)
{
  CImageFileSrc src(filename);

  image_ = CImageMgrInst->createImage(src);

  return image_.isValid();
}

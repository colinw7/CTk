#include <CTkAppBitmaps.h>
#include <CTkAppImage.h>
#include <CTkApp.h>

namespace {

#include <bitmaps/error.xbm>
#include <bitmaps/gray12.xbm>
#include <bitmaps/gray25.xbm>
#include <bitmaps/gray50.xbm>
#include <bitmaps/gray75.xbm>
#include <bitmaps/hourglass.xbm>
#include <bitmaps/info.xbm>
#include <bitmaps/questhead.xbm>
#include <bitmaps/question.xbm>
#include <bitmaps/warning.xbm>

}

#define ADD_BITMAP(NAME) { \
auto image = std::make_shared<CTkAppImage>(app, #NAME); \
image->loadXBMData(NAME##_width, NAME##_height, NAME##_bits); \
app->addBitmap(#NAME, image); \
}

namespace CTkAppBitmaps {

void addStandardBitmaps(CTkApp *app)
{
  ADD_BITMAP(error)
  ADD_BITMAP(gray12)
  ADD_BITMAP(gray25)
  ADD_BITMAP(gray50)
  ADD_BITMAP(gray75)
  ADD_BITMAP(hourglass)
  ADD_BITMAP(info)
  ADD_BITMAP(questhead)
  ADD_BITMAP(question)
  ADD_BITMAP(warning)
}

}

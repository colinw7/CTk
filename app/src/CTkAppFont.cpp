#include <CTkAppFont.h>
#include <CTkApp.h>

CTkAppFont::
CTkAppFont(CTkApp *tk, const QString &name) :
 tk_(tk), name_(name)
{
}

CTkAppFont::
~CTkAppFont()
{
}

QFont
CTkAppFont::
getQFont() const
{
  return qfont_;
}

QString
CTkAppFont::
family() const
{
  return qfont_.family();
}

void
CTkAppFont::
setFamily(const QString &name)
{
  qfont_.setFamily(name);
}

void
CTkAppFont::
setPointSize(double size)
{
  qfont_.setPointSize(size);
}

void
CTkAppFont::
setPixelSize(double size)
{
  qfont_.setPixelSize(size);
}

void
CTkAppFont::
setNormal()
{
  qfont_.setBold(false);
}

void
CTkAppFont::
setBold()
{
  qfont_.setBold(true);
}

void
CTkAppFont::
setItalic(bool b)
{
  qfont_.setItalic(b);
}

void
CTkAppFont::
setUnderline(bool b)
{
  qfont_.setUnderline(b);
}

void
CTkAppFont::
setOverstrike(bool b)
{
  qfont_.setStrikeOut(b);
}

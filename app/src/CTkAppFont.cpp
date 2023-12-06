#include <CTkAppFont.h>
#include <CTkApp.h>

#include <QFontInfo>
#include <QFontMetricsF>

namespace {

bool isFixedPitch(const QFont &font) {
  const QFontInfo fi(font);
  //qDebug() << fi.family() << fi.fixedPitch();

  return fi.fixedPitch();
}

QFont getMonospaceFont() {
  QFont font("monospace");
  if (isFixedPitch(font)) return font;

  font.setStyleHint(QFont::Monospace);
  if (isFixedPitch(font)) return font;

  font.setStyleHint(QFont::TypeWriter);
  if (isFixedPitch(font)) return font;

  font.setFamily("courier");
  if (isFixedPitch(font)) return font;

  return font;
}

}

//----

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

CTkAppFont &
CTkAppFont::
setFamily(const QString &name)
{
  qfont_.setFamily(name);

  if (name == "monospace")
    qfont_ = getMonospaceFont();

  return *this;
}

double
CTkAppFont::
pointSize() const
{
  return qfont_.pointSizeF();
}

CTkAppFont &
CTkAppFont::
setPointSize(double size)
{
  if (size > 0)
    qfont_.setPointSize(size);

  return *this;
}

double
CTkAppFont::
pixelSize() const
{
  return qfont_.pixelSize();
}

CTkAppFont &
CTkAppFont::
setPixelSize(double size)
{
  if (size > 0)
    qfont_.setPixelSize(size);

  return *this;
}

CTkAppFont &
CTkAppFont::
setNormal()
{
  qfont_.setBold(false);

  return *this;
}

bool
CTkAppFont::
isBold() const
{
  return qfont_.bold();
}

CTkAppFont &
CTkAppFont::
setBold(bool b)
{
  qfont_.setBold(b);

  return *this;
}

bool
CTkAppFont::
isItalic() const
{
  return qfont_.italic();
}

CTkAppFont &
CTkAppFont::
setItalic(bool b)
{
  qfont_.setItalic(b);

  return *this;
}

bool
CTkAppFont::
isUnderline() const
{
  return qfont_.underline();
}

CTkAppFont &
CTkAppFont::
setUnderline(bool b)
{
  qfont_.setUnderline(b);

  return *this;
}

bool
CTkAppFont::
isOverstrike() const
{
  return qfont_.strikeOut();
}

CTkAppFont &
CTkAppFont::
setOverstrike(bool b)
{
  qfont_.setStrikeOut(b);

  return *this;
}

//---

void
CTkAppFont::
getFontData(CTkAppFontData &data) const
{
  getFontData(qfont_, data);
}

void
CTkAppFont::
getFontData(const QFont &qfont, CTkAppFontData &data)
{
  data.family     = qfont.family();
  data.size       = qfont.pointSizeF();
  data.weight     = (qfont.bold() ? "bold" : "normal");
  data.slant      = (qfont.italic() ? "italic" : "roman");
  data.underline  = qfont.underline();
  data.overstrike = qfont.strikeOut();

  QFontMetricsF fm(qfont);

  data.ascent    = fm.ascent();
  data.descent   = fm.descent();
  data.linespace = fm.height();
  data.fixed     = isFixedPitch(qfont);
}

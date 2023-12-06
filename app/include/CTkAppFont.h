#ifndef CTkAppFont_H
#define CTkAppFont_H

#include <QFont>
#include <QObject>

#include <string>

class CTkApp;

struct CTkAppFontData {
  QString family     { };
  double  size       { 12.0 };
  QString weight     { "normal" };
  QString slant      { "roman" };
  bool    underline  { false };
  bool    overstrike { false };

  double ascent    { 10 };
  double descent   { 2 };
  double linespace { 12 };
  bool   fixed     { false };
};

class CTkAppFont : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString name       READ name)
  Q_PROPERTY(QString family     READ family       WRITE setFamily)
  Q_PROPERTY(bool    bold       READ isBold       WRITE setBold)
  Q_PROPERTY(bool    italic     READ isItalic     WRITE setItalic)
  Q_PROPERTY(bool    underline  READ isUnderline  WRITE setUnderline)
  Q_PROPERTY(bool    overstrike READ isOverstrike WRITE setOverstrike)

 public:
  explicit CTkAppFont(CTkApp *tk, const QString &name);

 ~CTkAppFont();

  //---

  const QString &name() const { return name_; }

  QString family() const;
  CTkAppFont &setFamily(const QString &name);

  QFont getQFont() const;

  //--

  double pointSize() const;
  CTkAppFont &setPointSize(double size);

  double pixelSize() const;
  CTkAppFont &setPixelSize(double size);

  //---

  CTkAppFont &setNormal();

  bool isBold() const;
  CTkAppFont &setBold(bool b=true);

  bool isItalic() const;
  CTkAppFont &setItalic(bool b);

  bool isUnderline() const;
  CTkAppFont &setUnderline(bool b);

  bool isOverstrike() const;
  CTkAppFont &setOverstrike(bool b);

  //---

  void getFontData(CTkAppFontData &data) const;

  static void getFontData(const QFont &qfont, CTkAppFontData &data);

 private:
  CTkApp* tk_ { nullptr };
  QString name_;
  QFont   qfont_;
};

#endif

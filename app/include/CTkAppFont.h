#ifndef CTkAppFont_H
#define CTkAppFont_H

#include <QFont>

#include <string>

class CTkApp;

class CTkAppFont {
 public:
  explicit CTkAppFont(CTkApp *tk, const QString &name);

 ~CTkAppFont();

  const QString &name() const { return name_; }

  QFont getQFont() const;

  QString family() const;
  void setFamily(const QString &name);

  void setPointSize(double size);
  void setPixelSize(double size);

  void setNormal();
  void setBold();
  void setItalic(bool b);
  void setUnderline(bool b);
  void setOverstrike(bool b);

 private:
  CTkApp* tk_ { nullptr };
  QString name_;
  QFont   qfont_;
};

#endif

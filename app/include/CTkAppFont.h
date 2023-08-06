#ifndef CTkAppFont_H
#define CTkAppFont_H

#include <QFont>

#include <string>

class CTkApp;

class CTkAppFont {
 public:
  explicit CTkAppFont(CTkApp *tk, const std::string &name);

 ~CTkAppFont();

  const std::string &name() const { return name_; }

  QFont getQFont() const;

  std::string family() const;
  void setFamily(const std::string &name);

  void setPointSize(double size);
  void setPixelSize(double size);

  void setNormal();
  void setBold();
  void setItalic(bool b);
  void setUnderline(bool b);
  void setOverstrike(bool b);

 private:
  CTkApp*     tk_ { nullptr };
  std::string name_;
  QFont       qfont_;
};

#endif

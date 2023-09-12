#ifndef CTkAppFont_H
#define CTkAppFont_H

#include <QFont>
#include <QObject>

#include <string>

class CTkApp;

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
  void setFamily(const QString &name);

  QFont getQFont() const;

  //--

  void setPointSize(double size);
  void setPixelSize(double size);

  void setNormal();

  bool isBold() const;
  void setBold(bool b=true);

  bool isItalic() const;
  void setItalic(bool b);

  bool isUnderline() const;
  void setUnderline(bool b);

  bool isOverstrike() const;
  void setOverstrike(bool b);

 private:
  CTkApp* tk_ { nullptr };
  QString name_;
  QFont   qfont_;
};

#endif

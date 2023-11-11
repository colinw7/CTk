#ifndef CTkAppCursor_H
#define CTkAppCursor_H

#include <QObject>

class CTkAppCursor;
class CTkApp;

class CTkAppCursorMgr {
 public:
  CTkAppCursorMgr(CTkApp *tk) : tk_(tk) { }
 ~CTkAppCursorMgr() { }

  CTkAppCursor *getCursor(const QString &name);

 private:
  using Cursors = std::map<QString, CTkAppCursor *>;

  CTkApp* tk_ { nullptr };
  Cursors cursors_;
};

class CTkAppCursor : public QObject {
 public:
  explicit CTkAppCursor(CTkApp *tk, const QString &name);

 ~CTkAppCursor();

 private:
  CTkApp* tk_ { nullptr };
  QString name_;
};

#endif

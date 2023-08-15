#ifndef CTkWidget_H
#define CTkWidget_H

#include <QFrame>

class CTkApp;

class CTkWidget : public QFrame {
  Q_OBJECT

 public:
  CTkWidget(CTkApp *app, const QString &id);

  void loadFile(const QString &filename);

 private:
  CTkApp *app_ { nullptr };
  QString id_;
};

#endif

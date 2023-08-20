#ifndef CTkWidget_H
#define CTkWidget_H

#include <QFrame>

class CTkApp;

class CTkWidget : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString id READ id)

 public:
  CTkWidget(CTkApp *app, const QString &id);

  const QString &id() const { return id_; }

  void loadFile(const QString &filename);

 private:
  CTkApp *app_ { nullptr };
  QString id_;
};

#endif

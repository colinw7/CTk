#ifndef CQTkWidget_H
#define CQTkWidget_H

#include <QFrame>

class CQTkWidget : public QFrame {
  Q_OBJECT

 public:
  CQTkWidget(QWidget *parent, const QString &name);

  void resizeEvent(QResizeEvent *) override;

 private:
  QString name_;
};

#endif

#ifndef CTkAppLayout_H
#define CTkAppLayout_H

#include <QLayout>

class CTkAppLayout : public QLayout {
  Q_OBJECT

 public:
  CTkAppLayout(QWidget *w) :
   QLayout(w) {
  }

  virtual std::string name() const = 0;
};

#endif

#ifndef CTkAppLayout_H
#define CTkAppLayout_H

#include <QLayout>

class CTkAppLayoutWidget;

class CTkAppLayout : public QLayout {
  Q_OBJECT

 public:
  enum class Type {
    NONE,
    PACK,
    GRID,
    PLACE
  };

 public:
  CTkAppLayout(QWidget *w, const Type &type) :
   QLayout(w), type_(type) {
  }

  virtual QString name() const = 0;

  virtual std::vector<CTkAppLayoutWidget *> getLayoutWidgets() const = 0;

 protected:
  enum SizeType { MinimumSize, SizeHint };

  Type type_ { Type::NONE };
};

#endif

#ifndef CTkAppLayout_H
#define CTkAppLayout_H

#include <QLayout>

class CTkAppLayoutWidget;
class CTkAppWidget;

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
  CTkAppLayout(CTkAppWidget *w, const Type &type);
  CTkAppLayout(const Type &type);

  virtual QString name() const = 0;

  virtual std::vector<CTkAppLayoutWidget *> getLayoutWidgets(bool flat=false) const = 0;

 protected:
  enum SizeType { MinimumSize, SizeHint };

  CTkAppWidget* w_    { nullptr };
  Type          type_ { Type::NONE };
};

#endif

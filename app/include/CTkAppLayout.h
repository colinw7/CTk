#ifndef CTkAppLayout_H
#define CTkAppLayout_H

#include <QLayout>
#include <QPointer>

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

  CTkAppWidget *widget() const;

  virtual std::vector<CTkAppLayoutWidget *> getLayoutWidgets(bool flat=false) const = 0;

 protected:
  using WidgetP = QPointer<CTkAppWidget>;

  enum SizeType { MinimumSize, SizeHint };

  WidgetP widget_;
  Type    type_ { Type::NONE };
};

#endif

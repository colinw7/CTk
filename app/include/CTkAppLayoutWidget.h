#ifndef CTkAppLayoutWidget_H
#define CTkAppLayoutWidget_H

#include <QLayout>
#include <QObject>
#include <QRect>
#include <QWidgetItem>

#include <vector>

class CTkAppLayout;
class CTkAppWidget;

class CTkAppLayoutWidget : public QObject, public QLayoutItem {
  Q_OBJECT

 public:
  using TkWidget = CTkAppWidget;

  explicit CTkAppLayoutWidget(CTkAppLayout *l, TkWidget *widget);

  explicit CTkAppLayoutWidget(CTkAppLayout *l, QLayout *layout);

  TkWidget *getTkWidget() const { return widget_; }
  QLayout  *getLayout() const { return layout_; }

  Qt::Orientations expandingDirections() const override;

  QRect geometry() const override;

  bool hasHeightForWidth() const override;
  int heightForWidth(int w) const override;

  bool isEmpty() const override;

  QSize maximumSize() const override;
  QSize minimumSize() const override;

  void setGeometry(const QRect &rect) override;

  void show();

  QMargins contentsMargins() const;

  QSize sizeHint() const override;

  QSize minimumSizeHint() const;

  QWidget *widget() override;

  QString name() const;

 private:
  void init();

  QWidget *getQWidget() const;

 private:
  CTkAppLayout* l_      { nullptr };
  TkWidget*     widget_ { nullptr };
  QLayout*      layout_ { nullptr };
};

#endif

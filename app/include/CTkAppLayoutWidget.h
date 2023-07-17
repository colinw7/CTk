#ifndef CTkAppLayoutWidget_H
#define CTkAppLayoutWidget_H

#include <QLayout>
#include <QRect>
#include <QWidgetItem>

#include <vector>

class CTkAppWidget;

class CTkAppLayoutWidget : public QLayoutItem {
 public:
  using TkWidget = CTkAppWidget;

  CTkAppLayoutWidget(TkWidget *widget) :
   widget_(widget) {
  }

  CTkAppLayoutWidget(QLayout *layout) :
   layout_(layout) {
  }

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

  QSize sizeHint() const override;

  QWidget *widget() override;

 private:
  QWidget *getQWidget() const;

 private:
  TkWidget* widget_ { nullptr };
  QLayout*  layout_ { nullptr };
};

#endif

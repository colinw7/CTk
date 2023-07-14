#ifndef CTK_LAYOUT_H
#define CTK_LAYOUT_H

#include <QLayout>
#include <QRect>
#include <QWidgetItem>

#include <vector>

class CTkAppWidget;

class CTkLayoutWidget : public QLayoutItem {
 public:
  CTkLayoutWidget(CTkAppWidget *widget) :
   widget_(widget) {
  }

  CTkAppWidget *getTkWidget() const { return widget_; }

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
  CTkAppWidget *widget_ { nullptr };
};

#endif

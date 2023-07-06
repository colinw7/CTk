#ifndef CTK_LAYOUT_H
#define CTK_LAYOUT_H

#include <QLayout>
#include <QRect>
#include <QWidgetItem>

#include <vector>

class CTkWidget;

class CTkLayoutWidget : public QLayoutItem {
 public:
  CTkLayoutWidget(CTkWidget *widget) :
   widget_(widget) {
  }

  CTkWidget *getTkWidget() const { return widget_; }

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
  CTkWidget *widget_ { nullptr };
};

#endif

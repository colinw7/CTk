#include <QWidget>

class CQTkWidgetTest : public QWidget {
  Q_OBJECT

 public:
  CQTkWidgetTest();

  void init();

  QSize sizeHint() const override { return QSize(400, 400); }

 private slots:
  void timerSlot();
};

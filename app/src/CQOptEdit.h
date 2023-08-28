#ifndef CQOptEdit_H
#define CQOptEdit_H

#include <QFrame>

class QHBoxLayout;
class QCheckBox;

class CQOptEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)

 public:
  CQOptEdit(QWidget *parent=nullptr);

  bool isEnabled() const { return enabled_; }
  void setEnabled(bool b);

  QWidget *widget() const { return widget_; }
  void setWidget(QWidget *w);

 Q_SIGNALS:
  void enabledChanged(bool);

 private Q_SLOTS:
  void checkSlot(int);

 private:
  QHBoxLayout* layout_ { nullptr };
  QCheckBox*   check_  { nullptr };
  QWidget*     widget_ { nullptr };

  bool enabled_ { true };
};

#endif

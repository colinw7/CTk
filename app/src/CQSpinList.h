#ifndef CQSpinList_H
#define CQSpinList_H

#include <QSpinBox>

class CQSpinList : public QSpinBox {
  Q_OBJECT

  Q_PROPERTY(Mode mode READ mode WRITE setMode)

  Q_ENUMS(Mode)

 public:
  enum class Mode {
    INTEGER,
    STRINGS
  };

 public:
  CQSpinList(QWidget *parent=nullptr);

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &v) { mode_ = v; }

  const QStringList &strings() const { return strings_; }
  void setStrings(const QStringList &s);

 private:
  QString textFromValue(int value) const override;
  int valueFromText(const QString &text) const override;

  QValidator::State validate(QString &text, int &pos) const override;

  void fixup(QString &input) const override;

 private:
  QStringList strings_;
  Mode        mode_ { Mode::INTEGER };
};

#endif

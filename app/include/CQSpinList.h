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
    REAL,
    STRINGS
  };

 public:
  CQSpinList(QWidget *parent=nullptr);

  const Mode &mode() const { return mode_; }
  void setMode(const Mode &v) { mode_ = v; }

  const QStringList &strings() const { return strings_; }
  void setStrings(const QStringList &s);

  double rmin() const { return rmin_; }
  void setRMin(double r) { rmin_ = r; }

  double rmax() const { return rmax_; }
  void setRMax(double r) { rmax_ = r; }

  double rstep() const { return rstep_; }
  void setRStep(double r) { rstep_ = r; }

  QString getText() const;
  void setText(const QString &text);

  void setValidator(QValidator *validator);

  void deleteChars(int start, int len);
  void insertChars(int pos, const QString &text);

  void deselectAll();
  void selectChars(int start, int len);

  int cursorPos() const;
  void setCursorPos(int pos);

 private:
  QString textFromValue(int value) const override;
  int valueFromText(const QString &text) const override;

  QValidator::State validate(QString &text, int &pos) const override;

  void fixup(QString &input) const override;

 private:
  Mode        mode_  { Mode::INTEGER };
  QStringList strings_;
  double      rmin_  { 0 };
  double      rmax_  { 100 };
  double      rstep_ { 100 };
};

#endif

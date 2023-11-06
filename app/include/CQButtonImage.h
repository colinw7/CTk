#ifndef CQButtonImage_H
#define CQButtonImage_H

#include <QAbstractButton>

class CQButtonImage : public QAbstractButton {
  Q_OBJECT

  Q_PROPERTY(Type          type       READ type        WRITE setType)
  Q_PROPERTY(QString       label      READ label       WRITE setLabel)
  Q_PROPERTY(QImage        image      READ image       WRITE setImage)
  Q_PROPERTY(int           userWidth  READ userWidth   WRITE setUserWidth)
  Q_PROPERTY(int           userHeight READ userHeight  WRITE setUserHeight)
  Q_PROPERTY(bool          autoRaise  READ isAutoRaise WRITE setAutoRaise)
  Q_PROPERTY(int           borderX    READ borderX     WRITE setBorderX)
  Q_PROPERTY(int           borderY    READ borderY     WRITE setBorderY)
  Q_PROPERTY(Qt::Alignment justify    READ justify     WRITE setJustify)

  Q_ENUMS(Type)

 public:
  enum class Type {
    NONE,
    BOTTOM,
    TOP,
    LEFT,
    RIGHT,
    CENTER
  };

 public:
  CQButtonImage(QWidget *parent=nullptr);

  //! get/set type
  const Type &type() const { return type_; }
  void setType(const Type &t) { type_ = t; update(); }

  //! get/set label
  const QString &label() const { return label_; }
  void setLabel(const QString &s) { label_ = s; updateLines(); update(); }

  //! get/set image
  const QImage &image() const { return image_; }
  void setImage(const QImage &i) { image_ = i; update(); }

  int userWidth() const { return userWidth_; }
  void setUserWidth(int i) { userWidth_ = i; update(); }

  int userHeight() const { return userHeight_; }
  void setUserHeight(int i) { userHeight_ = i; update(); }

  bool isAutoRaise() const { return autoRaise_; }
  void setAutoRaise(bool b) { autoRaise_ = b; }

  int borderX() const { return borderX_; }
  void setBorderX(int i) { borderX_ = i; update(); }

  int borderY() const { return borderY_; }
  void setBorderY(int i) { borderY_ = i; update(); }

  const Qt::Alignment &justify() const { return justify_; }
  void setJustify(const Qt::Alignment &a) { justify_ = a; update(); }

  //! get size
  QSize sizeHint() const override;

 protected:
  void updateLines();

  void paintEvent(QPaintEvent *) override;

 private:
  Type    type_ { Type::LEFT };
  QString label_;
  QImage  image_;

  int           userWidth_  { -1 };
  int           userHeight_ { -1 };
  bool          autoRaise_  { false };
  int           borderX_    { 2 };
  int           borderY_    { 2 };
  Qt::Alignment justify_    { Qt::AlignCenter };

  mutable QStringList lines_;
};

#endif

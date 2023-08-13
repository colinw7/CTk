#ifndef CQTextWidget_H
#define CQTextWidget_H

#include <QTextEdit>
#include <QTextObjectInterface>
#include <QPointer>

class CQTextWidget;

//---

class CQTextWidgetWidgetObj : public QObject, public QTextObjectInterface {
  Q_OBJECT

  Q_INTERFACES(QTextObjectInterface)

 public:
  CQTextWidgetWidgetObj(CQTextWidget *edit);

  QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format) override;

  void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc,
                  int posInDocument, const QTextFormat &format) override;

 private:
  CQTextWidget *edit_ { nullptr };
};

//---

class CQTextWidgetImageObj : public QObject, public QTextObjectInterface {
  Q_OBJECT

  Q_INTERFACES(QTextObjectInterface)

 public:
  CQTextWidgetImageObj(CQTextWidget *edit);

  QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format) override;

  void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc,
                  int posInDocument, const QTextFormat &format) override;

 private:
  CQTextWidget *edit_ { nullptr };
};

//---

class CQTextWidget : public QTextEdit {
  Q_OBJECT

 public:
  enum { WidgetObjFormat = QTextFormat::UserObject + 1 };
  enum { ImageObjFormat  = QTextFormat::UserObject + 2 };

 public:
  CQTextWidget(QWidget *parent=nullptr);

  void insertWidget(QWidget *w);

  void insertImage(QImage i);

  void deleteText(int startLine, int startChar, int endLine, int endChar);

  void setCurrentInd(int lineNum, int charNum,
                     const QTextCursor::MoveMode &moveMode=QTextCursor::MoveAnchor);

  void setCurrentInd(QTextCursor &cursor, int lineNum, int charNum,
                     const QTextCursor::MoveMode &moveMode=QTextCursor::MoveAnchor);

 protected:
  friend class CQTextWidgetWidgetObj;
  friend class CQTextWidgetImageObj;

  QWidget *getWidget(const QString &name) const;
  QImage   getImage (const QString &name) const;

 private:
  using WidgetP = QPointer<QWidget>;
  using Widgets = std::map<QString, WidgetP>;
  using Images  = std::map<QString, QImage>;

  Widgets widgets_;
  Images  images_;
};

#endif

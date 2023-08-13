#include <CQTextWidget.h>

#include <QPainter>
#include <QTextFrame>

CQTextWidget::
CQTextWidget(QWidget *parent) :
 QTextEdit(parent)
{
  auto *widgetObj = new CQTextWidgetWidgetObj(this);
  auto *imageObj  = new CQTextWidgetImageObj (this);

  document()->documentLayout()->registerHandler(WidgetObjFormat, widgetObj);
  document()->documentLayout()->registerHandler(ImageObjFormat , imageObj );
}

void
CQTextWidget::
insertWidget(QWidget *w)
{
  auto name = QString("widget.%1").arg(widgets_.size());

  widgets_[name] = WidgetP(w);

  QTextImageFormat c;

  c.setObjectType(WidgetObjFormat);
  c.setName(name);

  textCursor().insertText(QString(QChar::ObjectReplacementCharacter), c);

  w->setParent(this);
}

void
CQTextWidget::
insertImage(QImage i)
{
  auto name = QString("image.%1").arg(images_.size());

  images_[name] = i;

  QTextImageFormat c;

  c.setObjectType(ImageObjFormat);
  c.setName(name);

  textCursor().insertText(QString(QChar::ObjectReplacementCharacter), c);
}

QWidget *
CQTextWidget::
getWidget(const QString &name) const
{
  auto pw = widgets_.find(name);
  if (pw == widgets_.end()) return nullptr;

  return (*pw).second;
}

QImage
CQTextWidget::
getImage(const QString &name) const
{
  auto pw = images_.find(name);
  if (pw == images_.end()) return QImage();

  return (*pw).second;
}

void
CQTextWidget::
deleteText(int startLine, int startChar, int endLine, int endChar)
{
  auto cursor = textCursor();
  setCurrentInd(cursor, startLine, startChar);
  setCurrentInd(cursor, endLine, endChar, QTextCursor::KeepAnchor);
  cursor.removeSelectedText();
}

void
CQTextWidget::
setCurrentInd(int lineNum, int charNum, const QTextCursor::MoveMode &moveMode)
{
  auto cursor = this->textCursor();
  setCurrentInd(cursor, lineNum, charNum, moveMode);
  setTextCursor(cursor);
}

void
CQTextWidget::
setCurrentInd(QTextCursor &cursor, int lineNum, int charNum,
              const QTextCursor::MoveMode &moveMode)
{
  cursor.movePosition(QTextCursor::Start, moveMode, 0);
  cursor.movePosition(QTextCursor::NextBlock, moveMode, lineNum - 1);
  cursor.movePosition(QTextCursor::StartOfBlock, moveMode);
  cursor.movePosition(QTextCursor::NextCharacter, moveMode, charNum);
}

//---

CQTextWidgetWidgetObj::
CQTextWidgetWidgetObj(CQTextWidget *edit) :
 edit_(edit)
{
}

QSizeF
CQTextWidgetWidgetObj::
intrinsicSize(QTextDocument * /*doc*/, int /*posInDocument*/, const QTextFormat &format)
{
  auto format1 = format.toImageFormat();

  auto *w = edit_->getWidget(format1.name());

  if (w)
    return w->sizeHint();

  return QSize(100, 100);
}

void
CQTextWidgetWidgetObj::
drawObject(QPainter *painter, const QRectF &rect, QTextDocument * /*doc*/,
           int /*posInDocument*/, const QTextFormat &format)
{
  auto format1 = format.toImageFormat();

  auto *w = edit_->getWidget(format1.name());

  if (w) {
    w->move(rect.x(), rect.y());
  }
  else
    painter->fillRect(rect, Qt::red);
}

//---

CQTextWidgetImageObj::
CQTextWidgetImageObj(CQTextWidget *edit) :
 edit_(edit)
{
}

QSizeF
CQTextWidgetImageObj::
intrinsicSize(QTextDocument * /*doc*/, int /*posInDocument*/, const QTextFormat &format)
{
  auto format1 = format.toImageFormat();

  auto i = edit_->getImage(format1.name());

  if (i.isNull())
    return i.size();

  return QSize(100, 100);
}

void
CQTextWidgetImageObj::
drawObject(QPainter *painter, const QRectF &rect, QTextDocument * /*doc*/,
           int /*posInDocument*/, const QTextFormat &format)
{
  auto format1 = format.toImageFormat();

  auto i = edit_->getImage(format1.name());

  if (! i.isNull()) {
    painter->drawImage(rect.x(), rect.y(), i);
  }
  else
    painter->fillRect(rect, Qt::red);
}

#include <CQOptEdit.h>

#include <QCheckBox>
#include <QHBoxLayout>

CQOptEdit::
CQOptEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("optEdit");

  layout_ = new QHBoxLayout(this);

  check_ = new QCheckBox;
  check_->setObjectName("check");
  check_->setChecked(enabled_);

  connect(check_, SIGNAL(stateChanged(int)), this, SLOT(checkSlot(int)));

  layout_->addWidget(check_);
}

void
CQOptEdit::
setEnabled(bool b)
{
  enabled_ = b;

  if (enabled_ != check_->isChecked())
    check_->setChecked(enabled_);

  if (widget_)
    widget_->setEnabled(enabled_);
}

void
CQOptEdit::
setWidget(QWidget *w)
{
  widget_ = w;

  layout_->addWidget(widget_);

  widget_->setEnabled(enabled_);
}

void
CQOptEdit::
checkSlot(int)
{
  setEnabled(check_->isChecked());

  Q_EMIT enabledChanged(enabled_);
}

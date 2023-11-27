#include <CTkAppRoot.h>
#include <CTkAppGridLayout.h>
#include <CTkAppMenu.h>

#include <QApplication>
#include <QMenuBar>
#include <QPainter>

CTkAppRoot::
CTkAppRoot(CTkApp *tk) :
 CTkAppWidget(tk, nullptr, "")
{
  qroot_ = new CTkAppRootWidget(this);

  qroot_->setWindowTitle(".");

  setQWidget(qroot_);
}

void
CTkAppRoot::
setRoot(QFrame *qroot)
{
  delete qroot_;

  qroot_ = qroot;

  setQWidget(qroot_);
}

bool
CTkAppRoot::
show()
{
  if (! qroot_->isVisible()) {
    auto s = qroot_->sizeHint();

    qroot_->resize(s.expandedTo(QApplication::globalStrut()));
  }

  CTkAppWidget::show();

  qroot_->resize(qroot_->sizeHint());

  return true;
}

bool
CTkAppRoot::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-menu") {
    auto name = tk_->variantToString(var);

    auto *w = tk_->lookupWidgetByName(name, /*quiet*/true);
    if (! w) return false;

    auto *menu = dynamic_cast<CTkAppMenu *>(w);
    if (! menu) return false;

    auto *rootWidget = qobject_cast<CTkAppRootWidget *>(qroot_);
    if (! rootWidget) return false;

    rootWidget->setMenu(menu->qmenu());

    setMenuName(name);
  }
  else if (name == "-use") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppRoot::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  static auto optionNames = std::vector<QString>({ "cget", "configure" });

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  return CTkAppWidget::execDefaultOp(option, args);
}

bool
CTkAppRoot::
decodeWidgetName(const QVariant &var, CTkAppWidget **parent, QString &childName) const
{
  auto name = tk_->variantToString(var);

  uint len = name.size();

  if (len == 0) return false;

  if (name[0] != '.') return false;

  if (len == 1) {
    *parent = const_cast<CTkAppRoot *>(this);

    childName = "";

    return true;
  }

  CTkAppWidget* parent1 = const_cast<CTkAppRoot *>(this);
  CTkAppWidget* child1  = nullptr;

  uint pos1 = 1;
  uint pos2 = pos1;

  while (pos2 < len) {
    while (pos2 < len) {
      if (name[pos2] == '.')
        break;

      ++pos2;
    }

    if (pos2 < len)
      childName = name.mid(pos1, pos2 - pos1);
    else
      childName = name.mid(pos1);

    if (childName == "")
      return false;

    if (pos2 >= len)
      break;

    child1 = parent1->getChild(childName);

    if (child1 == nullptr)
      return false;

    parent1 = child1;
    child1  = nullptr;

    pos1 = pos2 + 1;
    pos2 = pos1;
  }

  *parent = parent1;

  return true;
}

//---

CTkAppRootWidget::
CTkAppRootWidget(CTkAppRoot *root) :
 QFrame(nullptr), root_(root)
{
  setObjectName("qroot");
}

void
CTkAppRootWidget::
setMenu(QMenu *menu)
{
  menu_ = menu;

  if (! menuBar_) {
    menuBar_ = new QMenuBar(this);
    menuBar_->setObjectName("menuBar");
  }

  for (auto *a : menu_->actions()) {
    if (a->menu())
      menuBar_->addMenu(a->menu());
  }

  QFontMetrics fm(font());

  setContentsMargins(0, fm.height(), 0, 0);

  menuBar_->move(0, 0);
  menuBar_->resize(width(), fm.height());
}

void
CTkAppRootWidget::
paintEvent(QPaintEvent *e)
{
  QFrame::paintEvent(e);

//QPainter painter(this);
//auto *pack = root_->getTkPackLayout();
//if (pack) pack->draw(&painter);

  QPainter painter(this);
  auto *grid = root_->getTkGridLayout(/*create*/false);
  if (grid) grid->draw(&painter);
}

QSize
CTkAppRootWidget::
sizeHint() const
{
  auto *layout = qobject_cast<CTkAppLayout *>(this->layout());

  auto s = (layout ? layout->sizeHint() : QFrame::sizeHint());

  if (s.width() <= 0 || s.height() <= 0)
    s = QSize(200, 200);

  return s;
}

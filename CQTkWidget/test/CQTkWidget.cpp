#include <CQTkWidget.h>
#include <CQTclUtil.h>

#ifndef QT_NO_X11
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#endif

static CQTcl *s_tcl;

CQTkWidget::
CQTkWidget(QWidget *parent, const QString &name) :
 QFrame(parent), name_(name)
{
  if (! s_tcl) {
    s_tcl = new CQTcl;

    if (! s_tcl->init()) {
      auto str = CTclUtil::errorInfo(s_tcl->interp(), -1);
      std::cerr << "Tcl Init Failed: " << str << "\n";
      assert(false);
    }

    if (! s_tcl->initTk()) {
      auto str = CTclUtil::errorInfo(s_tcl->interp(), -1);
      std::cerr << "Tk Init Failed: " << str << "\n";
      assert(false);
    }
  }

  auto pname = QString(".%1").arg(name_);

  QVariant res;
  s_tcl->eval(QString("toplevel %1 -use %2").arg(pname).arg(winId()), res, /*showError*/true);

  s_tcl->eval(QString("button %1.b -text Hello").arg(pname));
  s_tcl->eval(QString("%1.b configure -background white").arg(pname));
  s_tcl->eval(QString("%1.b configure -foreground black").arg(pname));
  s_tcl->eval(QString("pack %1.b").arg(pname));

  s_tcl->processEvents();

  auto tkwin = s_tcl->nameToWindow(pname.toStdString());

  if (tkwin)
    s_tcl->mapWindow(tkwin);

  s_tcl->processEvents();

  Window root, parentWin, *children;
  uint   numChildren;

  XQueryTree(QX11Info::display(), s_tcl->winId(tkwin), &root, &parentWin, &children, &numChildren);
}

void
CQTkWidget::
resizeEvent(QResizeEvent *)
{
  s_tcl->eval(QString("wm geometry .%1 %2x%3+0+0").
    arg(name_).arg(width()).arg(height()), true, true);

  s_tcl->eval(QString("puts [wm geometry .%1]").arg(name_), true, true);
}

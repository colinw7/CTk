#include <CTkAppX11.h>

#ifndef QT_NO_X11
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#undef None
#endif

namespace CTkAppX11 {

ulong
getRoot()
{
  return QX11Info::appRootWindow();
}

uint
getAtomInd(const QString &name, bool create)
{
#ifndef QT_NO_X11
  auto *disp = QX11Info::display();
  if (! disp) return 0;

  return XInternAtom(disp, name.toLatin1().constData(), create ? True : False);
#else
  return 0
#endif
}

QString
getAtomName(long ind)
{
#ifndef QT_NO_X11
  auto *disp = QX11Info::display();
  if (! disp) return QString();

  auto *name = XGetAtomName(disp, ind);
  if (! name) return QString();

  return QString(name);
#else
  return QString();
#endif
}

bool
setWmAspect(ulong winId, const QPoint &minAspect, const QPoint &maxAspect)
{
#ifndef QT_NO_X11
  auto *disp = QX11Info::display();
  if (! disp) return false;

  XSizeHints sizeHints;

  sizeHints.flags |= PAspect;

  sizeHints.min_aspect.x = minAspect.x();
  sizeHints.min_aspect.y = minAspect.y();
  sizeHints.max_aspect.x = maxAspect.x();
  sizeHints.max_aspect.y = maxAspect.y();

  XSetWMNormalHints(disp, winId, &sizeHints);

  return true;
#else
  return false;
#endif
}

bool
setWmClientMachine(ulong winId, const QString &name)
{
  auto atom = getAtomInd("WM_CLIENT_MACHINE", /*create*/true);

  return setStringProperty(winId, atom, name.toLatin1().constData());
}

bool
getWmClientMachine(ulong winId, QString &name)
{
  auto atom = getAtomInd("WM_CLIENT_MACHINE", /*create*/false);

  return getStringProperty(winId, atom, name);
}

bool
getWmColormapWindows(ulong winId, std::vector<uint> &windows)
{
  auto atom = getAtomInd("WM_COLORMAP_WINDOWS", /*create*/true);

  return getWindowListProperty(winId, atom, windows);
}

bool
setWmColormapWindows(ulong winId, const std::vector<uint> &windows)
{
  auto atom = getAtomInd("WM_COLORMAP_WINDOWS", /*create*/true);

  return setWindowListProperty(winId, atom, windows);
}

bool
getWmCommand(ulong winId, QString &name)
{
  auto atom = getAtomInd("WM_COMMAND", /*create*/true);

  return getStringProperty(winId, atom, name);
}

bool
setWmCommand(ulong winId, const QString &name)
{
  auto atom = getAtomInd("WM_COMMAND", /*create*/true);

  return setStringProperty(winId, atom, name);
}

bool
setStringProperty(ulong winId, uint atom, const QString &value)
{
#ifndef QT_NO_X11
  auto *disp = QX11Info::display();
  if (! disp) return false;

  XChangeProperty(disp, winId, atom, XA_STRING, 8, PropModeReplace,
                  reinterpret_cast<uchar *>(const_cast<char *>(value.toLatin1().constData())),
                  int(value.length()));

  return true;
#else
  return false;
#endif
}

bool
getStringProperty(ulong winId, uint atom, QString &value)
{
#ifndef QT_NO_X11
  auto *disp = QX11Info::display();
  if (! disp) return false;

  ulong  n;
  Atom   type;
  ulong  left;
  uchar *data;
  int    format;

  if (XGetWindowProperty(disp, winId, atom, 0, 1024, False, XA_STRING,
                         &type, &format, &n, &left, &data) != Success)
    return false;

  if (n == 0 || ! data)
    return false;

  value = QString(reinterpret_cast<char *>(data));

  XFree(data);

  return true;
#else
  return false;
#endif
}

bool
getWindowListProperty(ulong xwin, uint atom, std::vector<uint> &windows)
{
#ifndef QT_NO_X11
  auto *disp = QX11Info::display();
  if (! disp) return false;

  ulong  n;
  Atom   type;
  ulong  left;
  uchar *data;
  int    format;

  if (XGetWindowProperty(disp, xwin, atom, 0, 65535, False, XA_WINDOW,
                         &type, &format, &n, &left, &data) != Success)
    return false;

  if (format != 32 || left != 0)
    return false;

  for (ulong i = 0; i < n; ++i)
    windows.push_back((reinterpret_cast<Window *>(data))[i]);

  XFree(data);

  return true;
#else
  return false;
#endif
}

bool
setWindowListProperty(ulong xwin, uint atom, const std::vector<uint> &windows)
{
#ifndef QT_NO_X11
  auto *disp = QX11Info::display();
  if (! disp) return false;

  int numWindows = windows.size();

  XChangeProperty(disp, xwin, atom, XA_WINDOW, 32, PropModeReplace,
                  reinterpret_cast<uchar *>(const_cast<uint *>(&windows[0])), numWindows);

  return true;
#else
  return false;
#endif
}

}

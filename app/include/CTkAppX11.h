#ifndef CTkAppX11_H
#define CTkAppX11_H

#include <QString>
#include <QPoint>

namespace CTkAppX11 {
  ulong getRoot();

  uint getAtomInd(const QString &name, bool create);

  QString getAtomName(long ind);

  bool setWmAspect(ulong winId, const QPoint &minAspect, const QPoint &maxAspect);

  bool getWmClientMachine(ulong winId, QString &name);
  bool setWmClientMachine(ulong winId, const QString &name);

  bool getWmColormapWindows(ulong winId, std::vector<uint> &windows);
  bool setWmColormapWindows(ulong winId, const std::vector<uint> &windows);

  bool getWmCommand(ulong winId, QString &name);
  bool setWmCommand(ulong winId, const QString &name);

  bool getStringProperty(ulong winId, uint atom, QString &value);
  bool setStringProperty(ulong winId, uint atom, const QString &value);

  bool getWindowListProperty(ulong xwin, uint atom, std::vector<uint> &windows);
  bool setWindowListProperty(ulong xwin, uint atom, const std::vector<uint> &windows);
}

#endif

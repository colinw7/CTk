#include <CTkAppPanedWindow.h>
#include <CTkAppRoot.h>

#include <QSplitter>

CTkAppPanedWindow::
CTkAppPanedWindow(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qpane_ = new QSplitter(parent ? parent->getQWidget() : nullptr);
  qpane_->setObjectName("pane");

  qpane_->setChildrenCollapsible(false);

  setQWidget(qpane_);
}

bool
CTkAppPanedWindow::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-orient") {
    Qt::Orientation orient;
    if (! CTkAppUtil::variantToOrient(tk_, var, orient))
      return tk_->throwError(tk_->msg() + "bad orientation \"" + var + "\": must be "
                             "vertical or horizontal");

    qpane_->setOrientation(orient);
  }
  else if (name == "-showhandle") {
    tk_->TODO(name);
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppPanedWindow::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  static auto optionNames = std::vector<QString>({
    "add", "cget", "configure", "forget", "identify", "panecget",
    "paneconfigure", "panes", "proxy", "sash"});

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  if      (option == "add") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " add widget ?widget ...?");

    int min_size = -1;
    int padx     = -1;
    int pady     = -1;

    for (uint i = 1; i < numArgs; ++i) {
      auto var  = args[i];
      auto name = tk_->variantToString(var);

      if (name.size() > 0 && name[0] == '-') {
        if      (name == "-minsize") {
          ++i;

          if (i >= numArgs)
            return tk_->throwError("Missing value for \"-minsize\"");

          long i;
          if (! tk_->getOptionInt(name, args[i], i))
            return tk_->invalidInteger(var);

          min_size = i;
        }
        else if (name == "-padx") {
          ++i;

          if (i >= numArgs)
            return tk_->throwError("Missing value for \"-padx\"");

          long pad;
          if (! tk_->getOptionInt(name, args[i], pad))
            return tk_->invalidInteger(var);

          padx = pad;

          tk_->TODO("padx " + QString::number(padx) + " not supported");
        }
        else if (name == "-pady") {
          ++i;

          if (i >= numArgs)
            return tk_->throwError("Missing value for \"-pady\"");

          long pad;
          if (! tk_->getOptionInt(name, args[i], pad))
            return tk_->invalidInteger(var);

          pady = pad;

          tk_->TODO("pady " + QString::number(pady) + " not supported");
        }
      }
      else {
        CTkAppWidget* parent;
        QString       childName;

        if (! root()->decodeWidgetName(args[i], &parent, childName))
          return tk_->throwError("Invalid name '" + name + "'");

        auto *child = parent->getChild(childName);

        if (child == nullptr)
          return tk_->throwError("Invalid name '" + name + "'");

        auto *qwidget = child->getQWidget();

        if (min_size > 0) {
          if (qpane_->orientation() == Qt::Horizontal)
            qwidget->setMinimumHeight(min_size);
          else
            qwidget->setMinimumWidth (min_size);
        }

        qpane_->addWidget(qwidget);
      }
    }
  }
  else if (option == "forget") {
    tk_->TODO(args);
  }
  else if (option == "identify") {
    tk_->TODO(args);
  }
  else if (option == "panecget") {
    tk_->TODO(args);
  }
  else if (option == "paneconfigure") {
    tk_->TODO(args);
  }
  else if (option == "panes") {
    tk_->TODO(args);
  }
  else if (option == "proxy") {
    tk_->TODO(args);
  }
  else if (option == "sash") {
    tk_->TODO(args);
  }
  else
    return CTkAppWidget::execDefaultOp(option, args);

  return true;
}

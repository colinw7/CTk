#include <CTkAppMenu.h>
#include <CTkAppMenuButton.h>

#include <CQUtil.h>

CTkAppMenu::
CTkAppMenu(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qmenu_ = new QMenu(parent ? parent->getQWidget() : nullptr);
  qmenu_->setObjectName("menu");

  setQWidget(qmenu_);

  auto *menuButton = dynamic_cast<CTkAppMenuButton *>(parent);

  if (menuButton)
    menuButton->updateMenu();
}

bool
CTkAppMenu::
execConfig(const QString &name, const QVariant &var)
{
  if (name == "-tearoff") {
    bool b;
    if (! tk_->variantToBool(var, b))
      return tk_->throwError(tk_->msg() + "expected boolean value but got \"" + var + "\"");
    tearOff_ = b;
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppMenu::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  static auto optionNames = std::vector<QString>({
    "activate", "add", "cget", "clone", "configure", "delete", "entrycget",
    "entryconfigure", "id", "index", "insert", "invoke", "post", "postcascade",
    "type", "unpost", "xposition", "yposition"});

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  if      (option == "activate") {
    tk_->TODO(args);
  }
  else if (option == "add") {
    if (numArgs < 2)
      tk_->throwError("Invalid menu add args");

    auto type = tk_->variantToString(args[1]);

    if      (type == "cascade") {
      auto *menu = new CTkAppSubMenu(tk_);

      QString label;
      long    underlinePos = -1;

      for (uint i = 2; i < numArgs - 1; i += 2) {
        auto name = tk_->variantToString(args[i]);
        auto var  = args[i + 1];

        auto value = tk_->variantToString(var);

        if      (name == "-label") {
          label = value;
        }
        else if (name == "-menu") {
          menu->setMenu(value);
        }
        else if (name == "-underline") {
          if (! tk_->getOptionInt(name, value, underlinePos))
            return tk_->invalidInteger(var);
        }
        else
          tk_->throwError("Invalid menu cascade option \"" + name + "\"");
      }

      if (label != "") {
        auto label1 = CTkAppUtil::underlineLabel(label, underlinePos);

        menu->setTitle(label1);
      }

      qmenu_->addMenu(menu);
    }
    else if (type == "check" || type == "checkbutton") {
      auto *check = new CTkAppCheckAction(tk_);

      long underlinePos = -1;

      for (uint i = 2; i < numArgs - 1; i += 2) {
        auto name = tk_->variantToString(args[i]);
        auto var  = args[i + 1];

        auto value = tk_->variantToString(var);

        if      (name == "-label") {
          check->setText(value);
        }
        else if (name == "-command") {
          check->setProperty("CTkApp::Command", value);

          connect(check, SIGNAL(stateChanged(int)), this, SLOT(actionPressedSlot()));
        }
        else if (name == "-variable") {
          check->setVarName(value);
        }
        else if (name == "-underline") {
          if (! tk_->getOptionInt(name, value, underlinePos))
            return tk_->invalidInteger(var);
        }
        else
          tk_->throwError("Invalid menu check option \"" + name + "\"");
      }

      connect(check, SIGNAL(stateChanged(int)), this, SLOT(checkChangedSlot(int)));

      qmenu_->addAction(check);
    }
    else if (type == "command") {
      auto *action = new QAction("");

      long    underlinePos = -1;
      QString accelerator;

      for (uint i = 2; i < numArgs - 1; i += 2) {
        auto name = tk_->variantToString(args[i]);
        auto var  = args[i + 1];

        auto value = tk_->variantToString(var);

        if      (name == "-label") {
          action->setText(value);
        }
        else if (name == "-command") {
          action->setProperty("CTkApp::Command", value);

          connect(action, SIGNAL(triggered()), this, SLOT(actionPressedSlot()));
        }
        else if (name == "-variable") {
          tk_->TODO(name);
        }
        else if (name == "-underline") {
          if (! tk_->getOptionInt(name, value, underlinePos))
            return tk_->invalidInteger(var);
        }
        else if (name == "-accelerator") {
          accelerator = name;
        }
        else
          tk_->throwError("Invalid menu command option \"" + name + "\"");
      }

      qmenu_->addAction(action);
    }
    else if (type == "radio" || type == "radiobutton") {
      auto *radio = new CTkAppRadioAction(tk_);

      for (uint i = 2; i < numArgs - 1; i += 2) {
        auto name = tk_->variantToString(args[i]);
        auto var  = args[i + 1];

        auto value = tk_->variantToString(var);

        if      (name == "-label") {
          radio->setText(value);
        }
        else if (name == "-variable") {
          radio->setVarName(value);
        }
        else if (name == "-value") {
          radio->setValue(value);
        }
        else
          tk_->throwError("Invalid menu radio option \"" + name + "\"");
      }

      connect(radio, SIGNAL(toggled(bool)), this, SLOT(radioToggledSlot(bool)));

      qmenu_->addAction(radio);
    }
    else if (type == "separator") {
      qmenu_->addSeparator();
    }
    else
      tk_->throwError("Invalid menu add type \"" + type + "\"");
  }
  else if (option == "clone") {
    tk_->TODO(args);
  }
  else if (option == "delete") {
    tk_->TODO(args);
  }
  else if (option == "entrycget") {
    tk_->TODO(args);
  }
  else if (option == "entryconfigure") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getFullName() +
               " entryconfigure index ?-option value ...?");

    auto pattern = tk_->variantToString(args[1]);

    uint i = 2;

    QString state;

    for ( ; i < numArgs; ++i) {
      auto opt = tk_->variantToString(args[i]);

      if (opt == "-state") {
        ++i;

        if (i >= numArgs)
          return false;

        state = tk_->variantToString(args[i]);
      }
      else {
        tk_->TODO(getName() + " entryconfigure " + pattern + " " + opt);
      }
    }

    QRegExp regexp(pattern, Qt::CaseSensitive, QRegExp::Wildcard);

    for (auto *action : qmenu_->actions()) {
      if (regexp.exactMatch(action->text())) {
        if (state != "") {
          if      (state == "normal")
            action->setEnabled(true);
          else if (state == "disabled")
            action->setEnabled(false);
          else if (state != "")
            tk_->TODO(getName() + " entryconfigure -state " + state);
        }
      }
    }
  }
  else if (option == "id") {
    tk_->TODO(args);
  }
  else if (option == "index") {
    tk_->TODO(args);
  }
  else if (option == "insert") {
    tk_->TODO(args);
  }
  else if (option == "invoke") {
    tk_->TODO(args);
  }
  else if (option == "post") {
    tk_->TODO(args);
  }
  else if (option == "postcascade") {
    tk_->TODO(args);
  }
  else if (option == "type") {
    tk_->TODO(args);
  }
  else if (option == "unpost") {
    tk_->TODO(args);
  }
  else if (option == "xposition") {
    tk_->TODO(args);
  }
  else if (option == "yposition") {
    tk_->TODO(args);
  }
  else
    return CTkAppWidget::execDefaultOp(option, args);

  return true;
}

void
CTkAppMenu::
actionPressedSlot()
{
  auto *action = qobject_cast<QAction *>(sender());
  if (! action) return;

  auto cmd = action->property("CTkApp::Command").toString();

  if (cmd != "")
    tk_->eval(cmd);
}

void
CTkAppMenu::
checkChangedSlot(int)
{
  auto *action = qobject_cast<CTkAppCheckAction *>(sender());
  if (! action) return;

  action->updateFromVar();
}

void
CTkAppMenu::
radioToggledSlot(bool b)
{
  auto *action = qobject_cast<CTkAppRadioAction *>(sender());
  if (! action) return;

  if (b)
    action->updateFromVar();
}

//---

CTkAppSubMenu::
CTkAppSubMenu(CTkApp *tk) :
 QMenu(nullptr), tk_(tk)
{
  setObjectName("qmenu");

  connectSlots(true);
}

void
CTkAppSubMenu::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, this,
    SIGNAL(aboutToShow()), this, SLOT(showSlot()));
  CQUtil::connectDisconnect(b, this,
    SIGNAL(aboutToHide()), this, SLOT(hideSlot()));
}

void
CTkAppSubMenu::
showSlot()
{
  //std::cerr << "Show " << menu_ << "\n";

  auto *w = tk_->lookupWidgetByName(menu_);
  auto *menu = dynamic_cast<CTkAppMenu *>(w);
  if (! menu) { std::cerr << "No menu '" << menu_.toStdString() << "'\n"; return; }

  auto actions = menu->qmenu()->actions();

  for (auto *action : actions) {
    //std::cerr << "Add Action " << action->text().toStdString() << "\n";

    //menu->qmenu()->removeAction(action);
    //action->setParent(this);

    this->addAction(action);
  }
}

void
CTkAppSubMenu::
hideSlot()
{
  //std::cerr << "Hide " << menu_ << "\n";

  auto *w    = tk_->lookupWidgetByName(menu_);
  auto *menu = dynamic_cast<CTkAppMenu *>(w);
  if (! menu) { std::cerr << "No menu '" << menu_.toStdString() << "'\n"; return; }

  auto actions = this->actions();

  for (auto *action : actions) {
    //std::cerr << "Remove Action " << action->text().toStdString() << "\n";

    //action->setParent(menu->qmenu());

    menu->qmenu()->addAction(action);
  }
}

//---

void
CTkAppCheckAction::
updateFromVar()
{
  if (varName() != "") {
    tk_->setBoolGlobalVar(varName(), check_->isChecked());
  }
}

void
CTkAppRadioAction::
updateFromVar()
{
  if (varName() != "") {
    if (value_ != "")
      tk_->setStringGlobalVar(varName(), value_);
    else
      tk_->setBoolGlobalVar(varName(), radio_->isChecked());
  }
}

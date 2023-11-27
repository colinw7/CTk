#include <CTkAppNoteBook.h>

CTkAppNoteBook::
CTkAppNoteBook(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qtab_ = new CTkAppNoteBookWidget(this);

  setQWidget(qtab_);
}

bool
CTkAppNoteBook::
execConfig(const QString &name, const QVariant &var)
{
  return CTkAppWidget::execConfig(name, var);
}

bool
CTkAppNoteBook::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  //---

  auto stringToIndex = [&](const QVariant &var, int &ind) {
    if (! CTkAppUtil::stringToIndex(tk_, var, ind))
      return false;
    if (ind == CTkAppUtil::END_INDEX)
      ind = qtab_->count() - 1;
    return true;
  };

  //---

  static auto optionNames = std::vector<QString>({
    "add", "cget", "configure", "forget", "hide", "identify", "index", "insert",
    "instate", "select", "state", "style", "tab", "tabs"});

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  if      (option == "add") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " add window ?-option value ...?");

    auto *w = tk_->lookupWidgetByName(args[1], /*quiet*/true);
    if (! w) return false;

    qtab_->addTab(w->getQWidget(), "");
  }
  else if (option == "cget") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " cget option");

    tk_->TODO(args);
  }
  else if (option == "configure") {
    if (numArgs == 1)
      tk_->setStringResult("{-width width Width 0 0} "
                           "{-height height Height 0 0} "
                           "{-padding padding Padding {} {}} "
                           "{-takefocus takeFocus TakeFocus ttk::takefocus ttk::takefocus} "
                           "{-cursor cursor Cursor {} {}} "
                           "{-style style Style {} {}} "
                           "{-class {} {} {} {}}");
    else
      tk_->TODO(args);
  }
  else if (option == "forget") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " forget tab");

    tk_->TODO(args);
  }
  else if (option == "hide") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " hide tab");

    tk_->TODO(args);
  }
  else if (option == "identify") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " identify ?what? x y");

    tk_->TODO(args);
  }
  else if (option == "index") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " index tab");

    tk_->TODO(args);
  }
  else if (option == "insert") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " insert index slave ?-option value ...?");

    int ind;
    if (! stringToIndex(args[1], ind))
      return false;

    tk_->TODO(args);
  }
  else if (option == "instate") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " instate state-spec ?script?");

    tk_->TODO(args);
  }
  else if (option == "select") {
    tk_->TODO(args);
  }
  else if (option == "state") {
    tk_->TODO(args);
  }
  else if (option == "style") {
    tk_->TODO(args);
  }
  else if (option == "tab") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " tab tab ?-option ?value??...");

    tk_->TODO(args);
  }
  else if (option == "tabs") {
    tk_->TODO(args);
  }
  else
    return CTkAppWidget::execDefaultOp(option, args);

  return true;
}

//---

CTkAppNoteBookWidget::
CTkAppNoteBookWidget(CTkAppNoteBook *notebook) :
 QTabWidget(notebook->getParent() ? notebook->getParent()->getQWidget() : nullptr),
 notebook_(notebook)
{
  setObjectName("notebook");
}

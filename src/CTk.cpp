#include <CTk.h>
#include <CTkWidget.h>
#include <CTkCommands.h>
#include <CTkLayout.h>
#include <CTkImage.h>

#include <CStrUtil.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QKeyEvent>

CTk::
CTk(CTcl *tcl) :
 tcl_(tcl)
{
  root_ = new CTkRootWidget(this);

  CTkCommands::addCommands(this);
}

void
CTk::
exec()
{
  if (toplevels_.empty())
    root_->show();
  else {
    uint num = toplevels_.size();

    for (uint i = 0; i < num; ++i)
      toplevels_[i]->show();
  }
}

void
CTk::
processEvents()
{
  //while (QCoreApplication::hasPendingEvents())
  //  QCoreApplication::processEvents();
  QCoreApplication::processEvents();

  purgeWidgets();
}

//---

CTkWidget *
CTk::
lookupWidgetByName(const std::string &widgetName) const
{
  if (widgetName == ".")
    return getRootWidget();

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root_->decodeWidgetName(widgetName, &parent, childName)) {
    throwError("Invalid name '" + widgetName + "'");
    return nullptr;
  }

  auto *child = parent->getChild(childName);

  if (child == nullptr) {
    throwError("bad window path name \"" + childName + "\"");
    return nullptr;
  }

  return child;
}

//---

bool
CTk::
processOption(CTkOption *opts, const std::vector<CTclValueRef> &args, uint &i,
              CTkOptionValueMap &values)
{
  uint numArgs = args.size();

  const auto &arg = args[i]->toString();

  for (uint j = 0; opts[j].name != nullptr; ++j) {
    const auto &option = opts[j];

    if (arg != option.name && (option.alias == nullptr || arg != option.alias))
      continue;

    auto &value = values[arg];

    if      (option.type == CTkOptionType::Flag)
      value.i = 1;
    else if (option.type == CTkOptionType::String) {
      if (i < numArgs - 1)
        value.s = args[++i]->toString();
      else
        return false;
    }
    else if (option.type == CTkOptionType::Int) {
      if (i < numArgs - 1) {
        if (! args[++i]->toInt(value.i))
          return false;
      }
      else
        return false;
    }
    else if (option.type == CTkOptionType::Real) {
      if (i < numArgs - 1) {
        if (! args[++i]->toReal(value.r))
          return false;
      }
      else
        return false;
    }

    return true;
  }

  return false;
}

//---

std::string
CTk::
getNewImageName() const
{
  static uint id = 1;

  std::string name = CStrUtil::strprintf("image%u", id);

  while (images_.find(name) != images_.end()) {
    ++id;

    name = CStrUtil::strprintf("image%u", id);
  }

  ++id;

  return name;
}

CTkImageRef
CTk::
createImage(const std::string &, const std::string &name)
{
  auto image = std::make_shared<CTkImage>(name);

  images_[name] = image;

  return image;
}

CTkImageRef
CTk::
getImage(const std::string &name) const
{
  auto p = images_.find(name);

  if (p == images_.end())
    return CTkImageRef();

  return (*p).second;
}

//---

void
CTk::
bindEvent(const std::string &tagName, const std::string &pattern, const std::string &command)
{
  events_[tagName][pattern] = command;
}

bool
CTk::
triggerEnterEvents(const char *className, CTkWidget *w, QEvent *e)
{
  auto p1 = events_.find(className);

  if (p1 != events_.end()) {
    auto p2 = (*p1).second.find("<Enter>");

    if (p2 != (*p1).second.end())
      return execEvent(w, e, (*p2).second);
  }

  return false;
}

bool
CTk::
triggerLeaveEvents(const char *className, CTkWidget *w, QEvent *e)
{
  auto p1 = events_.find(className);

  if (p1 != events_.end()) {
    auto p2 = (*p1).second.find("<Leave>");

    if (p2 != (*p1).second.end())
      return execEvent(w, e, (*p2).second);
  }

  return false;
}

bool
CTk::
triggerKeyPressEvents(const char *className, CTkWidget *w, QEvent *e, const std::string &str)
{
  auto p1 = events_.find(className);

  if (p1 != events_.end()) {
    auto p2 = (*p1).second.find(str);

    if (p2 != (*p1).second.end())
      return execEvent(w, e, (*p2).second);
  }

  p1 = events_.find("all");

  if (p1 != events_.end()) {
    auto p2 = (*p1).second.find(str);

    if (p2 != (*p1).second.end())
      return execEvent(w, e, (*p2).second);
  }

  return false;
}

bool
CTk::
execEvent(CTkWidget *w, QEvent *e, const std::string &str)
{
  auto str1 = str;

  auto pos1 = str1.find("%W");

  if (pos1 != std::string::npos)
    str1 = str1.substr(0, pos1) + w->getFullName() + str1.substr(pos1 + 2);

  auto type = e->type();

  if (type == QEvent::MouseButtonPress) {
    auto *me = dynamic_cast<QMouseEvent *>(e);

    auto pos1 = str1.find("%x");

    if (pos1 != std::string::npos)
      str1 = str1.substr(0, pos1) + CStrUtil::toString(me->x()) + str1.substr(pos1 + 2);

    auto pos2 = str1.find("%y");

    if (pos2 != std::string::npos)
      str1 = str1.substr(0, pos2) + CStrUtil::toString(me->y()) + str1.substr(pos2 + 2);

    std::string::size_type pos3 = str1.find("%b");

    if (pos3 != std::string::npos)
      str1 = str1.substr(0, pos3) + CStrUtil::toString(me->button()) + str1.substr(pos3 + 2);
  }

  getTcl()->parseLine(str1);

  return true;
}

//---

void
CTk::
addTopLevel(CTkTopLevel *toplevel)
{
  toplevels_.push_back(toplevel);
}

void
CTk::
addWidget(CTkWidget *w)
{
  widgets_.insert(w);
}

CTkWidget *
CTk::
lookupWidget(QWidget *w) const
{
  for (auto *tw : widgets_) {
    if (tw->getQWidget() == w)
      return tw;
  }

  return nullptr;
}

void
CTk::
removeWidget(CTkWidget *w)
{
  auto p = widgets_.find(w);

  if (p != widgets_.end())
    widgets_.erase(p);
}

void
CTk::
addDeleteWidget(CTkWidget *w)
{
  deleteWidgets_.push_back(w);
}

void
CTk::
purgeWidgets()
{
  auto num = deleteWidgets_.size();

  for (uint i = 0; i < num; ++i)
    delete deleteWidgets_[i];

  deleteWidgets_.clear();
}

//---

CTclValueRef
CTk::
wrongNumArgs(const std::string &msg) const
{
  getTcl()->wrongNumArgs(msg);

  return CTclValueRef();
}

CTclValueRef
CTk::
throwError(const std::string &msg) const
{
  getTcl()->throwError(msg);

  return CTclValueRef();
}

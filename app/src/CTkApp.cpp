#include <CTkApp.h>
#include <CTkAppWidget.h>
#include <CTkAppImage.h>
#include <CTkAppCommands.h>

#include <CImageLib.h>

#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>

#include <tcl.h>

//---

static int Tk_Init(Tcl_Interp *) { return TCL_OK; }
static int Tk_SafeInit(Tcl_Interp *) { return TCL_OK; }

//---

CTkApp::
CTkApp(int argc, char **argv) :
 CTclApp(argc, argv)
{
  root_ = new CTkAppRoot(this);

  initInterp();

  Tcl_StaticPackage(getInterp(), "Tk", Tk_Init, Tk_SafeInit);

  Tcl_PkgProvide(getInterp(), "Tk", "1.0");

  setInteractive(false);

  init();
}

std::string
CTkApp::
getTclStr()
{
  return "";
}

void
CTkApp::
addCommands()
{
  CTkAppCommands::addCommands(this);
}

//---

void
CTkApp::
processEvents()
{
  QCoreApplication::processEvents();

  purgeWidgets();
}

void
CTkApp::
show()
{
  if (toplevels_.empty())
    root_->show();
  else {
    uint num = toplevels_.size();

    for (uint i = 0; i < num; ++i)
      toplevels_[i]->show();
  }
}

//---

CTkWidget *
CTkApp::
lookupWidgetByName(const std::string &widgetName) const
{
  if (widgetName == ".")
    return root();

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
CTkApp::
processOption(CTkOption *opts, const std::vector<std::string> &args, uint &i,
              CTkOptionValueMap &values)
{
  uint numArgs = args.size();

  const auto &arg = args[i];

  for (uint j = 0; opts[j].name != nullptr; ++j) {
    const auto &option = opts[j];

    if (arg != option.name && (option.alias == nullptr || arg != option.alias))
      continue;

    auto &value = values[arg];

    if      (option.type == CTkOptionType::Flag)
      value.i = 1;
    else if (option.type == CTkOptionType::String) {
      if (i < numArgs - 1)
        value.s = args[++i];
      else
        return false;
    }
    else if (option.type == CTkOptionType::Int) {
      if (i < numArgs - 1) {
        if (! CTkAppUtil::stringToInt(args[++i], value.i))
          return false;
      }
      else
        return false;
    }
    else if (option.type == CTkOptionType::Real) {
      if (i < numArgs - 1) {
        if (! CTkAppUtil::stringToReal(args[++i], value.r))
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
CTkApp::
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
CTkApp::
createImage(const std::string &, const std::string &name)
{
  auto image = std::make_shared<CTkImage>(name);

  images_[name] = image;

  return image;
}

CTkImageRef
CTkApp::
getImage(const std::string &name) const
{
  auto p = images_.find(name);

  if (p == images_.end())
    return CTkImageRef();

  return (*p).second;
}

CImagePtr
CTkApp::
loadImage(const std::string &name) const
{
  CImageFileSrc src(name);

  return CImageMgrInst->createImage(src);
}

//---

void
CTkApp::
bindEvent(const std::string &tagName, const std::string &pattern, const std::string &command)
{
  events_[tagName][pattern] = command;
}

bool
CTkApp::
triggerEnterEvents(const std::string &className, CTkWidget *w, QEvent *e)
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
CTkApp::
triggerLeaveEvents(const std::string &className, CTkWidget *w, QEvent *e)
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
CTkApp::
triggerKeyPressEvents(const std::string &className, CTkWidget *w, QEvent *e, const std::string &str)
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
CTkApp::
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

  eval(str1);

  return true;
}

//---

void
CTkApp::
addTopLevel(CTkTopLevel *toplevel)
{
  toplevels_.push_back(toplevel);
}

void
CTkApp::
addWidget(CTkWidget *w)
{
  widgets_.insert(w);
}

CTkWidget *
CTkApp::
lookupWidget(QWidget *w) const
{
  for (auto *tw : widgets_) {
    if (tw->getQWidget() == w)
      return tw;
  }

  return nullptr;
}

void
CTkApp::
removeWidget(CTkWidget *w)
{
  auto p = widgets_.find(w);

  if (p != widgets_.end())
    widgets_.erase(p);
}

void
CTkApp::
addDeleteWidget(CTkWidget *w)
{
  deleteWidgets_.push_back(w);
}

void
CTkApp::
purgeWidgets()
{
  auto num = deleteWidgets_.size();

  for (uint i = 0; i < num; ++i)
    delete deleteWidgets_[i];

  deleteWidgets_.clear();
}

//---

bool
CTkApp::
wrongNumArgs(const std::string &msg) const
{
  std::cerr << "wrong # args: should be \"" << msg << "\"\n";

  return false;
}

bool
CTkApp::
throwError(const std::string &msg) const
{
  std::cerr << msg << "\n";

  return false;
}

bool
CTkApp::
TODO(const std::string &msg) const
{
  std::cerr << "TODO: " << msg << "\n";

  return false;
}

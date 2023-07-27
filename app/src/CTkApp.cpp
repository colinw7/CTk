#include <CTkApp.h>
#include <CTkAppWidget.h>
#include <CTkAppImage.h>
#include <CTkAppCommands.h>
#include <CTkAppEventData.h>
#include <CTkAppUtil.h>

#include <CImageLib.h>
#include <CStrParse.h>
#include <CStrUniqueMatch.h>
#include <CGlob.h>

#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>

#include <tcl.h>

//---

static int Tk_Init(Tcl_Interp *) { return TCL_OK; }
static int Tk_SafeInit(Tcl_Interp *) { return TCL_OK; }

//---

CTkApp::
CTkApp(Tcl_Interp *interp) :
 CTclApp(interp)
{
  static const char *argv[2] = { "CTkApp", nullptr };

  construct(1, argv);
}

CTkApp::
CTkApp(int argc, const char **argv) :
 CTclApp(argc, argv)
{
  construct(argc, argv);
}

void
CTkApp::
construct(int argc, const char **argv)
{
  root_ = new CTkAppRoot(this);

  tclInit();

  Tcl_StaticPackage(getInterp(), "Tk", Tk_Init, Tk_SafeInit);

  Tcl_PkgProvide(getInterp(), "Tk", "8.0");

  setInteractive(false);

  init();

  std::vector<std::string> args;

  for (int i = 1; i < argc; ++i)
    args.push_back(std::string(argv[i]));

  setStringArrayVar("argv", args);
  setIntegerVar    ("argc", args.size());

  setStringVar("tk_version", "8.6");
  setStringVar("tk_library", "/usr/share/tcltk/tk8.6");
  setStringVar("tk_patchLevel", "8.6.12");
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

  addWidgetClass("Root");
  addWidgetClass("Button");
  addWidgetClass("Canvas");
  addWidgetClass("CheckButton");
  addWidgetClass("Entry");
  addWidgetClass("Frame");
  addWidgetClass("Label");
  addWidgetClass("LabelFrame");
  addWidgetClass("ListBox");
  addWidgetClass("Menu");
  addWidgetClass("MenuButton");
  addWidgetClass("Message");
  addWidgetClass("PanedWindow");
  addWidgetClass("RadioButton");
  addWidgetClass("Scale");
  addWidgetClass("ScrollBar");
  addWidgetClass("SpinBox");
  addWidgetClass("Text");
  addWidgetClass("TopLevel");
}

//---

void
CTkApp::
addWidgetClass(const std::string &name)
{
  widgetClasses_.insert(name);
}

//---

void
CTkApp::
setRootFrame(QFrame *frame)
{
  root_->setRoot(frame);
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

CTkAppWidget *
CTkApp::
lookupWidgetByName(const std::string &widgetName, bool quiet) const
{
  if (widgetName == ".")
    return root();

  CTkAppWidget* parent = nullptr;
  std::string   childName;

  if (! root_->decodeWidgetName(widgetName, &parent, childName)) {
    if (! quiet) throwError("Invalid name '" + widgetName + "'");
    return nullptr;
  }

  auto *child = parent->getChild(childName);

  if (child == nullptr) {
    if (! quiet) throwError("bad window path name \"" + childName + "\"");
    return nullptr;
  }

  return child;
}

//---

bool
CTkApp::
processOption(CTkAppOption *opts, const std::vector<std::string> &args, uint &i,
              CTkAppOptionValueMap &values)
{
  uint numArgs = args.size();

  const auto &arg = args[i];

  for (uint j = 0; opts[j].name != nullptr; ++j) {
    const auto &option = opts[j];

    if (arg != option.name && (option.alias == nullptr || arg != option.alias))
      continue;

    auto &value = values[arg];

    if      (option.type == CTkAppOptionType::Flag)
      value.i = 1;
    else if (option.type == CTkAppOptionType::String) {
      if (i < numArgs - 1)
        value.s = args[++i];
      else
        return false;
    }
    else if (option.type == CTkAppOptionType::Int) {
      if (i < numArgs - 1) {
        if (! CTkAppUtil::stringToInt(args[++i], value.i))
          return false;
      }
      else
        return false;
    }
    else if (option.type == CTkAppOptionType::Real) {
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

CTkAppImageRef
CTkApp::
createImage(const std::string &, const std::string & /*format*/, const std::string &name)
{
  auto image = std::make_shared<CTkAppImage>(name);

  images_[name] = image;

  return image;
}

CTkAppImageRef
CTkApp::
getImage(const std::string &name) const
{
  auto p = images_.find(name);

  if (p == images_.end())
    return CTkAppImageRef();

  return (*p).second;
}

CImagePtr
CTkApp::
getBitmap(const std::string &name) const
{
  auto name1 = name;

  if (name[0] == '@') {
    auto *env = getenv("CTK_APP_IMAGE_DIR");
    if (! env)
      (void) throwError("set CTK_APP_IMAGE_DIR to images path for @ name");

    if (env)
      name1 = std::string(env) + name.substr(1);
  }
  else {
    auto *env = getenv("CTK_APP_BITMAP_DIR");
    if (! env)
      (void) throwError("set CTK_APP_BITMAP_DIR to bitmap path for name");

    if (env)
      name1 = std::string(env) + "/" + name + ".xbm";
  }

  CImageFileSrc src(name1);

  return CImageMgrInst->createImage(src);
}

//---

void *
CTkApp::
getWidgetClassData(const std::string &name) const
{
  auto p = widgetClasses_.find(name);

  if (p == widgetClasses_.end())
    return nullptr;

  return const_cast<CTkApp *>(this);
}

//---

bool
CTkApp::
bindTagEvent(const std::string &name, const CTkAppEventData &data)
{
  tagEvents_[name].push_back(data);

  return true;
}

bool
CTkApp::
bindClassEvent(const std::string &name, const CTkAppEventData &data)
{
  classEvents_[name].push_back(data);

  return true;
}

bool
CTkApp::
bindAllEvent(const CTkAppEventData &data)
{
  allEvents_.push_back(data);

  return true;
}

bool
CTkApp::
triggerEvents(const std::string &className, CTkAppWidget *w, QEvent *e,
              const CTkAppEventData &matchEventData)
{
  auto p1 = classEvents_.find(className);

  if (p1 != classEvents_.end()) {
    for (const auto &eventData : (*p1).second) {
      if (eventData == matchEventData)
        execEvent(w, e, matchEventData, eventData.command);
    }
  }

  for (const auto &eventData : allEvents_) {
    if (eventData == matchEventData)
      execEvent(w, e, matchEventData, eventData.command);
  }

  return true;
}

bool
CTkApp::
triggerKeyPressEvents(const std::string &className, CTkAppWidget *w, QEvent *e)
{
  auto *ke = dynamic_cast<QKeyEvent *>(e);

  CTkAppEventData matchEventData;

  encodeEvent(ke, /*press*/true, matchEventData);

  //---

  auto p1 = classEvents_.find(className);

  if (p1 != classEvents_.end()) {
    for (const auto &eventData : (*p1).second) {
      if (eventData == matchEventData)
        execEvent(w, e, matchEventData, eventData.command);
    }
  }

  for (const auto &eventData : allEvents_) {
    if (eventData == matchEventData)
      execEvent(w, e, matchEventData, eventData.command);
  }

  return true;
}

bool
CTkApp::
execEvent(CTkAppWidget *w, QEvent *e, const CTkAppEventData &data, const std::string &command)
{
  auto *me = dynamic_cast<QMouseEvent *>(e);

  std::string command1;

  CStrParse parse(command);

  while (! parse.eof()) {
    if (parse.isChar('%')) {
      parse.skipChar();

      if (! parse.eof()) {
        auto c = parse.readChar();

        switch (c) {
          case '%':
            command1 += '%';
            break;
          case 'b': // button
            command1 += CStrUtil::toString(data.button);
            break;
          case 'd': // detail
            command1 += ""; // TODO
            break;
          case 'h': // height
            command1 += CStrUtil::toString(w->qwidget()->height());
            break;
          case 't': // time
            command1 += ""; // TODO
            break;
          case 'w': // width
            command1 += CStrUtil::toString(w->qwidget()->width());
            break;
          case 'x': // x
            command1 += (me ? CStrUtil::toString(me->x()) : "0");
            break;
          case 'y': // y
            command1 += (me ? CStrUtil::toString(me->y()) : "0");
            break;
          case 'A': // key (unicode)
            command1 += data.key;
            break;
          case 'D': // delta
            command1 += ""; // TODO
            break;
          case 'K': // key (keysym)
            command1 += data.key;
            break;
          case 'W': // widget name
            command1 += w->getFullName();
            break;
          case 'X': // root x
            command1 += (me ? CStrUtil::toString(w->qwidget()->mapToGlobal(me->pos()).x()) : "0");
            break;
          case 'Y': // root y
            command1 += (me ? CStrUtil::toString(w->qwidget()->mapToGlobal(me->pos()).y()) : "0");
            break;
          default:
            command1 += '%';
            command1 += c;
            break;
        }
      }
      else
        command1 += '%';
    }
    else
      command1 += parse.readChar();
  }

  //std::cerr << "Exec: " << command1 << "\n";

  eval(command1);

  w->qwidget()->update();

  return true;
}

//---

CTkAppTopLevel *
CTkApp::
installToplevel(const std::string &id, QFrame *frame)
{
  static CTkAppOpt opts[] = {
    { nullptr, nullptr, nullptr, nullptr }
  };

  auto widgetName = "." + id;

  auto *w = lookupWidgetByName(widgetName, /*quiet*/true);
  if (w) { throwError("widget already exists"); return nullptr; }

  auto *toplevel = new CTkAppTopLevel(this, root(), id);

  toplevel->setFrame(frame);

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, toplevel, opts);
  assert(cmd);

  addTopLevel(toplevel);

  return toplevel;
}

void
CTkApp::
addTopLevel(CTkAppTopLevel *toplevel)
{
  toplevels_.push_back(toplevel);
}

void
CTkApp::
addWidget(CTkAppWidget *w)
{
  widgets_.insert(w);
}

CTkAppWidget *
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
removeWidget(CTkAppWidget *w)
{
  auto p = widgets_.find(w);

  if (p != widgets_.end())
    widgets_.erase(p);
}

void
CTkApp::
addDeleteWidget(CTkAppWidget *w)
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

void
CTkApp::
encodeEvent(QKeyEvent *e, bool press, CTkAppEventData &data) const
{
  data.type = CTkAppEventType::Key;
  data.mode = (press ? CTkAppEventMode::Press : CTkAppEventMode::Release);

  int key = e->key();

  if      (key >= Qt::Key_A && key <= Qt::Key_Z)
    data.key = char('a' + (key - Qt::Key_A));
  else if (key >= Qt::Key_1 && key <= Qt::Key_9)
    data.key = char('0' + (key - Qt::Key_0));
  else
    data.key  = e->text().toStdString();

  if (e->modifiers() & Qt::ControlModifier)
    data.modifiers |= uint(CTkAppEventModifier::Control);

  if (e->modifiers() & Qt::ShiftModifier)
    data.modifiers |= uint(CTkAppEventModifier::Shift);

  if (e->modifiers() & Qt::AltModifier)
    data.modifiers |= uint(CTkAppEventModifier::Alt);
}

void
CTkApp::
encodeEvent(QMouseEvent *e, CTkAppEventMode mode, int button, CTkAppEventData &data) const
{
  data.type   = CTkAppEventType::Button;
  data.mode   = mode;
  data.button = button;

  if (e->modifiers() & Qt::ControlModifier)
    data.modifiers |= uint(CTkAppEventModifier::Control);

  if (e->modifiers() & Qt::ShiftModifier)
    data.modifiers |= uint(CTkAppEventModifier::Shift);

  if (e->modifiers() & Qt::AltModifier)
    data.modifiers |= uint(CTkAppEventModifier::Alt);
}

bool
CTkApp::
parseEvent(const std::string &pattern, CTkAppEventData &data)
{
  // See xmodmap ?

  /*
  Modifier Types
  --------------
  Control, Shift, Alt, Lock, Command
  Meta, M
  Mod1, M1
  Mod2, M2
  Mod3, M3
  Mod4, M4
  Mod5, M5
  Button1, B1
  Button2, B2
  Button3, B3
  Button4, B4
  Button5, B5
  Double, Triple, Quadruple
  Any
  */

  /*
  Event Types
  -----------
  Activate, Deactivate
  Button, ButtonPress, ButtonRelease, MouseWheel, Motion
  Circulate, CirculateRequest
  Colormap
  Configure, ConfigureRequest
  Create, Destroy,
  Expose
  FocusIn, FocusOut
  Gravity
  Key, KeyPress, KeyRelease
  Enter, Leave
  Property
  Reparent
  ResizeRequest
  MapRequest, Map, Unmap
  Visibility
  */

  data.pattern = pattern;

  CStrParse parse(pattern);

  auto parseClose = [&](char c) {
    parse.skipSpace();

    if (! parse.isChar(c))
      return false;

    parse.skipChar();

    parse.skipSpace();

    if (! parse.eof())
      return false;

    return true;
  };

  parse.skipSpace();

  if (parse.isChar('<')) {
    parse.skipChar();

    parse.skipSpace();

    if      (parse.isString("<Help>")) {
      parse.skipLastString();

      data.type = CTkAppEventType::Help;

      if (! parseClose('>'))
        return false;

      return true;
    }
    else if (parse.isString("<MenuSelect>")) {
      parse.skipLastString();

      data.type = CTkAppEventType::MenuSelect;

      if (! parseClose('>'))
        return false;

      return true;
    }
    else if (parse.isString("Expose")) {
      parse.skipLastString();

      data.type = CTkAppEventType::Expose;

      if (! parseClose('>'))
        return false;

      return true;
    }

    data.anyModifier = true;

    while (true) {
      if      (parse.isString("Control")) {
        parse.skipLastString();

        data.modifiers |= uint(CTkAppEventModifier::Control);
        data.anyModifier = false;
      }
      else if (parse.isString("Shift")) {
        parse.skipLastString();

        data.modifiers |= uint(CTkAppEventModifier::Shift);
        data.anyModifier = false;
      }
      else if (parse.isString("Alt")) {
        parse.skipLastString();

        data.modifiers |= uint(CTkAppEventModifier::Shift);
        data.anyModifier = false;
      }
      else
        break;

      if (! parse.isChar('-'))
        return false;

      parse.skipChar();
    }

    data.clicks = 1;

    while (true) {
      if      (parse.isString("Double")) {
        parse.skipLastString();

        data.clicks = 2;
      }
      else if (parse.isString("Triple")) {
        parse.skipLastString();

        data.clicks = 3;
      }
      else if (parse.isString("Quadruple")) {
        parse.skipLastString();

        data.clicks = 4;
      }
      else
        break;

      if (! parse.isChar('-'))
        return false;

      parse.skipChar();
    }

    if      (parse.isString("Button")) {
      parse.skipLastString();

      data.type   = CTkAppEventType::Button;
      data.button = 1;

      if (parse.isDigit()) {
        if (! parse.readInteger(&data.button))
          return false;

        if (parse.isChar('-')) {
          parse.skipChar();

          if      (parse.isString("Press")) {
            parse.skipLastString();

            data.mode = CTkAppEventMode::Press;
          }
          else if (parse.isString("Motion")) {
            parse.skipLastString();

            data.mode = CTkAppEventMode::Motion;
          }
          else if (parse.isString("Release")) {
            parse.skipLastString();

            data.mode = CTkAppEventMode::Release;
          }
          else
            data.mode = CTkAppEventMode::Press;
        }
      }
      else {
        if      (parse.isString("Press")) {
          parse.skipLastString();

          data.mode = CTkAppEventMode::Press;
        }
        else if (parse.isString("Motion")) {
          parse.skipLastString();

          data.mode = CTkAppEventMode::Motion;
        }
        else if (parse.isString("Release")) {
          parse.skipLastString();

          data.mode = CTkAppEventMode::Release;
        }
        else
          data.mode = CTkAppEventMode::Press;

        if (parse.isChar('-')) {
          parse.skipChar();

          if (parse.isDigit()) {
            if (! parse.readInteger(&data.button))
              return false;
          }
          else
            return false;
        }
      }

      if (! parseClose('>'))
        return false;
    }
    else if (parse.isString("B1") || parse.isString("B2") || parse.isString("B3") ||
             parse.isString("B4") || parse.isString("B5")) {
      data.type = CTkAppEventType::Button;
      data.mode = CTkAppEventMode::Press;

      parse.skipChar();

      if (! parse.isDigit())
        return false;

      if (! parse.readInteger(&data.button))
        return false;

      if (parse.isChar('-')) {
        parse.skipChar();

        if      (parse.isString("Press")) {
          parse.skipLastString();

          data.mode = CTkAppEventMode::Press;
        }
        else if (parse.isString("Motion")) {
          parse.skipLastString();

          data.mode = CTkAppEventMode::Motion;
        }
        else if (parse.isString("Release")) {
          parse.skipLastString();

          data.mode = CTkAppEventMode::Release;
        }
        else
          return false;
      }

      if (! parseClose('>'))
        return false;
    }
    else if (parse.isString("Key")) {
      parse.skipLastString();

      data.type = CTkAppEventType::Key;

      if      (parse.isString("Press")) {
        parse.skipLastString();

        data.mode = CTkAppEventMode::Press;
      }
      else if (parse.isString("Release")) {
        parse.skipLastString();

        data.mode = CTkAppEventMode::Release;
      }
      else
        data.mode = CTkAppEventMode::Press;

      if (parse.isChar('-')) {
        parse.skipChar();

        while (! parse.eof() && ! parse.isChar('>'))
          data.key += parse.readChar();

        if (! parseClose('>'))
          return false;
      }
      else {
        data.anyKey = true;
        data.key    = ""; // Any ?
      }
    }
    else if (parse.isString("Enter")) {
      parse.skipLastString();

      data.type = CTkAppEventType::Enter;

      if (! parseClose('>'))
        return false;
    }
    else if (parse.isString("Leave")) {
      parse.skipLastString();

      data.type = CTkAppEventType::Leave;

      if (! parseClose('>'))
        return false;
    }
    else if (parse.isDigit()) {
      data.type = CTkAppEventType::Button;
      data.mode = CTkAppEventMode::Press;

      if (! parse.readInteger(&data.button))
        return false;

      if (! parseClose('>'))
        return false;
    }
    else {
      data.type = CTkAppEventType::Key;
      data.mode = CTkAppEventMode::Press;

      while (! parse.eof() && ! parse.isChar('>'))
        data.key += parse.readChar();

      if (! parseClose('>'))
        return false;
    }
  }

  return true;
}

//---

void
CTkApp::
addOption(const std::string &pattern, const std::string &value, int priority)
{
  //std::cerr << "addOption: " << pattern << " " << value << " " << priority << "\n";

  options_.emplace_back(pattern, value, priority);
}

bool
CTkApp::
matchOption(const std::string &widgetClass, const std::string &optName,
            const std::string &optClass, std::string &optValue) const
{
  optValue = "";
  //std::cerr << "matchOption: " << widgetClass << " " << optClass << " " << optName << "\n";

  auto pattern1 = widgetClass + "." + optName;

  for (const auto &opt : options_) {
    //std::cerr << "compare: " << pattern1 << " " << opt.pattern << "\n";

    CGlob glob(opt.pattern);

    if (glob.compare(pattern1)) {
      //std::cerr << "match: " << opt.pattern << " = " << opt.value << "\n";
      optValue = opt.value;
      return true;
    }
  }

  auto pattern2 = widgetClass + "." + optClass;

  for (const auto &opt : options_) {
    //std::cerr << "compare: " << pattern2 << " " << opt.pattern << "\n";

    CGlob glob(opt.pattern);

    if (glob.compare(pattern2)) {
      //std::cerr << "match: " << opt.pattern << " = " << opt.value << "\n";
      optValue = opt.value;
      return true;
    }
  }

  return true;
}

void
CTkApp::
clearOptions()
{
  options_.clear();
}

//---

bool
CTkApp::
lookupOptionName(const std::vector<std::string> &names,
                 const std::string &arg, std::string &opt) const
{
  return lookupName("option", names, arg, opt);
}

bool
CTkApp::
lookupName(const std::string &msg, const std::vector<std::string> &names,
           const std::string &arg, std::string &opt) const
{
  auto concatOptionNames = [&]() {
    std::string str;

    auto n = names.size();

    for (uint i = 0; i < n; ++i) {
      if (i > 0) {
        if (i == n - 1)
          str += " or ";
        else
          str += ", ";
      }

      str += names[i];
    }

    return str;
  };

  CStrUniqueMatchInds optionValues(names);

  int argNum = -1;

  if (! optionValues.match(arg, argNum))
    return throwError("bad " + msg + " \"" + arg + "\": must be " + concatOptionNames());

  opt = names[argNum];

  return true;
}

bool
CTkApp::
getOptionInt(const std::string &name, const std::string &value, long &i) const
{
  if (! CStrUtil::toInteger(value, &i)) {
    throwError("Invalid value \"" + value + "\" for \"" + name + "\"");
    return false;
  }

  return true;
}

bool
CTkApp::
getOptionReal(const std::string &name, const std::string &value, double &r) const
{
  if (! CStrUtil::toReal(value, &r)) {
    throwError("Invalid value for \"" + name + "\"");
    return false;
  }

  return true;
}

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
  std::cerr << "TODO: " << currentCommand() << " " << msg << "\n";

  return false;
}

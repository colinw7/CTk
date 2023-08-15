#include <CTkApp.h>
#include <CTkAppWidget.h>
#include <CTkAppImage.h>
#include <CTkAppFont.h>
#include <CTkAppCommands.h>
#include <CTkAppEventData.h>
#include <CTkAppUtil.h>

#include <CStrParse.h>
#include <CStrUniqueMatch.h>
#include <CGlob.h>

#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFontDialog>
#include <QTimer>

#include <tcl.h>

//---

namespace {

static CTkApp *s_app;

struct VirtualEventData {
  std::string            name;
  CTkAppVirtualEventType type { CTkAppVirtualEventType::None };
};

static VirtualEventData virtualEventData[] = {
  { "<<AltUnderlined>>"  , CTkAppVirtualEventType::AltUnderlined   },
  { "<<Invoke>>"         , CTkAppVirtualEventType::Invoke          },
  { "<<Modified>>"       , CTkAppVirtualEventType::Modified        },
  { "<<Selection>>"      , CTkAppVirtualEventType::Selection       },
  { "<<ThemeChanged>>"   , CTkAppVirtualEventType::ThemeChanged    },
  { "<<TkWorldChanged>>" , CTkAppVirtualEventType::TkWorldChanged  },
  { "<<TraverseIn>>"     , CTkAppVirtualEventType::TraverseIn      },
  { "<<TraverseOut>>"    , CTkAppVirtualEventType::TraverseOut     },
  { "<<UndoStack>>"      , CTkAppVirtualEventType::UndoStack       },
  { "<<WidgetViewSync>>" , CTkAppVirtualEventType::WidgetViewSync  },

  { "<<ListboxSelect>>"  , CTkAppVirtualEventType::ListboxSelect   },
  { "<<MenuSelect>>"     , CTkAppVirtualEventType::MenuSelect      },
  { "<<TreeviewSelect>>" , CTkAppVirtualEventType::TreeviewSelect  },

  { "<<Clear>>"          , CTkAppVirtualEventType::Clear           },
  { "<<Copy>>"           , CTkAppVirtualEventType::Copy            },
  { "<<Cut>>"            , CTkAppVirtualEventType::Cut             },
  { "<<LineEnd>>"        , CTkAppVirtualEventType::LineEnd         },
  { "<<LineStart>>"      , CTkAppVirtualEventType::LineStart       },
  { "<<NextChar>>"       , CTkAppVirtualEventType::NextChar        },
  { "<<NextLine>>"       , CTkAppVirtualEventType::NextLine        },
  { "<<NextPara>>"       , CTkAppVirtualEventType::NextPara        },
  { "<<NextWord>>"       , CTkAppVirtualEventType::NextWord        },
  { "<<Paste>>"          , CTkAppVirtualEventType::Paste           },
  { "<<PasteSelection>>" , CTkAppVirtualEventType::PasteSelection  },
  { "<<PrevChar>>"       , CTkAppVirtualEventType::PrevChar        },
  { "<<PrevLine>>"       , CTkAppVirtualEventType::PrevLine        },
  { "<<PrevPara>>"       , CTkAppVirtualEventType::PrevPara        },
  { "<<PrevWindow>>"     , CTkAppVirtualEventType::PrevWindow      },
  { "<<PrevWord>>"       , CTkAppVirtualEventType::PrevWord        },
  { "<<Redo>>"           , CTkAppVirtualEventType::Redo            },
  { "<<SelectAll>>"      , CTkAppVirtualEventType::SelectAll       },
  { "<<SelectLineEnd>>"  , CTkAppVirtualEventType::SelectLineEnd   },
  { "<<SelectLineStart>>", CTkAppVirtualEventType::SelectLineStart },
  { "<<SelectNextChar>>" , CTkAppVirtualEventType::SelectNextChar  },
  { "<<SelectNextLine>>" , CTkAppVirtualEventType::SelectNextLine  },
  { "<<SelectNextPara>>" , CTkAppVirtualEventType::SelectNextPara  },
  { "<<SelectNextWord>>" , CTkAppVirtualEventType::SelectNextWord  },
  { "<<SelectNone>>"     , CTkAppVirtualEventType::SelectNone      },
  { "<<SelectPrevChar>>" , CTkAppVirtualEventType::SelectPrevChar  },
  { "<<SelectPrevLine>>" , CTkAppVirtualEventType::SelectPrevLine  },
  { "<<SelectPrevPara>>" , CTkAppVirtualEventType::SelectPrevPara  },
  { "<<SelectPrevWord>>" , CTkAppVirtualEventType::SelectPrevWord  },
  { "<<ToggleSelection>>", CTkAppVirtualEventType::ToggleSelection },
  { "<<Undo>>"           , CTkAppVirtualEventType::Undo            },

  { "<<Help>>"           , CTkAppVirtualEventType::Help            },

#if 0
  { "<<TkFontchooserVisibility>>" , CTkAppVirtualEventType::FontchooserVisibility  },
  { "<<TkFontchooserFontChanged>>", CTkAppVirtualEventType::FontchooserFontChanged },
#endif

  { ""                   , CTkAppVirtualEventType::None            }
};

#if 0
static Tcl_NotifierProcs s_notifierProcs;
#endif

//static Tcl_NotifierProcs s_noNotifierProcs;

#if 0
int s_waitForEventProc(const Tcl_Time *) {
  if (s_app)
    return s_app->waitForEventProc();

  return 0;
}
#endif

int s_eventEventProc(Tcl_Event * /*evPtr*/, int flags) {
  if (! (flags & TCL_WINDOW_EVENTS))
    return 0;

  QCoreApplication::processEvents();

  return 1;
}

void s_eventSetupProc(ClientData /*clientData*/, int flags) {
//auto *app = static_cast<CTkApp *>(clientData);
  Tcl_Time block_time = {0, 0};
  if (! (flags & TCL_WINDOW_EVENTS))
    return;

  block_time.usec = 10000;
  Tcl_SetMaxBlockTime(&block_time);
}

void s_eventCheckProc(ClientData /*clientData*/, int flags) {
//auto *app = static_cast<CTkApp *>(clientData);
  if (! (flags & TCL_WINDOW_EVENTS))
    return;

  auto *event = reinterpret_cast<Tcl_Event *>(Tcl_Alloc(sizeof(Tcl_Event)));
  event->proc = s_eventEventProc;
  Tcl_QueueEvent(event, TCL_QUEUE_TAIL);
}

}

//---

static int CTkApp_Init(Tcl_Interp *) { return TCL_OK; }
static int CTkApp_SafeInit(Tcl_Interp *) { return TCL_OK; }

//---

CTkApp::
CTkApp(Tcl_Interp *interp, bool useNamespace) :
 CTclApp(interp), useNamespace_(useNamespace)
{
  static const char *argv[2] = { "CTkApp", nullptr };

  construct(1, argv);
}

CTkApp::
CTkApp(int argc, const char **argv, bool useNamespace) :
 CTclApp(argc, argv), useNamespace_(useNamespace)
{
  construct(argc, argv);
}

void
CTkApp::
construct(int argc, const char **argv)
{
  s_app = this;

  root_ = new CTkAppRoot(this);

  tclInit();

  Tcl_StaticPackage(getInterp(), "Tk", CTkApp_Init, CTkApp_SafeInit);

  Tcl_PkgProvide(getInterp(), "Tk", "8.0");

  if (useNamespace_)
    Tcl_CreateNamespace(getInterp(), "::tkapp", nullptr, nullptr);

  setInteractive(false);

  init();

  std::vector<std::string> args;

  for (int i = 1; i < argc; ++i)
    args.push_back(std::string(argv[i]));

  setStringArrayGlobalVar("argv", args);
  setIntegerGlobalVar    ("argc", args.size());

  setStringGlobalVar("tk_version", "8.6");
  setStringGlobalVar("tk_library", "/usr/share/tcltk/tk8.6");
  setStringGlobalVar("tk_patchLevel", "8.6.12");

  //---

  // standard fonts
  createFont("TkCaptionFont");
  createFont("TkSmallCaptionFont");
  createFont("TkTooltipFont");
  createFont("TkFixedFont");
  createFont("TkHeadingFont");
  createFont("TkMenuFont");
  createFont("TkIconFont");
  createFont("TkTextFont");
  createFont("TkDefaultFont");

  //---

#if 0
  s_notifierProcs.waitForEventProc = s_waitForEventProc;

  Tcl_SetNotifier(&s_notifierProcs);
#endif

  Tcl_CreateEventSource(s_eventSetupProc, s_eventCheckProc, this);

  //---

  timer_ = new QTimer;

  connect(timer_, SIGNAL(timeout()), this, SLOT(timerSlot()));

  timer_->start(100);
}

int
CTkApp::
waitForEventProc()
{
  timer_->stop();

  processEvents();

//Tcl_SetNotifier(&s_noNotifierProcs);

//Tcl_Time time;
//time.sec  = 0;
//time.usec = 100;

//auto rc = Tcl_WaitForEvent(&time);

//Tcl_SetNotifier(&s_notifierProcs);

//return rc;

  timer_->start(100);

  return 1;
}

void
CTkApp::
timerSlot()
{
  Tcl_DoOneEvent(TCL_ALL_EVENTS);
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
createImage(const std::string &type, const std::string & /*format*/, const std::string &name,
            int width, int height)
{
  auto image = std::make_shared<CTkAppImage>(this, name, width, height);

  image->setType(type);

  images_[name] = image;

  return image;
}

void
CTkApp::
deleteImage(const CTkAppImageRef &image)
{
  images_.erase(image->name());
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

CTkAppImageRef
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

  auto pm = bitmaps_.find(name1);

  if (pm == bitmaps_.end()) {
    auto appImage = std::make_shared<CTkAppImage>(const_cast<CTkApp *>(this), name1);

    if (! appImage->loadFile(name1))
      return CTkAppImageRef();

    pm = bitmaps_.insert(pm, ImageMap::value_type(name1, appImage));
  }

  return (*pm).second;
}

void
CTkApp::
getImageNames(std::vector<std::string> &names) const
{
  for (const auto &pi : images_)
    names.push_back(pi.first);
}

//---

std::string
CTkApp::
getNewFontName() const
{
  static uint id = 1;

  std::string name = CStrUtil::strprintf("font%u", id);

  while (fonts_.find(name) != fonts_.end()) {
    ++id;

    name = CStrUtil::strprintf("font%u", id);
  }

  ++id;

  return name;
}

CTkAppFontRef
CTkApp::
createFont(const std::string &name)
{
  auto font = std::make_shared<CTkAppFont>(this, name);

  fonts_[name] = font;

  return font;
}

void
CTkApp::
deleteFont(const CTkAppFontRef &font)
{
  fonts_.erase(font->name());
}

CTkAppFontRef
CTkApp::
getFont(const std::string &name) const
{
  auto p = fonts_.find(name);

  if (p == fonts_.end())
    return CTkAppFontRef();

  return (*p).second;
}

QFont
CTkApp::
getQFont(const std::string &name) const
{
  auto font = getFont(name);

  QFont qfont;

  if (font)
    return font->getQFont();

  CTkAppUtil::stringToQFont(name, qfont);

  return qfont;
}

void
CTkApp::
getFontData(const QFont &qfont, FontData &data) const
{
  data.family     = qfont.family().toStdString();
  data.size       = qfont.pointSizeF();
  data.weight     = (qfont.bold() ? "bold" : "normal");
  data.slant      = (qfont.italic() ? "italic" : "roman");
  data.underline  = qfont.underline();
  data.overstrike = qfont.strikeOut();

  QFontMetricsF fm(qfont);

  data.ascent    = fm.ascent();
  data.descent   = fm.descent();
  data.linespace = fm.height();
  data.fixed     = qfont.fixedPitch();
}

void
CTkApp::
getFontNames(std::vector<std::string> &names) const
{
  for (const auto &pf : fonts_)
    names.push_back(pf.first);
}

void
CTkApp::
showFontDialog(bool b)
{
  static QFontDialog *dialog;

  if (b) {
    if (! dialog)
      dialog = new QFontDialog(root_->qwidget());

    dialog->show();
  }
  else {
    if (dialog)
      dialog->hide();
  }
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

void
CTkApp::
getClassBindings(const std::string &name, const CTkAppEventData &data,
                 std::vector<std::string> &bindings)
{
  auto pc = classEvents_.find(name);
  if (pc == classEvents_.end()) return;

  for (const auto &cdata : (*pc).second) {
    if (cdata == data)
      bindings.push_back(cdata.pattern);
  }
}

void
CTkApp::
getTagBindings(const std::string &name, const CTkAppEventData &data,
               std::vector<std::string> &bindings)
{
  auto pt = tagEvents_.find(name);
  if (pt == tagEvents_.end()) return;

  for (const auto &tdata : (*pt).second) {
    if (tdata == data)
      bindings.push_back(tdata.pattern);
  }
}

void
CTkApp::
getAllBindings(const CTkAppEventData &data, std::vector<std::string> &bindings)
{
  for (const auto &adata : allEvents_) {
    if (adata == data)
      bindings.push_back(adata.pattern);
  }
}

void
CTkApp::
getClassBindings(const std::string &name, std::vector<std::string> &bindings)
{
  auto pc = classEvents_.find(name);
  if (pc == classEvents_.end()) return;

  for (const auto &cdata : (*pc).second)
    bindings.push_back(cdata.pattern);
}

void
CTkApp::
getTagBindings(const std::string &name, std::vector<std::string> &bindings)
{
  auto pt = tagEvents_.find(name);
  if (pt == tagEvents_.end()) return;

  for (const auto &tdata : (*pt).second)
    bindings.push_back(tdata.pattern);
}

void
CTkApp::
getAllBindings(std::vector<std::string> &bindings)
{
  for (const auto &adata : allEvents_)
    bindings.push_back(adata.pattern);
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
triggerVirtualEvents(const std::string &className, CTkAppWidget *w,
                     const CTkAppEventData &matchEventData)
{
  auto p1 = classEvents_.find(className);

  if (p1 != classEvents_.end()) {
    for (const auto &eventData : (*p1).second) {
      if (eventData == matchEventData)
        execVirtualEvent(w, matchEventData, eventData.command);
    }
  }

  for (const auto &eventData : allEvents_) {
    if (eventData == matchEventData)
      execVirtualEvent(w, matchEventData, eventData.command);
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

bool
CTkApp::
execVirtualEvent(CTkAppWidget *w, const CTkAppEventData &, const std::string &command)
{
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
          case 'W': // widget name
            command1 += w->getFullName();
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

int
CTkApp::
addWidget(CTkAppWidget *w)
{
  widgets_.insert(w);

  return int(widgets_.size());
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

  for (uint i = 0; i < num; ++i) {
    if (deleteWidgets_[i])
      delete deleteWidgets_[i];
  }

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
stringToVirtualEvent(const std::string &str, CTkAppVirtualEventData &data, bool quiet) const
{
  CStrParse parse(str);

  parse.skipSpace();

  for (uint i = 0; virtualEventData[i].type != CTkAppVirtualEventType::None; ++i) {
    if (parse.isString(virtualEventData[i].name)) {
      parse.skipLastString();
      parse.skipSpace();

      if (! parse.eof()) {
        if (! quiet) return throwError("invalid virtual event \"" + str + " \"");
        return false;
      }

      data.type = virtualEventData[i].type;

      return true;
    }
  }

  if (! quiet) return throwError("invalid virtual event \"" + str + " \"");
  return false;
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

  // Virtual Events
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

    if      (parse.isChar('<')) {
      for (uint i = 0; virtualEventData[i].type != CTkAppVirtualEventType::None; ++i) {
        if (parse.isString(virtualEventData[i].name.substr(1))) {
          parse.skipLastString();
          parse.skipSpace();

          data.type  = CTkAppEventType::Virtual;
          data.vtype = virtualEventData[i].type;

          if (! parse.eof())
            return false;

          return true;
        }
      }

      return false;
    }
    else if (parse.isString("Expose")) {
      parse.skipLastString();

      data.type = CTkAppEventType::Expose;

      if (! parseClose('>'))
        return false;

      return true;
    }
    else if (parse.isString("ShiftNext") ||
             parse.isString("ControlNext") ||
             parse.isString("Shift-ControlNext")) {
      parse.skipLastString();

      //data.type = CTkAppEventType::Next;

      if (! parseClose('>'))
        return false;

      return true;
    }
    else if (parse.isString("ShiftPrior") ||
             parse.isString("ControlPrior") ||
             parse.isString("Shift-ControlPrior")) {
      parse.skipLastString();

      //data.type = CTkAppEventType::Prior;

      if (! parseClose('>'))
        return false;

      return true;
    }
    else if (parse.isString("ShiftUp") ||
             parse.isString("ControlUp") ||
             parse.isString("Shift-ControlUp")) {
      parse.skipLastString();

      //data.type = CTkAppEventType::Up;

      if (! parseClose('>'))
        return false;

      return true;
    }
    else if (parse.isString("ShiftDown") ||
             parse.isString("ControlDown") ||
             parse.isString("Shift-ControlDown")) {
      parse.skipLastString();

      //data.type = CTkAppEventType::Up;

      if (! parseClose('>'))
        return false;

      return true;
    }
    else if (parse.isString("ShiftLeft") ||
             parse.isString("ControlLeft") ||
             parse.isString("Shift-ControlLeft")) {
      parse.skipLastString();

      //data.type = CTkAppEventType::Up;

      if (! parseClose('>'))
        return false;

      return true;
    }
    else if (parse.isString("ShiftRight") ||
             parse.isString("ControlRight") ||
             parse.isString("Shift-ControlRight")) {
      parse.skipLastString();

      //data.type = CTkAppEventType::Up;

      if (! parseClose('>'))
        return false;

      return true;
    }
    else if (parse.isString("ShiftHome") ||
             parse.isString("ControlHome") ||
             parse.isString("Shift-ControlHome")) {
      parse.skipLastString();

      //data.type = CTkAppEventType::Up;

      if (! parseClose('>'))
        return false;

      return true;
    }
    else if (parse.isString("ShiftEnd") ||
             parse.isString("ControlEnd") ||
             parse.isString("Shift-ControlEnd")) {
      parse.skipLastString();

      //data.type = CTkAppEventType::Up;

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
        else if (parse.isString("Enter")) {
          parse.skipLastString();
          //??
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

void
CTkApp::
addVirtualEventData(const CTkAppVirtualEventData &vdata, const CTkAppEventData &edata)
{
  virtualEventData_[vdata].push_back(edata);
}

void
CTkApp::
virtualEventNames(std::vector<std::string> &names) const
{
  for (uint i = 0; virtualEventData[i].type != CTkAppVirtualEventType::None; ++i)
    names.push_back(virtualEventData[i].name);
}

void
CTkApp::
virtualEventNames(const CTkAppVirtualEventData &vdata, std::vector<std::string> &names) const
{
  for (const auto &pv : virtualEventData_) {
    if (pv.first == vdata) {
      for (const auto &edata : pv.second)
        names.push_back(edata.pattern);
    }
  }
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
                 const std::string &arg, std::string &opt, bool quiet) const
{
  return lookupName("option", names, arg, opt, quiet);
}

bool
CTkApp::
lookupName(const std::string &msg, const std::vector<std::string> &names,
           const std::string &arg, std::string &opt, bool quiet) const
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

  if (! optionValues.match(arg, argNum)) {
    if (! quiet)
      return throwError("bad " + msg + " \"" + arg + "\": must be " + concatOptionNames());
    else
      return false;
  }

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

//---

QTransform
CTkApp::
toQTransform(const CMatrix2D &m) const
{
  double a, b, c, d, tx, ty;

  m.getValues(&a, &b, &c, &d, &tx, &ty);

  //return QTransform(a, b, c, d, tx, ty);
  return QTransform(a, c, b, d, tx, ty);
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
  std::cerr << "Error: " << currentCommand() << " " << msg << "\n";

  return false;
}

#if 0
void
CTkApp::
debugCmd(const std::string &cmd, const std::vector<std::string> &args) const
{
  std::cerr << "Run: " << cmd;
  for (const auto &arg : args)
    std::cerr << " " << arg;
  std::cerr << "\n";
}
#else
void
CTkApp::
debugCmd(const std::string &, const std::vector<std::string> &) const
{
}
#endif

bool
CTkApp::
TODO(const std::string &msg) const
{
  std::cerr << "TODO: " << currentCommand() << " " << msg << "\n";

  return false;
}

bool
CTkApp::
TODO(const std::vector<std::string> &args) const
{
  std::cerr << "TODO: " << currentCommand();

  for (const auto &arg : args)
    std::cerr << " " << arg;

  std::cerr << "\n";

  return false;
}

bool
CTkApp::
TODO(const std::string &arg, const std::vector<std::string> &args) const
{
  std::cerr << "TODO: ";

  std::cerr << arg << " in ";

  std::cerr << currentCommand();

  for (const auto &arg : args)
    std::cerr << " " << arg;

  std::cerr << "\n";

  return false;
}

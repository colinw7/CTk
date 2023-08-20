#include <CTkApp.h>
#include <CTkAppWidget.h>
#include <CTkAppImage.h>
#include <CTkAppFont.h>
#include <CTkAppCommands.h>
#include <CTkAppEventData.h>
#include <CTkAppUtil.h>

#include <CQStrParse.h>

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
  QString                name;
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
CTkApp(Tcl_Interp *interp, const QString &context) :
 CTclApp(interp), context_(context)
{
  static const char *argv[2] = { "CTkApp", nullptr };

  construct(1, argv);
}

CTkApp::
CTkApp(int argc, const char **argv, const QString &context) :
 CTclApp(argc, argv), context_(context)
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

  if (context_ != "") {
    auto context = "::" + context_;

    Tcl_CreateNamespace(getInterp(), context.toLatin1().constData(), nullptr, nullptr);
  }

  setInteractive(false);

  init();

  std::vector<QString> args;

  for (int i = 1; i < argc; ++i)
    args.push_back(QString(argv[i]));

  setStringArrayGlobalVar("argv", args);
  setIntegerGlobalVar    ("argc", args.size());

  setStringGlobalVar("tk_version", QString("8.6"));
  setStringGlobalVar("tk_library", QString("/usr/share/tcltk/tk8.6"));
  setStringGlobalVar("tk_patchLevel", QString("8.6.12"));

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
addWidgetClass(const QString &name)
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
lookupWidgetByName(const QString &widgetName, bool quiet) const
{
  if (widgetName == ".")
    return root();

  CTkAppWidget* parent = nullptr;
  QString       childName;

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
processOption(CTkAppOption *opts, const Args &args, uint &i, CTkAppOptionValueMap &values)
{
  uint numArgs = args.size();

  auto arg = args[i].toString();

  for (uint j = 0; opts[j].name != nullptr; ++j) {
    const auto &option = opts[j];

    if (arg != option.name && (option.alias == nullptr || arg != option.alias))
      continue;

    auto &value = values[arg];

    if      (option.type == CTkAppOptionType::Flag)
      value.setBool(true);
    else if (option.type == CTkAppOptionType::String) {
      if (i < numArgs - 1)
        value.setString(args[++i].toString());
      else
        return false;
    }
    else if (option.type == CTkAppOptionType::Int) {
      if (i < numArgs - 1) {
        long l;
        if (! CTkAppUtil::stringToInt(args[++i].toString(), l))
          return false;
        value.setInteger(l);
      }
      else
        return false;
    }
    else if (option.type == CTkAppOptionType::Real) {
      if (i < numArgs - 1) {
        double r;
        if (! CTkAppUtil::stringToReal(args[++i].toString(), r))
          return false;
        value.setReal(r);
      }
      else
        return false;
    }

    return true;
  }

  return false;
}

//---

QString
CTkApp::
getNewImageName() const
{
  static uint id = 1;

  auto name = QString("image%1").arg(id);

  while (images_.find(name) != images_.end()) {
    ++id;

    name = QString("image%1").arg(id);
  }

  ++id;

  return name;
}

CTkAppImageRef
CTkApp::
createImage(const QString &type, const QString & /*format*/, const QString &name,
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
getImage(const QString &name) const
{
  auto p = images_.find(name);

  if (p == images_.end())
    return CTkAppImageRef();

  return (*p).second;
}

CTkAppImageRef
CTkApp::
getBitmap(const QString &name) const
{
  auto name1 = name;

  if (name[0] == '@') {
    auto *env = getenv("CTK_APP_IMAGE_DIR");
    if (! env)
      (void) throwError("set CTK_APP_IMAGE_DIR to images path for @ name");

    if (env)
      name1 = QString(env) + name.mid(1);
  }
  else {
    auto *env = getenv("CTK_APP_BITMAP_DIR");
    if (! env)
      (void) throwError("set CTK_APP_BITMAP_DIR to bitmap path for name");

    if (env)
      name1 = QString(env) + "/" + name + ".xbm";
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
getImageNames(std::vector<QString> &names) const
{
  for (const auto &pi : images_)
    names.push_back(pi.first);
}

//---

QString
CTkApp::
getNewFontName() const
{
  static uint id = 1;

  auto name = QString("font%1").arg(id);

  while (fonts_.find(name) != fonts_.end()) {
    ++id;

    name = QString("font%1").arg(id);
  }

  ++id;

  return name;
}

CTkAppFontRef
CTkApp::
createFont(const QString &name)
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
getFont(const QString &name) const
{
  auto p = fonts_.find(name);

  if (p == fonts_.end())
    return CTkAppFontRef();

  return (*p).second;
}

QFont
CTkApp::
getQFont(const QString &name) const
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
  data.family     = qfont.family();
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
getFontNames(std::vector<QString> &names) const
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
      dialog = new QFontDialog(root_->getQWidget());

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
getWidgetClassData(const QString &name) const
{
  auto p = widgetClasses_.find(name);

  if (p == widgetClasses_.end())
    return nullptr;

  return const_cast<CTkApp *>(this);
}

//---

bool
CTkApp::
bindTagEvent(const QString &name, const CTkAppEventData &data)
{
  tagEvents_[name].push_back(data);

  return true;
}

bool
CTkApp::
bindClassEvent(const QString &name, const CTkAppEventData &data)
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
getClassBindings(const QString &name, const CTkAppEventData &data,
                 std::vector<QString> &bindings)
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
getTagBindings(const QString &name, const CTkAppEventData &data,
               std::vector<QString> &bindings)
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
getAllBindings(const CTkAppEventData &data, std::vector<QString> &bindings)
{
  for (const auto &adata : allEvents_) {
    if (adata == data)
      bindings.push_back(adata.pattern);
  }
}

void
CTkApp::
getClassBindings(const QString &name, std::vector<QString> &bindings)
{
  auto pc = classEvents_.find(name);
  if (pc == classEvents_.end()) return;

  for (const auto &cdata : (*pc).second)
    bindings.push_back(cdata.pattern);
}

void
CTkApp::
getTagBindings(const QString &name, std::vector<QString> &bindings)
{
  auto pt = tagEvents_.find(name);
  if (pt == tagEvents_.end()) return;

  for (const auto &tdata : (*pt).second)
    bindings.push_back(tdata.pattern);
}

void
CTkApp::
getAllBindings(std::vector<QString> &bindings)
{
  for (const auto &adata : allEvents_)
    bindings.push_back(adata.pattern);
}

bool
CTkApp::
triggerEvents(const QString &className, CTkAppWidget *w, QEvent *e,
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
triggerVirtualEvents(const QString &className, CTkAppWidget *w,
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
execEvent(CTkAppWidget *w, QEvent *e, const CTkAppEventData &data, const QString &command)
{
  auto *me = dynamic_cast<QMouseEvent *>(e);

  QString command1;

  CQStrParse parse(command);

  while (! parse.eof()) {
    if (parse.isChar('%')) {
      parse.skipChar();

      if (! parse.eof()) {
        auto c = parse.getChar().toLatin1();

        switch (c) {
          case '%':
            command1 += '%';
            break;
          case 'b': // button
            command1 += QString::number(data.button);
            break;
          case 'd': // detail
            command1 += ""; // TODO
            break;
          case 'h': // height
            command1 += QString::number(w->getQWidget()->height());
            break;
          case 't': // time
            command1 += ""; // TODO
            break;
          case 'w': // width
            command1 += QString::number(w->getQWidget()->width());
            break;
          case 'x': // x
            command1 += (me ? QString::number(me->x()) : "0");
            break;
          case 'y': // y
            command1 += (me ? QString::number(me->y()) : "0");
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
            command1 += (me ?
              QString::number(w->getQWidget()->mapToGlobal(me->pos()).x()) : "0");
            break;
          case 'Y': // root y
            command1 += (me ?
              QString::number(w->getQWidget()->mapToGlobal(me->pos()).y()) : "0");
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
      command1 += parse.getChar().toLatin1();
  }

  //std::cerr << "Exec: " << command1 << "\n";

  eval(command1);

  w->getQWidget()->update();

  return true;
}

bool
CTkApp::
execVirtualEvent(CTkAppWidget *w, const CTkAppEventData &, const QString &command)
{
  QString command1;

  CQStrParse parse(command);

  while (! parse.eof()) {
    if (parse.isChar('%')) {
      parse.skipChar();

      if (! parse.eof()) {
        auto c = parse.getChar().toLatin1();

        switch (c) {
          case '%':
            command1 += '%';
            break;
          case 'd': // detail
            command1 += ""; // TODO
            break;
          case 'h': // height
            command1 += QString::number(w->getQWidget()->height());
            break;
          case 't': // time
            command1 += ""; // TODO
            break;
          case 'w': // width
            command1 += QString::number(w->getQWidget()->width());
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
      command1 += parse.getChar().toLatin1();
  }

  //std::cerr << "Exec: " << command1 << "\n";

  eval(command1);

  w->getQWidget()->update();

  return true;
}

//---

CTkAppTopLevel *
CTkApp::
installToplevel(const QString &id, QFrame *frame)
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
    data.key  = e->text();

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
stringToVirtualEvent(const QString &str, CTkAppVirtualEventData &data, bool quiet) const
{
  CQStrParse parse(str);

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
parseEvent(const QString &pattern, CTkAppEventData &data)
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

  CQStrParse parse(pattern);

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
        if (parse.isString(virtualEventData[i].name.mid(1))) {
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
          data.key += parse.getChar().toLatin1();

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
        data.key += parse.getChar().toLatin1();

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
virtualEventNames(std::vector<QString> &names) const
{
  for (uint i = 0; virtualEventData[i].type != CTkAppVirtualEventType::None; ++i)
    names.push_back(virtualEventData[i].name);
}

void
CTkApp::
virtualEventNames(const CTkAppVirtualEventData &vdata, std::vector<QString> &names) const
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
addOption(const QString &pattern, const QString &value, int priority)
{
  //std::cerr << "addOption: " << pattern.toStdString() << " " <<
  //             value.toStdString() << " " << priority << "\n";

  options_.emplace_back(pattern, value, priority);
}

bool
CTkApp::
matchOption(const QString &widgetClass, const QString &optName,
            const QString &optClass, QString &optValue) const
{
  optValue = "";
  //std::cerr << "matchOption: " << widgetClass.toStdString() << " " <<
  //             optClass.toStdString() << " " << optName.toStdString() << "\n";

  auto pattern1 = widgetClass + "." + optName;

  for (const auto &opt : options_) {
    //std::cerr << "compare: " << pattern1.toStdString() <<
    //             " " << opt.pattern.toStdString() << "\n";

    QRegExp regexp(opt.pattern, Qt::CaseSensitive, QRegExp::Wildcard);

    if (regexp.exactMatch(pattern1)) {
      //std::cerr << "match: " << opt.pattern << " = " << opt.value << "\n";
      optValue = opt.value;
      return true;
    }
  }

  auto pattern2 = widgetClass + "." + optClass;

  for (const auto &opt : options_) {
    //std::cerr << "compare: " << pattern2.toStdString() <<
    //             " " << opt.pattern.toStdString() << "\n";

    QRegExp regexp(opt.pattern, Qt::CaseSensitive, QRegExp::Wildcard);

    if (regexp.exactMatch(pattern2)) {
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
lookupOptionName(const std::vector<QString> &names,
                 const QString &arg, QString &opt, bool quiet) const
{
  return lookupName("option", names, arg, opt, quiet);
}

bool
CTkApp::
lookupName(const QString &msg, const std::vector<QString> &names,
           const QString &arg, QString &opt, bool quiet) const
{
  auto concatOptionNames = [&]() {
    QString str;

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

  if (! CTkAppUtil::uniqueMatch(names, arg, opt)) {
    if (! quiet)
      return throwError("bad " + msg + " \"" + arg + "\": must be " + concatOptionNames());
    else
      return false;
  }

  return true;
}

bool
CTkApp::
getOptionInt(const QString &name, const QString &value, long &i) const
{
  if (! CTkAppUtil::stringToInt(value, i)) {
    throwError("Invalid value \"" + value + "\" for \"" + name + "\"");
    return false;
  }

  return true;
}

bool
CTkApp::
getOptionReal(const QString &name, const QString &value, double &r) const
{
  if (! CTkAppUtil::stringToReal(value, r)) {
    throwError("Invalid value \"" + value + "\" for \"" + name + "\"");
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
wrongNumArgs(const QString &msg) const
{
  std::cerr << "wrong # args: should be \"" << msg.toStdString() << "\"\n";

  return false;
}

bool
CTkApp::
throwError(const QString &msg) const
{
  std::cerr << "Error: " << currentCommand().toStdString() << " " << msg.toStdString() << "\n";

  return false;
}

#if 0
void
CTkApp::
debugCmd(const QString &cmd, const Args &args) const
{
  std::cerr << "Run: " << cmd.toStdString();
  for (const auto &arg : args)
    std::cerr << " " << arg;
  std::cerr << "\n";
}
#else
void
CTkApp::
debugCmd(const QString &, const Args &) const
{
}
#endif

bool
CTkApp::
TODO(const QString &msg) const
{
  std::cerr << "TODO: " << currentCommand().toStdString() << " " << msg.toStdString() << "\n";

  return false;
}

bool
CTkApp::
TODO(const Args &args) const
{
  std::cerr << "TODO: " << currentCommand().toStdString();

  for (const auto &arg : args)
    std::cerr << " " << arg.toString().toStdString();

  std::cerr << "\n";

  return false;
}

bool
CTkApp::
TODO(const QString &arg, const Args &args) const
{
  std::cerr << "TODO: ";

  std::cerr << arg.toStdString() << " in ";

  std::cerr << currentCommand().toStdString();

  for (const auto &arg : args)
    std::cerr << " " << arg.toString().toStdString();

  std::cerr << "\n";

  return false;
}

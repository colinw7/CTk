#include <CTkApp.h>
#include <CTkAppWidget.h>
#include <CTkAppRoot.h>
#include <CTkAppTopLevel.h>
#include <CTkAppImage.h>
#include <CTkAppFont.h>
#include <CTkAppCommands.h>
#include <CTkAppWidgetCommand.h>
#include <CTkAppImageCommand.h>
#include <CTkAppEventData.h>
#include <CTkAppUtil.h>
#include <CTkAppDebug.h>
#include <CTkAppBitmaps.h>
#include <CTkAppCursor.h>

#include <CQStrParse.h>
#include <CScreenUnits.h>

#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFontDialog>
#include <QTimer>
#include <QThread>
#include <QAbstractEventDispatcher>
#include <QClipboard>
#include <QFileInfo>

#include <tcl.h>

#define MY_EVENT_SOURCE 1

//---

namespace {

static CTkApp *s_app;

static CTkAppDebug *s_debug;

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
  { "<<NoManagedChild>>" , CTkAppVirtualEventType::NoManagedChild  },
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

  { "", CTkAppVirtualEventType::None }
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

#ifdef MY_EVENT_SOURCE
int s_eventEventProc(Tcl_Event * /*evPtr*/, int flags) {
  if (! (flags & TCL_WINDOW_EVENTS))
    return 0;

  QCoreApplication::processEvents();

  s_app->purgeWidgets();

  s_app->showToplevels();

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

  if (! QThread::currentThread()->eventDispatcher()->hasPendingEvents() &&
      ! s_app->numDeleteWidgets() &&
      ! s_app->numShowToplevels())
    return;

  auto *event = reinterpret_cast<Tcl_Event *>(Tcl_Alloc(sizeof(Tcl_Event)));
  event->proc = s_eventEventProc;
  Tcl_QueueEvent(event, TCL_QUEUE_TAIL);
}
#endif

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

  setAppName("CTkApp");
  setAppFont(qApp->font());

  root_ = new CTkAppRoot(this);

  tclInit();

  Tcl_CreateNamespace(getInterp(), "::tk"       , nullptr, nullptr);
  Tcl_CreateNamespace(getInterp(), "::tk::icons", nullptr, nullptr);

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

  eval("proc ::tk::icons::IconBadge {win badgenumber} { }");

  //---

  // standard fonts
  createFont("TkCaptionFont"     )->setPointSize(12).setBold(true);
  createFont("TkSmallCaptionFont")->setPointSize(9);
  createFont("TkTooltipFont"     )->setPointSize(9);
  createFont("TkFixedFont"       )->setPointSize(10).setFamily("monospace");
  createFont("TkHeadingFont"     )->setPointSize(10).setBold(true);
  createFont("TkMenuFont"        )->setPointSize(10);
  createFont("TkIconFont"        )->setPointSize(10);
  createFont("TkTextFont"        )->setPointSize(10);
  createFont("TkDefaultFont"     )->setPointSize(10);

  //---

  // standard bitmaps
  CTkAppBitmaps::addStandardBitmaps(this);

  //---

#if 0
  s_notifierProcs.waitForEventProc = s_waitForEventProc;

  Tcl_SetNotifier(&s_notifierProcs);
#endif

#ifdef MY_EVENT_SOURCE
  Tcl_CreateEventSource(s_eventSetupProc, s_eventCheckProc, this);
#endif

  //---

  timer_ = new QTimer;

  connect(timer_, SIGNAL(timeout()), this, SLOT(timerSlot()));

  timer_->start(100);

  //---

  createAlias("echo", "puts");

  //---

  cursorMgr_ = new CTkAppCursorMgr(this);
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
  showToplevels();

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
    root_->setNeedsShow(true);

  showToplevels();
}

int
CTkApp::
numShowToplevels() const
{
  int n = 0;

  if (root_->isNeedsShow())
    ++n;

  for (const auto &toplevel : toplevels_) {
    if (! toplevel || toplevel->isDeleted())
      continue;

    if (toplevel->isNeedsShow())
      ++n;
  }

  return n;
}

void
CTkApp::
showToplevels()
{
  if (root_->isNeedsShow()) {
    root_->setNeedsShow(false);

    root_->show();
  }

  for (const auto &toplevel : toplevels_) {
    if (! toplevel || toplevel->isDeleted())
      continue;

    if (toplevel->isNeedsShow()) {
      toplevel->setNeedsShow(false);

      toplevel->show();

      processEvents();
    }
  }
}

//---

CTkAppWidget *
CTkApp::
lookupWidgetByName(const QVariant &widgetName, bool quiet) const
{
  if (widgetName == ".")
    return root();

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root_->decodeWidgetName(widgetName, &parent, childName)) {
    if (! quiet) throwError(msg() + "bad window path name \"" + widgetName + "\"");
    return nullptr;
  }

  auto *child = parent->getChild(childName);

  if (child == nullptr) {
    if (! quiet) throwError(msg() + "bad window path name \"" + widgetName + "\"");
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

  auto arg = variantToString(args[i]);

  for (uint j = 0; opts[j].name != nullptr; ++j) {
    const auto &option = opts[j];

    if (arg != option.name && (option.alias == nullptr || arg != option.alias))
      continue;

    auto &value = values[arg];

    if      (option.type == CTkAppOptionType::Flag)
      value.setBool(true);
    else if (option.type == CTkAppOptionType::String) {
      if (i < numArgs - 1)
        value.setValue(args[++i]);
      else
        return false;
    }
    else if (option.type == CTkAppOptionType::Int) {
      if (i < numArgs - 1) {
        long l;
        if (! variantToInt(args[++i], l))
          return false;
        value.setInteger(l);
      }
      else
        return false;
    }
    else if (option.type == CTkAppOptionType::Real) {
      if (i < numArgs - 1) {
        double r;
        if (! variantToReal(args[++i], r))
          return false;
        value.setReal(r);
      }
      else
        return false;
    }
    else if (option.type == CTkAppOptionType::Boolean) {
      if (i < numArgs - 1) {
        bool b;
        if (! variantToBool(args[++i], b))
          return false;
        value.setBool(b);
      }
      else
        return false;
    }
    else {
      assert(false);
    }

    return true;
  }

  return false;
}

//---

bool
CTkApp::
commandExists(const QString &name) const
{
  Tcl_CmdInfo info;
  auto rc = Tcl_GetCommandInfo(getInterp(), name.toLatin1().constData(), &info);
  return (rc != 0);
}

//---

QString
CTkApp::
getNewImageName() const
{
  static uint id = 1;

  auto name = QString("image%1").arg(id);

  auto isValidImageName = [&](const QString &name) {
    if (images_.find(name) != images_.end())
      return false;

    if (commandExists(name))
      return false;

    return true;
  };

  while (! isValidImageName(name)) {
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
getImage(const QVariant &var) const
{
  auto name = variantToString(var);

  auto p = images_.find(name);

  if (p == images_.end())
    return CTkAppImageRef();

  return (*p).second;
}

void
CTkApp::
getImageNames(std::vector<QString> &names) const
{
  for (const auto &pi : images_)
    names.push_back(pi.first);
}

CTkAppImageRef
CTkApp::
getBitmap(const QString &name, bool create) const
{
  auto pm = bitmaps_.find(name);

  if (pm != bitmaps_.end())
    return (*pm).second;

  if (! create)
    return CTkAppImageRef();

  //---

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

  auto appImage = std::make_shared<CTkAppImage>(const_cast<CTkApp *>(this), name1);

  if (! appImage->loadFile(name1))
    return CTkAppImageRef();

  const_cast<CTkApp *>(this)->addBitmap(name, appImage);

  return appImage;
}

void
CTkApp::
addBitmap(const QString &name, CTkAppImageRef &image)
{
  bitmaps_[name] = image;
}

void
CTkApp::
getBitmapNames(std::vector<QString> &names) const
{
  for (const auto &pi : bitmaps_)
    names.push_back(pi.first);
}

//---

CTkAppImageCommand *
CTkApp::
addImageCommand(const QString &name, const QString &type)
{
  auto pi = imageCommands_.find(name);

  if (pi == imageCommands_.end())
    pi = imageCommands_.emplace_hint(pi, name, new CTkAppImageCommand(this, name, type));

  return (*pi).second;
}

//---

QString
CTkApp::
getNewFontName() const
{
  uint id   = 1;
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
createFont(const QVariant &var)
{
  auto name = variantToString(var);

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
getFont(const QVariant &var) const
{
  auto name = variantToString(var);

  auto p = fonts_.find(name);

  if (p == fonts_.end())
    return CTkAppFontRef();

  return (*p).second;
}

QFont
CTkApp::
getQFont(const QVariant &var) const
{
  auto name = variantToString(var);

  auto font = getFont(name);

  QFont qfont;

  if (font)
    return font->getQFont();

  CTkAppUtil::variantToQFont(const_cast<CTkApp *>(this), var, qfont);

  return qfont;
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

CTkAppCursor *
CTkApp::
getCursor(const QString &name)
{
  return cursorMgr_->getCursor(name);
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
            command1 += data.keyStr;
            break;
          case 'D': // delta
            command1 += ""; // TODO
            break;
          case 'K': // key (keysym)
            command1 += data.keyStr;
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

  evalNoExcept(command1);

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
installToplevel(const QString &id, CTkAppTopLevelWidget *qtoplevel)
{
  static CTkAppOpt opts[] = {
    { nullptr, nullptr, nullptr, nullptr }
  };

  auto widgetName = "." + id;

  auto *w = lookupWidgetByName(widgetName, /*quiet*/true);
  if (w) { throwError("widget already exists"); return nullptr; }

  auto *toplevel = new CTkAppTopLevel(this, root(), id);

  toplevel->setTopLevelWidget(qtoplevel);

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, toplevel, opts);
  assert(cmd);

  return toplevel;
}

void
CTkApp::
addTopLevel(CTkAppTopLevel *toplevel)
{
  toplevels_.insert(toplevel);
}

void
CTkApp::
removeTopLevel(CTkAppTopLevel *toplevel)
{
  toplevels_.erase(toplevel);
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
  for (const auto &tw : widgets_) {
    if (! tw || tw->isDeleted())
      continue;

    if (tw->getQWidget() == w)
      return tw;
  }

  return nullptr;
}

CTkAppWidget *
CTkApp::
lookupWidgetId(ulong id) const
{
  for (const auto &tw : widgets_) {
    if (! tw || tw->isDeleted())
      continue;

    if (tw->getQWidget()->winId() == id)
      return tw;
  }

  return nullptr;
}

CTkAppWidget *
CTkApp::
getWidgetAt(long x, long y) const
{
  using Widgets      = std::vector<CTkAppWidget *>;
  using DepthWidgets = std::map<int, Widgets>;

  DepthWidgets depthWidgets;

  auto p = QPoint(x, y);

  for (const auto &tw : widgets_) {
    if (! tw || tw->isDeleted())
      continue;

    auto rect = tw->getQWidget()->rect();
    auto tl   = tw->getQWidget()->mapToGlobal(rect.topLeft());

    auto r = QRect(tl.x(), tl.y(), rect.width(), rect.height());

    if (r.contains(p))
      depthWidgets[tw->parentDepth()].push_back(tw);
  }

  if (depthWidgets.empty())
    return nullptr;

  return (*depthWidgets.rbegin()).second.front();
}

CTkAppWidget *
CTkApp::
getWidgetForWin(ulong xwin) const
{
  for (const auto &tw : widgets_) {
    if (! tw || tw->isDeleted())
      continue;

    auto xwin1 = tw->getQWidget()->winId();

    if (xwin1 == xwin)
      return tw;
  }

  return nullptr;
}

//---

void
CTkApp::
removeWidget(CTkAppWidget *w)
{
  auto p = widgets_.find(w);

  if (p != widgets_.end())
    widgets_.erase(p);

  if (w->isTopLevel()) {
    auto *topWidget = qobject_cast<CTkAppTopLevel *>(w);

    removeTopLevel(topWidget);
  }
}

uint
CTkApp::
numDeleteWidgets() const
{
  return deleteWidgets_.size();
}

void
CTkApp::
addDeleteWidget(CTkAppWidget *w)
{
  for (const auto &dw : deleteWidgets_) {
    if (w == dw)
      return;
  }

  deleteWidgets_.push_back(w);
}

void
CTkApp::
purgeWidgets()
{
  if (deleteWidgets_.empty())
    return;

  WidgetArray deleteWidgets;

  std::swap(deleteWidgets, deleteWidgets_);

  auto num = deleteWidgets.size();

  for (uint i = 0; i < num; ++i) {
    if (deleteWidgets[i])
      delete deleteWidgets[i];
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
    data.keyStr = char('a' + (key - Qt::Key_A));
  else if (key >= Qt::Key_1 && key <= Qt::Key_9)
    data.keyStr = char('0' + (key - Qt::Key_0));
  else
    data.keyStr  = e->text();

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
  data.x      = e->x();
  data.y      = e->y();

  if (e->modifiers() & Qt::ControlModifier)
    data.modifiers |= uint(CTkAppEventModifier::Control);

  if (e->modifiers() & Qt::ShiftModifier)
    data.modifiers |= uint(CTkAppEventModifier::Shift);

  if (e->modifiers() & Qt::AltModifier)
    data.modifiers |= uint(CTkAppEventModifier::Alt);
}

bool
CTkApp::
stringToVirtualEvent(const QVariant &var, CTkAppVirtualEventData &data, bool quiet) const
{
  auto str = variantToString(var);

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
parseEvent(const QVariant &var, CTkAppEventData &data)
{
  auto pattern = variantToString(var);

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
    else if (parse.isString("Destroy")) {
      parse.skipLastString();

      data.type = CTkAppEventType::Destroy;

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
          data.keyStr += parse.getChar().toLatin1();

        if (! parseClose('>'))
          return false;
      }
      else {
        data.anyKey = true;
        data.keyStr = ""; // Any ?
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
        data.keyStr += parse.getChar().toLatin1();

      if (! parseClose('>'))
        return false;
    }
  }

  return true;
}

int
CTkApp::
stringToKeySym(const QString &key) const
{
  struct NameKeySym {
    const char *name;
    int         key;
  };

  static NameKeySym nameKeySyms[] = {
    { "BackSpace"                  , Qt::Key_Backspace },
    { "Tab"                        , Qt::Key_Tab },
//  { "Linefeed"                   , 0xFF0A },
    { "Clear"                      , Qt::Key_Clear },
    { "Return"                     , Qt::Key_Return },
    { "Pause"                      , Qt::Key_Pause },
    { "Scroll_Lock"                , Qt::Key_ScrollLock },
    { "Sys_Req"                    , Qt::Key_SysReq },
    { "Escape"                     , Qt::Key_Escape },
//  { "Multi_key"                  , 0xFF20 },
#if 0
    { "Kanji"                      , 0xFF21 },
    { "Muhenkan"                   , 0xFF22 },
    { "Henkan_Mode"                , 0xFF23 },
    { "Henkan"                     , 0xFF23 },
    { "Romaji"                     , 0xFF24 },
    { "Hiragana"                   , 0xFF25 },
    { "Katakana"                   , 0xFF26 },
    { "Hiragana_Katakana"          , 0xFF27 },
    { "Zenkaku"                    , 0xFF28 },
    { "Hankaku"                    , 0xFF29 },
    { "Zenkaku_Hankaku"            , 0xFF2A },
    { "Touroku"                    , 0xFF2B },
    { "Massyo"                     , 0xFF2C },
    { "Kana_Lock"                  , 0xFF2D },
    { "Kana_Shift"                 , 0xFF2E },
    { "Eisu_Shift"                 , 0xFF2F },
    { "Eisu_toggle"                , 0xFF30 },
    { "Hangul"                     , 0xFF31 },
    { "Hangul_Start"               , 0xFF32 },
    { "Hangul_End"                 , 0xFF33 },
    { "Hangul_Hanja"               , 0xFF34 },
    { "Hangul_Jamo"                , 0xFF35 },
    { "Hangul_Romaja"              , 0xFF36 },
    { "Codeinput"                  , 0xFF37 },
    { "Hangul_Jeonja"              , 0xFF38 },
    { "Hangul_Banja"               , 0xFF39 },
    { "Hangul_PreHanja"            , 0xFF3A },
    { "Hangul_PostHanja"           , 0xFF3B },
    { "SingleCandidate"            , 0xFF3C },
    { "MultipleCandidate"          , 0xFF3D },
    { "PreviousCandidate"          , 0xFF3E },
    { "Hangul_Special"             , 0xFF3F },
#endif
    { "Home"                       , Qt::Key_Home },
    { "Left"                       , Qt::Key_Left },
    { "Up"                         , Qt::Key_Up },
    { "Right"                      , Qt::Key_Right },
    { "Down"                       , Qt::Key_Down },
//  { "Prior"                      , 0xFF55 },
    { "Page_Up"                    , Qt::Key_PageUp },
//  { "Next"                       , 0xFF56 },
    { "Page_Down"                  , Qt::Key_PageDown },
    { "End"                        , Qt::Key_End },
//  { "Begin"                      , 0xFF58 },
    { "Select"                     , Qt::Key_Select },
    { "Print"                      , Qt::Key_Print },
    { "Execute"                    , Qt::Key_Execute },
    { "Insert"                     , Qt::Key_Insert },
    { "Undo"                       , Qt::Key_Undo },
    { "Redo"                       , Qt::Key_Redo },
    { "Menu"                       , Qt::Key_Menu },
#if 0
#ifndef TK_NO_DEPRECATED
    { "App"                        , 0xFF67 },
#endif
#endif
    { "Find"                       , Qt::Key_Find },
    { "Cancel"                     , Qt::Key_Cancel },
    { "Help"                       , Qt::Key_Help },
//  { "Break"                      , 0xFF6B },
//  { "Mode_switch"                , 0xFF7E },
#if 0
#ifndef TK_NO_DEPRECATED
    { "script_switch"              , 0xFF7E },
    { "kana_switch"                , 0xFF7E },
    { "Arabic_switch"              , 0xFF7E },
    { "Greek_switch"               , 0xFF7E },
    { "Hebrew_switch"              , 0xFF7E },
#endif
#endif
    { "Num_Lock"                   , Qt::Key_NumLock },
    { "KP_Space"                   , Qt::Key_Space },
    { "KP_Tab"                     , Qt::Key_Tab },
    { "KP_Enter"                   , Qt::Key_Enter },
    { "KP_F1"                      , Qt::Key_F1 },
    { "KP_F2"                      , Qt::Key_F2 },
    { "KP_F3"                      , Qt::Key_F3 },
    { "KP_F4"                      , Qt::Key_F4 },
    { "KP_Home"                    , Qt::Key_Home },
    { "KP_Left"                    , Qt::Key_Left },
    { "KP_Up"                      , Qt::Key_Up },
    { "KP_Right"                   , Qt::Key_Right },
    { "KP_Down"                    , Qt::Key_Down },
//  { "KP_Prior"                   , 0xFF9A },
    { "KP_Page_Up"                 , Qt::Key_PageUp },
//  { "KP_Next"                    , 0xFF9B },
    { "KP_Page_Down"               , Qt::Key_PageDown },
    { "KP_End"                     , Qt::Key_End },
//  { "KP_Begin"                   , 0xFF9D },
    { "KP_Insert"                  , Qt::Key_Insert },
    { "KP_Delete"                  , Qt::Key_Delete },
    { "KP_Multiply"                , Qt::Key_multiply },
//  { "KP_Add"                     , 0xFFAB },
//  { "KP_Separator"               , 0xFFAC },
//  { "KP_Subtract"                , 0xFFAD },
//  { "KP_Decimal"                 , 0xFFAE },
//  { "KP_Divide"                  , 0xFFAF },
    { "KP_0"                       , Qt::Key_0 },
    { "KP_1"                       , Qt::Key_1 },
    { "KP_2"                       , Qt::Key_2 },
    { "KP_3"                       , Qt::Key_3 },
    { "KP_4"                       , Qt::Key_4 },
    { "KP_5"                       , Qt::Key_5 },
    { "KP_6"                       , Qt::Key_6 },
    { "KP_7"                       , Qt::Key_7 },
    { "KP_8"                       , Qt::Key_8 },
    { "KP_9"                       , Qt::Key_9 },
    { "KP_Equal"                   , Qt::Key_Equal },
    { "F1"                         , Qt::Key_F1 },
    { "F2"                         , Qt::Key_F2 },
    { "F3"                         , Qt::Key_F3 },
    { "F4"                         , Qt::Key_F4 },
    { "F5"                         , Qt::Key_F5 },
    { "F6"                         , Qt::Key_F6 },
    { "F7"                         , Qt::Key_F7 },
    { "F8"                         , Qt::Key_F8 },
    { "F9"                         , Qt::Key_F9 },
    { "F10"                        , Qt::Key_F10 },
    { "F11"                        , Qt::Key_F11 },
//  { "L1"                         , 0xFFC8 },
    { "F12"                        , Qt::Key_F12 },
//  { "L2"                         , 0xFFC9 },
    { "F13"                        , Qt::Key_F13 },
//  { "L3"                         , 0xFFCA },
    { "F14"                        , Qt::Key_F14 },
//  { "L4"                         , 0xFFCB },
    { "F15"                        , Qt::Key_F15 },
//  { "L5"                         , 0xFFCC },
    { "F16"                        , Qt::Key_F16 },
//  { "L6"                         , 0xFFCD },
    { "F17"                        , Qt::Key_F17 },
//  { "L7"                         , 0xFFCE },
    { "F18"                        , Qt::Key_F18 },
//  { "L8"                         , 0xFFCF },
    { "F19"                        , Qt::Key_F19 },
//  { "L9"                         , 0xFFD0 },
    { "F20"                        , Qt::Key_F20 },
//  { "L10"                        , 0xFFD1 },
    { "F21"                        , Qt::Key_F21 },
//  { "R1"                         , 0xFFD2 },
    { "F22"                        , Qt::Key_F22 },
//  { "R2"                         , 0xFFD3 },
    { "F23"                        , Qt::Key_F23 },
//  { "R3"                         , 0xFFD4 },
    { "F24"                        , Qt::Key_F24 },
//  { "R4"                         , 0xFFD5 },
    { "F25"                        , Qt::Key_F25 },
//  { "R5"                         , 0xFFD6 },
    { "F26"                        , Qt::Key_F26 },
//  { "R6"                         , 0xFFD7 },
    { "F27"                        , Qt::Key_F27 },
//  { "R7"                         , 0xFFD8 },
    { "F28"                        , Qt::Key_F28 },
//  { "R8"                         , 0xFFD9 },
    { "F29"                        , Qt::Key_F29 },
//  { "R9"                         , 0xFFDA },
    { "F30"                        , Qt::Key_F30 },
//  { "R10"                        , 0xFFDB },
    { "F31"                        , Qt::Key_F31 },
//  { "R11"                        , 0xFFDC },
    { "F32"                        , Qt::Key_F32 },
//  { "R12"                        , 0xFFDD },
    { "F33"                        , Qt::Key_F33 },
//  { "R13"                        , 0xFFDE },
    { "F34"                        , Qt::Key_F34 },
//  { "R14"                        , 0xFFDF },
    { "F35"                        , Qt::Key_F35 },
//  { "R15"                        , 0xFFE0 },
//  { "Shift_L"                    , 0xFFE1 },
//  { "Shift_R"                    , 0xFFE2 },
//  { "Control_L"                  , 0xFFE3 },
//  { "Control_R"                  , 0xFFE4 },
    { "Caps_Lock"                  , Qt::Key_CapsLock },
//  { "Shift_Lock"                 , 0xFFE6 },
//  { "Meta_L"                     , 0xFFE7 },
//  { "Meta_R"                     , 0xFFE8 },
//  { "Alt_L"                      , 0xFFE9 },
//  { "Alt_R"                      , 0xFFEA },
//  { "Super_L"                    , 0xFFEB },
#ifndef TK_NO_DEPRECATED
//  { "Win_L"                      , 0xFFEB },
#endif
//  { "Super_R"                    , 0xFFEC },
#ifndef TK_NO_DEPRECATED
//  { "Win_R"                      , 0xFFEC },
#endif
 // { "Hyper_L"                    , 0xFFED },
//  { "Hyper_R"                    , 0xFFEE },
#if 0
    { "braille_dot_1"              , 0xFFF1 },
    { "braille_dot_2"              , 0xFFF2 },
    { "braille_dot_3"              , 0xFFF3 },
    { "braille_dot_4"              , 0xFFF4 },
    { "braille_dot_5"              , 0xFFF5 },
    { "braille_dot_6"              , 0xFFF6 },
    { "braille_dot_7"              , 0xFFF7 },
    { "braille_dot_8"              , 0xFFF8 },
    { "braille_dot_9"              , 0xFFF9 },
    { "braille_dot_10"             , 0xFFFA },
    { "Delete"                     , 0xFFFF },
    { "ISO_Lock"                   , 0xFE01 },
    { "ISO_Level2_Latch"           , 0xFE02 },
    { "ISO_Level3_Shift"           , 0xFE03 },
    { "ISO_Level3_Latch"           , 0xFE04 },
    { "ISO_Level3_Lock"            , 0xFE05 },
    { "ISO_Group_Latch"            , 0xFE06 },
    { "ISO_Group_Lock"             , 0xFE07 },
    { "ISO_Next_Group"             , 0xFE08 },
    { "ISO_Next_Group_Lock"        , 0xFE09 },
    { "ISO_Prev_Group"             , 0xFE0A },
    { "ISO_Prev_Group_Lock"        , 0xFE0B },
    { "ISO_First_Group"            , 0xFE0C },
    { "ISO_First_Group_Lock"       , 0xFE0D },
    { "ISO_Last_Group"             , 0xFE0E },
    { "ISO_Last_Group_Lock"        , 0xFE0F },
    { "ISO_Level5_Shift"           , 0xFE11 },
    { "ISO_Level5_Latch"           , 0xFE12 },
    { "ISO_Level5_Lock"            , 0xFE13 },
    { "ISO_Left_Tab"               , 0xFE20 },
    { "ISO_Move_Line_Up"           , 0xFE21 },
    { "ISO_Move_Line_Down"         , 0xFE22 },
    { "ISO_Partial_Line_Up"        , 0xFE23 },
    { "ISO_Partial_Line_Down"      , 0xFE24 },
    { "ISO_Partial_Space_Left"     , 0xFE25 },
    { "ISO_Partial_Space_Right"    , 0xFE26 },
    { "ISO_Set_Margin_Left"        , 0xFE27 },
    { "ISO_Set_Margin_Right"       , 0xFE28 },
    { "ISO_Release_Margin_Left"    , 0xFE29 },
    { "ISO_Release_Margin_Right"   , 0xFE2A },
    { "ISO_Release_Both_Margins"   , 0xFE2B },
    { "ISO_Fast_Cursor_Left"       , 0xFE2C },
    { "ISO_Fast_Cursor_Right"      , 0xFE2D },
    { "ISO_Fast_Cursor_Up"         , 0xFE2E },
    { "ISO_Fast_Cursor_Down"       , 0xFE2F },
    { "ISO_Continuous_Underline"   , 0xFE30 },
    { "ISO_Discontinuous_Underline", 0xFE31 },
    { "ISO_Emphasize"              , 0xFE32 },
    { "ISO_Center_Object"          , 0xFE33 },
    { "ISO_Enter"                  , 0xFE34 },
    { "dead_grave"                 , 0xFE50 },
    { "dead_acute"                 , 0xFE51 },
    { "dead_circumflex"            , 0xFE52 },
    { "dead_tilde"                 , 0xFE53 },
    { "dead_perispomeni"           , 0xFE53 },
    { "dead_macron"                , 0xFE54 },
    { "dead_breve"                 , 0xFE55 },
    { "dead_abovedot"              , 0xFE56 },
    { "dead_diaeresis"             , 0xFE57 },
    { "dead_abovering"             , 0xFE58 },
    { "dead_doubleacute"           , 0xFE59 },
    { "dead_caron"                 , 0xFE5A },
    { "dead_cedilla"               , 0xFE5B },
    { "dead_ogonek"                , 0xFE5C },
    { "dead_iota"                  , 0xFE5D },
    { "dead_voiced_sound"          , 0xFE5E },
    { "dead_semivoiced_sound"      , 0xFE5F },
    { "dead_belowdot"              , 0xFE60 },
    { "dead_hook"                  , 0xFE61 },
    { "dead_horn"                  , 0xFE62 },
    { "dead_stroke"                , 0xFE63 },
    { "dead_abovecomma"            , 0xFE64 },
    { "dead_psili"                 , 0xFE64 },
    { "dead_abovereversedcomma"    , 0xFE65 },
    { "dead_dasia"                 , 0xFE65 },
    { "dead_doublegrave"           , 0xFE66 },
    { "dead_belowring"             , 0xFE67 },
    { "dead_belowmacron"           , 0xFE68 },
    { "dead_belowcircumflex"       , 0xFE69 },
    { "dead_belowtilde"            , 0xFE6A },
    { "dead_belowbreve"            , 0xFE6B },
    { "dead_belowdiaeresis"        , 0xFE6C },
    { "dead_invertedbreve"         , 0xFE6D },
    { "dead_belowcomma"            , 0xFE6E },
    { "dead_currency"              , 0xFE6F },
    { "AccessX_Enable"             , 0xFE70 },
    { "AccessX_Feedback_Enable"    , 0xFE71 },
    { "RepeatKeys_Enable"          , 0xFE72 },
    { "SlowKeys_Enable"            , 0xFE73 },
    { "BounceKeys_Enable"          , 0xFE74 },
    { "StickyKeys_Enable"          , 0xFE75 },
    { "MouseKeys_Enable"           , 0xFE76 },
    { "MouseKeys_Accel_Enable"     , 0xFE77 },
    { "Overlay1_Enable"            , 0xFE78 },
    { "Overlay2_Enable"            , 0xFE79 },
    { "AudibleBell_Enable"         , 0xFE7A },
    { "dead_a"                     , 0xFE80 },
    { "dead_A"                     , 0xFE81 },
    { "dead_e"                     , 0xFE82 },
    { "dead_E"                     , 0xFE83 },
    { "dead_i"                     , 0xFE84 },
    { "dead_I"                     , 0xFE85 },
    { "dead_o"                     , 0xFE86 },
    { "dead_O"                     , 0xFE87 },
    { "dead_u"                     , 0xFE88 },
    { "dead_U"                     , 0xFE89 },
    { "dead_small_schwa"           , 0xFE8A },
    { "dead_capital_schwa"         , 0xFE8B },
    { "dead_greek"                 , 0xFE8C },
    { "dead_lowline"               , 0xFE90 },
    { "dead_aboveverticalline"     , 0xFE91 },
    { "dead_belowverticalline"     , 0xFE92 },
    { "dead_longsolidusoverlay"    , 0xFE93 },
    { "ch"                         , 0xFEA0 },
    { "Ch"                         , 0xFEA1 },
    { "CH"                         , 0xFEA2 },
    { "c_h"                        , 0xFEA3 },
    { "C_h"                        , 0xFEA4 },
    { "C_H"                        , 0xFEA5 },
    { "First_Virtual_Screen"       , 0xFED0 },
    { "Prev_Virtual_Screen"        , 0xFED1 },
    { "Next_Virtual_Screen"        , 0xFED2 },
    { "Last_Virtual_Screen"        , 0xFED4 },
    { "Terminate_Server"           , 0xFED5 },
    { "Pointer_Left"               , 0xFEE0 },
    { "Pointer_Right"              , 0xFEE1 },
    { "Pointer_Up"                 , 0xFEE2 },
    { "Pointer_Down"               , 0xFEE3 },
    { "Pointer_UpLeft"             , 0xFEE4 },
    { "Pointer_UpRight"            , 0xFEE5 },
    { "Pointer_DownLeft"           , 0xFEE6 },
    { "Pointer_DownRight"          , 0xFEE7 },
    { "Pointer_Button_Dflt"        , 0xFEE8 },
    { "Pointer_Button1"            , 0xFEE9 },
    { "Pointer_Button2"            , 0xFEEA },
    { "Pointer_Button3"            , 0xFEEB },
    { "Pointer_Button4"            , 0xFEEC },
    { "Pointer_Button5"            , 0xFEED },
    { "Pointer_DblClick_Dflt"      , 0xFEEE },
    { "Pointer_DblClick1"          , 0xFEEF },
    { "Pointer_DblClick2"          , 0xFEF0 },
    { "Pointer_DblClick3"          , 0xFEF1 },
    { "Pointer_DblClick4"          , 0xFEF2 },
    { "Pointer_DblClick5"          , 0xFEF3 },
    { "Pointer_Drag_Dflt"          , 0xFEF4 },
    { "Pointer_Drag1"              , 0xFEF5 },
    { "Pointer_Drag2"              , 0xFEF6 },
    { "Pointer_Drag3"              , 0xFEF7 },
    { "Pointer_Drag4"              , 0xFEF8 },
    { "Pointer_EnableKeys"         , 0xFEF9 },
    { "Pointer_Accelerate"         , 0xFEFA },
    { "Pointer_DfltBtnNext"        , 0xFEFB },
    { "Pointer_DfltBtnPrev"        , 0xFEFC },
    { "Pointer_Drag5"              , 0xFEFD },
#endif
    { "space"                      , 0x20 },
#ifndef TK_NO_DEPRECATED
    { "exclam"                     , 0x21 },
#endif
    { "quotedbl"                   , 0x22 },
    { "numbersign"                 , 0x23 },
    { "dollar"                     , 0x24 },
#ifndef TK_NO_DEPRECATED
    { "percent"                    , 0x25 },
    { "ampersand"                  , 0x26 },
    { "apostrophe"                 , 0x27 },
    { "quoteright"                 , 0x27 },
    { "parenleft"                  , 0x28 },
    { "parenright"                 , 0x29 },
    { "asterisk"                   , 0x2A },
    { "plus"                       , 0x2B },
    { "comma"                      , 0x2C },
#endif
    { "minus"                      , 0x2D },
#ifndef TK_NO_DEPRECATED
    { "period"                     , 0x2E },
    { "slash"                      , 0x2F },
    { "0"                          , 0x30 },
    { "1"                          , 0x31 },
    { "2"                          , 0x32 },
    { "3"                          , 0x33 },
    { "4"                          , 0x34 },
    { "5"                          , 0x35 },
    { "6"                          , 0x36 },
    { "7"                          , 0x37 },
    { "8"                          , 0x38 },
    { "9"                          , 0x39 },
    { "colon"                      , 0x3A },
#endif
    { "semicolon"                  , 0x3B },
    { "less"                       , 0x3C },
#ifndef TK_NO_DEPRECATED
    { "equal"                      , 0x3D },
#endif
    { "greater"                    , 0x3E },
#ifndef TK_NO_DEPRECATED
    { "question"                   , 0x3F },
    { "at"                         , 0x40 },
    { "A"                          , 0x41 },
    { "B"                          , 0x42 },
    { "C"                          , 0x43 },
    { "D"                          , 0x44 },
    { "E"                          , 0x45 },
    { "F"                          , 0x46 },
    { "G"                          , 0x47 },
    { "H"                          , 0x48 },
    { "I"                          , 0x49 },
    { "J"                          , 0x4A },
    { "K"                          , 0x4B },
    { "L"                          , 0x4C },
    { "M"                          , 0x4D },
    { "N"                          , 0x4E },
    { "O"                          , 0x4F },
    { "P"                          , 0x50 },
    { "Q"                          , 0x51 },
    { "R"                          , 0x52 },
    { "S"                          , 0x53 },
    { "T"                          , 0x54 },
    { "U"                          , 0x55 },
    { "V"                          , 0x56 },
    { "W"                          , 0x57 },
    { "X"                          , 0x58 },
    { "Y"                          , 0x59 },
    { "Z"                          , 0x5A },
#endif
    { "bracketleft"                , 0x5B },
    { "backslash"                  , 0x5C },
    { "bracketright"               , 0x5D },
#ifndef TK_NO_DEPRECATED
    { "asciicircum"                , 0x5E },
    { "underscore"                 , 0x5F },
    { "grave"                      , 0x60 },
    { "quoteleft"                  , 0x60 },
    { "a"                          , 0x61 },
    { "b"                          , 0x62 },
    { "c"                          , 0x63 },
    { "d"                          , 0x64 },
    { "e"                          , 0x65 },
    { "f"                          , 0x66 },
    { "g"                          , 0x67 },
    { "h"                          , 0x68 },
    { "i"                          , 0x69 },
    { "j"                          , 0x6A },
    { "k"                          , 0x6B },
    { "l"                          , 0x6C },
    { "m"                          , 0x6D },
    { "n"                          , 0x6E },
    { "o"                          , 0x6F },
    { "p"                          , 0x70 },
    { "q"                          , 0x71 },
    { "r"                          , 0x72 },
    { "s"                          , 0x73 },
    { "t"                          , 0x74 },
    { "u"                          , 0x75 },
    { "v"                          , 0x76 },
    { "w"                          , 0x77 },
    { "x"                          , 0x78 },
    { "y"                          , 0x79 },
    { "z"                          , 0x7A },
#endif
    { "braceleft"                  , 0x7B },
#ifndef TK_NO_DEPRECATED
    { "bar"                        , 0x7C },
#endif
    { "braceright"                 , 0x7D },
#ifndef TK_NO_DEPRECATED
    { "asciitilde"                 , 0x7E },
#endif
    { "nobreakspace"               , 0xA0 },
#ifndef TK_NO_DEPRECATED
    { "exclamdown"                 , 0xA1 },
    { "cent"                       , 0xA2 },
    { "sterling"                   , 0xA3 },
    { "currency"                   , 0xA4 },
    { "yen"                        , 0xA5 },
    { "brokenbar"                  , 0xA6 },
    { "section"                    , 0xA7 },
    { "diaeresis"                  , 0xA8 },
    { "copyright"                  , 0xA9 },
    { "ordfeminine"                , 0xAA },
    { "guillemotleft"              , 0xAB },
    { "notsign"                    , 0xAC },
    { "hyphen"                     , 0xAD },
    { "registered"                 , 0xAE },
    { "macron"                     , 0xAF },
    { "degree"                     , 0xB0 },
    { "plusminus"                  , 0xB1 },
    { "twosuperior"                , 0xB2 },
    { "threesuperior"              , 0xB3 },
    { "acute"                      , 0xB4 },
    { "mu"                         , 0xB5 },
    { "paragraph"                  , 0xB6 },
    { "periodcentered"             , 0xB7 },
    { "cedilla"                    , 0xB8 },
    { "onesuperior"                , 0xB9 },
    { "masculine"                  , 0xBA },
    { "guillemotright"             , 0xBB },
    { "onequarter"                 , 0xBC },
    { "onehalf"                    , 0xBD },
    { "threequarters"              , 0xBE },
    { "questiondown"               , 0xBF },
    { "Agrave"                     , 0xC0 },
    { "Aacute"                     , 0xC1 },
    { "Acircumflex"                , 0xC2 },
    { "Atilde"                     , 0xC3 },
    { "Adiaeresis"                 , 0xC4 },
    { "Aring"                      , 0xC5 },
    { "AE"                         , 0xC6 },
    { "Ccedilla"                   , 0xC7 },
    { "Egrave"                     , 0xC8 },
    { "Eacute"                     , 0xC9 },
    { "Ecircumflex"                , 0xCA },
    { "Ediaeresis"                 , 0xCB },
    { "Igrave"                     , 0xCC },
    { "Iacute"                     , 0xCD },
    { "Icircumflex"                , 0xCE },
    { "Idiaeresis"                 , 0xCF },
    { "ETH"                        , 0xD0 },
    { "Eth"                        , 0xD0 },
    { "Ntilde"                     , 0xD1 },
    { "Ograve"                     , 0xD2 },
    { "Oacute"                     , 0xD3 },
    { "Ocircumflex"                , 0xD4 },
    { "Otilde"                     , 0xD5 },
    { "Odiaeresis"                 , 0xD6 },
    { "multiply"                   , 0xD7 },
    { "Oslash"                     , 0xD8 },
#ifndef TK_NO_DEPRECATED
    { "Ooblique"                   , 0xD8 },
#endif
    { "Ugrave"                     , 0xD9 },
    { "Uacute"                     , 0xDA },
    { "Ucircumflex"                , 0xDB },
    { "Udiaeresis"                 , 0xDC },
    { "Yacute"                     , 0xDD },
    { "THORN"                      , 0xDE },
    { "Thorn"                      , 0xDE },
    { "ssharp"                     , 0xDF },
    { "agrave"                     , 0xE0 },
    { "aacute"                     , 0xE1 },
    { "acircumflex"                , 0xE2 },
    { "atilde"                     , 0xE3 },
    { "adiaeresis"                 , 0xE4 },
    { "aring"                      , 0xE5 },
    { "ae"                         , 0xE6 },
    { "ccedilla"                   , 0xE7 },
    { "egrave"                     , 0xE8 },
    { "eacute"                     , 0xE9 },
    { "ecircumflex"                , 0xEA },
    { "ediaeresis"                 , 0xEB },
    { "igrave"                     , 0xEC },
    { "iacute"                     , 0xED },
    { "icircumflex"                , 0xEE },
    { "idiaeresis"                 , 0xEF },
    { "eth"                        , 0xF0 },
    { "ntilde"                     , 0xF1 },
    { "ograve"                     , 0xF2 },
    { "oacute"                     , 0xF3 },
    { "ocircumflex"                , 0xF4 },
    { "otilde"                     , 0xF5 },
    { "odiaeresis"                 , 0xF6 },
    { "division"                   , 0xF7 },
    { "oslash"                     , 0xF8 },
    { "ugrave"                     , 0xF9 },
    { "uacute"                     , 0xFA },
    { "ucircumflex"                , 0xFB },
    { "udiaeresis"                 , 0xFC },
    { "yacute"                     , 0xFD },
    { "thorn"                      , 0xFE },
    { "ydiaeresis"                 , 0xFF },
#endif /* TK_NO_DEPRECATED */
#if 0
    { "Aogonek"                    , 0x1A1 },
    { "breve"                      , 0x1A2 },
    { "Lstroke"                    , 0x1A3 },
    { "Lcaron"                     , 0x1A5 },
    { "Sacute"                     , 0x1A6 },
    { "Scaron"                     , 0x1A9 },
    { "Scedilla"                   , 0x1AA },
    { "Tcaron"                     , 0x1AB },
    { "Zacute"                     , 0x1AC },
    { "Zcaron"                     , 0x1AE },
    { "Zabovedot"                  , 0x1AF },
    { "aogonek"                    , 0x1B1 },
    { "ogonek"                     , 0x1B2 },
    { "lstroke"                    , 0x1B3 },
    { "lcaron"                     , 0x1B5 },
    { "sacute"                     , 0x1B6 },
    { "caron"                      , 0x1B7 },
    { "scaron"                     , 0x1B9 },
    { "scedilla"                   , 0x1BA },
    { "tcaron"                     , 0x1BB },
    { "zacute"                     , 0x1BC },
    { "doubleacute"                , 0x1BD },
    { "zcaron"                     , 0x1BE },
    { "zabovedot"                  , 0x1BF },
    { "Racute"                     , 0x1C0 },
    { "Abreve"                     , 0x1C3 },
    { "Lacute"                     , 0x1C5 },
    { "Cacute"                     , 0x1C6 },
    { "Ccaron"                     , 0x1C8 },
    { "Eogonek"                    , 0x1CA },
    { "Ecaron"                     , 0x1CC },
    { "Dcaron"                     , 0x1CF },
    { "Dstroke"                    , 0x1D0 },
    { "Nacute"                     , 0x1D1 },
    { "Ncaron"                     , 0x1D2 },
    { "Odoubleacute"               , 0x1D5 },
    { "Rcaron"                     , 0x1D8 },
    { "Uring"                      , 0x1D9 },
    { "Udoubleacute"               , 0x1DB },
    { "Tcedilla"                   , 0x1DE },
    { "racute"                     , 0x1E0 },
    { "abreve"                     , 0x1E3 },
    { "lacute"                     , 0x1E5 },
    { "cacute"                     , 0x1E6 },
    { "ccaron"                     , 0x1E8 },
    { "eogonek"                    , 0x1EA },
    { "ecaron"                     , 0x1EC },
    { "dcaron"                     , 0x1EF },
    { "dstroke"                    , 0x1F0 },
    { "nacute"                     , 0x1F1 },
    { "ncaron"                     , 0x1F2 },
    { "odoubleacute"               , 0x1F5 },
    { "rcaron"                     , 0x1F8 },
    { "uring"                      , 0x1F9 },
    { "udoubleacute"               , 0x1FB },
    { "tcedilla"                   , 0x1FE },
    { "abovedot"                   , 0x1FF },
    { "Hstroke"                    , 0x2A1 },
    { "Hcircumflex"                , 0x2A6 },
    { "Iabovedot"                  , 0x2A9 },
    { "Gbreve"                     , 0x2AB },
    { "Jcircumflex"                , 0x2AC },
    { "hstroke"                    , 0x2B1 },
    { "hcircumflex"                , 0x2B6 },
    { "idotless"                   , 0x2B9 },
    { "gbreve"                     , 0x2BB },
    { "jcircumflex"                , 0x2BC },
    { "Cabovedot"                  , 0x2C5 },
    { "Ccircumflex"                , 0x2C6 },
    { "Gabovedot"                  , 0x2D5 },
    { "Gcircumflex"                , 0x2D8 },
    { "Ubreve"                     , 0x2DD },
    { "Scircumflex"                , 0x2DE },
    { "cabovedot"                  , 0x2E5 },
    { "ccircumflex"                , 0x2E6 },
    { "gabovedot"                  , 0x2F5 },
    { "gcircumflex"                , 0x2F8 },
    { "ubreve"                     , 0x2FD },
    { "scircumflex"                , 0x2FE },
    { "kra"                        , 0x3A2 },
#ifndef TK_NO_DEPRECATED
    { "kappa"                      , 0x3A2 },
#endif
    { "Rcedilla"                   , 0x3A3 },
    { "Itilde"                     , 0x3A5 },
    { "Lcedilla"                   , 0x3A6 },
    { "Emacron"                    , 0x3AA },
    { "Gcedilla"                   , 0x3AB },
    { "Tslash"                     , 0x3AC },
    { "rcedilla"                   , 0x3B3 },
    { "itilde"                     , 0x3B5 },
    { "lcedilla"                   , 0x3B6 },
    { "emacron"                    , 0x3BA },
    { "gcedilla"                   , 0x3BB },
#ifndef TK_NO_DEPRECATED
    { "gacute"                     , 0x3BB },
#endif
    { "tslash"                     , 0x3BC },
    { "ENG"                        , 0x3BD },
    { "eng"                        , 0x3BF },
    { "Amacron"                    , 0x3C0 },
    { "Iogonek"                    , 0x3C7 },
    { "Eabovedot"                  , 0x3CC },
    { "Imacron"                    , 0x3CF },
    { "Ncedilla"                   , 0x3D1 },
    { "Omacron"                    , 0x3D2 },
    { "Kcedilla"                   , 0x3D3 },
    { "Uogonek"                    , 0x3D9 },
    { "Utilde"                     , 0x3DD },
    { "Umacron"                    , 0x3DE },
    { "amacron"                    , 0x3E0 },
    { "iogonek"                    , 0x3E7 },
    { "eabovedot"                  , 0x3EC },
    { "imacron"                    , 0x3EF },
    { "ncedilla"                   , 0x3F1 },
    { "omacron"                    , 0x3F2 },
    { "kcedilla"                   , 0x3F3 },
    { "uogonek"                    , 0x3F9 },
    { "utilde"                     , 0x3FD },
    { "umacron"                    , 0x3FE },
    { "OE"                         , 0x13BC },
    { "oe"                         , 0x13BD },
    { "Ydiaeresis"                 , 0x13BE },
    { "overline"                   , 0x47E },
    { "kana_fullstop"              , 0x4A1 },
    { "kana_openingbracket"        , 0x4A2 },
    { "kana_closingbracket"        , 0x4A3 },
    { "kana_comma"                 , 0x4A4 },
    { "kana_conjunctive"           , 0x4A5 },
#ifndef TK_NO_DEPRECATED
    { "kana_middledot"             , 0x4A5 },
#endif
    { "kana_WO"                    , 0x4A6 },
    { "kana_a"                     , 0x4A7 },
    { "kana_i"                     , 0x4A8 },
    { "kana_u"                     , 0x4A9 },
    { "kana_e"                     , 0x4AA },
    { "kana_o"                     , 0x4AB },
    { "kana_ya"                    , 0x4AC },
    { "kana_yu"                    , 0x4AD },
    { "kana_yo"                    , 0x4AE },
    { "kana_tsu"                   , 0x4AF },
#ifndef TK_NO_DEPRECATED
    { "kana_tu"                    , 0x4AF },
#endif
    { "prolongedsound"             , 0x4B0 },
    { "kana_A"                     , 0x4B1 },
    { "kana_I"                     , 0x4B2 },
    { "kana_U"                     , 0x4B3 },
    { "kana_E"                     , 0x4B4 },
    { "kana_O"                     , 0x4B5 },
    { "kana_KA"                    , 0x4B6 },
    { "kana_KI"                    , 0x4B7 },
    { "kana_KU"                    , 0x4B8 },
    { "kana_KE"                    , 0x4B9 },
    { "kana_KO"                    , 0x4BA },
    { "kana_SA"                    , 0x4BB },
    { "kana_SHI"                   , 0x4BC },
    { "kana_SU"                    , 0x4BD },
    { "kana_SE"                    , 0x4BE },
    { "kana_SO"                    , 0x4BF },
    { "kana_TA"                    , 0x4C0 },
    { "kana_CHI"                   , 0x4C1 },
#ifndef TK_NO_DEPRECATED
    { "kana_TI"                    , 0x4C1 },
#endif
    { "kana_TSU"                   , 0x4C2 },
#ifndef TK_NO_DEPRECATED
    { "kana_TU"                    , 0x4C2 },
#endif
    { "kana_TE"                    , 0x4C3 },
    { "kana_TO"                    , 0x4C4 },
    { "kana_NA"                    , 0x4C5 },
    { "kana_NI"                    , 0x4C6 },
    { "kana_NU"                    , 0x4C7 },
    { "kana_NE"                    , 0x4C8 },
    { "kana_NO"                    , 0x4C9 },
    { "kana_HA"                    , 0x4CA },
    { "kana_HI"                    , 0x4CB },
    { "kana_FU"                    , 0x4CC },
#ifndef TK_NO_DEPRECATED
    { "kana_HU"                    , 0x4CC },
#endif
    { "kana_HE"                    , 0x4CD },
    { "kana_HO"                    , 0x4CE },
    { "kana_MA"                    , 0x4CF },
    { "kana_MI"                    , 0x4D0 },
    { "kana_MU"                    , 0x4D1 },
    { "kana_ME"                    , 0x4D2 },
    { "kana_MO"                    , 0x4D3 },
    { "kana_YA"                    , 0x4D4 },
    { "kana_YU"                    , 0x4D5 },
    { "kana_YO"                    , 0x4D6 },
    { "kana_RA"                    , 0x4D7 },
    { "kana_RI"                    , 0x4D8 },
    { "kana_RU"                    , 0x4D9 },
    { "kana_RE"                    , 0x4DA },
    { "kana_RO"                    , 0x4DB },
    { "kana_WA"                    , 0x4DC },
    { "kana_N"                     , 0x4DD },
    { "voicedsound"                , 0x4DE },
    { "semivoicedsound"            , 0x4DF },
    { "Arabic_comma"               , 0x5AC },
    { "Arabic_semicolon"           , 0x5BB },
    { "Arabic_question_mark"       , 0x5BF },
    { "Arabic_hamza"               , 0x5C1 },
    { "Arabic_maddaonalef"         , 0x5C2 },
    { "Arabic_hamzaonalef"         , 0x5C3 },
    { "Arabic_hamzaonwaw"          , 0x5C4 },
    { "Arabic_hamzaunderalef"      , 0x5C5 },
    { "Arabic_hamzaonyeh"          , 0x5C6 },
    { "Arabic_alef"                , 0x5C7 },
    { "Arabic_beh"                 , 0x5C8 },
    { "Arabic_tehmarbuta"          , 0x5C9 },
    { "Arabic_teh"                 , 0x5CA },
    { "Arabic_theh"                , 0x5CB },
    { "Arabic_jeem"                , 0x5CC },
    { "Arabic_hah"                 , 0x5CD },
    { "Arabic_khah"                , 0x5CE },
    { "Arabic_dal"                 , 0x5CF },
    { "Arabic_thal"                , 0x5D0 },
    { "Arabic_ra"                  , 0x5D1 },
    { "Arabic_zain"                , 0x5D2 },
    { "Arabic_seen"                , 0x5D3 },
    { "Arabic_sheen"               , 0x5D4 },
    { "Arabic_sad"                 , 0x5D5 },
    { "Arabic_dad"                 , 0x5D6 },
    { "Arabic_tah"                 , 0x5D7 },
    { "Arabic_zah"                 , 0x5D8 },
    { "Arabic_ain"                 , 0x5D9 },
    { "Arabic_ghain"               , 0x5DA },
    { "Arabic_tatweel"             , 0x5E0 },
    { "Arabic_feh"                 , 0x5E1 },
    { "Arabic_qaf"                 , 0x5E2 },
    { "Arabic_kaf"                 , 0x5E3 },
    { "Arabic_lam"                 , 0x5E4 },
    { "Arabic_meem"                , 0x5E5 },
    { "Arabic_noon"                , 0x5E6 },
    { "Arabic_ha"                  , 0x5E7 },
#ifndef TK_NO_DEPRECATED
    { "Arabic_heh"                 , 0x5E7 },
#endif
    { "Arabic_waw"                 , 0x5E8 },
    { "Arabic_alefmaksura"         , 0x5E9 },
    { "Arabic_yeh"                 , 0x5EA },
    { "Arabic_fathatan"            , 0x5EB },
    { "Arabic_dammatan"            , 0x5EC },
    { "Arabic_kasratan"            , 0x5ED },
    { "Arabic_fatha"               , 0x5EE },
    { "Arabic_damma"               , 0x5EF },
    { "Arabic_kasra"               , 0x5F0 },
    { "Arabic_shadda"              , 0x5F1 },
    { "Arabic_sukun"               , 0x5F2 },
    { "Serbian_dje"                , 0x6A1 },
    { "Macedonia_gje"              , 0x6A2 },
    { "Cyrillic_io"                , 0x6A3 },
    { "Ukrainian_ie"               , 0x6A4 },
#ifndef TK_NO_DEPRECATED
    { "Ukranian_je"                , 0x6A4 },
#endif
    { "Macedonia_dse"              , 0x6A5 },
    { "Ukrainian_i"                , 0x6A6 },
#ifndef TK_NO_DEPRECATED
    { "Ukranian_i"                 , 0x6A6 },
#endif
    { "Ukrainian_yi"               , 0x6A7 },
#ifndef TK_NO_DEPRECATED
    { "Ukranian_yi"                , 0x6A7 },
#endif
    { "Cyrillic_je"                , 0x6A8 },
#ifndef TK_NO_DEPRECATED
    { "Serbian_je"                 , 0x6A8 },
#endif
    { "Cyrillic_lje"               , 0x6A9 },
#ifndef TK_NO_DEPRECATED
    { "Serbian_lje"                , 0x6A9 },
#endif
    { "Cyrillic_nje"               , 0x6AA },
#ifndef TK_NO_DEPRECATED
    { "Serbian_nje"                , 0x6AA },
#endif
    { "Serbian_tshe"               , 0x6AB },
    { "Macedonia_kje"              , 0x6AC },
    { "Ukrainian_ghe_with_upturn"  , 0x6AD },
    { "Byelorussian_shortu"        , 0x6AE },
    { "Cyrillic_dzhe"              , 0x6AF },
#ifndef TK_NO_DEPRECATED
    { "Serbian_dze"                , 0x6AF },
#endif
    { "numerosign"                 , 0x6B0 },
    { "Serbian_DJE"                , 0x6B1 },
    { "Macedonia_GJE"              , 0x6B2 },
    { "Cyrillic_IO"                , 0x6B3 },
    { "Ukrainian_IE"               , 0x6B4 },
#ifndef TK_NO_DEPRECATED
    { "Ukranian_JE"                , 0x6B4 },
#endif
    { "Macedonia_DSE"              , 0x6B5 },
    { "Ukrainian_I"                , 0x6B6 },
#ifndef TK_NO_DEPRECATED
    { "Ukranian_I"                 , 0x6B6 },
#endif
    { "Ukrainian_YI"               , 0x6B7 },
#ifndef TK_NO_DEPRECATED
    { "Ukranian_YI"                , 0x6B7 },
#endif
    { "Cyrillic_JE"                , 0x6B8 },
#ifndef TK_NO_DEPRECATED
    { "Serbian_JE"                 , 0x6B8 },
#endif
    { "Cyrillic_LJE"               , 0x6B9 },
#ifndef TK_NO_DEPRECATED
    { "Serbian_LJE"                , 0x6B9 },
#endif
    { "Cyrillic_NJE"               , 0x6BA },
#ifndef TK_NO_DEPRECATED
    { "Serbian_NJE"                , 0x6BA },
#endif
    { "Serbian_TSHE"               , 0x6BB },
    { "Macedonia_KJE"              , 0x6BC },
    { "Ukrainian_GHE_WITH_UPTURN"  , 0x6BD },
    { "Byelorussian_SHORTU"        , 0x6BE },
    { "Cyrillic_DZHE"              , 0x6BF },
#ifndef TK_NO_DEPRECATED
    { "Serbian_DZE"                , 0x6BF },
#endif
    { "Cyrillic_yu"                , 0x6C0 },
    { "Cyrillic_a"                 , 0x6C1 },
    { "Cyrillic_be"                , 0x6C2 },
    { "Cyrillic_tse"               , 0x6C3 },
    { "Cyrillic_de"                , 0x6C4 },
    { "Cyrillic_ie"                , 0x6C5 },
    { "Cyrillic_ef"                , 0x6C6 },
    { "Cyrillic_ghe"               , 0x6C7 },
    { "Cyrillic_ha"                , 0x6C8 },
    { "Cyrillic_i"                 , 0x6C9 },
    { "Cyrillic_shorti"            , 0x6CA },
    { "Cyrillic_ka"                , 0x6CB },
    { "Cyrillic_el"                , 0x6CC },
    { "Cyrillic_em"                , 0x6CD },
    { "Cyrillic_en"                , 0x6CE },
    { "Cyrillic_o"                 , 0x6CF },
    { "Cyrillic_pe"                , 0x6D0 },
    { "Cyrillic_ya"                , 0x6D1 },
    { "Cyrillic_er"                , 0x6D2 },
    { "Cyrillic_es"                , 0x6D3 },
    { "Cyrillic_te"                , 0x6D4 },
    { "Cyrillic_u"                 , 0x6D5 },
    { "Cyrillic_zhe"               , 0x6D6 },
    { "Cyrillic_ve"                , 0x6D7 },
    { "Cyrillic_softsign"          , 0x6D8 },
    { "Cyrillic_yeru"              , 0x6D9 },
    { "Cyrillic_ze"                , 0x6DA },
    { "Cyrillic_sha"               , 0x6DB },
    { "Cyrillic_e"                 , 0x6DC },
    { "Cyrillic_shcha"             , 0x6DD },
    { "Cyrillic_che"               , 0x6DE },
    { "Cyrillic_hardsign"          , 0x6DF },
    { "Cyrillic_YU"                , 0x6E0 },
    { "Cyrillic_A"                 , 0x6E1 },
    { "Cyrillic_BE"                , 0x6E2 },
    { "Cyrillic_TSE"               , 0x6E3 },
    { "Cyrillic_DE"                , 0x6E4 },
    { "Cyrillic_IE"                , 0x6E5 },
    { "Cyrillic_EF"                , 0x6E6 },
    { "Cyrillic_GHE"               , 0x6E7 },
    { "Cyrillic_HA"                , 0x6E8 },
    { "Cyrillic_I"                 , 0x6E9 },
    { "Cyrillic_SHORTI"            , 0x6EA },
    { "Cyrillic_KA"                , 0x6EB },
    { "Cyrillic_EL"                , 0x6EC },
    { "Cyrillic_EM"                , 0x6ED },
    { "Cyrillic_EN"                , 0x6EE },
    { "Cyrillic_O"                 , 0x6EF },
    { "Cyrillic_PE"                , 0x6F0 },
    { "Cyrillic_YA"                , 0x6F1 },
    { "Cyrillic_ER"                , 0x6F2 },
    { "Cyrillic_ES"                , 0x6F3 },
    { "Cyrillic_TE"                , 0x6F4 },
    { "Cyrillic_U"                 , 0x6F5 },
    { "Cyrillic_ZHE"               , 0x6F6 },
    { "Cyrillic_VE"                , 0x6F7 },
    { "Cyrillic_SOFTSIGN"          , 0x6F8 },
    { "Cyrillic_YERU"              , 0x6F9 },
    { "Cyrillic_ZE"                , 0x6FA },
    { "Cyrillic_SHA"               , 0x6FB },
    { "Cyrillic_E"                 , 0x6FC },
    { "Cyrillic_SHCHA"             , 0x6FD },
    { "Cyrillic_CHE"               , 0x6FE },
    { "Cyrillic_HARDSIGN"          , 0x6FF },
    { "Greek_ALPHAaccent"          , 0x7A1 },
    { "Greek_EPSILONaccent"        , 0x7A2 },
    { "Greek_ETAaccent"            , 0x7A3 },
    { "Greek_IOTAaccent"           , 0x7A4 },
    { "Greek_IOTAdieresis"         , 0x7A5 },
#ifndef TK_NO_DEPRECATED
    { "Greek_IOTAdiaeresis"        , 0x7A5 },
#endif
    { "Greek_IOTAaccentdiaeresis"  , 0x7A6 },
    { "Greek_OMICRONaccent"        , 0x7A7 },
    { "Greek_UPSILONaccent"        , 0x7A8 },
    { "Greek_UPSILONdieresis"      , 0x7A9 },
    { "Greek_UPSILONaccentdieresis", 0x7AA },
    { "Greek_OMEGAaccent"          , 0x7AB },
    { "Greek_accentdieresis"       , 0x7AE },
    { "Greek_horizbar"             , 0x7AF },
    { "Greek_alphaaccent"          , 0x7B1 },
    { "Greek_epsilonaccent"        , 0x7B2 },
    { "Greek_etaaccent"            , 0x7B3 },
    { "Greek_iotaaccent"           , 0x7B4 },
    { "Greek_iotadieresis"         , 0x7B5 },
    { "Greek_iotaaccentdieresis"   , 0x7B6 },
    { "Greek_omicronaccent"        , 0x7B7 },
    { "Greek_upsilonaccent"        , 0x7B8 },
    { "Greek_upsilondieresis"      , 0x7B9 },
    { "Greek_upsilonaccentdieresis", 0x7BA },
    { "Greek_omegaaccent"          , 0x7BB },
    { "Greek_ALPHA"                , 0x7C1 },
    { "Greek_BETA"                 , 0x7C2 },
    { "Greek_GAMMA"                , 0x7C3 },
    { "Greek_DELTA"                , 0x7C4 },
    { "Greek_EPSILON"              , 0x7C5 },
    { "Greek_ZETA"                 , 0x7C6 },
    { "Greek_ETA"                  , 0x7C7 },
    { "Greek_THETA"                , 0x7C8 },
    { "Greek_IOTA"                 , 0x7C9 },
    { "Greek_KAPPA"                , 0x7CA },
    { "Greek_LAMDA"                , 0x7CB },
    { "Greek_LAMBDA"               , 0x7CB },
    { "Greek_MU"                   , 0x7CC },
    { "Greek_NU"                   , 0x7CD },
    { "Greek_XI"                   , 0x7CE },
    { "Greek_OMICRON"              , 0x7CF },
    { "Greek_PI"                   , 0x7D0 },
    { "Greek_RHO"                  , 0x7D1 },
    { "Greek_SIGMA"                , 0x7D2 },
    { "Greek_TAU"                  , 0x7D4 },
    { "Greek_UPSILON"              , 0x7D5 },
    { "Greek_PHI"                  , 0x7D6 },
    { "Greek_CHI"                  , 0x7D7 },
    { "Greek_PSI"                  , 0x7D8 },
    { "Greek_OMEGA"                , 0x7D9 },
    { "Greek_alpha"                , 0x7E1 },
    { "Greek_beta"                 , 0x7E2 },
    { "Greek_gamma"                , 0x7E3 },
    { "Greek_delta"                , 0x7E4 },
    { "Greek_epsilon"              , 0x7E5 },
    { "Greek_zeta"                 , 0x7E6 },
    { "Greek_eta"                  , 0x7E7 },
    { "Greek_theta"                , 0x7E8 },
    { "Greek_iota"                 , 0x7E9 },
    { "Greek_kappa"                , 0x7EA },
    { "Greek_lamda"                , 0x7EB },
    { "Greek_lambda"               , 0x7EB },
    { "Greek_mu"                   , 0x7EC },
    { "Greek_nu"                   , 0x7ED },
    { "Greek_xi"                   , 0x7EE },
    { "Greek_omicron"              , 0x7EF },
    { "Greek_pi"                   , 0x7F0 },
    { "Greek_rho"                  , 0x7F1 },
    { "Greek_sigma"                , 0x7F2 },
    { "Greek_finalsmallsigma"      , 0x7F3 },
    { "Greek_tau"                  , 0x7F4 },
    { "Greek_upsilon"              , 0x7F5 },
    { "Greek_phi"                  , 0x7F6 },
    { "Greek_chi"                  , 0x7F7 },
    { "Greek_psi"                  , 0x7F8 },
    { "Greek_omega"                , 0x7F9 },
    { "leftradical"                , 0x8A1 },
    { "topleftradical"             , 0x8A2 },
    { "horizconnector"             , 0x8A3 },
    { "topintegral"                , 0x8A4 },
    { "botintegral"                , 0x8A5 },
    { "vertconnector"              , 0x8A6 },
    { "topleftsqbracket"           , 0x8A7 },
    { "botleftsqbracket"           , 0x8A8 },
    { "toprightsqbracket"          , 0x8A9 },
    { "botrightsqbracket"          , 0x8AA },
    { "topleftparens"              , 0x8AB },
    { "botleftparens"              , 0x8AC },
    { "toprightparens"             , 0x8AD },
    { "botrightparens"             , 0x8AE },
    { "leftmiddlecurlybrace"       , 0x8AF },
    { "rightmiddlecurlybrace"      , 0x8B0 },
    { "topleftsummation"           , 0x8B1 },
    { "botleftsummation"           , 0x8B2 },
    { "topvertsummationconnector"  , 0x8B3 },
    { "botvertsummationconnector"  , 0x8B4 },
    { "toprightsummation"          , 0x8B5 },
    { "botrightsummation"          , 0x8B6 },
    { "rightmiddlesummation"       , 0x8B7 },
    { "lessthanequal"              , 0x8BC },
    { "notequal"                   , 0x8BD },
    { "greaterthanequal"           , 0x8BE },
    { "integral"                   , 0x8BF },
    { "therefore"                  , 0x8C0 },
    { "variation"                  , 0x8C1 },
    { "infinity"                   , 0x8C2 },
    { "nabla"                      , 0x8C5 },
    { "approximate"                , 0x8C8 },
    { "similarequal"               , 0x8C9 },
    { "ifonlyif"                   , 0x8CD },
    { "implies"                    , 0x8CE },
    { "identical"                  , 0x8CF },
    { "radical"                    , 0x8D6 },
    { "includedin"                 , 0x8DA },
    { "includes"                   , 0x8DB },
    { "intersection"               , 0x8DC },
    { "union"                      , 0x8DD },
    { "logicaland"                 , 0x8DE },
    { "logicalor"                  , 0x8DF },
    { "partialderivative"          , 0x8EF },
    { "function"                   , 0x8F6 },
    { "leftarrow"                  , 0x8FB },
    { "uparrow"                    , 0x8FC },
    { "rightarrow"                 , 0x8FD },
    { "downarrow"                  , 0x8FE },
    { "blank"                      , 0x9DF },
    { "soliddiamond"               , 0x9E0 },
    { "checkerboard"               , 0x9E1 },
    { "ht"                         , 0x9E2 },
    { "ff"                         , 0x9E3 },
    { "cr"                         , 0x9E4 },
    { "lf"                         , 0x9E5 },
    { "nl"                         , 0x9E8 },
    { "vt"                         , 0x9E9 },
    { "lowrightcorner"             , 0x9EA },
    { "uprightcorner"              , 0x9EB },
    { "upleftcorner"               , 0x9EC },
    { "lowleftcorner"              , 0x9ED },
    { "crossinglines"              , 0x9EE },
    { "horizlinescan1"             , 0x9EF },
    { "horizlinescan3"             , 0x9F0 },
    { "horizlinescan5"             , 0x9F1 },
    { "horizlinescan7"             , 0x9F2 },
    { "horizlinescan9"             , 0x9F3 },
    { "leftt"                      , 0x9F4 },
    { "rightt"                     , 0x9F5 },
    { "bott"                       , 0x9F6 },
    { "topt"                       , 0x9F7 },
    { "vertbar"                    , 0x9F8 },
    { "emspace"                    , 0xAA1 },
    { "enspace"                    , 0xAA2 },
    { "em3space"                   , 0xAA3 },
    { "em4space"                   , 0xAA4 },
    { "digitspace"                 , 0xAA5 },
    { "punctspace"                 , 0xAA6 },
    { "thinspace"                  , 0xAA7 },
    { "hairspace"                  , 0xAA8 },
    { "emdash"                     , 0xAA9 },
    { "endash"                     , 0xAAA },
    { "signifblank"                , 0xAAC },
    { "ellipsis"                   , 0xAAE },
    { "doubbaselinedot"            , 0xAAF },
    { "onethird"                   , 0xAB0 },
    { "twothirds"                  , 0xAB1 },
    { "onefifth"                   , 0xAB2 },
    { "twofifths"                  , 0xAB3 },
    { "threefifths"                , 0xAB4 },
    { "fourfifths"                 , 0xAB5 },
    { "onesixth"                   , 0xAB6 },
    { "fivesixths"                 , 0xAB7 },
    { "careof"                     , 0xAB8 },
    { "figdash"                    , 0xABB },
    { "leftanglebracket"           , 0xABC },
    { "decimalpoint"               , 0xABD },
    { "rightanglebracket"          , 0xABE },
    { "marker"                     , 0xABF },
    { "oneeighth"                  , 0xAC3 },
    { "threeeighths"               , 0xAC4 },
    { "fiveeighths"                , 0xAC5 },
    { "seveneighths"               , 0xAC6 },
    { "trademark"                  , 0xAC9 },
    { "signaturemark"              , 0xACA },
    { "trademarkincircle"          , 0xACB },
    { "leftopentriangle"           , 0xACC },
    { "rightopentriangle"          , 0xACD },
    { "emopencircle"               , 0xACE },
    { "emopenrectangle"            , 0xACF },
    { "leftsinglequotemark"        , 0xAD0 },
    { "rightsinglequotemark"       , 0xAD1 },
    { "leftdoublequotemark"        , 0xAD2 },
    { "rightdoublequotemark"       , 0xAD3 },
    { "prescription"               , 0xAD4 },
    { "permille"                   , 0xAD5 },
    { "minutes"                    , 0xAD6 },
    { "seconds"                    , 0xAD7 },
    { "latincross"                 , 0xAD9 },
    { "hexagram"                   , 0xADA },
    { "filledrectbullet"           , 0xADB },
    { "filledlefttribullet"        , 0xADC },
    { "filledrighttribullet"       , 0xADD },
    { "emfilledcircle"             , 0xADE },
    { "emfilledrect"               , 0xADF },
    { "enopencircbullet"           , 0xAE0 },
    { "enopensquarebullet"         , 0xAE1 },
    { "openrectbullet"             , 0xAE2 },
    { "opentribulletup"            , 0xAE3 },
    { "opentribulletdown"          , 0xAE4 },
    { "openstar"                   , 0xAE5 },
    { "enfilledcircbullet"         , 0xAE6 },
    { "enfilledsqbullet"           , 0xAE7 },
    { "filledtribulletup"          , 0xAE8 },
    { "filledtribulletdown"        , 0xAE9 },
    { "leftpointer"                , 0xAEA },
    { "rightpointer"               , 0xAEB },
    { "club"                       , 0xAEC },
    { "diamond"                    , 0xAED },
    { "heart"                      , 0xAEE },
    { "maltesecross"               , 0xAF0 },
    { "dagger"                     , 0xAF1 },
    { "doubledagger"               , 0xAF2 },
    { "checkmark"                  , 0xAF3 },
    { "ballotcross"                , 0xAF4 },
    { "musicalsharp"               , 0xAF5 },
    { "musicalflat"                , 0xAF6 },
    { "malesymbol"                 , 0xAF7 },
    { "femalesymbol"               , 0xAF8 },
    { "telephone"                  , 0xAF9 },
    { "telephonerecorder"          , 0xAFA },
    { "phonographcopyright"        , 0xAFB },
    { "caret"                      , 0xAFC },
    { "singlelowquotemark"         , 0xAFD },
    { "doublelowquotemark"         , 0xAFE },
    { "cursor"                     , 0xAFF },
    { "leftcaret"                  , 0xBA3 },
    { "rightcaret"                 , 0xBA6 },
    { "downcaret"                  , 0xBA8 },
    { "upcaret"                    , 0xBA9 },
    { "overbar"                    , 0xBC0 },
    { "downtack"                   , 0xBC2 },
    { "upshoe"                     , 0xBC3 },
    { "downstile"                  , 0xBC4 },
    { "underbar"                   , 0xBC6 },
    { "jot"                        , 0xBCA },
    { "quad"                       , 0xBCC },
    { "uptack"                     , 0xBCE },
    { "circle"                     , 0xBCF },
    { "upstile"                    , 0xBD3 },
    { "downshoe"                   , 0xBD6 },
    { "rightshoe"                  , 0xBD8 },
    { "leftshoe"                   , 0xBDA },
    { "lefttack"                   , 0xBDC },
    { "righttack"                  , 0xBFC },
    { "hebrew_doublelowline"       , 0xCDF },
    { "hebrew_aleph"               , 0xCE0 },
    { "hebrew_bet"                 , 0xCE1 },
#ifndef TK_NO_DEPRECATED
    { "hebrew_beth"                , 0xCE1 },
#endif
    { "hebrew_gimel"               , 0xCE2 },
#ifndef TK_NO_DEPRECATED
    { "hebrew_gimmel"              , 0xCE2 },
#endif
    { "hebrew_dalet"               , 0xCE3 },
#ifndef TK_NO_DEPRECATED
    { "hebrew_daleth"              , 0xCE3 },
#endif
    { "hebrew_he"                  , 0xCE4 },
    { "hebrew_waw"                 , 0xCE5 },
    { "hebrew_zain"                , 0xCE6 },
#ifndef TK_NO_DEPRECATED
    { "hebrew_zayin"               , 0xCE6 },
#endif
    { "hebrew_chet"                , 0xCE7 },
#ifndef TK_NO_DEPRECATED
    { "hebrew_het"                 , 0xCE7 },
#endif
    { "hebrew_tet"                 , 0xCE8 },
#ifndef TK_NO_DEPRECATED
    { "hebrew_teth"                , 0xCE8 },
#endif
    { "hebrew_yod"                 , 0xCE9 },
    { "hebrew_finalkaph"           , 0xCEA },
    { "hebrew_kaph"                , 0xCEB },
    { "hebrew_lamed"               , 0xCEC },
    { "hebrew_finalmem"            , 0xCED },
    { "hebrew_mem"                 , 0xCEE },
    { "hebrew_finalnun"            , 0xCEF },
    { "hebrew_nun"                 , 0xCF0 },
    { "hebrew_samech"              , 0xCF1 },
#ifndef TK_NO_DEPRECATED
    { "hebrew_samekh"              , 0xCF1 },
#endif
    { "hebrew_ayin"                , 0xCF2 },
    { "hebrew_finalpe"             , 0xCF3 },
    { "hebrew_pe"                  , 0xCF4 },
    { "hebrew_finalzade"           , 0xCF5 },
#ifndef TK_NO_DEPRECATED
    { "hebrew_finalzadi"           , 0xCF5 },
#endif
    { "hebrew_zade"                , 0xCF6 },
#ifndef TK_NO_DEPRECATED
    { "hebrew_zadi"                , 0xCF6 },
#endif
    { "hebrew_qoph"                , 0xCF7 },
#ifndef TK_NO_DEPRECATED
    { "hebrew_kuf"                 , 0xCF7 },
#endif
    { "hebrew_resh"                , 0xCF8 },
    { "hebrew_shin"                , 0xCF9 },
    { "hebrew_taw"                 , 0xCFA },
#ifndef TK_NO_DEPRECATED
    { "hebrew_taf"                 , 0xCFA },
#endif
    { "Thai_kokai"                 , 0xDA1 },
    { "Thai_khokhai"               , 0xDA2 },
    { "Thai_khokhuat"              , 0xDA3 },
    { "Thai_khokhwai"              , 0xDA4 },
    { "Thai_khokhon"               , 0xDA5 },
    { "Thai_khorakhang"            , 0xDA6 },
    { "Thai_ngongu"                , 0xDA7 },
    { "Thai_chochan"               , 0xDA8 },
    { "Thai_choching"              , 0xDA9 },
    { "Thai_chochang"              , 0xDAA },
    { "Thai_soso"                  , 0xDAB },
    { "Thai_chochoe"               , 0xDAC },
    { "Thai_yoying"                , 0xDAD },
    { "Thai_dochada"               , 0xDAE },
    { "Thai_topatak"               , 0xDAF },
    { "Thai_thothan"               , 0xDB0 },
    { "Thai_thonangmontho"         , 0xDB1 },
    { "Thai_thophuthao"            , 0xDB2 },
    { "Thai_nonen"                 , 0xDB3 },
    { "Thai_dodek"                 , 0xDB4 },
    { "Thai_totao"                 , 0xDB5 },
    { "Thai_thothung"              , 0xDB6 },
    { "Thai_thothahan"             , 0xDB7 },
    { "Thai_thothong"              , 0xDB8 },
    { "Thai_nonu"                  , 0xDB9 },
    { "Thai_bobaimai"              , 0xDBA },
    { "Thai_popla"                 , 0xDBB },
    { "Thai_phophung"              , 0xDBC },
    { "Thai_fofa"                  , 0xDBD },
    { "Thai_phophan"               , 0xDBE },
    { "Thai_fofan"                 , 0xDBF },
    { "Thai_phosamphao"            , 0xDC0 },
    { "Thai_moma"                  , 0xDC1 },
    { "Thai_yoyak"                 , 0xDC2 },
    { "Thai_rorua"                 , 0xDC3 },
    { "Thai_ru"                    , 0xDC4 },
    { "Thai_loling"                , 0xDC5 },
    { "Thai_lu"                    , 0xDC6 },
    { "Thai_wowaen"                , 0xDC7 },
    { "Thai_sosala"                , 0xDC8 },
    { "Thai_sorusi"                , 0xDC9 },
    { "Thai_sosua"                 , 0xDCA },
    { "Thai_hohip"                 , 0xDCB },
    { "Thai_lochula"               , 0xDCC },
    { "Thai_oang"                  , 0xDCD },
    { "Thai_honokhuk"              , 0xDCE },
    { "Thai_paiyannoi"             , 0xDCF },
    { "Thai_saraa"                 , 0xDD0 },
    { "Thai_maihanakat"            , 0xDD1 },
    { "Thai_saraaa"                , 0xDD2 },
    { "Thai_saraam"                , 0xDD3 },
    { "Thai_sarai"                 , 0xDD4 },
    { "Thai_saraii"                , 0xDD5 },
    { "Thai_saraue"                , 0xDD6 },
    { "Thai_sarauee"               , 0xDD7 },
    { "Thai_sarau"                 , 0xDD8 },
    { "Thai_sarauu"                , 0xDD9 },
    { "Thai_phinthu"               , 0xDDA },
    { "Thai_maihanakat_maitho"     , 0xDDE },
    { "Thai_baht"                  , 0xDDF },
    { "Thai_sarae"                 , 0xDE0 },
    { "Thai_saraae"                , 0xDE1 },
    { "Thai_sarao"                 , 0xDE2 },
    { "Thai_saraaimaimuan"         , 0xDE3 },
    { "Thai_saraaimaimalai"        , 0xDE4 },
    { "Thai_lakkhangyao"           , 0xDE5 },
    { "Thai_maiyamok"              , 0xDE6 },
    { "Thai_maitaikhu"             , 0xDE7 },
    { "Thai_maiek"                 , 0xDE8 },
    { "Thai_maitho"                , 0xDE9 },
    { "Thai_maitri"                , 0xDEA },
    { "Thai_maichattawa"           , 0xDEB },
    { "Thai_thanthakhat"           , 0xDEC },
    { "Thai_nikhahit"              , 0xDED },
    { "Thai_leksun"                , 0xDF0 },
    { "Thai_leknung"               , 0xDF1 },
    { "Thai_leksong"               , 0xDF2 },
    { "Thai_leksam"                , 0xDF3 },
    { "Thai_leksi"                 , 0xDF4 },
    { "Thai_lekha"                 , 0xDF5 },
    { "Thai_lekhok"                , 0xDF6 },
    { "Thai_lekchet"               , 0xDF7 },
    { "Thai_lekpaet"               , 0xDF8 },
    { "Thai_lekkao"                , 0xDF9 },
    { "Hangul_Kiyeog"              , 0xEA1 },
    { "Hangul_SsangKiyeog"         , 0xEA2 },
    { "Hangul_KiyeogSios"          , 0xEA3 },
    { "Hangul_Nieun"               , 0xEA4 },
    { "Hangul_NieunJieuj"          , 0xEA5 },
    { "Hangul_NieunHieuh"          , 0xEA6 },
    { "Hangul_Dikeud"              , 0xEA7 },
    { "Hangul_SsangDikeud"         , 0xEA8 },
    { "Hangul_Rieul"               , 0xEA9 },
    { "Hangul_RieulKiyeog"         , 0xEAA },
    { "Hangul_RieulMieum"          , 0xEAB },
    { "Hangul_RieulPieub"          , 0xEAC },
    { "Hangul_RieulSios"           , 0xEAD },
    { "Hangul_RieulTieut"          , 0xEAE },
    { "Hangul_RieulPhieuf"         , 0xEAF },
    { "Hangul_RieulHieuh"          , 0xEB0 },
    { "Hangul_Mieum"               , 0xEB1 },
    { "Hangul_Pieub"               , 0xEB2 },
    { "Hangul_SsangPieub"          , 0xEB3 },
    { "Hangul_PieubSios"           , 0xEB4 },
    { "Hangul_Sios"                , 0xEB5 },
    { "Hangul_SsangSios"           , 0xEB6 },
    { "Hangul_Ieung"               , 0xEB7 },
    { "Hangul_Jieuj"               , 0xEB8 },
    { "Hangul_SsangJieuj"          , 0xEB9 },
    { "Hangul_Cieuc"               , 0xEBA },
    { "Hangul_Khieuq"              , 0xEBB },
    { "Hangul_Tieut"               , 0xEBC },
    { "Hangul_Phieuf"              , 0xEBD },
    { "Hangul_Hieuh"               , 0xEBE },
    { "Hangul_A"                   , 0xEBF },
    { "Hangul_AE"                  , 0xEC0 },
    { "Hangul_YA"                  , 0xEC1 },
    { "Hangul_YAE"                 , 0xEC2 },
    { "Hangul_EO"                  , 0xEC3 },
    { "Hangul_E"                   , 0xEC4 },
    { "Hangul_YEO"                 , 0xEC5 },
    { "Hangul_YE"                  , 0xEC6 },
    { "Hangul_O"                   , 0xEC7 },
    { "Hangul_WA"                  , 0xEC8 },
    { "Hangul_WAE"                 , 0xEC9 },
    { "Hangul_OE"                  , 0xECA },
    { "Hangul_YO"                  , 0xECB },
    { "Hangul_U"                   , 0xECC },
    { "Hangul_WEO"                 , 0xECD },
    { "Hangul_WE"                  , 0xECE },
    { "Hangul_WI"                  , 0xECF },
    { "Hangul_YU"                  , 0xED0 },
    { "Hangul_EU"                  , 0xED1 },
    { "Hangul_YI"                  , 0xED2 },
    { "Hangul_I"                   , 0xED3 },
    { "Hangul_J_Kiyeog"            , 0xED4 },
    { "Hangul_J_SsangKiyeog"       , 0xED5 },
    { "Hangul_J_KiyeogSios"        , 0xED6 },
    { "Hangul_J_Nieun"             , 0xED7 },
    { "Hangul_J_NieunJieuj"        , 0xED8 },
    { "Hangul_J_NieunHieuh"        , 0xED9 },
    { "Hangul_J_Dikeud"            , 0xEDA },
    { "Hangul_J_Rieul"             , 0xEDB },
    { "Hangul_J_RieulKiyeog"       , 0xEDC },
    { "Hangul_J_RieulMieum"        , 0xEDD },
    { "Hangul_J_RieulPieub"        , 0xEDE },
    { "Hangul_J_RieulSios"         , 0xEDF },
    { "Hangul_J_RieulTieut"        , 0xEE0 },
    { "Hangul_J_RieulPhieuf"       , 0xEE1 },
    { "Hangul_J_RieulHieuh"        , 0xEE2 },
    { "Hangul_J_Mieum"             , 0xEE3 },
    { "Hangul_J_Pieub"             , 0xEE4 },
    { "Hangul_J_PieubSios"         , 0xEE5 },
    { "Hangul_J_Sios"              , 0xEE6 },
    { "Hangul_J_SsangSios"         , 0xEE7 },
    { "Hangul_J_Ieung"             , 0xEE8 },
    { "Hangul_J_Jieuj"             , 0xEE9 },
    { "Hangul_J_Cieuc"             , 0xEEA },
    { "Hangul_J_Khieuq"            , 0xEEB },
    { "Hangul_J_Tieut"             , 0xEEC },
    { "Hangul_J_Phieuf"            , 0xEED },
    { "Hangul_J_Hieuh"             , 0xEEE },
    { "Hangul_RieulYeorinHieuh"    , 0xEEF },
    { "Hangul_SunkyeongeumMieum"   , 0xEF0 },
    { "Hangul_SunkyeongeumPieub"   , 0xEF1 },
    { "Hangul_PanSios"             , 0xEF2 },
    { "Hangul_KkogjiDalrinIeung"   , 0xEF3 },
    { "Hangul_SunkyeongeumPhieuf"  , 0xEF4 },
    { "Hangul_YeorinHieuh"         , 0xEF5 },
    { "Hangul_AraeA"               , 0xEF6 },
    { "Hangul_AraeAE"              , 0xEF7 },
    { "Hangul_J_PanSios"           , 0xEF8 },
    { "Hangul_J_KkogjiDalrinIeung" , 0xEF9 },
    { "Hangul_J_YeorinHieuh"       , 0xEFA },
    { "Korean_Won"                 , 0xEFF },
    { "XF86ModeLock"               , 0x1008FF01 },
    { "XF86MonBrightnessUp"        , 0x1008FF02 },
    { "XF86MonBrightnessDown"      , 0x1008FF03 },
    { "XF86KbdLightOnOff"          , 0x1008FF04 },
    { "XF86KbdBrightnessUp"        , 0x1008FF05 },
    { "XF86KbdBrightnessDown"      , 0x1008FF06 },
    { "XF86MonBrightnessCycle"     , 0x1008FF07 },
    { "XF86Standby"                , 0x1008FF10 },
    { "XF86AudioLowerVolume"       , 0x1008FF11 },
    { "XF86AudioMute"              , 0x1008FF12 },
    { "XF86AudioRaiseVolume"       , 0x1008FF13 },
    { "XF86AudioPlay"              , 0x1008FF14 },
    { "XF86AudioStop"              , 0x1008FF15 },
    { "XF86AudioPrev"              , 0x1008FF16 },
    { "XF86AudioNext"              , 0x1008FF17 },
    { "XF86HomePage"               , 0x1008FF18 },
    { "XF86Mail"                   , 0x1008FF19 },
    { "XF86Start"                  , 0x1008FF1A },
    { "XF86Search"                 , 0x1008FF1B },
    { "XF86AudioRecord"            , 0x1008FF1C },
    { "XF86Calculator"             , 0x1008FF1D },
    { "XF86Memo"                   , 0x1008FF1E },
    { "XF86ToDoList"               , 0x1008FF1F },
    { "XF86Calendar"               , 0x1008FF20 },
    { "XF86PowerDown"              , 0x1008FF21 },
    { "XF86ContrastAdjust"         , 0x1008FF22 },
    { "XF86RockerUp"               , 0x1008FF23 },
    { "XF86RockerDown"             , 0x1008FF24 },
    { "XF86RockerEnter"            , 0x1008FF25 },
    { "XF86Back"                   , 0x1008FF26 },
    { "XF86Forward"                , 0x1008FF27 },
    { "XF86Stop"                   , 0x1008FF28 },
    { "XF86Refresh"                , 0x1008FF29 },
    { "XF86PowerOff"               , 0x1008FF2A },
    { "XF86WakeUp"                 , 0x1008FF2B },
    { "XF86Eject"                  , 0x1008FF2C },
    { "XF86ScreenSaver"            , 0x1008FF2D },
    { "XF86WWW"                    , 0x1008FF2E },
    { "XF86Sleep"                  , 0x1008FF2F },
    { "XF86Favorites"              , 0x1008FF30 },
    { "XF86AudioPause"             , 0x1008FF31 },
    { "XF86AudioMedia"             , 0x1008FF32 },
    { "XF86MyComputer"             , 0x1008FF33 },
    { "XF86VendorHome"             , 0x1008FF34 },
    { "XF86LightBulb"              , 0x1008FF35 },
    { "XF86Shop"                   , 0x1008FF36 },
    { "XF86History"                , 0x1008FF37 },
    { "XF86OpenURL"                , 0x1008FF38 },
    { "XF86AddFavorite"            , 0x1008FF39 },
    { "XF86HotLinks"               , 0x1008FF3A },
    { "XF86BrightnessAdjust"       , 0x1008FF3B },
    { "XF86Finance"                , 0x1008FF3C },
    { "XF86Community"              , 0x1008FF3D },
    { "XF86AudioRewind"            , 0x1008FF3E },
    { "XF86BackForward"            , 0x1008FF3F },
    { "XF86Launch0"                , 0x1008FF40 },
    { "XF86Launch1"                , 0x1008FF41 },
    { "XF86Launch2"                , 0x1008FF42 },
    { "XF86Launch3"                , 0x1008FF43 },
    { "XF86Launch4"                , 0x1008FF44 },
    { "XF86Launch5"                , 0x1008FF45 },
    { "XF86Launch6"                , 0x1008FF46 },
    { "XF86Launch7"                , 0x1008FF47 },
    { "XF86Launch8"                , 0x1008FF48 },
    { "XF86Launch9"                , 0x1008FF49 },
    { "XF86LaunchA"                , 0x1008FF4A },
    { "XF86LaunchB"                , 0x1008FF4B },
    { "XF86LaunchC"                , 0x1008FF4C },
    { "XF86LaunchD"                , 0x1008FF4D },
    { "XF86LaunchE"                , 0x1008FF4E },
    { "XF86LaunchF"                , 0x1008FF4F },
    { "XF86ApplicationLeft"        , 0x1008FF50 },
    { "XF86ApplicationRight"       , 0x1008FF51 },
    { "XF86Book"                   , 0x1008FF52 },
    { "XF86CD"                     , 0x1008FF53 },
    { "XF86Calculater"             , 0x1008FF54 },
    { "XF86Clear"                  , 0x1008FF55 },
    { "XF86Close"                  , 0x1008FF56 },
    { "XF86Copy"                   , 0x1008FF57 },
    { "XF86Cut"                    , 0x1008FF58 },
    { "XF86Display"                , 0x1008FF59 },
    { "XF86DOS"                    , 0x1008FF5A },
    { "XF86Documents"              , 0x1008FF5B },
    { "XF86Excel"                  , 0x1008FF5C },
    { "XF86Explorer"               , 0x1008FF5D },
    { "XF86Game"                   , 0x1008FF5E },
    { "XF86Go"                     , 0x1008FF5F },
    { "XF86iTouch"                 , 0x1008FF60 },
    { "XF86LogOff"                 , 0x1008FF61 },
    { "XF86Market"                 , 0x1008FF62 },
    { "XF86Meeting"                , 0x1008FF63 },
    { "XF86MenuKB"                 , 0x1008FF65 },
    { "XF86MenuPB"                 , 0x1008FF66 },
    { "XF86MySites"                , 0x1008FF67 },
    { "XF86New"                    , 0x1008FF68 },
    { "XF86News"                   , 0x1008FF69 },
    { "XF86OfficeHome"             , 0x1008FF6A },
    { "XF86Open"                   , 0x1008FF6B },
    { "XF86Option"                 , 0x1008FF6C },
    { "XF86Paste"                  , 0x1008FF6D },
    { "XF86Phone"                  , 0x1008FF6E },
    { "XF86Q"                      , 0x1008FF70 },
    { "XF86Reply"                  , 0x1008FF72 },
    { "XF86Reload"                 , 0x1008FF73 },
    { "XF86RotateWindows"          , 0x1008FF74 },
    { "XF86RotationPB"             , 0x1008FF75 },
    { "XF86RotationKB"             , 0x1008FF76 },
    { "XF86Save"                   , 0x1008FF77 },
    { "XF86ScrollUp"               , 0x1008FF78 },
    { "XF86ScrollDown"             , 0x1008FF79 },
    { "XF86ScrollClick"            , 0x1008FF7A },
    { "XF86Send"                   , 0x1008FF7B },
    { "XF86Spell"                  , 0x1008FF7C },
    { "XF86SplitScreen"            , 0x1008FF7D },
    { "XF86Support"                , 0x1008FF7E },
    { "XF86TaskPane"               , 0x1008FF7F },
    { "XF86Terminal"               , 0x1008FF80 },
    { "XF86Tools"                  , 0x1008FF81 },
    { "XF86Travel"                 , 0x1008FF82 },
    { "XF86UserPB"                 , 0x1008FF84 },
    { "XF86User1KB"                , 0x1008FF85 },
    { "XF86User2KB"                , 0x1008FF86 },
    { "XF86Video"                  , 0x1008FF87 },
    { "XF86WheelButton"            , 0x1008FF88 },
    { "XF86Word"                   , 0x1008FF89 },
    { "XF86Xfer"                   , 0x1008FF8A },
    { "XF86ZoomIn"                 , 0x1008FF8B },
    { "XF86ZoomOut"                , 0x1008FF8C },
    { "XF86Away"                   , 0x1008FF8D },
    { "XF86Messenger"              , 0x1008FF8E },
    { "XF86WebCam"                 , 0x1008FF8F },
    { "XF86MailForward"            , 0x1008FF90 },
    { "XF86Pictures"               , 0x1008FF91 },
    { "XF86Music"                  , 0x1008FF92 },
    { "XF86Battery"                , 0x1008FF93 },
    { "XF86Bluetooth"              , 0x1008FF94 },
    { "XF86WLAN"                   , 0x1008FF95 },
    { "XF86UWB"                    , 0x1008FF96 },
    { "XF86AudioForward"           , 0x1008FF97 },
    { "XF86AudioRepeat"            , 0x1008FF98 },
    { "XF86AudioRandomPlay"        , 0x1008FF99 },
    { "XF86Subtitle"               , 0x1008FF9A },
    { "XF86AudioCycleTrack"        , 0x1008FF9B },
    { "XF86CycleAngle"             , 0x1008FF9C },
    { "XF86FrameBack"              , 0x1008FF9D },
    { "XF86FrameForward"           , 0x1008FF9E },
    { "XF86Time"                   , 0x1008FF9F },
    { "XF86Select"                 , 0x1008FFA0 },
    { "XF86View"                   , 0x1008FFA1 },
    { "XF86TopMenu"                , 0x1008FFA2 },
    { "XF86Red"                    , 0x1008FFA3 },
    { "XF86Green"                  , 0x1008FFA4 },
    { "XF86Yellow"                 , 0x1008FFA5 },
    { "XF86Blue"                   , 0x1008FFA6 },
    { "XF86Suspend"                , 0x1008FFA7 },
    { "XF86Hibernate"              , 0x1008FFA8 },
    { "XF86TouchpadToggle"         , 0x1008FFA9 },
    { "XF86TouchpadOn"             , 0x1008FFB0 },
    { "XF86TouchpadOff"            , 0x1008FFB1 },
    { "XF86AudioMicMute"           , 0x1008FFB2 },
    { "XF86Keyboard"               , 0x1008FFB3 },
    { "XF86WWAN"                   , 0x1008FFB4 },
    { "XF86RFKill"                 , 0x1008FFB5 },
    { "XF86AudioPreset"            , 0x1008FFB6 },
    { "XF86RotationLockToggle"     , 0x1008FFB7 },
    { "XF86FullScreen"             , 0x1008FFB8 },
    { "XF86Switch_VT_1"            , 0x1008FE01 },
    { "XF86Switch_VT_2"            , 0x1008FE02 },
    { "XF86Switch_VT_3"            , 0x1008FE03 },
    { "XF86Switch_VT_4"            , 0x1008FE04 },
    { "XF86Switch_VT_5"            , 0x1008FE05 },
    { "XF86Switch_VT_6"            , 0x1008FE06 },
    { "XF86Switch_VT_7"            , 0x1008FE07 },
    { "XF86Switch_VT_8"            , 0x1008FE08 },
    { "XF86Switch_VT_9"            , 0x1008FE09 },
    { "XF86Switch_VT_10"           , 0x1008FE0A },
    { "XF86Switch_VT_11"           , 0x1008FE0B },
    { "XF86Switch_VT_12"           , 0x1008FE0C },
    { "XF86Ungrab"                 , 0x1008FE20 },
    { "XF86ClearGrab"              , 0x1008FE21 },
    { "XF86Next_VMode"             , 0x1008FE22 },
    { "XF86Prev_VMode"             , 0x1008FE23 },
    { "XF86LogWindowTree"          , 0x1008FE24 },
    { "XF86LogGrabInfo"            , 0x1008FE25 },
    { "XF86BrightnessAuto"         , 0x100810F4 },
    { "XF86DisplayOff"             , 0x100810F5 },
    { "XF86Info"                   , 0x10081166 },
    { "XF86AspectRatio"            , 0x10081177 },
    { "XF86DVD"                    , 0x10081185 },
    { "XF86Audio"                  , 0x10081188 },
    { "XF86ChannelUp"              , 0x10081192 },
    { "XF86ChannelDown"            , 0x10081193 },
    { "XF86Break"                  , 0x1008119B },
    { "XF86VideoPhone"             , 0x100811A0 },
    { "XF86ZoomReset"              , 0x100811A4 },
    { "XF86Editor"                 , 0x100811A6 },
    { "XF86GraphicsEditor"         , 0x100811A8 },
    { "XF86Presentation"           , 0x100811A9 },
    { "XF86Database"               , 0x100811AA },
    { "XF86Voicemail"              , 0x100811AC },
    { "XF86Addressbook"            , 0x100811AD },
    { "XF86DisplayToggle"          , 0x100811AF },
    { "XF86SpellCheck"             , 0x100811B0 },
    { "XF86ContextMenu"            , 0x100811B6 },
    { "XF86MediaRepeat"            , 0x100811B7 },
    { "XF8610ChannelsUp"           , 0x100811B8 },
    { "XF8610ChannelsDown"         , 0x100811B9 },
    { "XF86Images"                 , 0x100811BA },
    { "XF86NotificationCenter"     , 0x100811BC },
    { "XF86PickupPhone"            , 0x100811BD },
    { "XF86HangupPhone"            , 0x100811BE },
    { "XF86Fn"                     , 0x100811D0 },
    { "XF86Fn_Esc"                 , 0x100811D1 },
    { "XF86FnRightShift"           , 0x100811E5 },
    { "XF86Numeric0"               , 0x10081200 },
    { "XF86Numeric1"               , 0x10081201 },
    { "XF86Numeric2"               , 0x10081202 },
    { "XF86Numeric3"               , 0x10081203 },
    { "XF86Numeric4"               , 0x10081204 },
    { "XF86Numeric5"               , 0x10081205 },
    { "XF86Numeric6"               , 0x10081206 },
    { "XF86Numeric7"               , 0x10081207 },
    { "XF86Numeric8"               , 0x10081208 },
    { "XF86Numeric9"               , 0x10081209 },
    { "XF86NumericStar"            , 0x1008120A },
    { "XF86NumericPound"           , 0x1008120B },
    { "XF86NumericA"               , 0x1008120C },
    { "XF86NumericB"               , 0x1008120D },
    { "XF86NumericC"               , 0x1008120E },
    { "XF86NumericD"               , 0x1008120F },
    { "XF86CameraFocus"            , 0x10081210 },
    { "XF86WPSButton"              , 0x10081211 },
    { "XF86CameraZoomIn"           , 0x10081215 },
    { "XF86CameraZoomOut"          , 0x10081216 },
    { "XF86CameraUp"               , 0x10081217 },
    { "XF86CameraDown"             , 0x10081218 },
    { "XF86CameraLeft"             , 0x10081219 },
    { "XF86CameraRight"            , 0x1008121A },
    { "XF86AttendantOn"            , 0x1008121B },
    { "XF86AttendantOff"           , 0x1008121C },
    { "XF86AttendantToggle"        , 0x1008121D },
    { "XF86LightsToggle"           , 0x1008121E },
    { "XF86ALSToggle"              , 0x10081230 },
    { "XF86Buttonconfig"           , 0x10081240 },
    { "XF86Taskmanager"            , 0x10081241 },
    { "XF86Journal"                , 0x10081242 },
    { "XF86ControlPanel"           , 0x10081243 },
    { "XF86AppSelect"              , 0x10081244 },
    { "XF86Screensaver"            , 0x10081245 },
    { "XF86VoiceCommand"           , 0x10081246 },
    { "XF86Assistant"              , 0x10081247 },
    { "XF86EmojiPicker"            , 0x10081249 },
    { "XF86Dictate"                , 0x1008124A },
    { "XF86BrightnessMin"          , 0x10081250 },
    { "XF86BrightnessMax"          , 0x10081251 },
    { "XF86KbdInputAssistPrev"     , 0x10081260 },
    { "XF86KbdInputAssistNext"     , 0x10081261 },
    { "XF86KbdInputAssistPrevgroup", 0x10081262 },
    { "XF86KbdInputAssistNextgroup", 0x10081263 },
    { "XF86KbdInputAssistAccept"   , 0x10081264 },
    { "XF86KbdInputAssistCancel"   , 0x10081265 },
    { "XF86RightUp"                , 0x10081266 },
    { "XF86RightDown"              , 0x10081267 },
    { "XF86LeftUp"                 , 0x10081268 },
    { "XF86LeftDown"               , 0x10081269 },
    { "XF86RootMenu"               , 0x1008126A },
    { "XF86MediaTopMenu"           , 0x1008126B },
    { "XF86Numeric11"              , 0x1008126C },
    { "XF86Numeric12"              , 0x1008126D },
    { "XF86AudioDesc"              , 0x1008126E },
    { "XF863DMode"                 , 0x1008126F },
    { "XF86NextFavorite"           , 0x10081270 },
    { "XF86StopRecord"             , 0x10081271 },
    { "XF86PauseRecord"            , 0x10081272 },
    { "XF86VOD"                    , 0x10081273 },
    { "XF86Unmute"                 , 0x10081274 },
    { "XF86FastReverse"            , 0x10081275 },
    { "XF86SlowReverse"            , 0x10081276 },
    { "XF86Data"                   , 0x10081277 },
    { "XF86OnScreenKeyboard"       , 0x10081278 },
    { "XF86PrivacyScreenToggle"    , 0x10081279 },
    { "XF86SelectiveScreenshot"    , 0x1008127A },
    { "XF86Macro1"                 , 0x10081290 },
    { "XF86Macro2"                 , 0x10081291 },
    { "XF86Macro3"                 , 0x10081292 },
    { "XF86Macro4"                 , 0x10081293 },
    { "XF86Macro5"                 , 0x10081294 },
    { "XF86Macro6"                 , 0x10081295 },
    { "XF86Macro7"                 , 0x10081296 },
    { "XF86Macro8"                 , 0x10081297 },
    { "XF86Macro9"                 , 0x10081298 },
    { "XF86Macro10"                , 0x10081299 },
    { "XF86Macro11"                , 0x1008129A },
    { "XF86Macro12"                , 0x1008129B },
    { "XF86Macro13"                , 0x1008129C },
    { "XF86Macro14"                , 0x1008129D },
    { "XF86Macro15"                , 0x1008129E },
    { "XF86Macro16"                , 0x1008129F },
    { "XF86Macro17"                , 0x100812A0 },
    { "XF86Macro18"                , 0x100812A1 },
    { "XF86Macro19"                , 0x100812A2 },
    { "XF86Macro20"                , 0x100812A3 },
    { "XF86Macro21"                , 0x100812A4 },
    { "XF86Macro22"                , 0x100812A5 },
    { "XF86Macro23"                , 0x100812A6 },
    { "XF86Macro24"                , 0x100812A7 },
    { "XF86Macro25"                , 0x100812A8 },
    { "XF86Macro26"                , 0x100812A9 },
    { "XF86Macro27"                , 0x100812AA },
    { "XF86Macro28"                , 0x100812AB },
    { "XF86Macro29"                , 0x100812AC },
    { "XF86Macro30"                , 0x100812AD },
    { "XF86MacroRecordStart"       , 0x100812B0 },
    { "XF86MacroRecordStop"        , 0x100812B1 },
    { "XF86MacroPresetCycle"       , 0x100812B2 },
    { "XF86MacroPreset1"           , 0x100812B3 },
    { "XF86MacroPreset2"           , 0x100812B4 },
    { "XF86MacroPreset3"           , 0x100812B5 },
    { "XF86KbdLcdMenu1"            , 0x100812B8 },
    { "XF86KbdLcdMenu2"            , 0x100812B9 },
    { "XF86KbdLcdMenu3"            , 0x100812BA },
    { "XF86KbdLcdMenu4"            , 0x100812BB },
    { "XF86KbdLcdMenu5"            , 0x100812BC },
    { "SunFA_Grave"                , 0x1005FF00 },
    { "SunFA_Circum"               , 0x1005FF01 },
    { "SunFA_Tilde"                , 0x1005FF02 },
    { "SunFA_Acute"                , 0x1005FF03 },
    { "SunFA_Diaeresis"            , 0x1005FF04 },
    { "SunFA_Cedilla"              , 0x1005FF05 },
    { "SunF36"                     , 0x1005FF10 },
    { "SunF37"                     , 0x1005FF11 },
    { "SunSys_Req"                 , 0x1005FF60 },
    { "SunProps"                   , 0x1005FF70 },
    { "SunFront"                   , 0x1005FF71 },
    { "SunCopy"                    , 0x1005FF72 },
    { "SunOpen"                    , 0x1005FF73 },
    { "SunPaste"                   , 0x1005FF74 },
    { "SunCut"                     , 0x1005FF75 },
    { "SunPowerSwitch"             , 0x1005FF76 },
    { "SunAudioLowerVolume"        , 0x1005FF77 },
    { "SunAudioMute"               , 0x1005FF78 },
    { "SunAudioRaiseVolume"        , 0x1005FF79 },
    { "SunVideoDegauss"            , 0x1005FF7A },
    { "SunVideoLowerBrightness"    , 0x1005FF7B },
    { "SunVideoRaiseBrightness"    , 0x1005FF7C },
    { "SunPowerSwitchShift"        , 0x1005FF7D },
#endif
    { nullptr, 0 },
  };

  for (int i = 0; nameKeySyms[i].name != nullptr; ++i) {
    if (key == nameKeySyms[i].name)
     return nameKeySyms[i].key;
  }

  return 0;
}

//---

void
CTkApp::
addVirtualEventData(const CTkAppVirtualEventData &vdata, const CTkAppEventData &edata)
{
  virtualEventData_[vdata].push_back(edata);
}

void
CTkApp::
deleteAllVirtualEventData(const CTkAppVirtualEventData &vdata)
{
  auto p = virtualEventData_.find(vdata);
  if (p == virtualEventData_.end()) return;

  (*p).second.clear();
}

void
CTkApp::
deleteVirtualEventData(const CTkAppVirtualEventData &vdata, const CTkAppEventData &edata)
{
  auto p = virtualEventData_.find(vdata);
  if (p == virtualEventData_.end()) return;

  EventDatas eventDatas1;

  for (const auto &eventData : (*p).second) {
    if (eventData != edata)
      eventDatas1.push_back(edata);
  }

  std::swap((*p).second, eventDatas1);
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
addOption(const QString &pattern, const QVariant &value, int priority)
{
  //std::cerr << "addOption: " << pattern.toStdString() << " " <<
  //             value.toStdString() << " " << priority << "\n";

  options_.emplace_back(pattern, value, priority);
}

bool
CTkApp::
matchOption(const QString &widgetClass, const QString &optName,
            const QString &optClass, QVariant &optValue) const
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
                 const QVariant &arg, QString &opt, bool quiet) const
{
  return lookupName("option", names, arg, opt, quiet);
}

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
           const QVariant &arg, QString &opt, bool quiet) const
{
  auto arg1 = variantToString(arg);

  return lookupName(msg, names, arg1, opt, quiet);
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
        if (i == n - 1) {
          if (n > 2)
            str += ",";

          str += " or ";
        }
        else
          str += ", ";
      }

      str += names[i];
    }

    return str;
  };

  int nmatch;
  if (! CTkAppUtil::uniqueMatch(names, arg, opt, nmatch)) {
    if (! quiet) {
      if (nmatch > 1)
        return throwError("ambiguous " + msg + " \"" + arg + "\": must be " + concatOptionNames());
      else
        return throwError("bad " + msg + " \"" + arg + "\": must be " + concatOptionNames());
    }
    else
      return false;
  }

  return true;
}

bool
CTkApp::
getOptionInt(const QString &, const QVariant &var, long &i) const
{
  if (! variantToInt(var, i))
    return throwError(msg() + "expected integer but got \"" + var + "\"");

  return true;
}

bool
CTkApp::
getOptionReal(const QString &, const QVariant &var, double &r) const
{
  if (! variantToReal(var, r))
    return throwError(msg() + "expected floating-point number but got \"" + var + "\"");

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

CTkAppWidget *
CTkApp::
getWmGroup(CTkAppWidget *group) const
{
  auto p = wmGroups_.find(group);

  return (p != wmGroups_.end() ? (*p).second : nullptr);
}

void
CTkApp::
setWmGroup(CTkAppWidget *group, CTkAppWidget *child)
{
  if (child)
    wmGroups_[group] = child;
  else
    wmGroups_.erase(group);
}

void
CTkApp::
getWmGrid(int &w, int &h, int &iw, int &ih)
{
  w  = wmGrid_.w;
  h  = wmGrid_.h;
  iw = wmGrid_.iw;
  ih = wmGrid_.ih;
}

void
CTkApp::
setWmGrid(int w, int h, int iw, int ih)
{
  wmGrid_.w  = w;
  wmGrid_.h  = h;
  wmGrid_.iw = iw;
  wmGrid_.ih = ih;
}

//---

QString
CTkApp::
getClipboardText(CTkAppClipboard type) const
{
  auto *clipboard = QApplication::clipboard();

  if      (type == CTkAppClipboard::Clipboard)
    return clipboard->text(QClipboard::Clipboard);
  else if (type == CTkAppClipboard::Selection)
    return clipboard->text(QClipboard::Selection);
  else
    assert(false);
}

void
CTkApp::
setClipboardText(const QString &text, CTkAppClipboard type)
{
  auto *clipboard = QApplication::clipboard();

  if      (type == CTkAppClipboard::Clipboard)
    clipboard->setText(text, QClipboard::Clipboard);
  else if (type == CTkAppClipboard::Selection)
    clipboard->setText(text, QClipboard::Selection);
  else
    assert(false);
}

//---

double
CTkApp::
getScaling() const
{
  if (scaling_ <= 0.0)
    return CScreenUnitsMgrInst->dpi()/72.0;

  return scaling_;
}

//---

bool
CTkApp::
variantIsValid(const QVariant &var) const
{
  QString str;

  if (var.isValid())
    str = variantToString(var);

  return (str != "");
}

bool
CTkApp::
variantToCursor(const QVariant &var, CursorData &c) const
{
  auto str = variantToString(var);

  CQStrParse parse(str);

  //---

  auto readWord = [&](QString &word) {
    parse.skipSpace();

    if (parse.eof())
      return false;

    while (! parse.eof() && ! parse.isSpace())
      word += parse.getChar();

    return true;
  };

  //---

  parse.skipSpace();

  if (parse.isChar('@')) {
    c.isAt = true;

    parse.skipChar();

    if (! readWord(c.name))
      return false;

    parse.skipSpace();

    if (! parse.eof()) {
      if (! readWord(c.maskName))
        return false;

      if (! parse.eof()) {
        if (! readWord(c.fgColor) || ! readWord(c.bgColor))
          return false;
      }
      else {
        std::swap(c.maskName, c.fgColor);
      }
    }
  }
  else {
    if (! readWord(c.name))
      return false;

    parse.skipSpace();

    if (! parse.eof()) {
      if (! readWord(c.fgColor))
        return false;

      parse.skipSpace();

      if (! parse.eof()) {
        if (! readWord(c.bgColor))
          return false;
      }
    }
  }

  parse.skipSpace();

  if (! parse.eof())
    return false;

  return true;
}

bool
CTkApp::
variantToQColor(const QVariant &var, QColor &c) const
{
  return CTkAppUtil::variantToQColor(const_cast<CTkApp *>(this), var, c);
}

bool
CTkApp::
variantToDistance(const QVariant &var, Distance &d) const
{
  return CTkAppUtil::variantToDistance(const_cast<CTkApp *>(this), var, d);
}

bool
CTkApp::
variantToDistanceI(const QVariant &var, Distance &d) const
{
  return CTkAppUtil::variantToDistanceI(const_cast<CTkApp *>(this), var, d);
}

bool
CTkApp::
variantToInt(const QVariant &var, long &i) const
{
  return CTkAppUtil::variantToInt(const_cast<CTkApp *>(this), var, i);
}

bool
CTkApp::
variantToReal(const QVariant &var, double &r) const
{
  return CTkAppUtil::variantToReal(const_cast<CTkApp *>(this), var, r);
}

bool
CTkApp::
variantToBool(const QVariant &var, bool &b) const
{
  return CTkAppUtil::variantToBool(const_cast<CTkApp *>(this), var, b);
}

QString
CTkApp::
variantToString(const QVariant &var) const
{
  return CTkAppUtil::variantToString(const_cast<CTkApp *>(this), var);
}

bool
CTkApp::
variantToAlign(const QVariant &var, Qt::Alignment &align) const
{
  return CTkAppUtil::variantToAlign(const_cast<CTkApp *>(this), var, align);
}

//--

QVariant
CTkApp::
boolToVariant(bool b) const
{
  return QVariant(b);
}

QVariant
CTkApp::
intToVariant(long i) const
{
  return QVariant(int(i));
}

QVariant
CTkApp::
realToVariant(double r) const
{
  return QVariant(r);
}

QVariant
CTkApp::
colorToVariant(const QColor &c) const
{
  return QVariant(c.name());
}

QVariant
CTkApp::
fontToVariant(const QFont &f) const
{
  return QVariant(f.toString());
}

QVariant
CTkApp::
imageToVariant(const CTkAppImageRef &image) const
{
  if (image)
    return QVariant(image->name());
  else
    return "";
}

//---

bool
CTkApp::
fileExists(const QString &filename) const
{
  QFileInfo fi(filename);

  return fi.exists();
}

//---

QString
CTkApp::
geometryStr(const QRect &r) const
{
  auto xStr = (r.x() ? "+" : "-");
  auto yStr = (r.y() ? "+" : "-");

  auto str = QString("%1x%2%3%4%5%6").arg(r.width()).arg(r.height()).
    arg(xStr).arg(r.x()).arg(yStr).arg(r.y());

  return str;
}

//---

bool
CTkApp::
wrongNumArgs(const QString &msg) const
{
  auto cmsg = "wrong # args: should be \"" + msg.toStdString() + "\"";
//std::cerr << cmsg << "\n";

  auto *sobj = Tcl_NewStringObj(cmsg.c_str(), int(cmsg.size()));
  Tcl_SetErrorCode(getInterp(), "TCL", "WRONGARGS", nullptr);
  Tcl_SetObjResult(getInterp(), sobj);

  return false;
}

bool
CTkApp::
invalidBool(const QVariant &var) const
{
  return throwError(msg() + "expected boolean value but got \"" + var + "\"");
}

bool
CTkApp::
invalidInteger(const QVariant &var) const
{
  return throwError(msg() + "expected integer but got \"" + var + "\"");
}

bool
CTkApp::
invalidReal(const QVariant &var) const
{
  return throwError(msg() + "expected floating-point number but got \"" + var + "\"");
}

bool
CTkApp::
invalidDistance(const QVariant &var) const
{
  return throwError(msg() + "bad screen distance \"" + var + "\"");
}

bool
CTkApp::
invalidQColor(const QVariant &var) const
{
  return throwError(msg() + "unknown color name \"" + var + "\"");
}

bool
CTkApp::
throwError(const Msg &msg) const
{
  return throwError(msg.str());
}

bool
CTkApp::
throwError(const QString &msg) const
{
  auto cmsg = msg.toStdString();

//std::cerr << "Error: " << currentCommand().toStdString() << cmsg << "\n";

  auto *sobj = Tcl_NewStringObj(cmsg.c_str(), int(cmsg.size()));
  Tcl_SetErrorCode(getInterp(), "TCL", "", nullptr);
  Tcl_SetObjResult(getInterp(), sobj);

  return false;
}

void
CTkApp::
debugCmd(const QString &cmd, const Args &args) const
{
  if (getenv("CTK_APP_DEBUG")) {
    std::cerr << "Run: " << cmd.toStdString();
    for (const auto &arg : args)
      std::cerr << " " << variantToString(arg).toStdString();
    std::cerr << "\n";
  }
}

bool
CTkApp::
TODO(const Msg &msg) const
{
  return TODO(msg.str());
}

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
    std::cerr << " " << variantToString(arg).toStdString();

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
    std::cerr << " " << variantToString(arg).toStdString();

  std::cerr << "\n";

  return false;
}

void
CTkApp::
showDebug()
{
  if (! s_debug)
    s_debug = new CTkAppDebug(this);

  s_debug->show();
}

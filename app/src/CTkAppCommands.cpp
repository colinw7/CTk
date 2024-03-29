#include <CTkAppCommands.h>
#include <CTkAppWidgetCommand.h>

#include <CTkAppImageCommand.h>
#include <CTkApp.h>

#include <CTkAppWidget.h>
#include <CTkAppRoot.h>
#include <CTkAppButton.h>
#include <CTkAppCanvas.h>
#include <CTkAppCheckButton.h>
#include <CTkAppComboBox.h>
#include <CTkAppEntry.h>
#include <CTkAppFrame.h>
#include <CTkAppLabel.h>
#include <CTkAppLabelFrame.h>
#include <CTkAppListBox.h>
#include <CTkAppMenu.h>
#include <CTkAppMenuButton.h>
#include <CTkAppNoteBook.h>
#include <CTkAppMessage.h>
#include <CTkAppPanedWindow.h>
#include <CTkAppRadioButton.h>
#include <CTkAppScale.h>
#include <CTkAppScrollBar.h>
#include <CTkAppSpinBox.h>
#include <CTkAppText.h>
#include <CTkAppTopLevel.h>
#include <CTkAppTreeView.h>

#include <CTkAppPackLayout.h>
#include <CTkAppPlaceLayout.h>
#include <CTkAppGridLayout.h>
#include <CTkAppPackLayout.h>

#include <CTkAppImage.h>
#include <CTkAppFont.h>
#include <CTkAppOptionValue.h>
#include <CTkAppOptData.h>
#include <CTkAppUtil.h>
#include <CTkAppX11.h>

#include <CQTclUtil.h>

#include <CMatrix2D.h>
#include <CScreenUnits.h>

#include <QApplication>
#include <QFontDatabase>
#include <QMessageBox>
#include <QFileDialog>
#include <QScreen>
#include <QEventLoop>

//---

#define CTkAppCommandDef(NAME, CLASS) \
class CLASS : public CTkAppCommand { \
 public: \
  CLASS(CTkApp *tk, const char *name=NAME) : \
   CTkAppCommand(tk, tk->mapName(name)) { \
  } \
\
 protected: \
  bool run(const Args &args) override; \
};

#define CTkAppDerivedCommandDef(NAME, CLASS, DERIVED) \
class CLASS : public DERIVED { \
 public: \
  CLASS(CTkApp *tk) : \
   DERIVED(tk, NAME) { \
  } \
};

//---

  // widgets
  CTkAppCommandDef("button"     , CTkAppButtonCmd)
  CTkAppCommandDef("canvas"     , CTkAppCanvasCmd)
  CTkAppCommandDef("checkbutton", CTkAppCheckButtonCmd)
  CTkAppCommandDef("combobox"   , CTkAppComboBoxCmd)
  CTkAppCommandDef("entry"      , CTkAppEntryCmd)
  CTkAppCommandDef("frame"      , CTkAppFrameCmd)
  CTkAppCommandDef("label"      , CTkAppLabelCmd)
  CTkAppCommandDef("labelframe" , CTkAppLabelFrameCmd)
  CTkAppCommandDef("listbox"    , CTkAppListBoxCmd)
  CTkAppCommandDef("menu"       , CTkAppMenuCmd)
  CTkAppCommandDef("menubutton" , CTkAppMenuButtonCmd)
  CTkAppCommandDef("message"    , CTkAppMessageCmd)
  CTkAppCommandDef("notebook"   , CTkAppNoteBookCmd)
  CTkAppCommandDef("panedwindow", CTkAppPanedWindowCmd)
  CTkAppCommandDef("radiobutton", CTkAppRadioButtonCmd)
  CTkAppCommandDef("scale"      , CTkAppScaleCmd)
  CTkAppCommandDef("scrollbar"  , CTkAppScrollBarCmd)
  CTkAppCommandDef("spinbox"    , CTkAppSpinBoxCmd)
  CTkAppCommandDef("text"       , CTkAppTextCmd)
  CTkAppCommandDef("toplevel"   , CTkAppTopLevelCmd)
  CTkAppCommandDef("treeview"   , CTkAppTreeViewCmd)

  // widget manipulation
  CTkAppCommandDef("bell"     , CTkAppBellCmd)
  CTkAppCommandDef("bind"     , CTkAppBindCmd)
  CTkAppCommandDef("bindtags" , CTkAppBindTagsCmd)
  CTkAppCommandDef("clipboard", CTkAppClipBoardCmd);
  CTkAppCommandDef("destroy"  , CTkAppDestroyCmd);
  CTkAppCommandDef("event"    , CTkAppEventCmd)
  CTkAppCommandDef("focus"    , CTkAppFocusCmd)
  CTkAppCommandDef("font"     , CTkAppFontCmd)
  CTkAppCommandDef("grab"     , CTkAppGrabCmd)
  CTkAppCommandDef("grid"     , CTkAppGridCmd)
  CTkAppCommandDef("image"    , CTkAppImageCmd)
  CTkAppCommandDef("lower"    , CTkAppLowerCmd)
  CTkAppCommandDef("option"   , CTkAppOptionCmd)
  CTkAppCommandDef("pack"     , CTkAppPackCmd)
  CTkAppCommandDef("place"    , CTkAppPlaceCmd)
  CTkAppCommandDef("raise"    , CTkAppRaiseCmd)
  CTkAppCommandDef("selection", CTkAppSelectionCmd)
  CTkAppCommandDef("send"     , CTkAppSendCmd)
  CTkAppCommandDef("tk"       , CTkAppTkCmd)
//CTkAppCommandDef("tkerror"  , CTkAppTkErrorCmd)
  CTkAppCommandDef("tkwait"   , CTkAppTkWaitCmd)
  CTkAppCommandDef("update"   , CTkAppUpdateCmd)
  CTkAppCommandDef("winfo"    , CTkAppWInfoCmd)
  CTkAppCommandDef("wm"       , CTkAppWmCmd)

  // support procs
//CTkAppCommandDef("tk_bisque"           , CTkAppTkBisqueCmd)
//CTkAppCommandDef("tk_chooseColor"      , CTkAppTkChooseColorCmd)
  CTkAppCommandDef("tk_chooseDirectory"  , CTkAppTkChooseDirectoryCmd)
  CTkAppCommandDef("tk_dialog"           , CTkAppTkDialogCmd)
//CTkAppCommandDef("tk_focusFollowsMouse", CTkAppTkFocusFollowsMouseCmd)
//CTkAppCommandDef("tk_focusNext"        , CTkAppTkFocusNextCmd)
//CTkAppCommandDef("tk_focusPrev"        , CTkAppTkFocusPrevCmd)
  CTkAppCommandDef("tk_getOpenFile"      , CTkAppTkGetOpenFileCmd)
  CTkAppCommandDef("tk_getSaveFile"      , CTkAppTkGetSaveFileCmd)
  CTkAppCommandDef("tk_messageBox"       , CTkAppTkMessageBoxCmd)
  CTkAppCommandDef("tk_popup"            , CTkAppTkPopupCmd)
//CTkAppCommandDef("tk_setPalette"       , CTkAppTkSetPaletteCmd)

  CTkAppDerivedCommandDef("ttk::button"     , CTkAppTtkButtonCmd     , CTkAppButtonCmd)
  CTkAppDerivedCommandDef("ttk::checkbutton", CTkAppTtkCheckButtonCmd, CTkAppCheckButtonCmd)
  CTkAppDerivedCommandDef("ttk::combobox"   , CTkAppTtkComboBoxCmd   , CTkAppComboBoxCmd)
  CTkAppDerivedCommandDef("ttk::entry"      , CTkAppTtkEntryCmd      , CTkAppEntryCmd)
  CTkAppDerivedCommandDef("ttk::frame"      , CTkAppTtkFrameCmd      , CTkAppFrameCmd)
  CTkAppDerivedCommandDef("ttk::label"      , CTkAppTtkLabeCmd       , CTkAppLabelCmd)
  CTkAppDerivedCommandDef("ttk::labelframe" , CTkAppTtkLabeFrameCmd  , CTkAppLabelFrameCmd)
  CTkAppDerivedCommandDef("ttk::menubutton" , CTkAppTtkMenuButtonCmd , CTkAppMenuButtonCmd)
  CTkAppDerivedCommandDef("ttk::notebook"   , CTkAppTtkNoteBookCmd   , CTkAppNoteBookCmd)
  CTkAppDerivedCommandDef("ttk::panedwindow", CTkAppTtkPanedWindowCmd, CTkAppPanedWindowCmd)
//CTkAppCommandDef       ("ttk::progressbar", CTkAppTtkProgressBarCmd)
  CTkAppDerivedCommandDef("ttk::radiobutton", CTkAppTtkRadioButtonCmd, CTkAppRadioButtonCmd)
  CTkAppDerivedCommandDef("ttk::scale"      , CTkAppTtkScaleCmd      , CTkAppScaleCmd)
  CTkAppDerivedCommandDef("ttk::scrollbar"  , CTkAppTtkScrollBarCmd  , CTkAppScrollBarCmd)
//CTkAppCommandDef       ("ttk::separator"  , CTkAppTtkSeparatorCmd)
//CTkAppCommandDef       ("ttk::sizegrip"   , CTkAppTtkSizeGripCmd)
  CTkAppDerivedCommandDef("ttk::spinbox"    , CTkAppTtkSpinBoxCmd    , CTkAppSpinBoxCmd)
  CTkAppDerivedCommandDef("ttk::treeview"   , CTkAppTtkTreeViewCmd   , CTkAppTreeViewCmd)

//CTkAppCommandDef("ttk::style", CTkAppTtkStyleCmd)

//CTkAppCommandDef("ttk_image", CTkAppTtkImageCmd);
//CTkAppCommandDef("ttk_vsapi", CTkAppTtkVsAPICmd);

#ifdef CTK_APP_TKPATH
  CTkAppCommandDef("matrix"            , CTkAppMatrixCmd)
  CTkAppCommandDef("gradientstopsstyle", CTkAppGradientStopStyleCmd)
#endif

  CTkAppCommandDef("error_info", CTkAppErrorInfoCmd)

//---

class CTkVarTraceEventLoop : public CTclTraceProc {
 public:
  explicit CTkVarTraceEventLoop(CTclApp *app, const QString &name) :
   CTclTraceProc(app), name_(name) {
    eventLoop_ = new QEventLoop;
  }

 ~CTkVarTraceEventLoop() {
    delete eventLoop_;
  }

  void startLoop() {
    eventLoop_->exec();
  }

  void handleWrite(const char *) override { eventLoop_->quit(); }

 protected:
  QString     name_;
  QEventLoop* eventLoop_ { nullptr };
};

//---

namespace CTkAppCommands {

void
addCommands(CTkApp *tk)
{
  new CTkAppBellCmd       (tk);
  new CTkAppBindCmd       (tk);
  new CTkAppBindTagsCmd   (tk);
  new CTkAppClipBoardCmd  (tk);
  new CTkAppButtonCmd     (tk);
  new CTkAppCanvasCmd     (tk);
  new CTkAppCheckButtonCmd(tk);
//new CTkAppComboBoxCmd   (tk);
  new CTkAppDestroyCmd    (tk);
  new CTkAppEventCmd      (tk);
  new CTkAppEntryCmd      (tk);
  new CTkAppFocusCmd      (tk);
  new CTkAppFontCmd       (tk);
  new CTkAppFrameCmd      (tk);
  new CTkAppGrabCmd       (tk);
  new CTkAppGridCmd       (tk);
  new CTkAppImageCmd      (tk);
  new CTkAppLabelCmd      (tk);
  new CTkAppLabelFrameCmd (tk);
  new CTkAppListBoxCmd    (tk);
  new CTkAppMenuCmd       (tk);
  new CTkAppMenuButtonCmd (tk);
  new CTkAppMessageCmd    (tk);
//new CTkAppNoteBookCmd   (tk);
  new CTkAppLowerCmd      (tk);
  new CTkAppOptionCmd     (tk);
  new CTkAppPackCmd       (tk);
  new CTkAppPanedWindowCmd(tk);
  new CTkAppPlaceCmd      (tk);
  new CTkAppRadioButtonCmd(tk);
  new CTkAppRaiseCmd      (tk);
  new CTkAppScaleCmd      (tk);
  new CTkAppScrollBarCmd  (tk);
  new CTkAppSelectionCmd  (tk);
  new CTkAppSendCmd       (tk);
  new CTkAppSpinBoxCmd    (tk);
  new CTkAppTextCmd       (tk);
  new CTkAppTkCmd         (tk);
  new CTkAppTopLevelCmd   (tk);
//new CTkAppTreeViewCmd   (tk);
  new CTkAppUpdateCmd     (tk);
  new CTkAppWInfoCmd      (tk);
  new CTkAppWmCmd         (tk);

  new CTkAppTkChooseDirectoryCmd(tk);
  new CTkAppTkDialogCmd         (tk);
//new CTkAppTkErrorCmd          (tk);
  new CTkAppTkGetOpenFileCmd    (tk);
  new CTkAppTkGetSaveFileCmd    (tk);
  new CTkAppTkMessageBoxCmd     (tk);
  new CTkAppTkPopupCmd          (tk);
  new CTkAppTkWaitCmd           (tk);

#ifdef CTK_APP_TKPATH
  new CTkAppMatrixCmd           (tk);
  new CTkAppGradientStopStyleCmd(tk);
#endif

  new CTkAppTtkButtonCmd     (tk);
  new CTkAppTtkCheckButtonCmd(tk);
  new CTkAppTtkComboBoxCmd   (tk);
  new CTkAppTtkEntryCmd      (tk);
  new CTkAppTtkFrameCmd      (tk);
  new CTkAppTtkLabeCmd       (tk);
  new CTkAppTtkLabeFrameCmd  (tk);
  new CTkAppTtkMenuButtonCmd (tk);
  new CTkAppTtkNoteBookCmd   (tk);
  new CTkAppTtkPanedWindowCmd(tk);
//new CTkAppTtkProgressBarCmd(tk);
  new CTkAppTtkRadioButtonCmd(tk);
  new CTkAppTtkScaleCmd      (tk);
  new CTkAppTtkScrollBarCmd  (tk);
//new CTkAppTtkSeparatorCmd  (tk);
//new CTkAppTtkSizeGripCmd   (tk);
  new CTkAppTtkSpinBoxCmd    (tk);
  new CTkAppTtkTreeViewCmd   (tk);

  new CTkAppErrorInfoCmd(tk);

  //---

  (void) new CTkAppRootCommand(tk);
}

}

//---

bool
CTkAppBellCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs > 3)
    return tk_->wrongNumArgs("bell ?-displayof window? ?-nice?");

  for (uint i = 0; i < numArgs; ++i) {
    static auto argNames = std::vector<QString>({ "-displayof", "-nice" });
    QString arg;
    if (! tk_->lookupName("option", argNames, args[i], arg, /*quiet*/true))
      return tk_->throwError(tk_->msg() + "bad option \"" + args[i] + "\": must be "
                             "-displayof or -nice");

    if      (arg == "-displayof") {
      ++i;

      if (i < numArgs) {
        auto name = tk_->variantToString(args[i]);

        auto *window = tk_->lookupWidgetByName(name, /*quiet*/true);
        if (! window) return tk_->throwError("bad window path name \"" + name + "\"");
      }
      else
        return tk_->wrongNumArgs("bell ?-displayof window? ?-nice?");

      //tk_->TODO(args);
    }
    else if (arg == "-nice") {
      //tk_->TODO(args);
    }
  }

  QApplication::beep();

  return true;
}

//---

bool
CTkAppBindCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs < 1 || numArgs > 3)
    return tk_->wrongNumArgs("bind window ?pattern? ?command?");

  //---

  // get widget (TODO: late bind ?)
  auto name = tk_->variantToString(args[0]);

  bool          all       = false;
  CTkAppWidget* w         = nullptr;
  void*         classData = nullptr;

  if (name == "all")
    all = true;
  else {
    w = tk_->lookupWidgetByName(name, /*quiet*/true);

    if (! w)
      classData = tk_->getWidgetClassData(name);
  }

  //---

  if (numArgs > 1) {
    CTkAppEventData data;

    if (! tk_->parseEvent(args[1], data))
      return tk_->throwError("bad event pattern \"" + tk_->variantToString(args[1]) + "\"");

    if (numArgs > 2) {
      data.command = tk_->variantToString(args[2]);

      bool rc;

      if      (w)
        rc = w->bindEvent(data);
      else if (classData)
        rc = tk_->bindClassEvent(name, data);
      else if (! all)
        rc = tk_->bindTagEvent(name, data);
      else
        rc = tk_->bindAllEvent(data);

      return rc;
    }
    else {
      std::vector<QString> bindings;

      if      (w)
        w->getBindings(data, bindings);
      else if (classData)
        tk_->getClassBindings(name, data, bindings);
      else if (! all)
        tk_->getTagBindings(name, data, bindings);
      else
        tk_->getAllBindings(data, bindings);

      tk_->setStringArrayResult(bindings);
    }
  }
  else {
    std::vector<QString> bindings;

    if      (w)
      w->getBindings(bindings);
    else if (classData)
      tk_->getClassBindings(name, bindings);
    else if (! all)
      tk_->getTagBindings(name, bindings);
    else
      tk_->getAllBindings(bindings);

    tk_->setStringArrayResult(bindings);
  }

  return true;
}

//---

bool
CTkAppBindTagsCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("bindtags window ?tagList?");

  auto widgetName = args[0];

  auto *w = tk_->lookupWidgetByName(widgetName, /*quiet*/true);
  if (! w) return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  if (numArgs > 1) {
    std::vector<QString> bindtags;
    for (uint i = 1; i < numArgs; ++i) {
      auto arg = tk_->variantToString(args[i]);

      bindtags.push_back(arg);
    }
    w->setBindtags(bindtags);
  }
  else {
    auto bindtags = w->bindtags();
    setStringArrayResult(bindtags);
  }

  return true;
}

//---

bool
CTkAppButtonCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-activebackground"   , "activeBackground"   , "Foreground"         , "#ececec"       },
    { "-activeforeground"   , "activeForeground"   , "Background"         , "#000000"       },
    { "-anchor"             , "anchor"             , "Anchor"             , "center"        },
    { "-background"         , "background"         , "Background"         , "#d9d9d9"       },
    { "-bitmap"             , "bitmap"             , "Bitmap"             , ""              },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
    { "-command"            , "command"            , "Command"            , ""              },
    { "-compound"           , "compound"           , "Compound"           , "none"          },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-default"            , "default"            , "Default"            , "disabled"      },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"       },
    { "-font"               , "font"               , "Font"               , "TkDefaultFont" },
    { "-foreground"         , "foreground"         , "Foreground"         , "#000000"       },
    { "-height"             , "height"             , "Height"             , "0"             },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"       },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"       },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "1"             },
    { "-image"              , "image"              , "Image"              , ""              },
    { "-justify"            , "justify"            , "Justify"            , "center"        },
    { "-overrelief"         , "overRelief"         , "OverRelief"         , ""              },
    { "-padx"               , "padX"               , "Pad"                , "3m"            },
    { "-pady"               , "padY"               , "Pad"                , "1m"            },
    { "-relief"             , "relief"             , "Relief"             , "raised"        },
    { "-repeatdelay"        , "repeatDelay"        , "RepeatDelay"        , "0"             },
    { "-repeatinterval"     , "repeatInterval"     , "RepeatInterval"     , "0"             },
    { "-state"              , "state"              , "State"              , "normal"        },
    { "-takefocus"          , "takeFocus"          , "TakeFocus"          , ""              },
    { "-text"               , "text"               , "Text"               , ""              },
    { "-textvariable"       , "textVariable"       , "Variable"           , ""              },
    { "-underline"          , "underline"          , "Underline"          , "-1"            },
    { "-width"              , "width"              , "Width"              , "0"             },
    { "-wraplength"         , "wrapLength"         , "WrapLength"         , "0"             },

    CTkAppOpt::optSynonym("-bd", "-borderwidth"),
    CTkAppOpt::optSynonym("-bg", "-background" ),
    CTkAppOpt::optSynonym("-fg", "-foreground" ),

    CTkAppOpt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("button pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *button = new CTkAppButton(tk_, parent, childName);

  if (! tk_->useStyle())
    button->setBackground(QColor("#d9d9d9"));

  // TODO: process defaults
  CTkAppDistance pad;
  if (! tk_->variantToDistanceI("3m", pad))
    return false;
  button->setPadX(pad.rvalue);
  button->setPadY(pad.rvalue);

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), button, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    button->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppCanvasCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  using Opt = CTkAppOpt;

  static Opt opts[] = {
  {              "-background"         , "background"         , "Background"         , "#d9d9d9"},
  {              "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "0"      },
  Opt::optReal  ("-closeenough"        , "closeEnough"        , "CloseEnough"        , "1"      ),
  Opt::optBool  ("-confine"            , "confine"            , "Confine"            , "1"      ),
  Opt::optCursor("-cursor"             , "cursor"             , "Cursor"             , ""       ),
  {              "-height"             , "height"             , "Height"             , "7c"     },
  Opt::optColor ("-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"),
  Opt::optColor ("-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"),
  {              "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "1"      },
  {              "-insertbackground"   , "insertBackground"   , "Foreground"         , "#000000"},
  {              "-insertborderwidth"  , "insertBorderWidth"  , "BorderWidth"        , "0"      },
  Opt::optInt   ("-insertofftime"      , "insertOffTime"      , "OffTime"            , "300"    ),
  Opt::optInt   ("-insertontime"       , "insertOnTime"       , "OnTime"             , "600"    ),
  {              "-insertwidth"        , "insertWidth"        , "InsertWidth"        , "2"      },
  {              "-offset"             , "offset"             , "Offset"             , "0,0"    },
  {              "-relief"             , "relief"             , "Relief"             , "flat"   },
  Opt::optString("-scrollregion"       , "scrollRegion"       , "ScrollRegion"       , ""       ),
  {              "-selectbackground"   , "selectBackground"   , "Foreground"         , "#c3c3c3"},
  Opt::optInt   ("-selectborderwidth"  , "selectBorderWidth"  , "BorderWidth"        , "1"      ),
  Opt::optColor ("-selectforeground"   , "selectForeground"   , "Background"         , "#000000"),
  {              "-state"              , "state"              , "State"              , "normal" },
  Opt::optString("-takefocus"          , "takeFocus"          , "TakeFocus"          , ""       ),
  {              "-width"              , "width"              , "Width"              , "10c"    },
  Opt::optString("-xscrollcommand"     , "xScrollCommand"     , "ScrollCommand"      , ""       ),
  {              "-xscrollincrement"   , "xScrollIncrement"   , "ScrollIncrement"    , "0"      },
  Opt::optString("-yscrollcommand"     , "yScrollCommand"     , "ScrollCommand"      , ""       ),
  {              "-yscrollincrement"   , "yScrollIncrement"   , "ScrollIncrement"    , "0"      },

  Opt::optSynonym("-bd", "-borderwidth"),
  Opt::optSynonym("-bg", "-background" ),

  Opt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("canvas pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *canvas = new CTkAppCanvas(tk_, parent, childName);

  if (! tk_->useStyle())
    canvas->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), canvas, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    canvas->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppCheckButtonCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  using Opt = CTkAppOpt;

  static Opt opts[] = {
  { "-activebackground"   , "activeBackground"   , "Foreground"         , "#ececec"       },
  { "-activeforeground"   , "activeForeground"   , "Background"         , "#000000"       },
  { "-anchor"             , "anchor"             , "Anchor"             , "center"        },
  { "-background"         , "background"         , "Background"         , "#d9d9d9"       },
  { "-bitmap"             , "bitmap"             , "Bitmap"             , ""              },
  { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
  { "-command"            , "command"            , "Command"            , ""              },
  { "-compound"           , "compound"           , "Compound"           , "none"          },
  { "-cursor"             , "cursor"             , "Cursor"             , ""              },
  { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"       },
  { "-font"               , "font"               , "Font"               , "TkDefaultFont" },
  { "-foreground"         , "foreground"         , "Foreground"         , "#000000"       },
  { "-height"             , "height"             , "Height"             , "0"             },
  { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"       },
  { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"       },
  { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "1"             },
  { "-image"              , "image"              , "Image"              , ""              },
  Opt::optBool("-indicatoron", "indicatorOn", "IndicatorOn", "1"),
  { "-justify"            , "justify"            , "Justify"            , "center"        },
  { "-offrelief"          , "offRelief"          , "OffRelief"          , "raised"        },
  { "-offvalue"           , "offValue"           , "Value"              , "0"             },
  { "-onvalue"            , "onValue"            , "Value"              , "1"             },
  { "-overrelief"         , "overRelief"         , "OverRelief"         , ""              },
  { "-padx"               , "padX"               , "Pad"                , "1"             },
  { "-pady"               , "padY"               , "Pad"                , "1"             },
  { "-relief"             , "relief"             , "Relief"             , "flat"          },
  { "-selectcolor"        , "selectColor"        , "Background"         , "#ffffff"       },
  { "-selectimage"        , "selectImage"        , "SelectImage"        , ""              },
  { "-state"              , "state"              , "State"              , "normal"        },
  { "-takefocus"          , "takeFocus"          , "TakeFocus"          , ""              },
  { "-text"               , "text"               , "Text"               , ""              },
  { "-textvariable"       , "textVariable"       , "Variable"           , ""              },
  { "-tristateimage"      , "tristateImage"      , "TristateImage"      , ""              },
  { "-tristatevalue"      , "tristateValue"      , "TristateValue"      , ""              },
  { "-underline"          , "underline"          , "Underline"          , "-1"            },
  { "-variable"           , "variable"           , "Variable"           , ""              },
  { "-width"              , "width"              , "Width"              , "0"             },
  { "-wraplength"         , "wrapLength"         , "WrapLength"         , "0"             },

  Opt::optSynonym("-bd", "-borderwidth"),
  Opt::optSynonym("-bg", "-background" ),
  Opt::optSynonym("-fg", "-foreground" ),

  Opt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("checkbutton pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *check = new CTkAppCheckButton(tk_, parent, childName);

  if (! tk_->useStyle())
    check->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), check, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    check->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppComboBoxCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-background"         , "background"         , "Background"         , "#ffffff"    },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"          },
    { "-cursor"             , "cursor"             , "Cursor"             , "xterm"      },
    { "-disabledbackground" , "disabledBackground" , "DisabledBackground" , "#d9d9d9"    },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"    },
    { "-exportselection"    , "exportSelection"    , "ExportSelection"    , "1"          },
    { "-font"               , "font"               , "Font"               , "TkTextFont" },
    { "-foreground"         , "foreground"         , "Foreground"         , "#000000"    },
    { "-height"             , "height"             , "Height"             , "0"          },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"    },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"    },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "1"          },
    { "-insertbackground"   , "insertBackground"   , "Foreground"         , "#000000"    },
    { "-insertborderwidth"  , "insertBorderWidth"  , "BorderWidth"        , "0"          },
    { "-insertofftime"      , "insertOffTime"      , "OffTime"            , "300"        },
    { "-insertontime"       , "insertOnTime"       , "OnTime"             , "600"        },
    { "-insertwidth"        , "insertWidth"        , "InsertWidth"        , "2"          },
    { "-invalidcommand"     , "invalidCommand"     , "InvalidCommand"     , ""           },
    { "-justify"            , "justify"            , "Justify"            , "left"       },
    { "-postcommand"        , "postCommand"        , "PostCommand"        , ""           },
    { "-readonlybackground" , "readonlyBackground" , "ReadonlyBackground" , "#d9d9d9"    },
    { "-relief"             , "relief"             , "Relief"             , "sunken"     },
    { "-selectbackground"   , "selectBackground"   , "Foreground"         , "#c3c3c3"    },
    { "-selectborderwidth"  , "selectBorderWidth"  , "BorderWidth"        , "0"          },
    { "-selectforeground"   , "selectForeground"   , "Background"         , "#000000"    },
    { "-show"               , "show"               , "Show"               , ""           },
    { "-state"              , "state"              , "State"              , "normal"     },
    { "-takefocus"          , "takeFocus"          , "TakeFocus"          , ""           },
    { "-textvariable"       , "textVariable"       , "Variable"           , ""           },
    { "-validate"           , "validate"           , "Validate"           , "none"       },
    { "-validatecommand"    , "validateCommand"    , "ValidateCommand"    , ""           },
    { "-values"             , "values"             , "Values"             , ""           },
    { "-width"              , "width"              , "Width"              , "20"         },
    { "-xscrollcommand"     , "xScrollCommand"     , "ScrollCommand"      , ""           },

    CTkAppOpt::optSynonym("-bd"    , "-borderwidth"    ),
    CTkAppOpt::optSynonym("-bg"    , "-background"     ),
    CTkAppOpt::optSynonym("-fg"    , "-foreground"     ),
    CTkAppOpt::optSynonym("-invcmd", "-invalidcommand" ),
    CTkAppOpt::optSynonym("-vcmd"  , "-validatecommand"),

    CTkAppOpt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("combobox pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *combo = new CTkAppComboBox(tk_, parent, childName);

  if (! tk_->useStyle())
    combo->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), combo, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    combo->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppClipBoardCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("clipboard options ?arg arg ...?");

  static auto optionNames = std::vector<QString>({
    "append", "clear", "get"});

  QString opt;
  if (! tk_->lookupOptionName(optionNames, args[0], opt))
    return false;

  if      (opt == "clear") {
    uint i = 1;

    for ( ; i < numArgs; ++i) {
      auto arg = tk_->variantToString(args[i]);
      if (arg[0] != '-') break;

      static auto argNames = std::vector<QString>({ "-displayof" });
      QString arg1;
      if (! tk_->lookupName("option", argNames, arg, arg1, /*quiet*/true))
        return tk_->throwError("bad option \"" + arg + "\": must be -displayof");

      if      (arg1 == "-displayof") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("clipboard clear ?-displayof window?");
      }
    }

    if (i != numArgs)
      return tk_->wrongNumArgs("clipboard append ?-displayof window? "
                               "?-format format? ?-type type? ?--? data");

    tk_->setClipboardText("", CTkAppClipboard::Clipboard);
  }
  else if (opt == "append") {
    uint i = 1;

    for ( ; i < numArgs; ++i) {
      auto arg = tk_->variantToString(args[i]);
      if (arg == "--") { ++i; break; }

      if (arg[0] != '-') break;

      static auto argNames = std::vector<QString>({ "-displayof", "-format", "-type" });
      QString arg1;
      if (! tk_->lookupName("option", argNames, arg, arg1, /*quiet*/true))
        return tk_->throwError("bad option \"" + arg + "\": must be "
                               "-displayof, -format, or -type");

      if      (arg1 == "-displayof") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("clipboard append ?-displayof window?");
      }
      else if (arg1 == "-format") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("clipboard append ?-format format?");
      }
      else if (arg1 == "-type") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("clipboard append ?-type window?");
      }
    }

    if (i >= numArgs)
      return tk_->wrongNumArgs("clipboard append ?-option value ...? data");

    tk_->setClipboardText(tk_->getClipboardText(CTkAppClipboard::Clipboard) +
                          tk_->variantToString(args[i]), CTkAppClipboard::Clipboard);
  }
  else if (opt == "get") {
    uint i = 1;

    for ( ; i < numArgs; ++i) {
      auto arg = tk_->variantToString(args[i]);
      if (arg[0] != '-') break;

      static auto argNames = std::vector<QString>({ "-displayof", "-type" });
      QString arg1;
      if (! tk_->lookupName("option", argNames, arg, arg1, /*quiet*/true))
        return tk_->throwError("bad option \"" + arg + "\": must be -displayof, -type");

      if      (arg1 == "-displayof") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("clipboard get ?-displayof window?");
      }
      else if (arg1 == "-type") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("clipboard get ?-type window?");
      }
    }

    if (i != numArgs)
      return tk_->wrongNumArgs("clipboard get ?-displayof window? ?-type type?");

    setStringResult(tk_->getClipboardText(CTkAppClipboard::Clipboard));
  }

  return true;
}

//---

bool
CTkAppDestroyCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto *w = tk_->lookupWidgetByName(args[i], /*quiet*/true);
    if (! w) continue;

    auto *layout = w->getTkLayout();

    w->deleteLater();

    if (layout)
      layout->invalidate();
  }

  return true;
}

//---

bool
CTkAppEventCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("event option ?arg?");

  static auto optionNames = std::vector<QString>({
    "add", "delete", "generate", "info" });

  QString arg;
  if (! tk_->lookupOptionName(optionNames, args[0], arg))
    return false;

  if      (arg == "add") {
    if (numArgs < 3)
      return tk_->wrongNumArgs("event add virtual sequence ?sequence ...?");

    CTkAppVirtualEventData vdata;
    if (! tk_->stringToVirtualEvent(args[1], vdata))
      return false;

    CTkAppEventData edata;
    if (! tk_->parseEvent(args[2], edata))
      return tk_->throwError(tk_->msg() + "bad event pattern \"" + args[2] + "\"");

    tk_->addVirtualEventData(vdata, edata);
  }
  else if (arg == "delete") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("event delete virtual ?sequence ...?");

    CTkAppVirtualEventData vdata;
    if (! tk_->stringToVirtualEvent(args[1], vdata))
      return false;

    if (numArgs == 2)
      tk_->deleteAllVirtualEventData(vdata);
    else {
      for (uint i = 2; i < numArgs; ++i) {
        CTkAppEventData edata;
        if (! tk_->parseEvent(args[i], edata))
          return tk_->throwError(tk_->msg() + "bad event pattern \"" + args[i] + "\"");

        tk_->deleteVirtualEventData(vdata, edata);
      }
    }
  }
  else if (arg == "generate") {
    if (numArgs < 3)
      return tk_->wrongNumArgs("event generate window event ?-option value ...?");

    auto *w = tk_->lookupWidgetByName(args[1]);
    if (! w) return false;

    CTkAppEventData data;
    if (! tk_->parseEvent(args[2], data))
      return tk_->throwError(tk_->msg() + "bad event pattern \"" + args[2] + "\"");

    uint i = 3;

    while (i < numArgs) {
      auto name = tk_->variantToString(args[i++]);

      if (i >= numArgs)
        return tk_->throwError(tk_->msg() + "value for \"" + name + "\" missing");

      if      (name == "-x") {
        long x;
        if (! tk_->variantToInt(args[i], x))
          return tk_->invalidInteger(args[i]);

        data.x = x;
      }
      else if (name == "-y") {
        long y;
        if (! tk_->variantToInt(args[i], y))
          return tk_->invalidInteger(args[i]);

        data.y = y;
      }
      else if (name == "-keysym") {
        auto str = tk_->variantToString(args[i]);

        data.key    = tk_->stringToKeySym(str);
        data.keyStr = str;
      }
      else
        return tk_->throwError(tk_->msg() + "bad option \"" + name + "\"");

      ++i;
    }

    w->generateEvent(data);
  }
  else if (arg == "info") {
    if      (numArgs == 1) {
      std::vector<QString> names;
      tk_->virtualEventNames(names);

      tk_->setStringArrayResult(names);
    }
    else if (numArgs == 2) {
      CTkAppVirtualEventData vdata;
      if (! tk_->stringToVirtualEvent(args[1], vdata))
        return false;

      std::vector<QString> names;
      tk_->virtualEventNames(vdata, names);

      tk_->setStringArrayResult(names);
    }
    else {
      return tk_->wrongNumArgs("event info ?virtual?");
    }
  }

  return true;
}

//---

bool
CTkAppEntryCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-background"           , "background"           , "Background"           , "#ffffff"    },
    { "-borderwidth"          , "borderWidth"          , "BorderWidth"          , "1"          },
    { "-cursor"               , "cursor"               , "Cursor"               , "xterm"      },
    { "-disabledbackground"   , "disabledBackground"   , "DisabledBackground"   , "#d9d9d9"    },
    { "-disabledforeground"   , "disabledForeground"   , "DisabledForeground"   , "#a3a3a3"    },
    { "-exportselection"      , "exportSelection"      , "ExportSelection"      , "1"          },
    { "-font"                 , "font"                 , "Font"                 , "TkTextFont" },
    { "-foreground"           , "foreground"           , "Foreground"           , "#000000"    },
    { "-highlightbackground"  , "highlightBackground"  , "HighlightBackground"  , "#d9d9d9"    },
    { "-highlightcolor"       , "highlightColor"       , "HighlightColor"       , "#000000"    },
    { "-highlightthickness"   , "highlightThickness"   , "HighlightThickness"   , "1"          },
    { "-insertbackground"     , "insertBackground"     , "Foreground"           , "#000000"    },
    { "-insertborderwidth"    , "insertBorderWidth"    , "BorderWidth"          , "0"          },
    { "-insertofftime"        , "insertOffTime"        , "OffTime"              , "300"        },
    { "-insertontime"         , "insertOnTime"         , "OnTime"               , "600"        },
    { "-insertwidth"          , "insertWidth"          , "InsertWidth"          , "2"          },
    { "-invalidcommand"       , "invalidCommand"       , "InvalidCommand"       , ""           },
    { "-justify"              , "justify"              , "Justify"              , "left"       },
    { "-placeholder"          , "placeHolder"          , "PlaceHolder"          , ""           },
    { "-placeholderforeground", "placeholderForeground", "PlaceholderForeground", ""           },
    { "-readonlybackground"   , "readonlyBackground"   , "ReadonlyBackground"   , "#d9d9d9"    },
    { "-relief"               , "relief"               , "Relief"               , "sunken"     },
    { "-selectbackground"     , "selectBackground"     , "Foreground"           , "#c3c3c3"    },
    { "-selectborderwidth"    , "selectBorderWidth"    , "BorderWidth"          , "0"          },
    { "-selectforeground"     , "selectForeground"     , "Background"           , "#000000"    },
    { "-show"                 , "show"                 , "Show"                 , ""           },
    { "-state"                , "state"                , "State"                , "normal"     },
    { "-takefocus"            , "takeFocus"            , "TakeFocus"            , ""           },
    { "-textvariable"         , "textVariable"         , "Variable"             , ""           },
    { "-validate"             , "validate"             , "Validate"             , "none"       },
    { "-validatecommand"      , "validateCommand"      , "ValidateCommand"      , ""           },
    { "-width"                , "width"                , "Width"                , "20"         },
    { "-xscrollcommand"       , "xScrollCommand"       , "ScrollCommand"        , ""           },

    CTkAppOpt::optSynonym("-bd"    , "-borderwidth"    ),
    CTkAppOpt::optSynonym("-bg"    , "-background"     ),
    CTkAppOpt::optSynonym("-fg"    , "-foreground"     ),
    CTkAppOpt::optSynonym("-invcmd", "-invalidcommand" ),
    CTkAppOpt::optSynonym("-vcmd"  , "-validatecommand"),

    CTkAppOpt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("entry pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *entry = new CTkAppEntry(tk_, parent, childName);

  if (! tk_->useStyle())
    entry->setBackground(QColor("#ffffff"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), entry, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    entry->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppFocusCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if      (numArgs == 0) {
    auto *w = QApplication::focusWidget();
    if (! w) return false;

    auto *tw = tk_->lookupWidget(w);
    if (! tw) return false;

    setStringResult(tw->getFullName());

    return true;
  }
  else if (numArgs == 1) {
    auto *w = tk_->lookupWidgetByName(args[0]);
    if (! w) return false;

    w->setFocus();
  }
  else {
    uint i = 1;

    for ( ; i < numArgs; ++i) {
      auto arg = tk_->variantToString(args[i]);
      if (arg[0] != '-') break;

      if      (arg == "-displayof") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("focus -displayof window");
      }
      else if (arg == "-force") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("focus -force window");
      }
      else if (arg == "-lastfor") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("focus -lastfor window");
      }
      else
        return tk_->throwError("bad option \"" + arg + "\": must be "
                               "-displayof, -force, or -lastfor");
    }

    if (i >= numArgs)
      return false;

    auto *w = tk_->lookupWidgetByName(args[i]);
    if (! w) return false;

    w->setFocus();
  }

  return true;
}

//---

bool
CTkAppFontCmd::
run(const Args &args)
{
  static auto fontOptionNames = std::vector<QString>({
    "-family", "-size", "-weight", "-slant", "-underline", "-overstrike"});

  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  auto processFontOptions = [&](CTkAppFontRef &font, uint i) {
    for ( ; i < numArgs; ++i) {
      QString option;
      if (! tk_->lookupOptionName(fontOptionNames, args[i], option))
        return false;

      if      (option == "-family") {
        ++i; if (i >= numArgs) return tk_->throwError("value for \"-family\" option missing");

        font->setFamily(tk_->variantToString(args[i]));
      }
      else if (option == "-size") {
        ++i; if (i >= numArgs) return tk_->throwError("value for \"-size\" option missing");

        double size;
        if (! tk_->variantToReal(args[i], size))
          return tk_->invalidReal(args[i]);

        if (size >= 0)
          font->setPointSize(size);
        else
          font->setPixelSize(size);
      }
      else if (option == "-weight") {
        ++i; if (i >= numArgs) return tk_->throwError("value for \"-weight\" option missing");

        if      (args[i] == "normal")
          font->setNormal();
        else if (args[i] == "bold")
          font->setBold();
        else
          return tk_->throwError(tk_->msg() + "bad -weight value \"" + args[i] + "\": must be "
                                 "normal, or bold");
      }
      else if (option == "-slant") {
        ++i; if (i >= numArgs) return tk_->throwError("value for \"-slant\" option missing");

        if      (args[i] == "roman")
          font->setItalic(false);
        else if (args[i] == "italic")
          font->setItalic(true);
        else
          return tk_->throwError(tk_->msg() + "bad -slant value \"" + args[i] + "\" : must be "
                                 "roman, or italic");
      }
      else if (option == "-underline") {
        ++i; if (i >= numArgs) return tk_->throwError("value for \"-underline\" option missing");

        bool b;
        if (! tk_->variantToBool(args[i], b))
          return tk_->invalidBool(args[i]);

        font->setUnderline(b);
      }
      else if (option == "-overstrike") {
        ++i; if (i >= numArgs) return tk_->throwError("value for \"-overstrike\" option missing");

        bool b;
        if (! tk_->variantToBool(args[i], b))
          return tk_->invalidBool(args[i]);

        font->setOverstrike(b);
      }
    }

    return true;
  };

  if (numArgs == 0)
    return tk_->wrongNumArgs("font option ?arg?");

  static auto optionNames = std::vector<QString>({
    "actual", "configure", "create", "delete", "families", "measure", "metrics", "names"});

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  if      (option == "actual") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("font actual font ?-displayof window? ?-option? ?--? ?char?");

    auto qfont = tk_->getQFont(args[1]);

    CTkAppFontData fontData;
    CTkAppFont::getFontData(qfont, fontData);

    uint i = 2;

    if (i < numArgs && args[i] == "-displayof") {
      ++i;

      if (i >= numArgs)
        return tk_->throwError("value for \"-displayof\" missing");

      ++i;
    }

    if (i >= numArgs) {
      std::vector<QVariant> values;

      values.push_back("-family"    ); values.push_back(fontData.family);
      values.push_back("-size"      ); values.push_back(fontData.size);
      values.push_back("-weight"    ); values.push_back(fontData.weight);
      values.push_back("-slant"     ); values.push_back(fontData.slant);
      values.push_back("-underline" ); values.push_back(fontData.underline);
      values.push_back("-overstrike"); values.push_back(fontData.overstrike);

      tk_->setVariantArrayResult(values);

      return true;
    }

    for ( ; i < numArgs; ++i) {
      auto arg = tk_->variantToString(args[i]);
      if (arg == "--") { ++i; break; }

      if (arg[0] != '-') break;

      QString option;
      if (! tk_->lookupOptionName(fontOptionNames, args[i], option))
        return false;

      if      (option == "-family"    ) setStringResult(fontData.family);
      else if (option == "-size"      ) setRealResult  (fontData.size);
      else if (option == "-weight"    ) setStringResult(fontData.weight);
      else if (option == "-slant"     ) setStringResult(fontData.slant);
      else if (option == "-underline" ) setBoolResult  (fontData.underline);
      else if (option == "-overstrike") setBoolResult  (fontData.overstrike);
    }

    QString chars;
    if (i < numArgs)
      chars = tk_->variantToString(args[i++]);

    if (i < numArgs)
      return tk_->wrongNumArgs("font actual font ?-displayof window? ?-option? ?--? ?char?");

    if (chars.size() > 1)
      return tk_->throwError("expected a single character but got \"" + chars + "\"");
  }
  else if (option == "configure") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("font configure fontname ?-option value ...?");

    auto font = tk_->getFont(args[1]);

    if (! font)
      return tk_->throwError(tk_->msg() + "named font \"" + args[1] + "\" doesn't exist");

    CTkAppFontData fontData;
    font->getFontData(fontData);

    if      (numArgs == 2) {
      std::vector<QVariant> values;

      values.push_back("-family"    ); values.push_back(fontData.family);
      values.push_back("-size"      ); values.push_back(fontData.size);
      values.push_back("-weight"    ); values.push_back(fontData.weight);
      values.push_back("-slant"     ); values.push_back(fontData.slant);
      values.push_back("-underline" ); values.push_back(fontData.underline);
      values.push_back("-overstrike"); values.push_back(fontData.overstrike);

      tk_->setVariantArrayResult(values);

      return true;
    }
    else if (numArgs == 3) {
      QString option;
      if (! tk_->lookupOptionName(fontOptionNames, args[2], option))
        return false;

      if      (option == "-family"    ) setStringResult(fontData.family);
      else if (option == "-size"      ) setRealResult  (fontData.size);
      else if (option == "-weight"    ) setStringResult(fontData.weight);
      else if (option == "-slant"     ) setStringResult(fontData.slant);
      else if (option == "-underline" ) setBoolResult  (fontData.underline);
      else if (option == "-overstrike") setBoolResult  (fontData.overstrike);
    }
    else {
      if (! processFontOptions(font, 2))
        return false;
    }
  }
  else if (option == "create") {
    QVariant name;

    uint i = 1;

    if (numArgs > 1) {
      auto arg = tk_->variantToString(args[i]);

      if (arg[0] != '-')
        name = args[i++];
    }

    if (! tk_->variantIsValid(name))
      name = tk_->getNewFontName();

    auto font = tk_->getFont(name);

    if (font)
      return tk_->throwError(tk_->msg() + "named font \"" + name + "\" already exists");

    font = tk_->createFont(name);

    if (! processFontOptions(font, i)) {
      tk_->deleteFont(font);
      return false;
    }

    setResult(name);
  }
  else if (option == "delete") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("font delete fontname ?fontname ...?");

    uint i = 1;

    for ( ; i < numArgs; ++i) {
      auto font = tk_->getFont(args[i]);

      if (! font)
        return tk_->throwError(tk_->msg() + "named font \"" + args[i] +
                               "\" doesn't exist");

      tk_->deleteFont(font);
    }
  }
  else if (option == "families") {
    uint i = 1;

    if (i < numArgs && args[i] == "-displayof") {
      if (i + 1 >= numArgs)
        return tk_->throwError("value for \"-displayof\" missing");

      i += 2;
    }

    if (i < numArgs)
      return tk_->wrongNumArgs("font families ?-displayof window?");

    QFontDatabase fdb;

    std::vector<QString> familyNames;

    const auto families = fdb.families(QFontDatabase::Any);

    for (const QString &family : families) {
      if (fdb.isPrivateFamily(family)) continue;

      //if (fdb.isSmoothlyScalable(family)) continue;
      //if (fdb.isFixedPitch(family)) continue;

      auto familyStr = family;

      //if (familyStr.indexOf('[') >= 0) continue;

      familyNames.push_back(familyStr);
    }

    setStringArrayResult(familyNames);
  }
  else if (option == "measure") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("font measure font ?-displayof window? text");

    auto qfont = tk_->getQFont(args[1]);

    //---

    uint i = 2;

    if (i < numArgs && args[i] == "-displayof") {
      if (i + 1 >= numArgs)
        return tk_->throwError("value for \"-displayof\" missing");

      i += 2;
    }

    if (i >= numArgs)
      return tk_->wrongNumArgs("font measure font ?-displayof window? text");

    //---

    auto text = tk_->variantToString(args[i++]);

    if (i < numArgs)
      return tk_->wrongNumArgs("font measure font ?-displayof window? text");

    QFontMetricsF fm(qfont);

    auto w = fm.horizontalAdvance(text);

    setRealResult(w);
  }
  else if (option == "metrics") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("font metrics font ?-displayof window? ?-option?");

    auto qfont = tk_->getQFont(args[1]);

    //---

    uint i = 2;

    if (i < numArgs && args[i] == "-displayof") {
      if (i + 1 >= numArgs)
        return tk_->throwError("value for \"-displayof\" missing");

      i += 2;
    }

    if (i < numArgs - 1)
      return tk_->wrongNumArgs("font metrics font ?-displayof window? ?-option?");

    //---

    CTkAppFontData fontData;
    CTkAppFont::getFontData(qfont, fontData);

    if (i >= numArgs) {
      std::vector<QVariant> values;

      values.push_back("-ascent"   ); values.push_back(fontData.ascent);
      values.push_back("-descent"  ); values.push_back(fontData.descent);
      values.push_back("-fixed"    ); values.push_back(fontData.fixed);
      values.push_back("-linespace"); values.push_back(fontData.linespace);

      tk_->setVariantArrayResult(values);

      return true;
    }

    if (i == numArgs - 1) {
      if      (args[i] == "-ascent"   ) setRealResult(fontData.ascent);
      else if (args[i] == "-descent"  ) setRealResult(fontData.descent);
      else if (args[i] == "-fixed"    ) setBoolResult(fontData.fixed);
      else if (args[i] == "-linespace") setRealResult(fontData.linespace);
      else
        return tk_->throwError(tk_->msg() + "bad metric \"" + args[i] + "\": must be "
                               "-ascent, -descent, -fixed, or -linespace");
    }
    else
      return tk_->wrongNumArgs("font metrics font ?-displayof window? ?-option?");
  }
  else if (option == "names") {
    if (numArgs != 1)
      return tk_->wrongNumArgs("font names");

    std::vector<QString> names;
    tk_->getFontNames(names);
    setStringArrayResult(names);
  }

  return true;
}

//---

bool
CTkAppFrameCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    {"-background"         , "background"         , "Background"         , "#d9d9d9" },
    {"-backgroundimage"    , "backgroundImage"    , "BackgroundImage"    , ""        },
    {"-borderwidth"        , "borderWidth"        , "BorderWidth"        , "0"       },
    {"-class"              , "class"              , "Class"              , "Frame"   },
    {"-colormap"           , "colormap"           , "Colormap"           , ""        },
    {"-container"          , "container"          , "Container"          , "0"       },
    {"-cursor"             , "cursor"             , "Cursor"             , ""        },
    {"-height"             , "height"             , "Height"             , "0"       },
    {"-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9" },
    {"-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000" },
    {"-highlightthickness" , "highlightThickness" , "HighlightThickness" , "0"       },
    {"-padx"               , "padX"               , "Pad"                , "0"       },
    {"-pady"               , "padY"               , "Pad"                , "0"       },
    {"-relief"             , "relief"             , "Relief"             , "flat"    },
    {"-takefocus"          , "takeFocus"          , "TakeFocus"          , "0"       },
    {"-tile"               , "tile"               , "Tile"               , "0"       },
    {"-visual"             , "visual"             , "Visual"             , ""        },
    {"-width"              , "width"              , "Width"              , "0"       },

    CTkAppOpt::optSynonym("-bd"   , "-borderwidth"    ),
    CTkAppOpt::optSynonym("-bg"   , "-background"     ),
    CTkAppOpt::optSynonym("-bgimg", "-backgroundimage"),

    CTkAppOpt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("frame pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *frame = new CTkAppFrame(tk_, parent, childName);

  if (! tk_->useStyle())
    frame->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), frame, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    frame->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppGrabCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs("grab ?-global? window\" or \"grab option ?arg ...?");

  if      (args[0] == "current") {
    if (numArgs != 1 && numArgs != 2)
      return tk_->wrongNumArgs("grab status window");

    if (numArgs == 1) {
      setStringResult("."); // current grab
    }
    else {
      auto *w = tk_->lookupWidgetByName(args[1]);
      if (! w) return false;

      setStringResult("."); // current grab
    }
  }
  else if (args[0] == "release") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("grab release window");

    auto *w = tk_->lookupWidgetByName(args[1]);
    if (! w) return false;
  }
  else if (args[0] == "set") {
    uint i = 1;

    [[maybe_unused]] bool global = false;
    if (i < numArgs && args[i] == "-global") {
      global = true;
      ++i;
    }

    if (i != numArgs - 1)
      return tk_->wrongNumArgs("grab set ?-global? window");

    auto *w = tk_->lookupWidgetByName(args[i]);
    if (! w) return false;
  }
  else if (args[0] == "status") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("grab status window");

    auto *w = tk_->lookupWidgetByName(args[1]);
    if (! w) return false;

    setStringResult("none");
  }
  else {
    uint i = 0;

    [[maybe_unused]] bool global = false;
    if (i < numArgs && args[i] == "-global") {
      global = true;
      ++i;
    }

    CTkAppWidget *w = nullptr;

    if (i < numArgs)
      w = tk_->lookupWidgetByName(args[i], /*quiet*/true);

    if (! w)
     return tk_->throwError(tk_->msg() + "bad option \"" + args[i] + "\": must be "
              "current, release, set, or status");
  }

  return true;
}

//---

bool
CTkAppGridCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOption opts[] = {
    { "-column"    , CTkAppOptionType::Int   , nullptr },
    { "-columnspan", CTkAppOptionType::Int   , nullptr },
    { "-in"        , CTkAppOptionType::String, nullptr },
    { "-ipadx"     , CTkAppOptionType::Int   , nullptr },
    { "-ipady"     , CTkAppOptionType::Int   , nullptr },
    { "-padx"      , CTkAppOptionType::String, nullptr },
    { "-pady"      , CTkAppOptionType::String, nullptr },
    { "-row"       , CTkAppOptionType::Int   , nullptr },
    { "-rowspan"   , CTkAppOptionType::Int   , nullptr },
    { "-sticky"    , CTkAppOptionType::String, nullptr },
    { nullptr      , CTkAppOptionType::None  , nullptr },
  };

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("grid option arg ?arg ...?");

  bool configure = false;

  //---

  enum class RowColType {
    COLUMNS,
    ROWS
  };

  struct IndexData {
    std::set<long> inds;
  };

  auto addAllInds = [&](CTkAppGridLayout *layout, const RowColType &rowColType,
                        IndexData &indexData) {
    if (rowColType == RowColType::COLUMNS) {
      for (uint i = 0; i < layout->numCols(); ++i) {
        if (layout->isColumnUsed(i))
          indexData.inds.insert(i);
      }
    }
    else {
      for (uint i = 0; i < layout->numRows(); ++i) {
        if (layout->isRowUsed(i))
          indexData.inds.insert(i);
      }
    }
  };

  auto addWidgetInd = [&](CTkAppGridLayout *layout, const RowColType &rowColType,
                          CTkAppWidget *widget, IndexData &indexData) {
    CTkAppGridLayout::Info info;
    if (layout->getChildInfo(widget, info))
      indexData.inds.insert(rowColType == RowColType::COLUMNS ? info.getCol() : info.getRow());
  };

  auto parseLayoutInds = [&](CTkAppGridLayout *layout, const RowColType &rowColType,
                             const QVariant &var, IndexData &indexData) {
    auto str = tk_->variantToString(var);

    if (str == "all") {
      addAllInds(layout, rowColType, indexData);
      return true;
    }

    long ind;
    if (tk_->variantToInt(var, ind)) {
      indexData.inds.insert(ind);
      return true;
    }

    auto *window = tk_->lookupWidgetByName(str, /*quiet*/true);
    if (window) {
      addWidgetInd(layout, rowColType, window, indexData);
      return true;
    }

    std::vector<QString> strs;
    if (tk_->splitList(str, strs) && strs.size() > 1) {
      for (const auto &str1 : strs) {
        if (str1 == "all") {
          addAllInds(layout, rowColType, indexData);
          continue;
        }

        long ind1;
        if (tk_->variantToInt(str1, ind1)) {
          indexData.inds.insert(ind);
          continue;
        }

        auto *window = tk_->lookupWidgetByName(str1, /*quiet*/true);
        if (! window) return false;

        addWidgetInd(layout, rowColType, window, indexData);
      }

      return true;
    }

    return false;
  };

  //---

  static auto optionNames = std::vector<QString>({
    "anchor", "bbox", "columnconfigure", "configure", "content", "forget",
    "info", "location", "propagate", "rowconfigure", "remove", "size", "slaves" });

  uint i = 0;

  QString option;
  if (tk_->lookupName("option", optionNames, args[i], option, /*quiet*/true))
    ++i;

  if      (option == "anchor") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs("grid anchor window ?anchor?");

    auto *window = tk_->lookupWidgetByName(args[i]);
    if (! window) return false;

    ++i;

    if (numArgs == 2) {
      auto anchor = window->getGridAnchor();

      setStringResult(CTkAppUtil::alignToString(anchor));
    }
    else {
      Qt::Alignment anchor;
      if (! tk_->variantToAlign(args[i], anchor))
        return tk_->throwError(tk_->msg() + "bad anchor \"" + args[2] + "\": must be "
                               "n, ne, e, se, s, sw, w, nw, or center");

      ++i;

      window->setGridAnchor(anchor);
    }
  }
  else if (option == "bbox") {
    if (numArgs != 2 && numArgs != 4 && numArgs != 6)
      return tk_->wrongNumArgs("grid bbox window ?column row ?column row??");

    auto *window = tk_->lookupWidgetByName(args[i]);
    if (! window) return false;

    auto *layout = window->getTkGridLayout();
    if (! layout) { setIntegerArrayResult({ 0L, 0L, 0L, 0L }); return true; }

    ++i;

    if      (numArgs == 2) {
      //int column1, row1, column2, row2;
      //window->getGridBBox(column1, row1, column2, row2);
      //setIntegerArrayResult({ column1, row1, column2, row2 });

      QRect bbox;
      layout->getBBox(bbox);
      setIntegerArrayResult({ long(bbox.left()), long(bbox.top()),
                              long(bbox.width()), long(bbox.height()) });
    }
    else if (numArgs == 4) {
      long column, row;
      if (! tk_->variantToInt(args[i], column))
        return tk_->invalidInteger(args[i]);
      ++i;

      if (! tk_->variantToInt(args[i], row))
        return tk_->invalidInteger(args[i]);
      ++i;

      //window->setGridBBox(column, row, column, row);

      QRect bbox;
      layout->getCellBBox(bbox, row, column, row, column);
      setIntegerArrayResult({ bbox.left(), bbox.top(), bbox.width(), bbox.height() });
    }
    else if (numArgs == 6) {
      long column1, row1, column2, row2;
      if (! tk_->variantToInt(args[i], column1))
        return tk_->invalidInteger(args[i]);
      ++i;

      if (! tk_->variantToInt(args[i], row1))
        return tk_->invalidInteger(args[i]);
      ++i;

      if (! tk_->variantToInt(args[i], column2))
        return tk_->invalidInteger(args[i]);
      ++i;

      if (! tk_->variantToInt(args[i], row2))
        return tk_->invalidInteger(args[i]);
      ++i;

      //window->setGridBBox(column1, row1, column2, row2);
      //setIntegerArrayResult({ 0L, 0L, 0L, 0L }); // TODO

      QRect bbox;
      layout->getCellBBox(bbox, row1, column1, row2, column2);
      setIntegerArrayResult({ bbox.left(), bbox.top(), bbox.width(), bbox.height() });
    }
  }
  else if (option == "columnconfigure") {
    if (numArgs < 3)
      return tk_->wrongNumArgs("grid columnconfigure window index ?-option value ...?");

    auto *window = tk_->lookupWidgetByName(args[i]);
    if (! window) return false;

    auto *layout = window->getTkGridLayout(/*create*/true);
    if (! layout) return tk_->throwError(tk_->msg() + "no grid layout for \"" + args[i] + "\"");

    ++i;

    //---

    static auto optionNames = std::vector<QString>({
      "-minsize", "-pad", "-uniform", "-weight"});

    if      (numArgs == 3) {
      long ind;
      if (! tk_->variantToInt(args[i], ind))
        return tk_->invalidInteger(args[i]);

      std::vector<QVariant> vars;

      vars.push_back("-minsize"); vars.push_back(layout->getColumnMinSize(ind));
      vars.push_back("-pad"    ); vars.push_back(layout->getColumnPad    (ind));
      vars.push_back("-uniform"); vars.push_back(layout->getColumnUniform(ind));
      vars.push_back("-weight" ); vars.push_back(layout->getColumnWeight (ind));

      tk_->setVariantArrayResult(vars);
    }
    else if (numArgs == 4) {
      long ind;
      if (! tk_->variantToInt(args[i], ind))
        return tk_->invalidInteger(args[i]);

      auto name = tk_->variantToString(args[3]);

      QString name1;
      if (! tk_->lookupOptionName(optionNames, name, name1))
        return tk_->throwError("bad option \"" + name + "\": must be "
                                 "-minsize, -pad, -uniform, or -weight");

      if      (name1 == "-minsize") {
        setRealResult(layout->getColumnMinSize(ind));
      }
      else if (name1 == "-pad") {
        setIntegerResult(layout->getColumnPad(ind));
      }
      else if (name1 == "-uniform") {
        setStringResult(layout->getColumnUniform(ind));
      }
      else if (name1 == "-weight") {
        setIntegerResult(layout->getColumnWeight(ind));
      }
    }
    else {
      IndexData indexData;
      if (! parseLayoutInds(layout, RowColType::COLUMNS, args[i], indexData))
        return tk_->invalidInteger(args[i]);
      ++i;

      //---

      for ( ; i < numArgs; i += 2) {
        auto name = tk_->variantToString(args[i]);

        QString name1;
        if (! tk_->lookupOptionName(optionNames, name, name1))
          return tk_->throwError("bad option \"" + name + "\": must be "
                                   "-minsize, -pad, -uniform, or -weight");

        if (i + 1 >= numArgs)
          return tk_->throwError(tk_->msg() + "missing value for option \"" + args[i] + "\"");

        auto var = args[i + 1];

        if      (name1 == "-minsize") {
          CTkAppDistance size;
          if (! tk_->variantToDistance(var, size))
            return tk_->invalidDistance(var);

          for (const auto &ind : indexData.inds)
            layout->setColumnMinSize(ind, size.rvalue);
        }
        else if (name1 == "-pad") {
          long pad;
          if (! tk_->variantToInt(var, pad))
            return tk_->invalidInteger(var);

          if (pad < 0)
            return tk_->throwError(tk_->msg() + "invalid arg \"-pad\": should be non-negative");

          for (const auto &ind : indexData.inds)
            layout->setColumnPad(ind, pad);
        }
        else if (name1 == "-uniform") {
          auto str = tk_->variantToString(var);

          for (const auto &ind : indexData.inds)
            layout->setColumnUniform(ind, str);
        }
        else if (name1 == "-weight") {
          long weight;
          if (! tk_->variantToInt(var, weight))
            return tk_->invalidInteger(var);

          if (weight < 0)
            return tk_->throwError(tk_->msg() + "invalid arg \"-weight\": should be non-negative");

          for (const auto &ind : indexData.inds)
            layout->setColumnWeight(ind, weight);
        }
      }
    }
  }
  else if (option == "configure") {
    configure = true;
  }
  else if (option == "forget") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("grid forget slave ?slave ...?");

    for ( ; i < numArgs; ++i) {
      auto *child = tk_->lookupWidgetByName(args[i]);
      if (! child || ! child->getParent()) continue;

      auto *layout = child->getParent()->getTkGridLayout();
      if (! layout) return tk_->throwError(tk_->msg() + "no grid layout for \"" + args[i] + "\"");

      (void) layout->removeWidget(child);
    }
  }
  else if (option == "info") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("grid info window");

    auto *child = tk_->lookupWidgetByName(args[i]);
    if (! child || ! child->getParent()) return false;

    auto *layout = child->getParent()->getTkGridLayout();
    if (! layout) return tk_->throwError(tk_->msg() + "no grid layout for \"" + args[i] + "\"");

    ++i;

    CTkAppGridLayout::Info info;
    layout->getChildInfo(child, info);

    std::vector<QVariant> vars;

    vars.push_back("-in"        ); vars.push_back(info.getIn());
    vars.push_back("-column"    ); vars.push_back(info.getCol());
    vars.push_back("-row"       ); vars.push_back(info.getRow());
    vars.push_back("-columnspan"); vars.push_back(info.getColSpan());
    vars.push_back("-rowspan"   ); vars.push_back(info.getRowSpan());
    vars.push_back("-ipadx"     ); vars.push_back(info.getIPadX());
    vars.push_back("-ipady"     ); vars.push_back(info.getIPadY());
    vars.push_back("-padx"      ); vars.push_back(info.getPadX());
    vars.push_back("-pady"      ); vars.push_back(info.getPadY());
    vars.push_back("-sticky"    ); vars.push_back(info.getSticky());

    tk_->setVariantArrayResult(vars);
  }
  else if (option == "location") {
    if (numArgs != 4)
      return tk_->wrongNumArgs("grid location window x y");

    auto *window = tk_->lookupWidgetByName(args[i]);
    if (! window) return false;

    ++i;

    CTkAppDistance x, y;
    if (! tk_->variantToDistance(args[i], x))
      return tk_->invalidDistance(args[i]);
    ++i;

    if (! tk_->variantToDistance(args[i], y))
      return tk_->invalidDistance(args[i]);
    ++i;

    auto *layout = window->getTkGridLayout();

    int row = -1, col = -1;

    if (layout)
      layout->getCellAt(x.rvalue, y.rvalue, row, col);

    setIntegerArrayResult({row, col});
  }
  else if (option == "propagate") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs("grid propagate window ?boolean?");

    auto *window = tk_->lookupWidgetByName(args[i]);
    if (! window) return false;
    ++i;

    if (numArgs == 2) {
      setBoolResult(window->getGridPropagate());
    }
    else {
      bool b;
      if (! tk_->variantToBool(args[i], b))
        return tk_->invalidBool(args[i]);
      ++i;

      window->setGridPropagate(b);
    }
  }
  else if (option == "rowconfigure") {
    if (numArgs < 3)
      return tk_->wrongNumArgs("grid rowconfigure window index ?-option value...?");

    auto *window = tk_->lookupWidgetByName(args[i]);
    if (! window) return false;

    auto *layout = window->getTkGridLayout(/*create*/true);
    if (! layout) return tk_->throwError(tk_->msg() + "no grid layout for \"" + args[i] + "\"");

    ++i;

    //---

    static auto optionNames = std::vector<QString>({
      "-minsize", "-pad", "-uniform", "-weight"});

    if      (numArgs == 3) {
      long ind;
      if (! tk_->variantToInt(args[i], ind))
        return tk_->invalidInteger(args[i]);

      std::vector<QVariant> vars;

      vars.push_back("-minsize"); vars.push_back(layout->getRowMinSize(ind));
      vars.push_back("-pad"    ); vars.push_back(layout->getRowPad    (ind));
      vars.push_back("-uniform"); vars.push_back(layout->getRowUniform(ind));
      vars.push_back("-weight" ); vars.push_back(layout->getRowWeight (ind));

      tk_->setVariantArrayResult(vars);
    }
    else if (numArgs == 4) {
      long ind;
      if (! tk_->variantToInt(args[i], ind))
        return tk_->invalidInteger(args[i]);

      auto name = tk_->variantToString(args[3]);

      QString name1;
      if (! tk_->lookupOptionName(optionNames, name, name1))
        return tk_->throwError("bad option \"" + name + "\": must be "
                                 "-minsize, -pad, -uniform, or -weight");

      if      (name1 == "-minsize") {
        setRealResult(layout->getRowMinSize(ind));
      }
      else if (name1 == "-pad") {
        setIntegerResult(layout->getRowPad(ind));
      }
      else if (name1 == "-uniform") {
        setStringResult(layout->getRowUniform(ind));
      }
      else if (name1 == "-weight") {
        setIntegerResult(layout->getRowWeight(ind));
      }
    }
    else {
      IndexData indexData;
      if (! parseLayoutInds(layout, RowColType::ROWS, args[i], indexData))
        return tk_->invalidInteger(args[i]);
      ++i;

      //---

      for ( ; i < numArgs; i += 2) {
        auto name = tk_->variantToString(args[i]);

        QString name1;
        if (! tk_->lookupOptionName(optionNames, name, name1))
          return tk_->throwError("bad option \"" + name + "\": must be "
                                   "-minsize, -pad, -uniform, or -weight");

        if (i + 1 >= numArgs)
          return tk_->throwError(tk_->msg() + "missing value for option \"" + args[i] + "\"");

        auto var = args[i + 1];

        if      (name1 == "-minsize") {
          CTkAppDistance size;
          if (! tk_->variantToDistance(var, size))
            return tk_->invalidDistance(var);

          for (const auto &ind : indexData.inds)
            layout->setRowMinSize(ind, size.rvalue);
        }
        else if (name1 == "-pad") {
          long pad;
          if (! tk_->variantToInt(var, pad))
            return tk_->invalidInteger(var);

          if (pad < 0)
            return tk_->throwError(tk_->msg() + "invalid arg \"-pad\": should be non-negative");

          for (const auto &ind : indexData.inds)
            layout->setRowPad(ind, pad);
        }
        else if (name1 == "-uniform") {
          auto str = tk_->variantToString(var);

          for (const auto &ind : indexData.inds)
            layout->setRowUniform(ind, str);
        }
        else if (name1 == "-weight") {
          long weight;
          if (! tk_->variantToInt(var, weight))
            return tk_->invalidInteger(var);

          if (weight < 0)
            return tk_->throwError(tk_->msg() + "invalid arg \"-weight\": should be non-negative");

          for (const auto &ind : indexData.inds)
            layout->setRowWeight(ind, weight);
        }
      }
    }
  }
  else if (option == "remove") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("grid remove window ?window ...?");

    while (i < numArgs) {
      auto *child = tk_->lookupWidgetByName(args[i]);
      if (! child) return tk_->throwError(tk_->msg() + "Invalid window \"" + args[i] + "\"");

      ++i;

      auto *layout = child->getTkGridLayout();
      if (! layout) continue;
    }
  }
  else if (option == "size") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("grid size window");

    auto *window = tk_->lookupWidgetByName(args[i]);
    if (! window) return tk_->throwError(tk_->msg() + "bad window path name \"" + args[i] + "\"");

    auto *layout = window->getTkGridLayout();
    if (! layout) { setIntegerArrayResult({0, 0}); return true; }

    ++i;

    CTkAppGridLayout::Info info;
    layout->getChildInfo(window, info);

    setIntegerArrayResult({info.getColSpan(), info.getRowSpan()});
  }
  else if (option == "slaves" || option == "content") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("grid " + option + " window ?-option value ...?");

    auto *w = tk_->lookupWidgetByName(args[i]);
    if (! w) return false;

    auto *layout = w->getTkGridLayout();
    if (! layout) { tk_->setStringResult(""); return true; }

    ++i;

    auto widgets = layout->getWidgets();

    long row = -1, col = -1;

    for ( ; i < numArgs; ++i) {
      if      (args[i] == "-row") {
        if (i + 1 >= numArgs) return false;

        ++i;

        if (! tk_->variantToInt(args[i], row))
          return tk_->invalidInteger(args[i]);

        if (row < 0)
          return tk_->throwError(tk_->msg() + args[i] + " is an invalid value: should NOT be < 0");
      }
      else if (args[i] == "-column") {
        if (i + 1 >= numArgs) return false;

        ++i;

        if (! tk_->variantToInt(args[i], col))
          return tk_->invalidInteger(args[i]);

        if (col < 0)
          return tk_->throwError(tk_->msg() + args[i]  +" is an invalid value: should NOT be < 0");
      }
      else
        return tk_->throwError(tk_->msg() + "bad option \"" + args[i] + "\": must be "
                               "-column or -row");
    }

    //---

    std::vector<QString> names;

    for (auto *w : widgets) {
      CTkAppGridLayout::Info info;
      layout->getChildInfo(w, info);

      if (row >= 0 && info.getRow() != row)
        continue;

      if (col >= 0 && info.getCol() != col)
        continue;

      names.push_back(w->getFullName());
    }

    setStringArrayResult(names);
  }
  else {
    configure = true;
  }

  //---

  if (configure) {
    CTkAppWidget*                 parent = nullptr;
    std::vector<CTkAppWidget*>    children;
    CTkAppGridLayout::WidgetDatas widgetDatas;

    CTkAppOptionValueMap optValues;

    for ( ; i < numArgs; ++i) {
      auto arg = tk_->variantToString(args[i]);
      if (! arg.size()) continue;

      //---

      if      (arg == "-") {
        widgetDatas.emplace_back(CTkAppGridLayout::WidgetType::COL_SPAN);
      }
      else if (arg == "x") {
        widgetDatas.emplace_back(CTkAppGridLayout::WidgetType::EMPTY);
      }
      else if (arg == "^") {
        widgetDatas.emplace_back(CTkAppGridLayout::WidgetType::ROW_SPAN);
      }
      else if (arg[0] == '-') {
        if (! tk_->processOption(opts, args, i, optValues))
          return tk_->throwError("extra option or option with no value");
      }
      else {
        auto *widget = tk_->lookupWidgetByName(arg);
        if (! widget)
          return tk_->throwError("bad window path name \"" + arg + "\"");

        if (widget->isTopLevel())
          return tk_->throwError(tk_->msg() + "can't manage \"" + arg + "\": "
                                 "it's a top-level window");

        auto *parent1 = widget->getParent();

        if (! parent)
          parent = parent1;
        else {
          if (parent1 != parent)
            return tk_->throwError("can't put \"" + widget->getFullName() + "\" "
                                   "inside \"" + parent->getFullName() + "\"");
        }

        children.push_back(widget);

        widgetDatas.emplace_back(widget);
      }
    }

    //---

    CTkAppGridLayout::Info info;

    {
    auto p = optValues.find("-column");
    if (p != optValues.end()) {
      auto i = (*p).second.getInteger();
      if (i < 0) return tk_->throwError(tk_->msg() + "bad column value \"" + QString::number(i) +
                                        "\": must be a non-negative integer");
      info.setCol(i);
    }
    }

    {
    auto p = optValues.find("-columnspan");
    if (p != optValues.end()) {
      auto i = (*p).second.getInteger();
      if (i <= 0) return tk_->throwError(tk_->msg() + "bad columnspan value \"" +
                                         QString::number(i) + "\": must be a positive integer");
      info.setColSpan(i);
    }
    }

    {
    auto p = optValues.find("-in");
    if (p != optValues.end()) {
      auto inName = (*p).second.getString();

      auto *inWidget = tk_->lookupWidgetByName(inName);
      if (! inWidget)
        return tk_->throwError("bad window path name \"" + inName + "\"");

      for (auto *child : children)
        if (inWidget == child)
          return tk_->throwError("window can't be managed in itself");

      info.setIn(inName);
    }
    }

    {
    auto p = optValues.find("-ipadx");
    if (p != optValues.end()) info.setIPadX((*p).second.getInteger());
    }

    {
    auto p = optValues.find("-ipady");
    if (p != optValues.end()) info.setIPadY((*p).second.getInteger());
    }

    {
    auto p = optValues.find("-padx");
    if (p != optValues.end()) {
      auto str = (*p).second.getString();

      long padl = 0, padr = 0;
      std::vector<QString> strs;
      if (tk_->splitList(str, strs) && strs.size() == 2) {
        if (! tk_->variantToInt(strs[0], padl))
          return tk_->invalidInteger(strs[0]);
        if (! tk_->variantToInt(strs[1], padr))
          return tk_->invalidInteger(strs[1]);
      }
      else {
        if (! tk_->variantToInt(str, padl))
          return tk_->invalidInteger(str);
        padr = padl;
      }

      info.setPadX(padl, padr);
    }
    }

    {
    auto p = optValues.find("-pady");
    if (p != optValues.end()) {
      auto str = (*p).second.getString();

      long padb = 0, padt = 0;
      std::vector<QString> strs;
      if (tk_->splitList(str, strs) && strs.size() == 2) {
        if (! tk_->variantToInt(strs[0], padb))
          return tk_->invalidInteger(strs[0]);
        if (! tk_->variantToInt(strs[1], padt))
          return tk_->invalidInteger(strs[1]);
      }
      else {
        if (! tk_->variantToInt(str, padb))
          return tk_->invalidInteger(str);
        padt = padb;
      }

      info.setPadY(padb, padt);
    }
    }

    {
    auto p = optValues.find("-row");
    if (p != optValues.end()) {
      auto i = (*p).second.getInteger();
      if (i < 0) return tk_->throwError(tk_->msg() + "bad row value \"" + QString::number(i)  +
                                        "\": must be a non-negative integer");
      info.setRow(i);
    }
    }

    {
    auto p = optValues.find("-rowspan");
    if (p != optValues.end()) {
      auto i = (*p).second.getInteger();
      if (i <= 0) return tk_->throwError(tk_->msg() + "bad rowspan value \"" +
                                         QString::number(i) + "\": must be a positive integer");
      info.setRowSpan(i);
    }
    }

    {
    auto p = optValues.find("-sticky");
    if (p != optValues.end()) {
      auto s = (*p).second.getString();
      info.setSticky(s);

      bool valid;
      (void) info.getStickySides(valid);
      if (! valid)
        return tk_->throwError(tk_->msg() + "bad stickyness value \"" + s + "\": must be "
                               "a string containing n, e, s, and/or w");
    }
    }

    //---

    if (! parent)
      return tk_->throwError("can't determine container window");

    auto *layout = parent->getTkGridLayout(/*create*/true);
    if (! layout) return tk_->throwError("no grid layout for \"" + parent->getFullName() + "\"");

    layout->addWidgets(widgetDatas, info);

    layout->invalidate();

    if (parent->isTopLevel())
      parent->setNeedsShow(true);
  }

  return true;
}

//---

bool
CTkAppImageCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("image option ?args?");

  uint i = 0;

  static auto optionNames = std::vector<QString>({
    "create", "delete", "height", "inuse", "names", "type", "types", "width"});

  QString name;
  if (! tk_->lookupOptionName(optionNames, args[i], name))
    return false;
  ++i;

  if      (name == "create") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("image create type ?name? ?-option value ...?");

    static auto typeNames = std::vector<QString>({"photo", "bitmap", "svg"});

    QString type;
    if (! tk_->lookupOptionName(typeNames, args[i], type, /*quiet*/true))
      return tk_->throwError(tk_->msg() + "image type \"" + args[i] + "\" doesn't exist");
    ++i;

    using OptInt     = std::optional<int>;
    using OptString  = std::optional<QString>;
    using FormatData = CTkAppImage::FormatData;

    FormatData formatData;
    OptInt     width, height;
    QVariant   data, maskdata;
    OptString  imageName, filename, maskfilename, metadata;

    std::vector<QVariant> args1;

    for ( ; i < numArgs; ++i) {
      static auto argNames = std::vector<QString>({
        "-background", "-data", "-file", "-foreground", "-format", "-gamma", "-height",
        "-maskdata", "-maskfile", "-metadata", "-palette", "-width"});

      QString arg;
      if (tk_->lookupOptionName(argNames, args[i], arg, /*quiet*/true)) {
        ++i;

        if (i >= numArgs)
          return tk_->throwError("value for \"" + arg + "\" missing");

        if      (arg == "-file") {
          filename = tk_->variantToString(args[i]);
        }
        else if (arg == "-data") {
          data = args[i];
        }
        else if (arg == "-format") {
          auto str = tk_->variantToString(args[i]);

          if (! CTkAppImage::lookupFormatName(tk_, str, formatData))
            return false;
        }
        else if (arg == "-height") {
          long iheight;
          if (! tk_->variantToInt(args[i], iheight))
            return tk_->throwError(tk_->msg() + "bad image height \"" + args[i] + "\"");

          height = iheight;
        }
        else if (arg == "-maskdata") {
          maskdata = args[i];
        }
        else if (arg == "-maskfile") {
          maskfilename = tk_->variantToString(args[i]);
        }
        else if (arg == "-metadata") {
          metadata = tk_->variantToString(args[i]);
        }
        else if (arg == "-width") {
          long iwidth;
          if (! tk_->variantToInt(args[i], iwidth))
            return tk_->throwError(tk_->msg() + "bad image width \"" + args[i] + "\"");

          width = iwidth;
        }
        else {
          args1.push_back(args[i - 1]);
          args1.push_back(args[i    ]);
        }
      }
      else {
        if (imageName)
          return tk_->wrongNumArgs("image option ?args?");

        imageName = tk_->variantToString(args[i]);

        if ((*imageName)[0] == '-')
          return tk_->unknownOption(*imageName);
      }
    }

    if (i < numArgs)
      return tk_->wrongNumArgs("image option ?args?");

    if (! imageName)
      imageName = tk_->getNewImageName();

    if (imageName == ".")
      return tk_->throwError("bad image name");

    //---

    auto image = tk_->getImage(*imageName);

    if (! image) {
      image = tk_->createImage(type, formatData.format, *imageName, *width, *height);

      (void) tk_->addImageCommand(image);
    }
    else {
      image->setType(type);

      image->setFormat(formatData.format);

      image->setWidth (*width);
      image->setHeight(*height);
    }

    if      (filename) {
      if (formatData.format == "svg") {
        if (! image->loadSVG(*filename)) {
          tk_->deleteImage(image);
          return false;
        }
      }
      else {
        if (! image->loadFile(*filename)) {
          tk_->deleteImage(image);
          return false;
        }
      }
    }
    else if (tk_->variantIsValid(data)) {
      if (type == "bitmap" && formatData.format == "")
        formatData.format = "xbm";

      if (! image->loadVarData(data, formatData.format)) {
        tk_->deleteImage(image);
        return false;
      }
    }

    if (formatData.set)
      image->setFormat(formatData.format);

    if (metadata)
      image->setMetaData(*metadata);

    //---

    if      (maskfilename) {
      if (formatData.format == "svg") {
        if (! image->loadMaskSVG(*maskfilename)) {
          tk_->deleteImage(image);
          return false;
        }
      }
      else {
        if (! image->loadMaskFile(*maskfilename)) {
          tk_->deleteImage(image);
          return false;
        }
      }
    }
    else if (tk_->variantIsValid(maskdata)) {
      auto maskStr = tk_->variantToString(maskdata).toStdString();

      if (type == "bitmap" && formatData.format == "")
        formatData.format = "xbm";

      if (formatData.format == "xbm") {
        if (! image->loadMaskXBM(*imageName, maskStr)) {
          tk_->deleteImage(image);
          return false;
        }
      }
      else {
        if (! image->loadMaskEncodedData(*imageName, formatData.format, maskStr)) {
          if (! image->loadMaskData(*imageName, formatData.format, maskStr)) {
            tk_->deleteImage(image);
            return tk_->throwError("couldn't load mask data");
          }
        }
      }
    }

    //---

    uint i1       = 0;
    uint numArgs1 = args1.size();

    while (i1 < numArgs1) {
      auto name = tk_->variantToString(args1[i1++]);
      auto var  = args1[i1++];

      if (! CTkAppImageCommand::processOption(tk_, image, type, name, var)) {
        tk_->deleteImage(image);
        return false;
      }
    }

    //---

    setResult(*imageName);
  }
  else if (name == "delete") {
    for ( ; i < numArgs; ++i) {
      auto image = tk_->getImage(args[i]);
      if (! image) return false;

      tk_->deleteImage(image);
    }
  }
  else if (name == "height") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("image height name");

    auto image = tk_->getImage(args[i]);
    if (! image) return tk_->throwError(tk_->msg() + "image \"" + args[i] + "\" doesn't exist");

    tk_->setIntegerResult(image->height());
  }
  else if (name == "inuse") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("image inuse name");

    auto image = tk_->getImage(args[i]);
    if (! image) return tk_->throwError(tk_->msg() + "image \"" + args[i] + "\" doesn't exist");

    setBoolResult(image->numRefs() > 0 ? true : false);
  }
  else if (name == "names") {
    if (numArgs != 1)
      return tk_->wrongNumArgs("image names");

    std::vector<QString> names;
    tk_->getImageNames(names);
    setStringArrayResult(names);
  }
  else if (name == "type") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("image type name");

    auto image = tk_->getImage(args[i]);
    if (! image) return tk_->throwError(tk_->msg() + "image \"" + args[i] + "\" doesn't exist");

    tk_->setStringResult(image->type());
  }
  else if (name == "types") {
    if (numArgs != 1)
      return tk_->wrongNumArgs("image types");

    std::vector<QString> strs;

    strs.push_back("photo");
    strs.push_back("bitmap");

    setStringArrayResult(strs);

    return true;
  }
  else if (name == "width") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("image width name");

    auto image = tk_->getImage(args[i]);
    if (! image) return tk_->throwError(tk_->msg() + "image \"" + args[i] + "\" doesn't exist");

    tk_->setIntegerResult(image->width());
  }

  return true;
}

//---

bool
CTkAppLabelCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-activebackground"   , "activeBackground"   , "Foreground"         , "#ececec"       },
    { "-activeforeground"   , "activeForeground"   , "Background"         , "#000000"       },
    { "-anchor"             , "anchor"             , "Anchor"             , "center"        },
    { "-background"         , "background"         , "Background"         , "#d9d9d9"       },
    { "-bitmap"             , "bitmap"             , "Bitmap"             , ""              },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
    { "-compound"           , "compound"           , "Compound"           , "none"          },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"       },
    { "-font"               , "font"               , "Font"               , "TkDefaultFont" },
    { "-foreground"         , "foreground"         , "Foreground"         , "#000000"       },
    { "-height"             , "height"             , "Height"             , "0"             },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"       },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"       },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "0"             },
    { "-image"              , "image"              , "Image"              , ""              },
    { "-justify"            , "justify"            , "Justify"            , "center"        },
    { "-padx"               , "padX"               , "Pad"                , "1"             },
    { "-pady"               , "padY"               , "Pad"                , "1"             },
    { "-relief"             , "relief"             , "Relief"             , "flat"          },
    { "-state"              , "state"              , "State"              , "normal"        },
    { "-takefocus"          , "takeFocus"          , "TakeFocus"          , "0"             },
    { "-text"               , "text"               , "Text"               , ""              },
    { "-textvariable"       , "textVariable"       , "Variable"           , ""              },
    { "-underline"          , "underline"          , "Underline"          , "-1"            },
    { "-width"              , "width"              , "Width"              , "0"             },
    { "-wraplength"         , "wrapLength"         , "WrapLength"         , "0"             },

    CTkAppOpt::optSynonym("-bd", "-borderwidth"),
    CTkAppOpt::optSynonym("-bg", "-background" ),
    CTkAppOpt::optSynonym("-fg", "-foreground" ),

    CTkAppOpt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("label pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *label = new CTkAppLabel(tk_, parent, childName);

  if (! tk_->useStyle())
    label->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), label, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    label->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppLabelFrameCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-background"         , "background"         , "Background"         , "#d9d9d9"       },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "2"             },
    { "-class"              , "class"              , "Class"              , "Labelframe"    },
    { "-colormap"           , "colormap"           , "Colormap"           , ""              },
    { "-container"          , "container"          , "Container"          , "0"             },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-font"               , "font"               , "Font"               , "TkDefaultFont" },
    { "-foreground"         , "foreground"         , "Foreground"         , "#000000"       },
    { "-height"             , "height"             , "Height"             , "0"             },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"       },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"       },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "0"             },
    { "-labelanchor"        , "labelAnchor"        , "LabelAnchor"        , "nw"            },
    { "-labelwidget"        , "labelWidget"        , "LabelWidget"        , ""              },
    { "-padx"               , "padX"               , "Pad"                , "0"             },
    { "-pady"               , "padY"               , "Pad"                , "0"             },
    { "-relief"             , "relief"             , "Relief"             , "groove"        },
    { "-takefocus"          , "takeFocus"          , "TakeFocus"          , "0"             },
    { "-text"               , "text"               , "Text"               , ""              },
    { "-visual"             , "visual"             , "Visual"             , ""              },
    { "-width"              , "width"              , "Width"              , "0"             },

    CTkAppOpt::optSynonym("-bd", "-borderwidth"),
    CTkAppOpt::optSynonym("-bg", "-background" ),
    CTkAppOpt::optSynonym("-fg", "-foreground" ),

    CTkAppOpt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("labelframe pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *frame = new CTkAppLabelFrame(tk_, parent, childName);

  if (! tk_->useStyle())
    frame->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), frame, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    frame->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppListBoxCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  using Opt = CTkAppOpt;

  static CTkAppOpt opts[] = {
  {              "-activestyle"        , "activeStyle"        , "ActiveStyle"        , "dotbox"  },
  {              "-background"         , "background"         , "Background"         , "#ffffff" },
  Opt::optInt   ("-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"       ),
  Opt::optCursor("-cursor"             , "cursor"             , "Cursor"             , ""        ),
  {              "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3" },
  Opt::optBool  ("-exportselection"    , "exportSelection"    , "ExportSelection"    , "1"       ),
  {              "-font"               , "font"               , "Font"               , "TkDefaultFont" },
  {              "-foreground"         , "foreground"         , "Foreground"         , "#000000" },
  {              "-height"             , "height"             , "Height"             , "10"      },
  Opt::optColor ("-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9" ),
  Opt::optColor ("-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000" ),
  Opt::optInt   ("-highlightthickness" , "highlightThickness" , "HighlightThickness" , "0"       ),
  {              "-justify"            , "justify"            , "Justify"            , "center"  },
  {              "-listvariable"       , "listVariable"       , "Variable"           , ""        },
  {              "-relief"             , "relief"             , "Relief"             , "sunken"  },
  {              "-selectbackground"   , "selectBackground"   , "Foreground"         , "#c3c3c3" },
  {              "-selectborderwidth"  , "selectBorderWidth"  , "BorderWidth"        , "0"       },
  {              "-selectforeground"   , "selectForeground"   , "Background"         , "#000000" },
  {              "-selectmode"         , "selectMode"         , "SelectMode"         , "browse"  },
  {              "-setgrid"            , "setGrid"            , "SetGrid"            , "0"       },
  {              "-state"              , "state"              , "State"              , "normal"  },
  {              "-takefocus"          , "takeFocus"          , "TakeFocus"          , ""        },
  {              "-width"              , "width"              , "Width"              , "20"      },
  {              "-xscrollcommand"     , "xScrollCommand"     , "ScrollCommand"      , ""        },
  {              "-yscrollcommand"     , "yScrollCommand"     , "ScrollCommand"      , ""        },

  Opt::optSynonym("-bd", "-borderwidth"),
  Opt::optSynonym("-bg", "-background" ),
  Opt::optSynonym("-fg", "-foreground" ),

  Opt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("listbox pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *list = new CTkAppListBox(tk_, parent, childName);

  if (! tk_->useStyle())
    list->setBackground(QColor("#ffffff"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), list, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    list->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppLowerCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("lower window ?belowThis?");

  auto widgetName = args[0];

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (! child) return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  child->lower();

  return true;
}

//---

bool
CTkAppMenuCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-activebackground", "activeBackground", "Foreground" , "#ececec"       },
    { "-activeforeground", "activeForeground", "Background" , "#000000"       },
    { "-accelerator"     , "accelerator"     , "Accelerator", ""              },
    { "-background"      , "background"      , "Background" , "#ffffff"       },
    { "-bitmap"          , "bitmap"          , "Bitmap"     , ""              },
    { "-columnbreak"     , "columnbreak"     , "ColumnBreak", ""              },
    { "-command"         , "command"         , "Command"    , ""              },
    { "-compound"        , "compound"        , "Compound"   , "none"          },
    { "-font"            , "font"            , "Font"       , "TkDefaultFont" },
    { "-foreground"      , "foreground"      , "Foreground" , "#000000"       },
    { "-hidemargin"      , "hidemargin"      , "HideMargin" , ""              },
    { "-image"           , "image"           , "Image"      , ""              },
    { "-label"           , "label"           , "Label"      , ""              },
    { "-tearoff"         , "tearoff"         , "TearOff"    , ""              },
    { "-state"           , "state"           , "State"      , "normal"        },
    { "-underline"       , "underline"       , "Underline"  , "-1"            },

    CTkAppOpt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("menu pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *menu = new CTkAppMenu(tk_, parent, childName);

  if (! tk_->useStyle())
    menu->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), menu, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    menu->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppMenuButtonCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  using Opt = CTkAppOpt;

  static Opt opts[] = {
  { "-activebackground"   , "activeBackground"   , "Foreground"         , "#ececec"       },
  { "-activeforeground"   , "activeForeground"   , "Background"         , "#000000"       },
  { "-anchor"             , "anchor"             , "Anchor"             , "center"        },
  { "-background"         , "background"         , "Background"         , "#ffffff"       },
  { "-bitmap"             , "bitmap"             , "Bitmap"             , ""              },
  { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
  { "-cursor"             , "cursor"             , "Cursor"             , ""              },
  { "-direction"          , "direction"          , "Direction"          , "below"         },
  { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"       },
  { "-font"               , "font"               , "Font"               , "TkDefaultFont" },
  { "-foreground"         , "foreground"         , "Foreground"         , "#000000"       },
  { "-height"             , "height"             , "Height"             , "10"            },
  { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"       },
  { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"       },
  { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "0"             },
  { "-image"              , "image"              , "Image"              , ""              },
  Opt::optBool("-indicatoron", "indicatorOn", "IndicatorOn", "0"),
  { "-justify"            , "justify"            , "Justify"            , "center"        },
  { "-menu"               , "menu"               , "Menu"               , ""              },
  { "-padx"               , "padX"               , "Pad"                , "4p"            },
  { "-pady"               , "padY"               , "Pad"                , "3p"            },
  { "-relief"             , "relief"             , "Relief"             , "flat"          },
  { "-compound"           , "compound"           , "Compound"           , "none"          },
  { "-state"              , "state"              , "State"              , "normal"        },
  { "-takefocus"          , "takeFocus"          , "TakeFocus"          , ""              },
  { "-text"               , "text"               , "Text"               , ""              },
  { "-textvariable"       , "textVariable"       , "Variable"           , ""              },
  { "-underline"          , "underline"          , "Underline"          , "-1"            },
  { "-width"              , "width"              , "Width"              , "0"             },
  { "-wraplength"         , "wrapLength"         , "WrapLength"         , "0"             },

  Opt::optSynonym("-bd", "-borderwidth"),
  Opt::optSynonym("-bg", "-background" ),
  Opt::optSynonym("-fg", "-foreground" ),

  Opt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("menubutton pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *button = new CTkAppMenuButton(tk_, parent, childName);

  if (! tk_->useStyle())
    button->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), button, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    button->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppNoteBookCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-state"    , ""         , ""         , "normal" },
    { "-text"     , "text"     , "Text"     , ""       },
    { "-image"    , "image"    , "Image"    , nullptr  },
    { "-compound" , "compound" , "Compound" , nullptr  },
    { "-underline", "underline", "Underline", ""       },

    CTkAppOpt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("notebook pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *notebook = new CTkAppNoteBook(tk_, parent, childName);

  if (! tk_->useStyle())
    notebook->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), notebook, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    notebook->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppMessageCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-anchor"             , "anchor"             , "Anchor"             , "center"        },
    { "-aspect"             , "aspect"             , "Aspect"             , ""              },
    { "-background"         , "background"         , "Background"         , "#ffffff"       },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-font"               , "font"               , "Font"               , "TkDefaultFont" },
    { "-foreground"         , "foreground"         , "Foreground"         , "#000000"       },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"       },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"       },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "1"             },
    { "-justify"            , "justify"            , "Justify"            , "center"        },
    { "-padx"               , "padX"               , "Pad"                , "1"             },
    { "-pady"               , "padY"               , "Pad"                , "1"             },
    { "-relief"             , "relief"             , "Relief"             , "sunken"        },
    { "-takefocus"          , "takeFocus"          , "TakeFocus"          , ""              },
    { "-text"               , "text"               , "Text"               , ""              },
    { "-textvariable"       , "textVariable"       , "Variable"           , ""              },
    { "-width"              , "width"              , "Width"              , "20"            },

    CTkAppOpt::optSynonym("-bd", "-borderwidth"),
    CTkAppOpt::optSynonym("-bg", "-background" ),
    CTkAppOpt::optSynonym("-fg", "-foreground" ),

    CTkAppOpt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("message pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *message = new CTkAppMessage(tk_, parent, childName);

  if (! tk_->useStyle())
    message->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), message, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    message->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppOptionCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  auto numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs("option cmd arg ?arg ...?");

  auto cmd = tk_->variantToString(args[0]);

  if      (cmd == "add") {
    if (numArgs != 3 && numArgs != 4)
      return tk_->wrongNumArgs("option add pattern value ?priority?");

    auto pattern = tk_->variantToString(args[1]);

    QString priority;
    int     ipriority = -1;

    if (numArgs == 4) {
      priority = tk_->variantToString(args[3]);

      if      (priority == "widgetDefault") {
        ipriority = -1;
      }
      else if (priority == "startupFile") {
        ipriority = -2;
      }
      else if (priority == "userDefault") {
        ipriority = -3;
      }
      else if (priority == "interactive") {
        ipriority = -4;
      }
      else {
        long i;
        if (! tk_->variantToInt(args[3], i) || i < 0 || i > 100)
          return tk_->throwError("bad priority level \"" + priority + "\": must be "
            "widgetDefault, startupFile, userDefault, interactive, or a number between 0 and 100");

        ipriority = i;
      }
    }

    tk_->addOption(pattern, args[2], ipriority);
  }
  else if (cmd == "clear") {
    if (numArgs != 1)
      return tk_->wrongNumArgs("option clear");

    tk_->clearOptions();
  }
  else if (cmd == "get") {
    if (numArgs != 4)
      return tk_->wrongNumArgs("option get window name class");

    auto windowName = tk_->variantToString(args[1]);

    auto *window = tk_->lookupWidgetByName(windowName);
    if (! window) return false;

    auto optName  = tk_->variantToString(args[2]);
    auto optClass = tk_->variantToString(args[3]);

    QVariant optValue;
    if (! window->getOptionValue(optName, optClass, optValue))
      return false;

    setResult(optValue);
  }
  else if (cmd == "readfile") {
    tk_->TODO(cmd);
  }
  else
    return false;

  return true;
}

//---

bool
CTkAppPackCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOption opts[] = {
    { "-after" , CTkAppOptionType::String , nullptr },
    { "-anchor", CTkAppOptionType::String , nullptr },
    { "-before", CTkAppOptionType::String , nullptr },
    { "-expand", CTkAppOptionType::Boolean, nullptr },
    { "-fill"  , CTkAppOptionType::String , nullptr },
    { "-in"    , CTkAppOptionType::String , nullptr },
    { "-ipadx" , CTkAppOptionType::Int    , nullptr },
    { "-ipady" , CTkAppOptionType::Int    , nullptr },
    { "-padx"  , CTkAppOptionType::Int    , nullptr },
    { "-pady"  , CTkAppOptionType::Int    , nullptr },
    { "-side"  , CTkAppOptionType::String , nullptr },
    { nullptr  , CTkAppOptionType::None   , nullptr },
  };

  auto numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("pack option arg ?arg ...?");

  bool configure = false;

  static auto optionNames = std::vector<QString>({
    "configure", "content", "forget", "info", "propagate", "slaves" });

  uint i = 0;

  QString option;
  if (tk_->lookupName("option", optionNames, args[0], option, /*quiet*/true))
    ++i;

  if      (option == "configure") {
    configure = true;
  }
  else if (option == "forget") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("pack forget slave ?slave ...?");

    for ( ; i < numArgs; ++i) {
      auto *child = tk_->lookupWidgetByName(args[i], /*quiet*/true);
      if (! child || ! child->getParent()) continue;

      auto *layout = child->getParent()->getTkPackLayout();
      if (! layout) return tk_->throwError("no pack layout for \"" + option + "\"");

      layout->removeWidget(child);
    }
  }
  else if (option == "info") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("pack info window");

    auto *child = tk_->lookupWidgetByName(args[i]);
    if (! child || ! child->getParent()) return false;

    auto *layout = child->getParent()->getTkPackLayout();
    if (! layout) return tk_->throwError(tk_->msg() + "no pack layout for \"" + args[i] + "\"");

    ++i;

    CTkAppPackLayout::Info info;
    layout->getChildInfo(child, info);

    std::vector<QVariant> vars;

    vars.push_back("-in"    ); vars.push_back(info.getIn());
    vars.push_back("-anchor"); vars.push_back("center");
    vars.push_back("-expand"); vars.push_back(info.isExpand());
    vars.push_back("-fill"  ); vars.push_back(info.getFillStr());
    vars.push_back("-ipadx" ); vars.push_back(info.ipadX());
    vars.push_back("-ipady" ); vars.push_back(info.ipadY());
    vars.push_back("-padx"  ); vars.push_back(info.padX());
    vars.push_back("-pady"  ); vars.push_back(info.padY());
    vars.push_back("-side"  ); vars.push_back(info.getSideStr());

    tk_->setVariantArrayResult(vars);
  }
  else if (option == "propagate") {
    if (numArgs < 2 || numArgs > 3)
      return tk_->wrongNumArgs("pack option arg ?arg ...?");

    auto arg = tk_->variantToString(args[1]);

    auto *w = tk_->lookupWidgetByName(arg);
    if (! w) return false;

    auto *layout = w->getTkPackLayout();
    if (! layout) return tk_->throwError("no pack layout for \"" + arg + "\"");

    if (numArgs == 2)
      setIntegerResult(layout->isPropagate());
    else {
      auto var = args[2];

      bool b;
      if (! tk_->variantToBool(var, b))
        return tk_->invalidBool(var);

      layout->setPropagate(b);
    }
  }
  else if (option == "slaves" || option == "content") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("pack " + option + " arg ?arg ...?");

    auto arg = tk_->variantToString(args[1]);

    auto *w = tk_->lookupWidgetByName(arg);
    if (! w) return false;

    auto *layout = w->getTkPackLayout();
    if (! layout) return tk_->throwError("no pack layout for \"" + arg + "\"");

    if (numArgs == 2) {
      auto widgets = layout->getWidgets();

      std::vector<QString> names;

      for (auto *w : widgets)
        names.push_back(w->getFullName());

      setStringArrayResult(names);
    }
    else {
      return false;
    }
  }
  else {
    configure = true;
  }

  //---

  if (configure) {
    using Info = CTkAppPackLayoutInfo;

    Info::Side                  side     = Info::Side::NONE;
    Info::Fill                  fill     = Info::Fill::NONE;
    bool                        expand   = false;
    CTkAppWidget*               inparent = nullptr;
    int                         padx     = 0;
    int                         pady     = 0;
    int                         ipadx    = 0;
    int                         ipady    = 0;
    CTkAppWidget*               parent   = nullptr;
    std::vector<CTkAppWidget *> children;

    CTkAppOptionValueMap optValues;

    for ( ; i < numArgs; ++i) {
      auto arg = tk_->variantToString(args[i]);

      if (arg.size() > 0 && arg[0] == '-') {
        if (! tk_->processOption(opts, args, i, optValues))
          tk_->throwError("Unhandled option '" + arg + "'");
      }
      else {
        CTkAppWidget* parent1;
        QString       childName;

        if (! root()->decodeWidgetName(arg, &parent1, childName))
          return tk_->throwError("bad window path name \"" + arg + "\"");

        auto *child = parent1->getChild(childName);
        if (! child) return tk_->throwError("bad window path name \"" + arg + "\"");

        if (! parent)
          parent = parent1;

        children.push_back(child);
      }
    }

    {
    auto p = optValues.find("-fill");

    if (p != optValues.end()) {
      if      ((*p).second.getString() == "x"   ) fill = Info::Fill::X;
      else if ((*p).second.getString() == "y"   ) fill = Info::Fill::Y;
      else if ((*p).second.getString() == "both") fill = Info::Fill::BOTH;
    }
    }

    {
    auto p = optValues.find("-expand");

    if (p != optValues.end()) {
      expand = (*p).second.getBool();
    }
    }

    {
    auto p = optValues.find("-padx");

    if (p != optValues.end())
      padx = (*p).second.getInteger();
    }

    {
    auto p = optValues.find("-pady");

    if (p != optValues.end())
      pady = (*p).second.getInteger();
    }

    {
    auto p = optValues.find("-ipadx");

    if (p != optValues.end())
      ipadx = (*p).second.getInteger();
    }

    {
    auto p = optValues.find("-ipady");

    if (p != optValues.end())
      ipady = (*p).second.getInteger();
    }

    {
    auto p = optValues.find("-side");

    if (p != optValues.end()) {
      if      ((*p).second.getString() == "left"  ) side = Info::Side::LEFT;
      else if ((*p).second.getString() == "right" ) side = Info::Side::RIGHT;
      else if ((*p).second.getString() == "bottom") side = Info::Side::BOTTOM;
      else if ((*p).second.getString() == "top"   ) side = Info::Side::TOP;
    }
    else {
      if (! expand && fill == Info::Fill::NONE)
        side = Info::Side::TOP;
    }
    }

    {
    auto p = optValues.find("-in");

    if (p != optValues.end()) {
      CTkAppWidget* parent1;
      QString       childName;

      if (root()->decodeWidgetName((*p).second.getValue(), &parent1, childName)) {
        auto *child = parent1->getChild(childName);

        if (child)
          inparent = child;
      }
    }
    }

    if (inparent)
      parent = inparent;

    if (! parent)
      return tk_->throwError("no parent for \"" + option + "\"");

    auto *layout = parent->getTkPackLayout(/*create*/true);
    if (! layout) return tk_->throwError("no pack layout for \"" + option + "\"");

    if (side == Info::Side::NONE)
      side = Info::Side::TOP;

    Info info(side, fill, expand, padx, pady, ipadx, ipady);

    for (auto *child : children) {
      if (child == parent)
        return tk_->throwError("can't pack \"" + child->getFullName() + "\" inside itself");
    }

    layout->addWidgets(children, info);

    layout->invalidate();

    if (parent->isTopLevel())
      parent->setNeedsShow(true);
  }

  return true;
}

//---

bool
CTkAppPanedWindowCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-background"  , "background"  , "Background"  , "#d9d9d9"    },
    { "-borderwidth" , "borderWidth" , "BorderWidth" , "1"          },
    { "-cursor"      , "cursor"      , "Cursor"      , ""           },
    { "-handlepad"   , "handlePad"   , "HandlePad"   , "8"          },
    { "-handlesize"  , "handleSize"  , "HandleSize"  , "8"          },
    { "-height"      , "height"      , "Height"      , ""           },
    { "-opaqueresize", "opaqueResize", "OpaqueResize", "1"          },
    { "-orient"      , "orient"      , "Orient"      , "horizontal" },
    { "-relief"      , "relief"      , "Relief"      , "flat"       },
    { "-sashcursor"  , "sashCursor"  , "Cursor"      , ""           },
    { "-sashpad"     , "sashPad"     , "SashPad"     , "0"          },
    { "-sashrelief"  , "sashRelief"  , "Relief"      , "flat"       },
    { "-sashwidth"   , "sashWidth"   , "Width"       , "3"          },
    { "-showhandle"  , "showHandle"  , "ShowHandle"  , "0"          },
    { "-width"       , "width"       , "Width"       , ""           },

    CTkAppOpt::optSynonym("-bd", "-borderwidth"),
    CTkAppOpt::optSynonym("-bg", "-background" ),

    CTkAppOpt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("panedwindow pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *pane = new CTkAppPanedWindow(tk_, parent, childName);

  if (! tk_->useStyle())
    pane->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), pane, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    pane->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppPlaceCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOption opts[] = {
    { "-anchor"    , CTkAppOptionType::String, nullptr },
    { "-bordermode", CTkAppOptionType::String, nullptr },
    { "-height"    , CTkAppOptionType::Int   , nullptr },
    { "-in"        , CTkAppOptionType::String, nullptr },
    { "-relheight" , CTkAppOptionType::String, nullptr },
    { "-relwidth"  , CTkAppOptionType::String, nullptr },
    { "-relx"      , CTkAppOptionType::Real  , nullptr },
    { "-rely"      , CTkAppOptionType::Real  , nullptr },
    { "-width"     , CTkAppOptionType::Int   , nullptr },
    { "-x"         , CTkAppOptionType::Int   , nullptr },
    { "-y"         , CTkAppOptionType::Int   , nullptr },
    { nullptr      , CTkAppOptionType::None  , nullptr },
  };

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("place option|pathName args");

  bool configure = false;

  static auto optionNames = std::vector<QString>({
    "configure", "content", "forget", "info", "propagate", "slaves" });

  QString option;
  (void) tk_->lookupName("option", optionNames, args[0], option, /*quiet*/true);

  auto getPlaceLayout = [&](const QVariant &name, CTkAppPlaceLayout* &layout,
                            CTkAppWidget* &child) {
    CTkAppWidget* parent;
    QString       childName;

    if (! root()->decodeWidgetName(name, &parent, childName))
      return tk_->throwError(tk_->msg() + "bad window path name \"" + name + "\"");

    child = parent->getChild(childName);
    if (! child) return tk_->throwError(tk_->msg() + "bad window path name \"" + name + "\"");

    layout = parent->getTkPlaceLayout();
    if (! layout) return tk_->throwError(tk_->msg() + "no place layout for \"" + name + "\"");

    return true;
  };

  int ic = 0;

  if      (option == "configure") {
    configure = true;

    ++ic;
  }
  else if (option == "forget") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("place forget args");

    auto name = tk_->variantToString(args[1]);

    CTkAppPlaceLayout *layout;
    CTkAppWidget      *child;
    if (! getPlaceLayout(name, layout, child))
      return false;

    // Causes the placer to stop managing the geometry of window.
    // As a side effect of this command window will be unmapped so that it does not
    // appear on the screen. If window is not currently managed by the placer then
    // the command has no effect. This command returns an empty string.
    (void) layout->removeWidget(child);
  }
  else if (option == "info") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("place info window");

    auto name = tk_->variantToString(args[1]);

    CTkAppPlaceLayout *layout;
    CTkAppWidget      *child;
    if (! getPlaceLayout(name, layout, child))
      return false;

    CTkAppPlaceLayout::Info info;
    layout->getChildInfo(child, info);

    std::vector<QString> strs;

    strs.push_back(QString("-in %1").arg("."));
    strs.push_back(QString("-x %1").arg("0"));
    strs.push_back(QString("-relx %1").arg("0"));
    strs.push_back(QString("-y %1").arg("0"));
    strs.push_back(QString("-rely %1").arg("0"));
    strs.push_back(QString("-width %1").arg("{}"));
    strs.push_back(QString("-relwidth %1").arg("{}"));
    strs.push_back(QString("-height %1").arg("{}"));
    strs.push_back(QString("-relheight %1").arg("{}"));
    strs.push_back(QString("-anchor %1").arg("nw"));
    strs.push_back(QString("-bordermode %1").arg("inside"));

    setStringArrayResult(strs);
  }
  else if (option == "propagate") {
    tk_->TODO(args);
  }
  else if (option == "slaves" || option == "content") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("place " + option + "args");

    auto *w = tk_->lookupWidgetByName(args[1]);
    if (! w) return false;

    // Returns a list of all the content windows for which window is the container.
    // If there is no content for window then an empty string is returned.
    tk_->TODO(args);
  }
  else {
    configure = true;
  }

  //---

  if (configure) {
    CTkAppWidget*               parent = nullptr;
    std::vector<CTkAppWidget *> children;

    CTkAppOptionValueMap optValues;

    for (uint i = ic; i < numArgs; ++i) {
      auto arg = tk_->variantToString(args[i]);

      if (arg.size() > 0 && arg[0] == '-') {
        if (! tk_->processOption(opts, args, i, optValues))
          tk_->throwError("Unhandled option '" + arg + "'");
      }
      else {
        CTkAppWidget* parent1;
        QString       childName;

        if (! root()->decodeWidgetName(args[i], &parent1, childName))
          return tk_->throwError(tk_->msg() + "bad window path name \"" + args[i] + "\"");

        auto *child = parent1->getChild(childName);

        if (child) {
          if (! parent)
            parent = parent1;

          children.push_back(child);
        }
      }
    }

    //---

    CTkAppPlaceLayout::Info info;

    {
    auto p = optValues.find("-anchor");
    if (p != optValues.end()) {
      auto var = (*p).second.getValue();

      Qt::Alignment anchor;
      if (! tk_->variantToAlign(var, anchor))
        return tk_->throwError(tk_->msg() + "bad anchor \"" + var + "\": must be "
                               "n, ne, e, se, s, sw, w, nw, or center");
      info.setAnchor(anchor);
    }
    }

    {
    auto p = optValues.find("-bordermode");
    if (p != optValues.end()) {
      auto arg = (*p).second.getString();
      if      (arg == "inside")
        info.setBorderMode(CTkAppPlaceLayout::BorderMode::Inside);
      else if (arg == "outside")
        info.setBorderMode(CTkAppPlaceLayout::BorderMode::Outside);
      else if (arg == "ignore")
        info.setBorderMode(CTkAppPlaceLayout::BorderMode::Ignore);
      else
        return false;
    }
    }

    {
    auto p = optValues.find("-height");
    if (p != optValues.end()) {
      auto arg = (*p).second.getValue();

      CTkAppDistance height;
      if (! tk_->variantToDistance(arg, height))
        return tk_->invalidDistance(arg);

      info.setHeight(height.rvalue);
    }
    }

    {
    auto p = optValues.find("-in");
    if (p != optValues.end()) {
      auto arg = (*p).second.getString();
      info.setIn(arg);
    }
    }

    {
    auto p = optValues.find("-relheight");
    if (p != optValues.end()) info.setRelHeight((*p).second.getReal());
    }

    {
    auto p = optValues.find("-relwidth");
    if (p != optValues.end()) info.setRelWidth((*p).second.getReal());
    }

    {
    auto p = optValues.find("-relx");
    if (p != optValues.end()) info.setRelX((*p).second.getReal());
    }

    {
    auto p = optValues.find("-rely");
    if (p != optValues.end()) info.setRelY((*p).second.getReal());
    }

    {
    auto p = optValues.find("-width");
    if (p != optValues.end()) {
      auto arg = (*p).second.getValue();

      CTkAppDistance width;
      if (! tk_->variantToDistance(arg, width))
        return tk_->invalidDistance(arg);

      info.setWidth(width.rvalue);
    }
    }

    {
    auto p = optValues.find("-x");
    if (p != optValues.end()) {
      auto arg = (*p).second.getValue();

      CTkAppDistance x;
      if (! tk_->variantToDistance(arg, x))
        return tk_->invalidDistance(arg);

      info.setX(x.rvalue);
    }
    }

    {
    auto p = optValues.find("-y");
    if (p != optValues.end()) {
      auto arg = (*p).second.getValue();

      CTkAppDistance y;
      if (! tk_->variantToDistance(arg, y))
        return tk_->invalidDistance(arg);

      info.setY(y.rvalue);
    }
    }

    //---

    CTkAppWidget *inParent = nullptr;
    if (info.getIn() != "") {
      inParent = tk_->lookupWidgetByName(info.getIn());
      if (! inParent) return false;
    }

    //---

    CTkAppPlaceLayout *layout;

    if (! inParent)
      layout = parent->getTkPlaceLayout(/*create*/true);
    else
      layout = inParent->getTkPlaceLayout(/*create*/true);
    if (! layout) return tk_->throwError("no place layout for \"" + option + "\"");

    layout->addWidgets(children, info);

    layout->invalidate();

    if (parent->isTopLevel())
      parent->setNeedsShow(true);
  }

  return true;
}

bool
CTkAppRadioButtonCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  using Opt = CTkAppOpt;

  static Opt opts[] = {
    { "-activebackground"   , "activeBackground"   , "Foreground"         , "#ececec"        },
    { "-activeforeground"   , "activeForeground"   , "Background"         , "#000000"        },
    { "-anchor"             , "anchor"             , "Anchor"             , "center"         },
    { "-background"         , "background"         , "Background"         , "#d9d9d9"        },
    { "-bitmap"             , "bitmap"             , "Bitmap"             , ""               },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"              },
    { "-command"            , "command"            , "Command"            , ""               },
    { "-compound"           , "compound"           , "Compound"           , "none"           },
    { "-cursor"             , "cursor"             , "Cursor"             , ""               },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"        },
    { "-font"               , "font"               , "Font"               , "TkDefaultFont"  },
    { "-foreground"         , "foreground"         , "Foreground"         , "#000000"        },
    { "-height"             , "height"             , "Height"             , "0"              },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"        },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"        },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "1"              },
    { "-image"              , "image"              , "Image"              , ""               },
    Opt::optBool("-indicatoron", "indicatorOn", "IndicatorOn", "1"),
    { "-justify"            , "justify"            , "Justify"            , "center"         },
    { "-offrelief"          , "offRelief"          , "OffRelief"          , "raised"         },
    { "-overrelief"         , "overRelief"         , "OverRelief"         , ""               },
    { "-padx"               , "padX"               , "Pad"                , "1"              },
    { "-pady"               , "padY"               , "Pad"                , "1"              },
    { "-relief"             , "relief"             , "Relief"             , "flat"           },
    { "-selectcolor"        , "selectColor"        , "Background"         , "#ffffff"        },
    { "-selectimage"        , "selectImage"        , "SelectImage"        , ""               },
    { "-state"              , "state"              , "State"              , "normal"         },
    { "-takefocus"          , "takeFocus"          , "TakeFocus"          , ""               },
    { "-text"               , "text"               , "Text"               , ""               },
    { "-textvariable"       , "textVariable"       , "Variable"           , ""               },
    { "-tristateimage"      , "tristateImage"      , "TristateImage"      , ""               },
    { "-tristatevalue"      , "tristateValue"      , "TristateValue"      , ""               },
    { "-underline"          , "underline"          , "Underline"          , "-1"             },
    { "-value"              , "value"              , "Value"              , ""               },
    { "-variable"           , "variable"           , "Variable"           , "selectedButton" },
    { "-width"              , "width"              , "Width"              , "0"              },
    { "-wraplength"         , "wrapLength"         , "WrapLength"         , "0"              },

    Opt::optSynonym("-bd", "-borderwidth"),
    Opt::optSynonym("-bg", "-background" ),
    Opt::optSynonym("-fg", "-foreground" ),

    Opt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("radiobutton pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *radio = new CTkAppRadioButton(tk_, parent, childName);

  if (! tk_->useStyle())
    radio->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), radio, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    radio->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppRaiseCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("raise window ?aboveThis?");

  auto widgetName = args[0];

  auto *w = tk_->lookupWidgetByName(widgetName, /*quiet*/true);
  if (! w) return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  w->raise();

  return true;
}

//---

bool
CTkAppScaleCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-activebackground"   , "activeBackground"   , "Foreground"         , "#ececec"       },
    { "-background"         , "background"         , "Background"         , "#d9d9d9"       },
    { "-bigincrement"       , "bigIncrement"       , "BigIncrement"       , "0"             },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
    { "-command"            , "command"            , "Command"            , ""              },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-digits"             , "digits"             , "Digits"             , "0"             },
    { "-font"               , "font"               , "Font"               , "TkDefaultFont" },
    { "-foreground"         , "foreground"         , "Foreground"         , "#000000"       },
    { "-from"               , "from"               , "From"               , "0"             },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"       },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"       },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "1"             },
    { "-label"              , "label"              , "Label"              , ""              },
    { "-length"             , "length"             , "Length"             , "100"           },
    { "-orient"             , "orient"             , "Orient"             , "vertical"      },
    { "-relief"             , "relief"             , "Relief"             , "flat"          },
    { "-repeatdelay"        , "repeatDelay"        , "RepeatDelay"        , "300"           },
    { "-repeatinterval"     , "repeatInterval"     , "RepeatInterval"     , "100"           },
    { "-resolution"         , "resolution"         , "Resolution"         , "1"             },
    { "-showvalue"          , "showValue"          , "ShowValue"          , "1"             },
    { "-sliderlength"       , "sliderLength"       , "SliderLength"       , "30"            },
    { "-sliderrelief"       , "sliderRelief"       , "SliderRelief"       , "raised"        },
    { "-state"              , "state"              , "State"              , "normal"        },
    { "-takefocus"          , "takeFocus"          , "TakeFocus"          , ""              },
    { "-tickinterval"       , "tickInterval"       , "TickInterval"       , "0"             },
    { "-to"                 , "to"                 , "To"                 , "100"           },
    { "-troughcolor"        , "troughColor"        , "Background"         , "#b3b3b3"       },
    { "-variable"           , "variable"           , "Variable"           , ""              },
    { "-width"              , "width"              , "Width"              , "15"            },

    CTkAppOpt::optSynonym("-bd", "-borderwidth"),
    CTkAppOpt::optSynonym("-bg", "-background" ),
    CTkAppOpt::optSynonym("-fg", "-foreground" ),

    CTkAppOpt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs("scale pathName ?-option value ...?");

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("scale pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *scale = new CTkAppScale(tk_, parent, childName);

  if (! tk_->useStyle())
    scale->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), scale, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    scale->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppScrollBarCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  using Opt = CTkAppOpt;

  static Opt opts[] = {
  {              "-activebackground"   , "activeBackground"   , "Foreground"         , "#ececec" },
  {              "-activerelief"       , "activeRelief"       , "Relief"             , "raised"  },
  {              "-background"         , "background"         , "Background"         , "#d9d9d9" },
  Opt::optInt   ("-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"       ),
  Opt::optString("-command"            , "command"            , "Command"            , ""        ),
  Opt::optCursor("-cursor"             , "cursor"             , "Cursor"             , ""        ),
  Opt::optInt   ("-elementborderwidth" , "elementBorderWidth" , "BorderWidth"        , "-1"      ),
  Opt::optColor ("-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9" ),
  Opt::optColor ("-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000" ),
  Opt::optInt   ("-highlightthickness" , "highlightThickness" , "HighlightThickness" , "0"       ),
  Opt::optBool  ("-jump"               , "jump"               , "Jump"               , false     ),
  {              "-orient"             , "orient"             , "Orient"             , "vertical"},
  {              "-relief"             , "relief"             , "Relief"             , "sunken"  },
  Opt::optInt   ("-repeatdelay"        , "repeatDelay"        , "RepeatDelay"        , "300"     ),
  Opt::optInt   ("-repeatinterval"     , "repeatInterval"     , "RepeatInterval"     , "100"     ),
  Opt::optString("-takefocus"          , "takeFocus"          , "TakeFocus"          , ""        ),
  Opt::optColor ("-troughcolor"        , "troughColor"        , "Background"         , "#b3b3b3" ),
  Opt::optInt   ("-width"              , "width"              , "Width"              , "11"      ),

  Opt::optSynonym("-bd", "-borderwidth"),
  Opt::optSynonym("-bg", "-background" ),

  Opt::optEnd(),
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("scrollbar pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *scrollbar = new CTkAppScrollBar(tk_, parent, childName);

  if (! tk_->useStyle())
    scrollbar->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), scrollbar, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    scrollbar->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppSelectionCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("selection options ?arg arg ...?");

  static auto optionNames = std::vector<QString>({
    "clear", "get", "handle", "own" });

  QString opt;
  if (! tk_->lookupOptionName(optionNames, args[0], opt))
    return false;

  if      (opt == "clear") {
    uint i = 1;

    for ( ; i < numArgs; ++i) {
      auto arg = tk_->variantToString(args[i]);
      if (arg[0] != '-') break;

      static auto argNames = std::vector<QString>({ "-displayof", "-selection" });
      QString arg1;
      if (! tk_->lookupName("option", argNames, arg, arg1, /*quiet*/true))
        return tk_->throwError("bad option \"" + arg + "\": must be "
                               "-displayof, or -selection");

      if      (arg1 == "-displayof") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("selection clear -displayof window");
      }
      else if (arg1 == "-selection") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("selection clear -selection selection");
      }
    }

    if (i != numArgs)
      return tk_->wrongNumArgs("selection clear ?-displayof window? ?-selection selection?");

    // -displayof window
    tk_->setClipboardText("", CTkAppClipboard::Selection);
  }
  else if (opt == "get") {
    uint i = 1;

    for ( ; i < numArgs; ++i) {
      auto arg = tk_->variantToString(args[i]);
      if (arg[0] != '-') break;

      static auto argNames = std::vector<QString>({ "-displayof", "-selection", "-type" });
      QString arg1;
      if (! tk_->lookupName("option", argNames, arg, arg1, /*quiet*/true))
        return tk_->throwError("bad option \"" + arg + "\": must be "
                               "-displayof, -selection, or -type");

      if      (arg1 == "-displayof") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("selection get -displayof window");
      }
      else if (arg1 == "-selection") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("selection get -selection selection");
      }
      else if (arg1 == "-type") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("selection get -type type");
      }
    }

    if (i != numArgs)
      return tk_->wrongNumArgs("selection get ?-displayof window? "
                               "?-selection selection? ?-type type?");

    auto res = tk_->getClipboardText(CTkAppClipboard::Selection);

    setStringResult(res);
  }
  else if (opt == "handle") {
    uint i = 1;

    for ( ; i < numArgs; ++i) {
      auto arg = tk_->variantToString(args[i]);
      if (arg[0] != '-') break;

      static auto argNames = std::vector<QString>({ "-selection", "-type", "-format" });
      QString arg1;
      if (! tk_->lookupName("option", argNames, arg, arg1, /*quiet*/true))
        return tk_->throwError("bad option \"" + arg + "\": must be "
                               "-selection, -type, or -format");

      if      (arg1 == "-selection") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("selection handle -selection selection");
      }
      else if (arg1 == "-type") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("selection handle -type type");
      }
      else if (arg1 == "-format") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("selection handle -format format");
      }
    }

    if (i >= numArgs - 2)
      return tk_->wrongNumArgs("selection handle ?-selection selection? "
                               "?-type type? ?-format format? window command");
  }
  else if (opt == "own") {
    uint i = 1;

    for ( ; i < numArgs; ++i) {
      auto arg = tk_->variantToString(args[i]);
      if (arg[0] != '-') break;

      static auto argNames = std::vector<QString>({ "-displayof", "-command", "-selection" });
      QString arg1;
      if (! tk_->lookupName("option", argNames, arg, arg1, /*quiet*/true))
        return tk_->throwError("bad option \"" + arg + "\": must be "
                               "-displayof, -command, or -selection");

      if      (arg1 == "-displayof") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("selection own -displayof window");
      }
      else if (arg1 == "-command") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("selection own -command command");
      }
      else if (arg1 == "-selection") {
        tk_->TODO(args);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("selection own -selection selection");
      }
    }

    if (i != numArgs)
      return tk_->wrongNumArgs("selection own ?-displayof window? ?-selection selection?");

    ulong xwin;
    if (! CTkAppX11::getSelectionOwner(xwin))
      return true;

    auto *w = tk_->getWidgetForWin(xwin);
    if (! w) return true;

    tk_->setStringResult(w->getFullName());
  }

  return true;
}

//---

bool
CTkAppSendCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs("send ?-option value ...? interpName arg ?arg ...?");

  tk_->TODO("send");

  return true;
}

//---

bool
CTkAppSpinBoxCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  using Opt = CTkAppOpt;

  static CTkAppOpt opts[] = {
  Opt::optColor( "-activebackground"   , "activeBackground"   , "Background"         , "#ececec"),
  Opt::optColor( "-background"         , "background"         , "Background"         , "#ffffff"),
  { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"          },
  Opt::optColor( "-buttonbackground"   , "Button.background"  , "Background"         , "#d9d9d9"),
  Opt::optCursor("-buttoncursor"       , "Button.cursor"      , "Cursor"             , ""),
  { "-buttondownrelief"   , "Button.relief"      , "Relief"             , "raised"     },
  { "-buttonuprelief"     , "Button.relief"      , "Relief"             , "raised"     },
  { "-command"            , "command"            , "Command"            , ""           },
  { "-cursor"             , "cursor"             , "Cursor"             , "xterm"      },
  Opt::optColor( "-disabledbackground" , "disabledBackground" , "DisabledBackground" , "#d9d9d9"),
  Opt::optColor( "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"),
  { "-exportselection"    , "exportSelection"    , "ExportSelection"    , "1"          },
  Opt::optFont( "-font"             , "font"               , "Font"               , "TkTextFont"),
  { "-foreground"         , "foreground"         , "Foreground"         , "#000000"    },
  { "-format"             , "format"             , "Format"             , ""           },
  { "-from"               , "from"               , "From"               , "0"          },
  Opt::optColor( "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"),
  Opt::optColor( "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"),
  { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "1"          },
  { "-increment"          , "increment"          , "Increment"          , "1"          },
  Opt::optColor( "-insertbackground"   , "insertBackground"   , "Foreground"         , "#000000"),
  { "-insertborderwidth"  , "insertBorderWidth"  , "BorderWidth"        , "0"          },
  { "-insertofftime"      , "insertOffTime"      , "OffTime"            , "300"        },
  { "-insertontime"       , "insertOnTime"       , "OnTime"             , "600"        },
  { "-insertwidth"        , "insertWidth"        , "InsertWidth"        , "2"          },
  { "-invalidcommand"     , "invalidCommand"     , "InvalidCommand"     , ""           },
  { "-justify"            , "justify"            , "Justify"            , "left"       },
  { "-relief"             , "relief"             , "Relief"             , "sunken"     },
  Opt::optColor( "-readonlybackground" , "readonlyBackground" , "ReadonlyBackground" , "#d9d9d9"),
  { "-repeatdelay"        , "repeatDelay"        , "RepeatDelay"        , "400"        },
  { "-repeatinterval"     , "repeatInterval"     , "RepeatInterval"     , "100"        },
  Opt::optColor( "-selectbackground"   , "selectBackground"   , "Foreground"         , "#c3c3c3"),
  { "-selectborderwidth"  , "selectBorderWidth"  , "BorderWidth"        , "0"          },
  Opt::optColor( "-selectforeground"   , "selectForeground"   , "Background"         , "#000000"),
  { "-state"              , "state"              , "State"              , "normal"     },
  { "-takefocus"          , "takeFocus"          , "TakeFocus"          , ""           },
  { "-textvariable"       , "textVariable"       , "Variable"           , ""           },
  { "-to"                 , "to"                 , "To"                 , "0"          },
  { "-validate"           , "validate"           , "Validate"           , "none"       },
  { "-validatecommand"    , "validateCommand"    , "ValidateCommand"    , ""           },
  { "-values"             , "values"             , "Values"             , ""           },
  { "-width"              , "width"              , "Width"              , "20"         },
  { "-wrap"               , "wrap"               , "Wrap"               , "0"          },
  { "-xscrollcommand"     , "xScrollCommand"     , "ScrollCommand"      , ""           },

  CTkAppOpt::optSynonym("-bd"    , "-borderwidth"    ),
  CTkAppOpt::optSynonym("-bg"    , "-background"     ),
  CTkAppOpt::optSynonym("-fg"    , "-foreground"     ),
  CTkAppOpt::optSynonym("-invcmd", "-invalidcommand" ),
  CTkAppOpt::optSynonym("-vcmd"  , "-validatecommand"),

  CTkAppOpt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("spinbox pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *spin = new CTkAppSpinBox(tk_, parent, childName);

  if (! tk_->useStyle())
    spin->setBackground(QColor("#ffffff"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), spin, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    spin->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppTextCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    {"-autoseparators"          , "autoSeparators"          , "AutoSeparators"     , "1"          },
    {"-background"              , "background"              , "Background"         , "#ffffff"    },
    {"-blockcursor"             , "blockCursor"             , "BlockCursor"        , "0"          },
    {"-borderwidth"             , "borderWidth"             , "BorderWidth"        , "1"          },
    {"-cursor"                  , "cursor"                  , "Cursor"             , "xterm"      },
    {"-endline"                 , ""                        , ""                   , ""           },
    {"-exportselection"         , "exportSelection"         , "ExportSelection"    , "1"          },
    {"-font"                    , "font"                    , "Font"               , "TkFixedFont"},
    {"-foreground"              , "foreground"              , "Foreground"         , "#000000"    },
    {"-height"                  , "height"                  , "Height"             , "24"         },
    {"-highlightbackground"     , "highlightBackground"     , "HighlightBackground", "#d9d9d9"    },
    {"-highlightcolor"          , "highlightColor"          , "HighlightColor"     , "#000000"    },
    {"-highlightthickness"      , "highlightThickness"      , "HighlightThickness" , "1"          },
    {"-inactiveselectbackground", "inactiveSelectBackground", "Foreground"         , "#c3c3c3"    },
    {"-insertbackground"        , "insertBackground"        , "Foreground"         , "#000000"    },
    {"-insertborderwidth"       , "insertBorderWidth"       , "BorderWidth"        , "0"          },
    {"-insertofftime"           , "insertOffTime"           , "OffTime"            , "300"        },
    {"-insertontime"            , "insertOnTime"            , "OnTime"             , "600"        },
    {"-insertwidth"             , "insertWidth"             , "InsertWidth"        , "2"          },
    {"-maxundo"                 , "maxUndo"                 , "MaxUndo"            , "0"          },
    {"-padx"                    , "padX"                    , "Pad"                , "1"          },
    {"-pady"                    , "padY"                    , "Pad"                , "1"          },
    {"-relief"                  , "relief"                  , "Relief"             , "sunken"     },
    {"-selectbackground"        , "selectBackground"        , "Foreground"         , "#c3c3c3"    },
    {"-selectborderwidth"       , "selectBorderWidth"       , "BorderWidth"        , "0"          },
    {"-selectforeground"        , "selectForeground"        , "Background"         , "#000000"    },
    {"-setgrid"                 , "setGrid"                 , "SetGrid"            , "0"          },
    {"-spacing1"                , "spacing1"                , "Spacing"            , "0"          },
    {"-spacing2"                , "spacing2"                , "Spacing"            , "0"          },
    {"-spacing3"                , "spacing3"                , "Spacing"            , "0"          },
    {"-startline"               , ""                        , ""                   , ""           },
    {"-state"                   , "state"                   , "State"              , "normal"     },
    {"-tabs"                    , "tabs"                    , "Tabs"               , ""           },
    {"-tabstyle"                , "tabStyle"                , "TabStyle"           , "tabular"    },
    {"-takefocus"               , "takeFocus"               , "TakeFocus"          , ""           },
    {"-undo"                    , "undo"                    , "Undo"               , "0"          },
    {"-width"                   , "width"                   , "Width"              , "80"         },
    {"-wrap"                    , "wrap"                    , "Wrap"               , "char"       },
    {"-xscrollcommand"          , "xScrollCommand"          , "ScrollCommand"      , ""           },
    {"-yscrollcommand"          , "yScrollCommand"          , "ScrollCommand"      , ""           },

    CTkAppOpt::optSynonym("-bd", "-borderwidth"),
    CTkAppOpt::optSynonym("-bg", "-background" ),
    CTkAppOpt::optSynonym("-fg", "-foreground" ),

    {nullptr, nullptr, nullptr, nullptr }
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("text pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *text = new CTkAppText(tk_, parent, childName);

  if (! tk_->useStyle())
    text->setBackground(QColor("#ffffff"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), text, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    text->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppTkCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs("tk subcommand ?arg ...?");

  auto arg = tk_->variantToString(args[0]);

  if      (arg == "appname") {
    if      (numArgs == 1)
      tk_->setStringResult(tk_->getAppName());
    else if (numArgs == 2) {
      auto name = tk_->variantToString(args[1]);

      tk_->setAppName(name);
    }
    else
      return tk_->wrongNumArgs("tk appname ?newName?");
  }
  else if (arg == "busy") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("tk busy options ?arg ...?");

    auto option = tk_->variantToString(args[1]);

    if      (option == "cget") {
      if (numArgs != 4)
        return tk_->wrongNumArgs("tk busy cget window option");

      auto *w = tk_->lookupWidgetByName(args[2]);
      if (! w) return tk_->throwError(tk_->msg() + "bad window path name \"" + args[2] + "\"");

      tk_->TODO(args);
    }
    else if (option == "configure") {
      if (numArgs < 3)
        return tk_->wrongNumArgs("tk busy configure window ?option? ?value ...?");

      auto *w = tk_->lookupWidgetByName(args[2]);
      if (! w) return tk_->throwError(tk_->msg() + "bad window path name \"" + args[2] + "\"");

      tk_->TODO(args);
    }
    else if (option == "current") {
      tk_->setStringResult("");
      tk_->TODO(args);
    }
    else if (option == "forget") {
      if (numArgs != 3)
        return tk_->wrongNumArgs("tk busy forget window");

      auto *w = tk_->lookupWidgetByName(args[2]);
      if (! w) return tk_->throwError(tk_->msg() + "bad window path name \"" + args[2] + "\"");

      tk_->TODO(args);
    }
    else if (option == "hold") {
      if (numArgs < 3)
        return tk_->wrongNumArgs("tk busy hold window ?-option value ...?");

      auto *w = tk_->lookupWidgetByName(args[2]);
      if (! w) return tk_->throwError(tk_->msg() + "bad window path name \"" + args[2] + "\"");

      uint i = 3;

      while (i < numArgs) {
        if (args[i] == "-cursor")
          ++i;
        else
          return tk_->unknownOption(args[i]);

        ++i;
      }

      tk_->TODO(args);
    }
    else if (option == "status") {
      if (numArgs != 3)
        return tk_->wrongNumArgs("tk busy status window");

      auto *w = tk_->lookupWidgetByName(args[2]);
      if (! w) return tk_->throwError(tk_->msg() + "bad window path name \"" + args[2] + "\"");

      tk_->setIntegerResult(0);
      tk_->TODO(args);
    }
    else
      return false;
  }
  else if (arg == "caret") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("tk caret window ?-x x? ?-y y? ?-height height?");

    auto *w = tk_->lookupWidgetByName(args[1]);
    if (! w) return false;

    if (numArgs == 2)
      tk_->setStringResult("-height 0 -x 0 -y 0");
    else {
      auto opt = tk_->variantToString(args[2]);

      if      (opt == "-height") {
        if (numArgs == 3)
          tk_->setIntegerResult(0);
      }
      else if (opt == "-x") {
        if (numArgs == 3)
          tk_->setIntegerResult(0);
      }
      else if (opt == "-y") {
        if (numArgs == 3)
          tk_->setIntegerResult(0);
      }
      else
        return false;

      tk_->TODO(args);
    }
  }
  else if (arg == "fontchooser") {
    struct FontChooserData {
      QString parent   { "." };
      QString title    { "Font" };
      QString fontName { "TkDefaultFont" };
      QString command;
      bool    visible  { false };
    };

    static FontChooserData fontChooserData;

    if (numArgs < 2)
      return tk_->wrongNumArgs("tk fontchooser subcommand ?arg ...?");

    static auto commandNames = std::vector<QString>({"configure", "hide", "show"});

    QString subcommand;
    if (! tk_->lookupName("subcommand", commandNames, args[1], subcommand, /*quiet*/true))
      return tk_->throwError(tk_->msg() +  "unknown or ambiguous subcommand \"" + args[1] + "\": "
                             "must be configure, hide, or show");

    if      (subcommand == "configure") {
      if (numArgs == 2) {
        std::vector<QVariant> values;

        values.push_back("-parent" ); values.push_back(fontChooserData.parent  );
        values.push_back("-title"  ); values.push_back(fontChooserData.title   );
        values.push_back("-font"   ); values.push_back(fontChooserData.fontName);
        values.push_back("-command"); values.push_back(fontChooserData.command );
        values.push_back("-visible"); values.push_back(fontChooserData.visible );

        tk_->setVariantArrayResult(values);
      }
      else {
        for (uint i = 2; i < numArgs; i += 2) {
          const auto &name = tk_->variantToString(args[i]);

          static auto optionNames =
            std::vector<QString>({"-command", "-font", "-parent", "-title", "-visible"});

          QString optionName;
          if (! tk_->lookupName("option", optionNames, args[i], optionName, /*quiet*/true))
            return tk_->throwError(tk_->msg() + "bad option \"" + args[i] + "\"");

          if (i + 1 < numArgs) {
            auto value = tk_->variantToString(args[i + 1]);

            if      (optionName == "-command") {
              fontChooserData.command = value;
            }
            else if (optionName == "-font") {
              fontChooserData.fontName = value;
            }
            else if (optionName == "-parent") {
              auto *w = tk_->lookupWidgetByName(value);
              if (! w) return tk_->throwError(tk_->msg() +
                        "bad window path name \"" + value + "\"");

              fontChooserData.parent = value;
            }
            else if (optionName == "-title") {
              fontChooserData.title = value;
            }
            else if (optionName == "-visible") {
              bool b;
              if (! tk_->variantToBool(args[2], b))
                return tk_->invalidBool(args[2]);

              fontChooserData.visible = b;
            }
          }
          else {
            if (numArgs > 3)
              return tk_->throwError(tk_->msg() + "value for \"" + args[i] + "\" missing");

            if      (optionName == "-command") {
              tk_->setStringResult(fontChooserData.command);
            }
            else if (optionName == "-font") {
              tk_->setStringResult(fontChooserData.fontName);
            }
            else if (optionName == "-parent") {
              tk_->setStringResult(fontChooserData.parent);
            }
            else if (optionName == "-title") {
              tk_->setStringResult(fontChooserData.title);
            }
            else if (optionName == "-visible") {
              tk_->setBoolResult(fontChooserData.visible);
            }
          }
        }
      }
    }
    else if (subcommand == "hide") {
      if (numArgs != 2)
        return tk_->wrongNumArgs("tk fontchooser hide");

      tk_->showFontDialog(false);
    }
    else if (subcommand == "show") {
      if (numArgs != 2)
        return tk_->wrongNumArgs("tk fontchooser show");

      tk_->showFontDialog(true);
    }
  }
  else if (arg == "inactive") {
    uint i = 1;

    if (i < numArgs && args[i] == "-displayof")
      i += 2;

    tk_->setIntegerResult(0);
    tk_->TODO(args);
  }
  else if (arg == "scaling") {
    uint i = 1;

    if (i < numArgs && args[i] == "-displayof")
      i += 2;

    if (i >= numArgs) {
      tk_->setRealResult(tk_->getScaling());
    }
    else {
      double s;
      if (! tk_->variantToReal(args[1], s))
        return tk_->invalidReal(args[1]);

      tk_->setScaling(s);
    }
  }
  else if (arg == "useinputmethods") {
    uint i = 1;

    if (i < numArgs && args[i] == "-displayof")
      i += 2;

    tk_->setIntegerResult(1);
    tk_->TODO(args);
  }
  else if (arg == "windowingsystem") {
    tk_->setStringResult("x11");
  }
  else
    return tk_->throwError("unknown or ambiguous subcommand \"" + arg + "\": must be "
                           "appname, busy, caret, fontchooser, inactive, scaling, "
                           "useinputmethods, or windowingsystem");

  return true;
}

//---

bool
CTkAppTkChooseDirectoryCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  QWidget *parent = nullptr;

  QString              dir;
  QFileDialog::Options options;

  options |= QFileDialog::ShowDirsOnly;

  auto file = QFileDialog::getExistingDirectory(parent, "CHoose Directory", dir, options);

  tk_->setStringResult(file);

  return true;
}

//---

namespace {

static int s_CTkDialogButtonInd = -1;

}

CTkDialog::
CTkDialog(QWidget *parent) :
 QDialog(parent)
{
  setObjectName("dialog");

  s_CTkDialogButtonInd = -1;
}

void
CTkDialog::
buttonSlot()
{
  auto *button = qobject_cast<QPushButton *>(sender());
  if (! button) return;

  bool ok;
  s_CTkDialogButtonInd = button->property("ind").toInt(&ok);
  if (! ok) s_CTkDialogButtonInd = -1;

  close();
}

//---

bool
CTkAppTkDialogCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs < 5)
    return tk_->wrongNumArgs("tk_dialog w title text bitmap default ?arg ...?");

  uint i = 0;

  auto windowName = tk_->variantToString(args[i++]);

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(windowName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + windowName + "\"");

  auto title = tk_->variantToString(args[i++]);
  auto text  = tk_->variantToString(args[i++]);

  auto bitmap = tk_->variantToString(args[i++]);

  CTkAppImageRef bitmapImage;

  if (bitmap != "") {
    bitmapImage = tk_->getBitmap(bitmap);
    if (! bitmapImage)
      return tk_->throwError(tk_->msg() + "bitmap \"" + bitmap + "\" not defined");
  }

  long defInd = 0;
  if (! tk_->variantToInt(args[i], defInd))
    return tk_->invalidInteger(args[i]);
  ++i;

  std::vector<QString> buttons;

  while (i < numArgs) {
    auto name = tk_->variantToString(args[i++]);

    buttons.push_back(name);
  }

  auto *dialog = new CTkDialog(root()->getQWidget());

  dialog->setWindowTitle(title);

  auto *widget = new CTkAppWidget(tk_, parent, childName);
  widget->setQWidget(dialog);

  //---

  // add label
  auto *layout = new QVBoxLayout(dialog);

  auto *llayout = new QHBoxLayout;
  layout->addLayout(llayout);

  if (bitmapImage) {
    auto *blabel  = new QLabel; blabel->setPixmap(bitmapImage->getQPixmap());
    llayout->addWidget(blabel);
  }

  auto *label  = new QLabel; label->setText(text);
  llayout->addWidget(label);

  auto *blayout = new QHBoxLayout;
  layout->addLayout(blayout);

  // add buttons
  int buttonInd = 0;

  std::vector<CTkAppWidget        *> buttonWidgets;
  std::vector<CTkAppWidgetCommand *> buttonCmds;

  for (const auto &button : buttons) {
    auto *buttonw = new QPushButton;

    buttonw->setText(button);
    buttonw->setProperty("ind", buttonInd);

    if (buttonInd == defInd)
      buttonw->setDefault(true);

    buttonw->connect(buttonw, SIGNAL(clicked()), dialog, SLOT(buttonSlot()));

    auto buttonName    = QString("button%1").arg(buttonInd);
    auto buttonCmdName = QString("%1.%2").arg(windowName).arg(buttonName);

    auto *buttonWidget = new CTkAppWidget(tk_, widget, buttonName);
    auto *buttonCmd    = new CTkAppWidgetCommand(this, buttonCmdName, buttonWidget, nullptr);

    buttonWidget->setQWidget(buttonw);

    buttonWidgets.push_back(buttonWidget);
    buttonCmds   .push_back(buttonCmd);

    blayout->addWidget(buttonw);

    buttonw->show();

    ++buttonInd;
  }

  blayout->addStretch(1);

  //---

  // show dialog
  bool modal = true;
  if (getenv("CTK_APP_NO_MODAL"))
    modal = false;

  if (modal) {
    (void) dialog->exec();

  //delete dialog;

  //for (auto *buttonWidget : buttonWidgets)
  //  delete buttonWidget;

  //delete widget;

    widget->deleteLater();

    tk_->setIntegerResult(s_CTkDialogButtonInd);
  }
  else
    dialog->show();

  return true;
}

//---

bool
CTkAppTkGetOpenFileCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  QWidget *parent = nullptr;

  QString              dir;
  QString              filter;
  QString*             selectedFilter = nullptr;
  QFileDialog::Options options;

  auto file = QFileDialog::getOpenFileName(parent, "Open File", dir, filter,
                                           selectedFilter, options);

  tk_->setStringResult(file);

  return true;
}

//---

bool
CTkAppTkGetSaveFileCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint i       = 0;
  uint numArgs = args.size();

  bool rc = true;

  auto getValue = [&]() {
    rc = true;

    if (i + 1 >= numArgs) {
      rc = tk_->throwError(tk_->msg() + "value for \"" + args[i] + "\" missing");
      return QString();
    }

    return tk_->variantToString(args[++i]);
  };

  auto getBoolValue = [&]() {
    auto str = getValue(); if (! rc) return false;
    bool b;
    if (! tk_->variantToBool(str, b)) {
      rc = false;
      return false;
    }
    return rc;
  };

  CTkAppWidget* parentWidget = nullptr;
  bool          confirmOverwrite { false };
  QString       defaulTextExtension, fileTypes;
  QString       initialDir, initialFile, parent, title, typeVariable;

  for ( ; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if      (name == "-confirmoverwrite") {
      confirmOverwrite = getBoolValue(); if (! rc) return false;
    }
    else if (name == "-defaultextension") {
      defaulTextExtension = getValue(); if (! rc) return false;
    }
    else if (name == "-filetypes") {
      fileTypes = getValue(); if (! rc) return false;
    }
    else if (name == "-initialdir") {
      initialDir = getValue(); if (! rc) return false;
    }
    else if (name == "-initialfile") {
      initialFile = getValue(); if (! rc) return false;
    }
    else if (name == "-parent") {
      auto parentName = getValue(); if (! rc) return false;
      parentWidget = tk_->lookupWidgetByName(parentName);
      if (! parentWidget) return false;
    }
    else if (name == "-title") {
      title = getValue(); if (! rc) return false;
    }
    else if (name == "-typevariable") {
      typeVariable = getValue(); if (! rc) return false;
    }
    else
      return false;
  }

  //---

  if (! parentWidget)
    parentWidget = root();

  auto *parentW = parentWidget->getQWidget();

  if (title == "")
    title = "Open File";

  QString              filter;
  QString*             selectedFilter = nullptr;
  QFileDialog::Options options;

  if (! confirmOverwrite)
    options |= QFileDialog::Option::DontConfirmOverwrite;

  auto file = QFileDialog::getSaveFileName(parentW, title, initialDir, filter,
                                           selectedFilter, options);

  tk_->setStringResult(file);

  return true;
}

//---

bool
CTkAppTkWaitCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs != 2)
    return tk_->wrongNumArgs("tkwait variable|visibility|window name");

  static auto optionNames = std::vector<QString>({
    "variable", "visibility", "window" });

  QString arg;
  if (! tk_->lookupOptionName(optionNames, args[0], arg))
    return false;

  auto name = tk_->variantToString(args[1]);

  if      (arg == "variable") {
    auto *varProc = new CTkVarTraceEventLoop(tk_, name);

    tk_->traceVar(name, varProc);

    varProc->startLoop();

    delete varProc;
  }
  else if (arg == "visibility") {
    auto *w = tk_->lookupWidgetByName(name);
    if (! w) return false;

    if (w->isTopLevel()) {
      w->setNeedsShow(true);

      tk_->showToplevels();
    }

    auto *qwidget = w->getQWidget();
    if (! qwidget) return false;

    if (! qwidget->isVisible()) {
      auto *windowLoop = new CTkWindowVisibleEventLoop(tk_, qwidget);

      windowLoop->startLoop();

      delete windowLoop;
    }
  }
  else if (arg == "window") {
    auto *w = tk_->lookupWidgetByName(name);
    if (! w) return false;

    auto *windowLoop = new CTkWindowDestroyEventLoop(tk_, w->getQWidget());

    windowLoop->startLoop();

    delete windowLoop;
  }
  else
    return false;

  return true;
}

//---

bool
CTkAppTkMessageBoxCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  auto *messageBox = new QMessageBox(root()->getQWidget());

  messageBox->setIcon(QMessageBox::Information);
  messageBox->setText("Information");

  auto missingValue = [&](const QString &name) {
    return tk_->throwError("value for \"" + name + "\" missing");
  };

  auto stringToButton = [&](const QString &name, const QString &value,
                            QMessageBox::StandardButton &button) {
    if      (value == "abort" ) button = QMessageBox::Abort;
    else if (value == "retry" ) button = QMessageBox::Retry;
    else if (value == "ignore") button = QMessageBox::Ignore;
    else if (value == "ok"    ) button = QMessageBox::Ok;
    else if (value == "cancel") button = QMessageBox::Cancel;
    else if (value == "no"    ) button = QMessageBox::No;
    else if (value == "yes"   ) button = QMessageBox::Yes;
    else return tk_->throwError("bad " + name + " value \"" + value + "\": must be "
                  "abort, retry, ignore, ok, cancel, no, or yes");
    return true;
  };

  auto stringToIcon = [&](const QString &name, const QString &value,
                          QMessageBox::Icon &icon) {
    if      (value == "error"   ) icon = QMessageBox::Critical;
    else if (value == "info"    ) icon = QMessageBox::Information;
    else if (value == "question") icon = QMessageBox::Question;
    else if (value == "warning" ) icon = QMessageBox::Warning;
    else return tk_->throwError("bad " + name + " value \"" + value + "\": must be "
                  "error, info, question, or warning");
    return true;
  };

  bool buttonsAdded = false;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      bool hasValue = (i < numArgs - 1);
      auto value    = (hasValue ? tk_->variantToString(args[++i]) : "");

      if      (name == "-default") {
        if (! hasValue) return missingValue(name);

        QMessageBox::StandardButton button;
        if (! stringToButton(name, value, button))
          return false;

        messageBox->setDefaultButton(button);
      }
      else if (name == "-detail") {
        if (! hasValue) return missingValue(name);

        messageBox->setDetailedText(value);
      }
      else if (name == "-icon") {
        if (! hasValue) return missingValue(name);

        QMessageBox::Icon icon;
        if (! stringToIcon(name, value, icon))
          return false;

        messageBox->setIcon(icon);
      }
      else if (name == "-message") {
        if (! hasValue) return missingValue(name);

        messageBox->setText(value);
      }
      else if (name == "-parent") {
        if (! hasValue) return missingValue(name);

        auto *w = tk_->lookupWidgetByName(value);
        if (! w) return false;

        auto *parent = w->getQWidget();

        messageBox->setParent(parent);
      }
      else if (name == "-title") {
        if (! hasValue) return missingValue(name);

        messageBox->setWindowTitle(value);
      }
      else if (name == "-type") {
        if (! hasValue) return missingValue(name);

        if      (value == "abortretryignore") {
          messageBox->addButton(QMessageBox::Abort);
          messageBox->addButton(QMessageBox::Retry);
          messageBox->addButton(QMessageBox::Ignore);
          buttonsAdded = true;
        }
        else if (value == "ok") {
          messageBox->addButton(QMessageBox::Ok);
          buttonsAdded = true;
        }
        else if (value == "okcancel") {
          messageBox->addButton(QMessageBox::Ok);
          messageBox->addButton(QMessageBox::Cancel);
          buttonsAdded = true;
        }
        else if (value == "retrycancel") {
          messageBox->addButton(QMessageBox::Retry);
          messageBox->addButton(QMessageBox::Cancel);
          buttonsAdded = true;
        }
        else if (value == "yesno") {
          messageBox->addButton(QMessageBox::Yes);
          messageBox->addButton(QMessageBox::No);
          buttonsAdded = true;
        }
        else if (value == "yesnocancel") {
          messageBox->addButton(QMessageBox::Yes);
          messageBox->addButton(QMessageBox::No);
          messageBox->addButton(QMessageBox::Cancel);
          buttonsAdded = true;
        }
        else
          return tk_->throwError("bad -type value \"" + value + "\": must be "
            "abortretryignore, ok, okcancel, retrycancel, yesno, or yesnocancel");
      }
      else
        return false;
    }
    else
      tk_->TODO(name);
  }

  if (! buttonsAdded)
    messageBox->addButton(QMessageBox::Ok);

  messageBox->exec();

  delete messageBox;

  return true;
}

//---

bool
CTkAppTkPopupCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs != 3 && numArgs != 4)
    return tk_->wrongNumArgs("tk_popup menu x y ?entry?");

  auto *w = tk_->lookupWidgetByName(args[0]);
  if (! w) return false;

  auto *menu = dynamic_cast<CTkAppMenu *>(w);
  if (! menu) return tk_->throwError(tk_->msg() + "No menu '" + args[0] + "'");

  long x;
  if (! tk_->variantToInt(args[1], x))
    return tk_->invalidInteger(args[1]);

  long y;
  if (! tk_->variantToInt(args[2], y))
    return tk_->invalidInteger(args[2]);

  CTkAppWidget *refWidget = nullptr;

  if (numArgs == 4) {
    refWidget = tk_->lookupWidgetByName(args[3]);
    if (! refWidget) return false;
  }

  menu->qmenu()->popup(QPoint(x, y));

  return true;
}

//---

bool
CTkAppTopLevelCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    {"-backgroundimage"    , "backgroundImage"    , "BackgroundImage"    , ""         },
    {"-borderwidth"        , "borderWidth"        , "BorderWidth"        , "0"        },
    {"-class"              , "class"              , "Class"              , "Toplevel" },
    {"-menu"               , "menu"               , "Menu"               , ""         },
    {"-relief"             , "relief"             , "Relief"             , "flat"     },
    {"-screen"             , "screen"             , "Screen"             , ""         },
    {"-use"                , "use"                , "Use"                , ""         },
    {"-background"         , "background"         , "Background"         , "#d9d9d9"  },
    {"-colormap"           , "colormap"           , "Colormap"           , ""         },
    {"-container"          , "container"          , "Container"          , "0"        },
    {"-cursor"             , "cursor"             , "Cursor"             , ""         },
    {"-height"             , "height"             , "Height"             , "0"        },
    {"-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"  },
    {"-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"  },
    {"-highlightthickness" , "highlightThickness" , "HighlightThickness" , "0"        },
    {"-padx"               , "padX"               , "Pad"                , "0"        },
    {"-pady"               , "padY"               , "Pad"                , "0"        },
    {"-takefocus"          , "takeFocus"          , "TakeFocus"          , "0"        },
    {"-tile"               , "tile"               , "Tile"               , "0"        },
    {"-visual"             , "visual"             , "Visual"             , ""         },
    {"-width"              , "width"              , "Width"              , "0"        },

    CTkAppOpt::optSynonym("-bd"   , "-borderwidth"    ),
    CTkAppOpt::optSynonym("-bg"   , "-background"     ),
    CTkAppOpt::optSynonym("-bgimg", "-backgroundimage"),

    CTkAppOpt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("toplevel pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *toplevel = new CTkAppTopLevel(tk_, parent, childName);

  if (! tk_->useStyle())
    toplevel->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), toplevel, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    toplevel->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppTreeViewCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-class"         , "class"         , "Class"         , "Frame"        },
    { "-cursor"        , "cursor"        , "Cursor"        , ""             },
    { "-padding"       , "padding"       , "Padding"       , ""             },
    { "-style"         , "style"         , "Style"         , ""             },
    { "-takefocus"     , "takeFocus"     , "TakeFocus"     , "0"            },
    { "-xscrollcommand", "xScrollCommand", "ScrollCommand" , ""             },
    { "-yscrollcommand", "yScrollCommand", "ScrollCommand" , ""             },
    { "-columns"       , "columns"       , "Columns"       , ""             },
    { "-displaycolumns", "displayColumns", "DisplayColumns", ""             },
    { "-height"        , "height"        , "Height"        , ""             },
    { "-selectmode"    , "selectMode"    , "SelectMode"    , "browse"       },
    { "-show"          , "show"          , "Show"          , "tree headings"},

    CTkAppOpt::optEnd()
  };

  QVariant widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = args[i];
  }

  if (! widgetName.isValid())
    return tk_->wrongNumArgs("treeview pathName ?-option value ...?");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + widgetName + "\"");

  auto *child = parent->getChild(childName);
  if (child)
    return tk_->throwError(tk_->msg() + "window name \"" + childName + "\" "
                           "already exists in parent");

  auto *tree = new CTkAppTreeView(tk_, parent, childName);

  if (! tk_->useStyle())
    tree->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, tk_->variantToString(widgetName), tree, opts);

  if (! cmd->processArgs(args, /*inCreate*/true)) {
    tree->deleteLater();
    return false;
  }

  setResult(widgetName);

  return true;
}

//---

bool
CTkAppUpdateCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  tk_->showToplevels();

  uint numArgs = args.size();

  if      (numArgs == 0) {
    tk_->processEvents();
  }
  else if (numArgs == 1) {
    static auto optionNames = std::vector<QString>({
      "idletasks"});

    QString arg;
    if (! tk_->lookupOptionName(optionNames, args[0], arg))
      return false;

    if (arg == "idletasks")
      tk_->processEvents();
  }
  else
    return tk_->wrongNumArgs("update ?idletasks?");

  return true;
}

//---

bool
CTkAppWInfoCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("winfo option ?arg ...?");

  static auto optionNames = std::vector<QString>({
    "cells", "children", "class", "colormapfull", "depth", "geometry", "height", "id",
    "ismapped", "manager", "" "name", "parent", "pointerx", "pointery", "pointerxy",
    "reqheight", "reqwidth", "rootx", "rooty", "screen", "" "screencells", "screendepth",
    "screenheight", "screenwidth", "screenmmheight", "screenmmwidth", "screenvisual",
    "server", "toplevel", "viewable", "visual", "visualid", "vrootheight", "vrootwidth",
    "vrootx", "vrooty", "width", "x", "y", "atom", "atomname", "containing", "interps",
    "pathname", "exists", "fpixels", "pixels", "rgb", "visualsavailable", "debug" });

  QString arg;
  if (! tk_->lookupOptionName(optionNames, args[0], arg))
    return false;

  //---

  CTkAppWidget *w = nullptr;

  auto getWindow = [&](uint &i, bool quiet=false) {
    if (numArgs < 2)
      return tk_->wrongNumArgs("winfo " + arg + " window");

    auto widgetName = args[i];

    w = tk_->lookupWidgetByName(widgetName, quiet);
    if (! w) return false;

    ++i;

    return true;
  };

  //---

  CTkAppWidget *displayW = nullptr;

  auto getDisplayOf = [&](uint &i) {
    if (i >= numArgs)
      return true;

    static auto argNames = std::vector<QString>({ "-displayof" });
    QString arg;
    if (! tk_->lookupName("option", argNames, args[i], arg, /*quiet*/true))
      return true;

    ++i;

    displayW = tk_->lookupWidgetByName(args[i]);
    if (! displayW) return tk_->throwError(tk_->msg() + "bad window path name \"" + args[i] + "\"");

    ++i;

    return true;
  };

  //---

  uint i = 1;

  if      (arg == "atom") {
    if (! getDisplayOf(i))
      return false;

    if (i != numArgs - 1)
      return tk_->wrongNumArgs("winfo atom ?-displayof window? name");

    auto name = tk_->variantToString(args[i]);

    auto xatom = CTkAppX11::getAtomInd(name, /*create*/false);

    // get atom id for name
    setIntegerResult(xatom);
  }
  else if (arg == "atomname") {
    if (! getDisplayOf(i))
      return false;

    if (i != numArgs - 1)
      return tk_->wrongNumArgs("winfo atomname ?-displayof window? id");

    long atom;
    if (! tk_->variantToInt(args[i], atom))
      return tk_->invalidInteger(args[i]);

    auto name = CTkAppX11::getAtomName(atom);

    if (name == "")
      return tk_->throwError(tk_->msg() + "no atom exists with id \"" +
                             QString::number(atom) + "\"");

    setStringResult(name);
  }
  else if (arg == "cells") {
    if (! getWindow(i))
      return false;

    tk_->TODO(args); // window size in cells ?
  }
  else if (arg == "children") {
    if (! getWindow(i))
      return false;

    std::vector<CTkAppWidget *> children;

    w->getChildren(children);

    uint numChildren = children.size();

    std::vector<QString> list;

    for (uint ic = 0; ic < numChildren; ++ic) {
      auto *child = children[ic];

      list.push_back(child->getFullName());
    }

    setStringArrayResult(list);
  }
  else if (arg == "class") {
    if (! getWindow(i))
      return false;

    setStringResult(w->calcClassName());
  }
  else if (arg == "colormapfull") {
    if (i != numArgs)
      return tk_->wrongNumArgs("winfo colormapfull window");

    tk_->TODO(args);
  }
  else if (arg == "containing") {
    if (! getDisplayOf(i))
      return false;

    if (i + 2 != numArgs)
      return tk_->wrongNumArgs("winfo containing ?-displayof window? rootX rootY");

    CTkAppDistance x;
    if (! tk_->variantToDistanceI(args[i], x))
      return tk_->invalidDistance(args[i]);

    ++i;

    CTkAppDistance y;
    if (! tk_->variantToDistanceI(args[i], y))
      return tk_->invalidDistance(args[i]);

    auto *w = tk_->getWidgetAt(x.ivalue, y.ivalue);

    setStringResult(w ? w->getFullName() : "");
  }
  else if (arg == "depth") {
    if (! getWindow(i))
      return false;

    auto *screen = qApp->primaryScreen();
    setIntegerResult(screen->depth());
  }
  else if (arg == "exists") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("winfo exists window");

    getWindow(i, /*quiet*/true);

    setIntegerResult(w ? 1 : 0);
  }
  else if (arg == "fpixels") {
    if (numArgs != 3)
      return tk_->wrongNumArgs("winfo fpixels window number");

    auto *w = tk_->lookupWidgetByName(args[1]);
    if (! w) return false;

    CTkAppDistance size;
    if (! tk_->variantToDistance(args[2], size))
      return tk_->invalidDistance(args[2]);

    setRealResult(size.rvalue);
  }
  else if (arg == "geometry") {
    if (! getWindow(i))
      return false;

    auto *qwidget = w->getQWidget();
    if (! qwidget) return false;

    auto r = qwidget->geometry();

    auto res = tk_->geometryStr(r);

    setStringResult(res);
  }
  else if (arg == "height") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo height window");

    auto *qwidget = w->getQWidget();
    if (! qwidget) return false;

    setIntegerResult(qwidget->height());
  }
  else if (arg == "id") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo id window");

    setIntegerResult(ulong(w->getQWidget()));
  }
  else if (arg == "interps") {
    if (! getDisplayOf(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo interps ?-displayof window?");

    setStringResult("CTkApp");
  }
  else if (arg == "ismapped") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo ismapped window");

    auto *qwidget = w->getQWidget();

    setIntegerResult(qwidget ? qwidget->isVisible() : 0);
  }
  else if (arg == "manager") {
    if (! getWindow(i))
      return false;

    auto *parent = w->getParent();
    auto *layout = (parent ? parent->getTkLayout() : nullptr);

    setStringResult(layout ? layout->name() : "");
  }
  else if (arg == "name") {
    if (! getWindow(i))
      return false;

    setStringResult(w->getName());
  }
  else if (arg == "parent") {
    if (! getWindow(i))
      return false;

    auto *parent = w->getParent();

    setStringResult(parent ? parent->getFullName() : "");
  }
  else if (arg == "pathname") {
    if (! getDisplayOf(i))
      return false;

    if (i != numArgs - 1)
      return tk_->wrongNumArgs("winfo pathname ?-displayof window? id");

    long id;
    if (! tk_->variantToInt(args[i], id))
      return tk_->invalidInteger(args[i]);

//  XWindowAttributes attr;
//  if (! XGetWindowAttributes(display_, xwin, &attr))
//    return tk_->throwError(tk_->msg() + window id \"" + QString::number(id) +
//                           "\" doesn't exist in this application");
    auto *w = tk_->lookupWidgetId(ulong(id));
    if (! w)
      return tk_->throwError(tk_->msg() + "window id \"" + QString::number(id) +
                             "\" doesn't exist in this application");

    setStringResult(w->getFullName());
  }
  else if (arg == "pixels") {
    if (numArgs != 3)
      return tk_->wrongNumArgs("winfo fpixels window number");

    auto *w = tk_->lookupWidgetByName(args[1]);
    if (! w) return false;

    CTkAppDistance size;
    if (! tk_->variantToDistanceI(args[2], size))
      return tk_->invalidDistance(args[2]);

    setIntegerResult(size.ivalue);
  }
  else if (arg == "pointerx") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo pointerx window");

    auto pos = QCursor::pos();
    setIntegerResult(pos.x());
  }
  else if (arg == "pointerxy") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo pointerxy window");

    auto pos = QCursor::pos();
    setIntegerArrayResult({ pos.x(), pos.y()});
  }
  else if (arg == "pointery") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo pointery window");

    auto pos = QCursor::pos();
    setIntegerResult(pos.y());
  }
  else if (arg == "reqheight") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo reqheight window");

    auto *qwidget = w->getQWidget();
    if (! qwidget) return false;

    setIntegerResult(qwidget->sizeHint().height());
  }
  else if (arg == "reqwidth") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo reqwidth window");

    auto *qwidget = w->getQWidget();
    if (! qwidget) return false;

    setIntegerResult(qwidget->sizeHint().width());
  }
  else if (arg == "rgb") {
    if (numArgs != 3)
      return tk_->wrongNumArgs("winfo rgb window colorName");

    auto *w = tk_->lookupWidgetByName(args[1]);
    if (! w) return false;

    QColor c;
    if (! tk_->variantToQColor(args[2], c))
      return tk_->invalidQColor(args[2]);

    tk_->setIntegerArrayResult({int(65535*c.redF()), int(65535*c.greenF()), int(65535*c.blueF())});
  }
  else if (arg == "rootx") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo rootx window");

    auto *qwidget = w->getQWidget();
    if (! qwidget) return false;

    auto rect = qwidget->rect();
    auto tl   = qwidget->mapToGlobal(rect.topLeft());

    setIntegerResult(tl.x());
  }
  else if (arg == "rooty") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo rooty window");

    auto *qwidget = w->getQWidget();
    if (! qwidget) return false;

    auto rect = qwidget->rect();
    auto tl   = qwidget->mapToGlobal(rect.topLeft());

    setIntegerResult(tl.y());
  }
  else if (arg == "screen") {
    if (! getWindow(i))
      return false;

    auto *screen = qApp->primaryScreen();
    setStringResult(screen->name());
  }
  else if (arg == "screencells") {
    if (! getWindow(i))
      return false;

    tk_->TODO(args); // window size in cells ?
  }
  else if (arg == "screendepth") {
    if (! getWindow(i))
      return false;

    setIntegerResult(24);
  }
  else if (arg == "screenheight") {
    if (! getWindow(i))
      return false;

    auto *screen = qApp->primaryScreen();
    auto r = screen->geometry();
    setIntegerResult(r.height());
  }
  else if (arg == "screenmmheight") {
    if (! getWindow(i))
      return false;

    auto *screen = qApp->primaryScreen();
    auto r = screen->geometry();
    tk_->setRealResult(r.height()/CScreenUnitsMgrInst->mmSize());
  }
  else if (arg == "screenmmwidth") {
    if (! getWindow(i))
      return false;

    auto *screen = qApp->primaryScreen();
    auto r = screen->geometry();
    tk_->setRealResult(r.width()/CScreenUnitsMgrInst->mmSize());
  }
  else if (arg == "screenvisual") {
    if (! getWindow(i))
      return false;

    setStringResult("truecolor"); // TODO
  }
  else if (arg == "screenwidth") {
    if (! getWindow(i))
      return false;

    auto *screen = qApp->primaryScreen();
    auto r = screen->geometry();
    setIntegerResult(r.width());
  }
  else if (arg == "server") {
    if (! getWindow(i))
      return false;

    setStringResult("X11R0 The X.Org Foundation 12101004"); // TODO
  }
  else if (arg == "toplevel") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo toplevel window");

    auto *toplevel = w->toplevel();
    setStringResult(toplevel ? toplevel->getFullName() : ".");
  }
  else if (arg == "viewable") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo viewable window");

    auto *qwidget = w->getQWidget();

    setIntegerResult(qwidget ? qwidget->isVisible() : 0);
  }
  else if (arg == "visual") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo visual window");

    setStringResult("truecolor"); // TODO
  }
  else if (arg == "visualid") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo visualid window");

    setStringResult("0x21"); // TODO
  }
  else if (arg == "visualsavailable") {
    if (! getWindow(i))
      return false;

    if (i < numArgs) {
      if (args[i] != "includeids")
        return tk_->wrongNumArgs("winfo visualsavailable window ?includeids?");

      ++i;
    }

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo visualsavailable window ?includeids?");

    std::vector<QVariant> vars;
    QVariantList vars1;

    vars1.push_back("truecolor"); vars1.push_back(24);
    vars.push_back(vars1);

    tk_->setVariantArrayResult(vars);
  }
  else if (arg == "vrootheight") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo vrootheight window");

    auto *screen = qApp->primaryScreen();
    auto r = screen->geometry();
    setIntegerResult(r.height());
  }
  else if (arg == "vrootwidth") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo vrootwidth window");

    auto *screen = qApp->primaryScreen();
    auto r = screen->geometry();
    setIntegerResult(r.width());
  }
  else if (arg == "vrootx") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo vrootx window");

    auto *screen = qApp->primaryScreen();
    auto r = screen->geometry();
    setIntegerResult(r.x());
  }
  else if (arg == "vrooty") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo vrooty window");

    auto *screen = qApp->primaryScreen();
    auto r = screen->geometry();
    setIntegerResult(r.y());
  }
  else if (arg == "width") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo height window");

    auto *qwidget = w->getQWidget();

    setIntegerResult(qwidget ? qwidget->width() : 0);
  }
  else if (arg == "x") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo x window");

    auto *qwidget = (w ? w->getQWidget() : nullptr);

    setIntegerResult(qwidget ? qwidget->x() : 0);
  }
  else if (arg == "y") {
    if (! getWindow(i))
      return false;

    if (i != numArgs)
      return tk_->wrongNumArgs("winfo y window");

    auto *qwidget = (w ? w->getQWidget() : nullptr);

    setIntegerResult(qwidget ? qwidget->y() : 0);
  }
  else if (arg == "debug") {
    tk_->showDebug();
  }

  return true;
}

//---

bool
CTkAppWmCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("wm option window ?arg ...?");

  //---

  static auto optionNames = std::vector<QString>({
    "aspect", "attributes", "client", "colormapwindows",
    "command", "deiconify", "focusmodel", "forget",
    "frame", "geometry", "grid", "group", "iconbadge", "iconbitmap",
    "iconify", "iconmask", "iconname", "iconphoto",
    "iconposition", "iconwindow", "manage", "maxsize",
    "minsize", "overrideredirect", "positionfrom",
    "protocol", "resizable", "sizefrom", "stackorder",
    "state", "title", "transient", "withdraw" });

  QString arg;
  if (! tk_->lookupOptionName(optionNames, args[0], arg))
    return false;

  //---

  if (numArgs < 2)
    return tk_->wrongNumArgs("wm option window ?arg ...?");

  auto name = args[1];

  CTkAppWidget* parent;
  QString       childName;

  if (! root()->decodeWidgetName(name, &parent, childName))
    return tk_->throwError(tk_->msg() + "bad window path name \"" + name + "\"");

  CTkAppWidget *w;

  if (childName != "") {
    auto *child = parent->getChild(childName);
    if (! child) return tk_->throwError(tk_->msg() + "bad window path name \"" + name + "\"");

    w = child;
  }
  else
    w = parent;

  //---

  if      (arg == "aspect") {
    if      (numArgs == 6) {
      long minNumer = -1;
      if (tk_->variantIsValid(args[2])) {
        if (! tk_->variantToInt(args[2], minNumer))
          return tk_->invalidInteger(args[2]);
        if (minNumer <= 0) return tk_->throwError(tk_->msg() + "aspect number can't be <= 0");
      }

      long minDenom = -1;
      if (tk_->variantIsValid(args[3])) {
        if (! tk_->variantToInt(args[3], minDenom))
          return tk_->invalidInteger(args[3]);
        if (minDenom <= 0) return tk_->throwError(tk_->msg() + "aspect number can't be <= 0");
      }

      long maxNumer = -1;
      if (tk_->variantIsValid(args[4])) {
        if (! tk_->variantToInt(args[4], maxNumer))
          return tk_->invalidInteger(args[4]);
        if (maxNumer <= 0) return tk_->throwError(tk_->msg() + "aspect number can't be <= 0");
      }

      long maxDenom = -1;
      if (tk_->variantIsValid(args[5])) {
        if (! tk_->variantToInt(args[5], maxDenom))
          return tk_->invalidInteger(args[5]);
        if (maxDenom <= 0) return tk_->throwError(tk_->msg() + "aspect number can't be <= 0");
      }

      if (minNumer < 0 || minDenom < 0 || maxNumer < 0 || maxDenom < 0) {
        w->setWmMinAspect(CTkAppWidget::OptPointI());
        w->setWmMaxAspect(CTkAppWidget::OptPointI());
      }
      else {
        w->setWmMinAspect(CTkAppWidget::OptPointI(QPoint(minNumer, minDenom)));
        w->setWmMaxAspect(CTkAppWidget::OptPointI(QPoint(maxNumer, maxDenom)));
      }
      w->updateWmSizeHints();
    }
    else if (numArgs == 2) {
      if (w->wmMinAspect() && w->wmMaxAspect()) {
        auto minAspect = *w->wmMinAspect();
        auto maxAspect = *w->wmMaxAspect();

        QVariantList vars;

        vars.push_back(QVariant(minAspect.x()));
        vars.push_back(QVariant(minAspect.y()));
        vars.push_back(QVariant(maxAspect.x()));
        vars.push_back(QVariant(maxAspect.y()));

        tk_->setVariantListResult(vars);
      }
    }
    else
      return tk_->wrongNumArgs("wm aspect window ?minNumer minDenom maxNumer maxDenom?");
  }
  else if (arg == "attributes") {
    if (numArgs == 2) {
      std::vector<QVariant> vars;

      vars.push_back("-alpha"     ); vars.push_back(1.0);
      vars.push_back("-topmost"   ); vars.push_back(0);
      vars.push_back("-zoomed"    ); vars.push_back(0);
      vars.push_back("-fullscreen"); vars.push_back(0);
      vars.push_back("-type"      ); vars.push_back("");

      tk_->setVariantArrayResult(vars);
    }
    else {
      auto opt = tk_->variantToString(args[2]);

      if      (opt == "-alpha") {
        if (numArgs == 2)
          tk_->setStringResult("1.0"); // TODO
      }
      else if (opt == "-fullscreen") {
        if (numArgs == 2)
          tk_->setStringResult("0"); // TODO
      }
      else if (opt == "-topmost") {
        if (numArgs == 2)
          tk_->setStringResult("0"); // TODO
      }
      else if (opt == "-zoomed") {
        if (numArgs == 2)
          tk_->setStringResult("0"); // TODO
      }
      else if (opt == "-type") {
        if (numArgs == 2)
          tk_->setStringResult(""); // TODO
      }
      else
        return tk_->throwError("bad attribute \"" + opt + "\": must be "
          "-alpha, -topmost, -zoomed, -fullscreen, or -type");
    }
  }
  else if (arg == "client") {
    if (numArgs > 3)
      return tk_->wrongNumArgs("wm client window ?name?");

    auto *qwidget = w->getQWidget();
    if (! qwidget) return false;

    auto winId = qwidget->winId();

    if      (numArgs == 2) {
      QString name;
      CTkAppX11::getWmClientMachine(winId, name);

      tk_->setStringResult(name);
    }
    else if (numArgs == 3) {
      auto name = tk_->variantToString(args[2]);

      CTkAppX11::setWmClientMachine(winId, name);
    }
  }
  else if (arg == "colormapwindows") {
    auto *qwidget = w->getQWidget();
    if (! qwidget) return false;

    auto winId = qwidget->winId();

    std::vector<CTkAppWidget *> widgets;

    if      (numArgs == 2) {
      std::vector<uint> windows;
      if (! CTkAppX11::getWmColormapWindows(winId, windows)) {
        w->getColormapChildren(widgets);

        for (auto *w1 : widgets) {
          auto *qwidget1 = w1->getQWidget();

          auto winId1 = (qwidget1 ? qwidget1->winId() : 0);

          windows.push_back(winId1);
        }
      }

      std::vector<QString> winNames;
      for (auto *w : widgets)
        winNames.push_back(w->getFullName());

      tk_->setStringArrayResult(winNames);
    }
    else if (numArgs == 3) {
      auto str = tk_->variantToString(args[2]);

      std::vector<QString> strs;
      if (! tk_->splitList(str, strs))
        return false;

      std::vector<uint> windows;

      for (const auto &str : strs) {
        auto *w = tk_->lookupWidgetByName(str);
        if (! w) return tk_->throwError(tk_->msg() + "bad window path name \"" + str + "\"");

        auto *qwidget1 = w->getQWidget();

        auto winId1 = (qwidget1 ? qwidget1->winId() : 0);

        windows.push_back(winId1);
      }

      CTkAppX11::setWmColormapWindows(winId, windows);
    }
    else
      return tk_->wrongNumArgs("wm colormapwindows window ?windowList?");
  }
  else if (arg == "command") {
    auto *qwidget = w->getQWidget();
    if (! qwidget) return false;

    auto winId = qwidget->winId();

    if      (numArgs == 2) {
      QString name;
      CTkAppX11::getWmCommand(winId, name);

      tk_->setStringResult(name);
    }
    else if (numArgs == 3) {
      auto name = tk_->variantToString(args[2]);

      CTkAppX11::setWmCommand(winId, name);
    }
    else
      return tk_->wrongNumArgs("wm command window ?value?");
  }
  else if (arg == "deiconify") {
    if (numArgs == 2) {
      if (! w->isRoot() && w->isTopLevel()) {
        auto *toplevel = w->toplevel();
        assert(toplevel);

        if (toplevel->isIconWindow()) {
          auto itoplevel = toplevel->iconWindowP();

          return tk_->throwError(tk_->msg() + "can't deiconify " + w->getFullName() +
                                 ": it is an icon for " + itoplevel->getFullName());
        }
      }

      if (! w->show())
        return false;
    }
    else
      return tk_->wrongNumArgs("wm deiconify window");
  }
  else if (arg == "focusmodel") {
    if      (numArgs == 2) {
      if (w->focusModel() == CTkAppWidget::FocusModel::Passive)
        tk_->setStringResult("passive");
      else
        tk_->setStringResult("active");
    }
    else if (numArgs == 3) {
      static auto optionNames = std::vector<QString>({"active", "passive"});

      QString arg1;
      if (! tk_->lookupName("argument", optionNames, args[2], arg1))
        return false;

      if (arg1 == "passive")
        w->setFocusModel(CTkAppWidget::FocusModel::Passive);
      else
        w->setFocusModel(CTkAppWidget::FocusModel::Active);
    }
    else
      return tk_->wrongNumArgs("wm focusmodel window ?active|passive?");
  }
  else if (arg == "forget") {
    w->hide();
  }
  else if (arg == "frame") {
    if (! w->isTopLevel())
      return tk_->throwError(tk_->msg() + "window \"" + args[1] + "\" isn't a top-level window");

    if (numArgs != 2)
      return tk_->wrongNumArgs("wm frame window");

    tk_->setStringResult(""); // TODO
  }
  else if (arg == "geometry") {
    if (! w->isTopLevel())
      return tk_->throwError(tk_->msg() + "window \"" + args[1] + "\" isn't a top-level window");

    if      (numArgs == 2)
      setStringResult(w->getGeometry());
    else if (numArgs == 3) {
      auto *pw = tk_->lookupWidgetByName(w->wmTransientFor(), /*quiet*/true);

      if (pw) {
        if (! w->setTransientGeometry(pw, tk_->variantToString(args[2])))
          return tk_->throwError(tk_->msg() + "bad geometry specifier \"" + args[2] + "\"");
      }
      else {
        if (! w->setGeometry(tk_->variantToString(args[2])))
          return tk_->throwError(tk_->msg() + "bad geometry specifier \"" + args[2] + "\"");
      }
    }
    else
      return tk_->wrongNumArgs("wm geometry window ?newGeometry?");

    if (w->isTopLevel()) {
      w->setNeedsShow(true);

      tk_->showToplevels();
    }
  }
  else if (arg == "grid") {
    if (! w->isTopLevel())
      return tk_->throwError(tk_->msg() + "window \"" + args[1] + "\" isn't a top-level window");

    if      (numArgs == 2) {
      int w, h, iw, ih;
      tk_->getWmGrid(w, h, iw, ih);

      setIntegerArrayResult({w, h, iw, ih});
    }
    else if (numArgs == 6) {
      std::vector<int> errorInds;
      std::vector<int> badInds;

      auto variantToInt = [&](int ind, long &i) {
        i = 0;
        if (tk_->variantIsValid(args[ind])) {
          if (! tk_->variantToInt(args[ind], i)) {
            errorInds.push_back(ind);
            return false;
          }
        }
        return true;
      };

      long w, h, iw, ih;
      if (! variantToInt(2,  w) || ! variantToInt(3,  h) ||
          ! variantToInt(4, iw) || ! variantToInt(5, ih))
        return tk_->invalidInteger(args[*errorInds.begin()]);

      if (w  <  0) return tk_->throwError(tk_->msg() + "baseWidth can't be < 0");
      if (h  <  0) return tk_->throwError(tk_->msg() + "baseHeight can't be < 0");
      if (iw <= 0) return tk_->throwError(tk_->msg() + "widthInc can't be <= 0");
      if (ih <= 0) return tk_->throwError(tk_->msg() + "heightInc can't be <= 0");

      tk_->setWmGrid(w, h, iw, ih);
    }
    else
      return tk_->wrongNumArgs("wm grid window ?baseWidth baseHeight widthInc heightInc?");
  }
  else if (arg == "group") {
    if (! w->isTopLevel())
      return tk_->throwError(tk_->msg() + "window \"" + args[1] + "\" isn't a top-level window");

    if      (numArgs == 2) {
      auto *w1 = tk_->getWmGroup(w);

      setStringResult(w1 ? w1->getFullName() : "");
    }
    else if (numArgs == 3) {
      if (tk_->variantIsValid(args[2])) {
        auto *w1 = tk_->lookupWidgetByName(args[2]);
        if (! w1) return tk_->throwError(tk_->msg() + "bad window path name \"" + args[2] + "\"");

        tk_->setWmGroup(w, w1);
      }
      else
        tk_->setWmGroup(w, nullptr);
    }
    else
      return tk_->wrongNumArgs("wm group window ?pathName?");
  }
  else if (arg == "iconbadge") {
    if (numArgs != 3)
      return tk_->wrongNumArgs("wm iconbadge window badge");

    auto name1 = tk_->variantToString(args[1]);
    auto badge = tk_->variantToString(args[2]);

    auto cmd = QString("::tk::icons::IconBadge {%1} {%2}").arg(name1).arg(badge);

    tk_->eval(cmd);
  }
  else if (arg == "iconbitmap") {
    if (! w->isTopLevel())
      return tk_->throwError(tk_->msg() + "window \"" + args[1] + "\" isn't a top-level window");

    if      (numArgs == 2)
      setStringResult(""); // TODO
    else if (numArgs == 3) {
      auto bitmapImage = tk_->getBitmap(tk_->variantToString(args[2]));
      if (! bitmapImage)
        return tk_->throwError(tk_->msg() + "bitmap \"" + args[2] + "\" not defined");
    }
    else
      return tk_->wrongNumArgs("wm iconbitmap window ?bitmap?");
  }
  else if (arg == "iconify") {
    if (! w->isTopLevel())
      return tk_->throwError(tk_->msg() + "window \"" + args[1] + "\" isn't a top-level window");

    if (numArgs != 2)
      return tk_->wrongNumArgs("wm iconify window");

    auto *qwidget = w->getQWidget();

    if (qwidget)
      qwidget->showMinimized();
  }
  else if (arg == "iconmask") {
    if (! w->isTopLevel())
      return tk_->throwError(tk_->msg() + "window \"" + args[1] + "\" isn't a top-level window");

    if      (numArgs == 2) {
      auto im = w->iconMask();

      setStringResult(im ? im->name() : "");
    }
    else if (numArgs == 3) {
      if (tk_->variantIsValid(args[2])) {
        auto bitmapImage = tk_->getBitmap(tk_->variantToString(args[2]));
        if (! bitmapImage)
          return tk_->throwError(tk_->msg() + "bitmap \"" + args[2] + "\" not defined");

        w->setIconMask(bitmapImage);
      }
      else
        w->setIconMask(CTkAppImageRef());
    }
    else
      return tk_->wrongNumArgs("wm iconmask window ?bitmap?");
  }
  else if (arg == "iconname") {
    if (! w->isTopLevel())
      return tk_->throwError(tk_->msg() + "window \"" + args[1] + "\" isn't a top-level window");

    if      (numArgs == 2)
      setStringResult(w->getIcon());
    else if (numArgs == 3)
      w->setIcon(tk_->variantToString(args[2]));
    else
      return tk_->wrongNumArgs("wm iconname window ?newName?");
  }
  else if (arg == "iconphoto") {
    if (! w->isTopLevel())
      return tk_->throwError(tk_->msg() + "window \"" + args[1] + "\" isn't a top-level window");

    if (numArgs < 3)
      return tk_->wrongNumArgs("wm iconphoto window ?-default? image1 ?image2 ...?");
    else {
      for (uint i = 2; i < numArgs; ++i) {
        if (tk_->variantToString(args[i]) == "-default")
          continue;

        auto image = tk_->getImage(args[i]);
        if (! image) return tk_->throwError(tk_->msg() + "can't use \"" + args[i] + "\" as "
                                            "iconphoto: not a photo image");
      }
    }
  }
  else if (arg == "iconposition") {
    if (! w->isTopLevel())
      return tk_->throwError(tk_->msg() + "window \"" + args[1] + "\" isn't a top-level window");

    if      (numArgs == 2) {
      auto p = w->iconPosition();

      tk_->setIntegerArrayResult({p.x(), p.y()});
    }
    else if (numArgs == 4) {
      long x, y;
      if (! tk_->variantToInt(args[2], x))
        return tk_->invalidInteger(args[2]);

      if (! tk_->variantToInt(args[3], y))
        return tk_->invalidInteger(args[3]);

      w->setIconPosition(QPoint(x, y));
    }
    else
      return tk_->wrongNumArgs("wm iconposition window ?x y?");
  }
  else if (arg == "iconwindow") {
    if (! w->isTopLevel())
      return tk_->throwError(tk_->msg() + "window \"" + args[1] + "\" isn't a top-level window");

    auto *toplevel = dynamic_cast<CTkAppWidgetRoot *>(w);
//  auto *toplevel = w->toplevel();
    if (! toplevel) return false;

    auto itoplevel = toplevel->iconWindowP();

    if      (numArgs == 2) {
      setStringResult(itoplevel ? itoplevel->getFullName() : "");
    }
    else if (numArgs == 3) {
      if (tk_->variantIsValid(args[2])) {
        auto *iw = tk_->lookupWidgetByName(args[2]);
        if (! iw) return tk_->throwError(tk_->msg() + "bad window path name \"" + args[2] + "\"");

        if (! iw->isTopLevel())
          return tk_->throwError(tk_->msg() +
            "can't use " + args[2] + " as icon window: not at top level");

        if (itoplevel) {
          itoplevel->setIconWindow(false);
          itoplevel->setIconWindowP(CTkAppTopLevel::TopLevelP());
        }

        itoplevel = iw->toplevel();

        toplevel->setIconWindow(false);
        toplevel->setIconWindowP(itoplevel);

        itoplevel->setIconWindow(true);
        itoplevel->setIconWindowP(toplevel);

        itoplevel->hide();
      }
      else {
        if (itoplevel) {
          itoplevel->setIconWindow(false);
          itoplevel->setIconWindowP(CTkAppTopLevel::TopLevelP());
        }

        toplevel->setIconWindow(false);
        toplevel->setIconWindowP(CTkAppTopLevel::TopLevelP());
      }
    }
    else
      return tk_->wrongNumArgs("wm iconwindow window ?pathName?");
  }
  else if (arg == "manage") {
    // set as toplevel
    tk_->TODO(args);
  }
  else if (arg == "maxsize") {
    if      (numArgs == 2) {
      auto *qwidget = w->getQWidget();
      if (! qwidget) return false;

      auto size = qwidget->maximumSize();

      tk_->setIntegerArrayResult({size.width(), size.height()});
    }
    else if (numArgs == 4) {
      long sw, sh;
      if (! tk_->variantToInt(args[2], sw))
        return tk_->invalidDistance(args[2]);

      if (! tk_->variantToInt(args[3], sh))
        return tk_->invalidDistance(args[3]);

      sw = std::max(sw, 1L);
      sh = std::max(sh, 1L);

      auto *qwidget = w->getQWidget();
      if (! qwidget) return false;

      qwidget->setMaximumSize(QSize(sw, sh));
    }
    else if (numArgs != 4)
      return tk_->wrongNumArgs("wm maxsize window ?width height?");
  }
  else if (arg == "minsize") {
    if      (numArgs == 2) {
      auto *qwidget = w->getQWidget();
      if (! qwidget) return false;

      auto size = qwidget->minimumSize();

      tk_->setIntegerArrayResult({size.width(), size.height()});
    }
    else if (numArgs == 4) {
      long sw, sh;

      if (! tk_->variantToInt(args[2], sw))
        return tk_->invalidDistance(args[2]);

      if (! tk_->variantToInt(args[3], sh))
        return tk_->invalidDistance(args[3]);

      sw = std::max(sw, 1L);
      sh = std::max(sh, 1L);

      auto *qwidget = w->getQWidget();
      if (! qwidget) return false;

      qwidget->setMinimumSize(QSize(sw, sh));
    }
    else
      return tk_->wrongNumArgs("wm minsize window ?width height?");
  }
  else if (arg == "overrideredirect") {
    if      (numArgs == 2)
      tk_->setBoolResult(w->isOverrideRedirect());
    else if (numArgs == 3) {
      bool b;
      if (! tk_->variantToBool(args[2], b))
        return tk_->invalidBool(args[2]);

      w->setOverrideRedirect(b);
    }
    else
      return tk_->wrongNumArgs("wm overrideredirect window ?boolean?");
  }
  else if (arg == "positionfrom") {
    if      (numArgs == 2) {
      if      (w->positionFrom() == CTkAppWidget::PositionFrom::USER)
        setStringResult("user");
      else if (w->positionFrom() == CTkAppWidget::PositionFrom::PROGRAM)
        setStringResult("program");
      else
        setStringResult("");
    }
    else if (numArgs == 3) {
      if (tk_->variantIsValid(args[2])) {
        static auto optionNames = std::vector<QString>({"user", "program"});

        QString arg1;
        if (! tk_->lookupName("argument", optionNames, args[2], arg1))
          return false;

        if (arg1 == "user")
          w->setPositionFrom(CTkAppWidget::PositionFrom::USER);
        else
          w->setPositionFrom(CTkAppWidget::PositionFrom::PROGRAM);
      }
      else
        w->setPositionFrom(CTkAppWidget::PositionFrom::NONE);
    }
    else
      return tk_->wrongNumArgs("wm positionfrom window ?user|program?");
  }
  else if (arg == "protocol") {
    if      (numArgs == 2)
      setStringResult(""); // TODO
    else if (numArgs == 3 || numArgs == 4) {
      auto atomName = tk_->variantToString(args[2]);

      if (numArgs == 4) {
        auto atomValue = tk_->variantToString(args[3]);

        w->setWmAtomValue(atomName, atomValue);
      }
      else
        tk_->setStringResult(w->getWmAtomValue(atomName));
    }
    else
      return tk_->wrongNumArgs("wm protocol window ?name? ?command?");
  }
  else if (arg == "resizable") {
    if      (numArgs == 2) {
      bool wf, hf;
      w->isResizable(wf, hf);
      tk_->setStringResult(QString("%1 %2").arg(wf ? "1" : "0").arg(hf ? "1" : "0"));
    }
    else if (numArgs == 4) {
      bool wf, hf;
      if (! tk_->variantToBool(args[2], wf))
        return tk_->invalidBool(args[2]);
      if (! tk_->variantToBool(args[3], hf))
        return tk_->invalidBool(args[3]);

      w->setResizable(wf, hf);
    }
    else
      return tk_->wrongNumArgs("wm resizable window ?width height?");
  }
  else if (arg == "sizefrom") {
    if      (numArgs == 2) {
      if      (w->sizeFrom() == CTkAppWidget::SizeFrom::USER)
        setStringResult("user");
      else if (w->sizeFrom() == CTkAppWidget::SizeFrom::PROGRAM)
        setStringResult("program");
      else
        setStringResult("");
    }
    else if (numArgs == 3) {
      if (tk_->variantIsValid(args[2])) {
        static auto optionNames = std::vector<QString>({"user", "program"});

        QString arg1;
        if (! tk_->lookupName("argument", optionNames, args[2], arg1))
          return false;

        if (arg1 == "user")
          w->setSizeFrom(CTkAppWidget::SizeFrom::USER);
        else
          w->setSizeFrom(CTkAppWidget::SizeFrom::PROGRAM);
      }
      else
        w->setSizeFrom(CTkAppWidget::SizeFrom::NONE);
    }
    else
      return tk_->wrongNumArgs("wm sizefrom window ?user|program?");
  }
  else if (arg == "stackorder") {
    if      (numArgs == 2) {
      auto name1 = tk_->variantToString(args[1]);

      setStringResult(name1); // TODO
    }
    else if (numArgs == 4) {
      static auto optionNames = std::vector<QString>({"isabove", "isbelow"});

      QString arg1;
      if (! tk_->lookupName("argument", optionNames, args[2], arg1))
        return false;

      auto *w = tk_->lookupWidgetByName(args[3]);
      if (! w) return tk_->throwError(tk_->msg() + "bad window path name \"" + args[3] + "\"");

      setStringResult("0");
    }
    else
      return tk_->wrongNumArgs("wm stackorder window ?isabove|isbelow window?");
  }
  else if (arg == "state") {
    if (numArgs < 2 || numArgs > 3)
      return tk_->wrongNumArgs("wm state window ?arg ...?");

    auto *qwidget = w->getQWidget();
    if (! qwidget) return false;

    if (numArgs == 2) {
      if      (qwidget->isMinimized())
        setStringResult("iconic");
      else if (! qwidget->isVisible())
        setStringResult("withdrawn");
      else
        setStringResult("normal");
    }
    else {
      auto state = tk_->variantToString(args[2]);

      if      (state == "normal") {
        qwidget->showNormal();
      }
      else if (state == "iconic") {
        qwidget->showMinimized();
      }
      else if (state == "withdrawn") {
        w->hide();
      }
      else
        return tk_->throwError("bad argument \"" + state + "\": must be "
                               "normal, iconic, or withdrawn");
    }
  }
  else if (arg == "title") {
    if      (numArgs == 2) {
      const auto &title = w->getTitle();

      setStringResult(title);
    }
    else if (numArgs == 3) {
      auto title = tk_->variantToString(args[2]);

      w->setTitle(title);
    }
    else
      return tk_->wrongNumArgs("wm title window ?newTitle?");
  }
  else if (arg == "transient") {
    if      (numArgs == 2) {
      auto fullName = w->wmTransientFor();

      setStringResult(fullName);
    }
    else if (numArgs == 3) {
      if (tk_->variantIsValid(args[2])) {
        auto *pw = tk_->lookupWidgetByName(args[2]);
        if (! pw) return tk_->throwError(tk_->msg() + "bad window path name \"" + args[2] + "\"");

        w->setWmTransientFor(pw->getFullName());
      }
      else
        w->setWmTransientFor("");
    }
    else
      return tk_->wrongNumArgs("wm transient window ?window?");
  }
  else if (arg == "withdraw") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("wm withdraw window");

    w->hide();
  }

  return true;
}

//---

CTkAppRootCommand::
CTkAppRootCommand(CTkApp *tk) :
 CTkAppOptsCommand(tk, ".")
{
  static CTkAppOpt opts[] = {
    {"-backgroundimage"    , "backgroundImage"    , "BackgroundImage"    , ""         },
    {"-borderwidth"        , "borderWidth"        , "BorderWidth"        , "0"        },
    {"-class"              , "class"              , "Class"              , "Toplevel" },
    {"-menu"               , "menu"               , "Menu"               , ""         },
    {"-relief"             , "relief"             , "Relief"             , "flat"     },
    {"-screen"             , "screen"             , "Screen"             , ""         },
    {"-use"                , "use"                , "Use"                , ""         },
    {"-background"         , "background"         , "Background"         , "#d9d9d9"  },
    {"-colormap"           , "colormap"           , "Colormap"           , ""         },
    {"-container"          , "container"          , "Container"          , "0"        },
    {"-cursor"             , "cursor"             , "Cursor"             , ""         },
    {"-height"             , "height"             , "Height"             , "0"        },
    {"-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"  },
    {"-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"  },
    {"-highlightthickness" , "highlightThickness" , "HighlightThickness" , "0"        },
    {"-padx"               , "padX"               , "Pad"                , "0"        },
    {"-pady"               , "padY"               , "Pad"                , "0"        },
    {"-takefocus"          , "takeFocus"          , "TakeFocus"          , "0"        },
    {"-tile"               , "tile"               , "Tile"               , "0"        },
    {"-visual"             , "visual"             , "Visual"             , ""         },
    {"-width"              , "width"              , "Width"              , "0"        },

    CTkAppOpt::optSynonym("-bd"   , "-borderwidth"    ),
    CTkAppOpt::optSynonym("-bg"   , "-background"     ),
    CTkAppOpt::optSynonym("-bgimg", "-backgroundimage"),

    CTkAppOpt::optEnd()
  };

  opts_.init(opts);
}

bool
CTkAppRootCommand::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(". option ?arg arg ...?");

  static auto optionNames = std::vector<QString>({
    "cget", "configure"});

  QString arg;
  if (! tk_->lookupOptionName(optionNames, args[0], arg))
    return false;

  // set config name/value
  if      (arg == "configure" || arg == "config") {
    return execConfigure(args);
  }
  // get config name/value
  else if (arg == "cget") {
    return execCGet(args);
  }

  return true;
}

bool
CTkAppRootCommand::
setOptValue(const QString &name, const QVariant &value)
{
  const CTkAppOpt *opt;

  if (! opts_.setOptValue(name, value, &opt))
    return false;

  if (! root()->execConfig(opt->name(), value))
    return false;

  return true;
}

//---

#ifdef CTK_APP_TKPATH
bool
CTkAppMatrixCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs(getName() + " opt ?-option value ...?");

  auto opt = tk_->variantToString(args[0]);

  auto mname = tk_->newMatrixName();

  CTkApp::MatrixData mdata;

  if      (opt == "rotate") {
    if (numArgs < 4)
      return tk_->wrongNumArgs(getName() + " rotate a x y");

    double a, tx, ty;
    if (! tk_->variantToReal(args[1], a))
      return tk_->invalidReal(args[1]);
    if (! tk_->variantToReal(args[2], tx))
      return tk_->invalidReal(args[2]);
    if (! tk_->variantToReal(args[3], ty))
      return tk_->invalidReal(args[3]);

    CMatrix2D m1, m2, m3;

    m1.setTranslation(-tx, -ty);
    m2.setRotation   (a);
    m3.setTranslation( tx,  ty);

    auto mm = m3*m2*m1;

    mdata.transform = tk_->toQTransform(mm);
  }
  else if (opt == "scale") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getName() + " scale sx sy");

    double sx, sy;
    if (! tk_->variantToReal(args[1], sx))
      return tk_->invalidReal(args[1]);
    if (! tk_->variantToReal(args[2], sy))
      return tk_->invalidReal(args[2]);

    CMatrix2D m;

    m.setScale(sx, sy);

    mdata.transform = tk_->toQTransform(m);
  }
  else if (opt == "skewx") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " skew x");

    double a;
    if (! tk_->variantToReal(args[1], a))
      return tk_->invalidReal(args[1]);

    auto m = CMatrix2D::skewX(a);

    mdata.transform = tk_->toQTransform(m);
  }
  else if (opt == "skewy") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " skew y");

    double a;
    if (! tk_->variantToReal(args[1], a))
      return tk_->invalidReal(args[1]);

    auto m = CMatrix2D::skewY(a);

    mdata.transform = tk_->toQTransform(m);
  }
  else if (opt == "translate") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getName() + " translate tx ty");

    double tx, ty;
    if (! tk_->variantToReal(args[1], tx))
      return tk_->invalidReal(args[1]);
    if (! tk_->variantToReal(args[2], ty))
      return tk_->invalidReal(args[2]);

    CMatrix2D m;

    m.translate(tx, ty);

    mdata.transform = tk_->toQTransform(m);
  }
  else
    tk_->TODO(args);

  tk_->setNamedMatrix(mname, mdata);

  tk_->setStringResult(mname);

  return true;
}

bool
CTkAppGradientStopStyleCmd::
run(const Args &args)
{
  tk_->TODO(args);

  return true;
}
#endif

bool
CTkAppErrorInfoCmd::
run(const Args &)
{
  tk_->lastErrorInfo();

  return true;
}

//---

CTkAppCommand::
CTkAppCommand(CTkApp *tk, const QString &name) :
 CTclAppCommand(tk, name), tk_(tk)
{
}

CTkAppRoot *
CTkAppCommand::
root() const
{
  return tk_->root();
}

bool
CTkAppCommand::
proc(int objc, Tcl_Obj * const *objv)
{
  tk_->setCurrentCommand(getName());

  assert(objc > 0);

  arg0_ = CQTclUtil::variantFromObj(tk_->getInterp(), objv[0]).toString();

  Args args;

  for (int i = 1; i < objc; ++i) {
    auto var = CQTclUtil::variantFromObj(tk_->getInterp(), objv[i]);

    args.push_back(var);
  }

  auto rc = run(args);

  tk_->setCurrentCommand("");

  return rc;
}

//---

CTkAppOptsCommand::
CTkAppOptsCommand(CTkApp *tk, const QString &name) :
 CTkAppCommand(tk, name), opts_(tk)
{
}

bool
CTkAppOptsCommand::
getNameValue(const Args &args, QString &name, QVariant &value)
{
  uint numArgs = args.size();

  if (numArgs != 2)
    return tk_->wrongNumArgs(getName() + " cget option");

  name = tk_->variantToString(args[1]);

  if (! getOptValue(name, value))
    return tk_->unknownOption(name);

  return true;
}

bool
CTkAppOptsCommand::
execCGet(const Args &args)
{
  QString name; QVariant value;
  if (! getNameValue(args, name, value))
    return false;

  setResult(value);

  return true;
}

bool
CTkAppOptsCommand::
execConfigure(const Args &args)
{
  uint numArgs = args.size();

  // get all options
  if      (numArgs == 1) {
    auto var = opts_.getOptsVar();

    tk_->setResult(var);
  }
  // get single option
  else if (numArgs == 2) {
    auto name = tk_->variantToString(args[1]);

    QVariant var;
    if (! opts_.getOptVar(name, var))
      return tk_->unknownOption(name);

    tk_->setResult(var);
  }
  // set option
  else {
    uint i = 1;

    while (i < numArgs) {
      auto name = tk_->variantToString(args[i]);

      if (i + 1 >= numArgs)
        return tk_->throwError("value for \"" + name + "\" missing");

      if (! setOptValue(name, args[i + 1])) {
        //return tk_->throwError("unknown config option \"" + name + "\"");
        return false;
      }

      i += 2;
    }
  }

  return true;
}

bool
CTkAppOptsCommand::
getOptValue(const QString &name, QVariant &value)
{
  if (! opts_.getOptValue(name, value))
    return false;

  return true;
}

bool
CTkAppOptsCommand::
setOptValue(const QString &name, const QVariant &value)
{
  const CTkAppOpt *opt;

  if (! opts_.setOptValue(name, value, &opt))
    return tk_->unknownOption(name);

  return true;
}

//---

CTkWindowDestroyEventLoop::
CTkWindowDestroyEventLoop(CTclApp *app, QWidget *w) :
 QObject(), app_(app), w_(w)
{
  eventLoop_ = new QEventLoop;

  connect(w_, SIGNAL(destroyed(QObject *)), this, SLOT(destroySlot()));
}

CTkWindowDestroyEventLoop::
~CTkWindowDestroyEventLoop()
{
  delete eventLoop_;
}

void
CTkWindowDestroyEventLoop::
startLoop()
{
  eventLoop_->exec();
}

void
CTkWindowDestroyEventLoop::
destroySlot()
{
  eventLoop_->quit();
}

//---

CTkWindowVisibleEventLoop::
CTkWindowVisibleEventLoop(CTclApp *app, QWidget *w) :
 QObject(), app_(app), w_(w)
{
  eventLoop_ = new QEventLoop;

  w_->installEventFilter(this);
}

CTkWindowVisibleEventLoop::
~CTkWindowVisibleEventLoop()
{
  delete eventLoop_;
}

void
CTkWindowVisibleEventLoop::
startLoop()
{
  eventLoop_->exec();
}

bool
CTkWindowVisibleEventLoop::
eventFilter(QObject *obj, QEvent *event)
{
  switch (event->type()) {
    case QEvent::Show:
    case QEvent::Hide: {
      eventLoop_->quit();
      break;
    }
    default:
      break;
  }

  // standard event processing
  return QObject::eventFilter(obj, event);
}

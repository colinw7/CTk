#include <CTkAppCommands.h>
#include <CTkApp.h>
#include <CTkAppWidget.h>
#include <CTkAppPackLayout.h>
#include <CTkAppPlaceLayout.h>
#include <CTkAppGridLayout.h>
#include <CTkAppPackLayout.h>
#include <CTkAppImage.h>
#include <CTkAppFont.h>
#include <CTkAppOptionValue.h>
#include <CTkAppOptData.h>
#include <CTkAppUtil.h>

#include <CQTclUtil.h>

#include <CEncode64.h>
#include <CMatrix2D.h>
#include <CScreenUnits.h>

#include <QApplication>
#include <QClipboard>
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
//CTkAppCommandDef("tk_chooseDirectory"  , CTkAppTkChooseDirectoryCmd)
//CTkAppCommandDef("tk_dialog"           , CTkAppTkDialogCmd)
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

//---

class CTkAppRootCommand :  public CTkAppCommand {
 public:
  CTkAppRootCommand(CTkApp *tk);

  bool run(const Args &args) override;

 private:
  CTkAppOptData opts_;
};

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

//new CTkAppTkErrorCmd      (tk);
  new CTkAppTkGetOpenFileCmd(tk);
  new CTkAppTkGetSaveFileCmd(tk);
  new CTkAppTkMessageBoxCmd (tk);
  new CTkAppTkPopupCmd      (tk);
  new CTkAppTkWaitCmd       (tk);

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

  for (uint i = 1; i < numArgs; ++i) {
    auto &arg = args[i];

    if      (arg == "-displayof") {
      tk_->TODO(args);

      ++i;

      if (i >= numArgs)
        return tk_->wrongNumArgs("bell ?-displayof window? ?-nice?");
    }
    else if (arg == "-nice") {
      tk_->TODO(args);
    }
    else
      return tk_->throwError("bad option \"" + arg.toString() + "\": must be "
                             "-displayof, or -nice");
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
  auto name = args[0].toString();

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
    auto pattern = args[1].toString();

    CTkAppEventData data;

    if (! tk_->parseEvent(pattern, data))
      return tk_->throwError("bad event pattern \"" + pattern + "\"");

    if (numArgs > 2) {
      data.command = args[2].toString();

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

  auto widgetName = args[0].toString();

  auto *w = tk_->lookupWidgetByName(widgetName, /*quiet*/true);
  if (! w) return tk_->throwError("bad window path name \"" + widgetName + "\"");

  if (numArgs > 1) {
    std::vector<QString> bindtags;
    for (uint i = 1; i < numArgs; ++i)
      bindtags.push_back(args[i].toString());
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
    { "-bd"                 , "-borderwidth"       , nullptr              , nullptr         },
    { "-bg"                 , "-background"        , nullptr              , nullptr         },
    { "-bitmap"             , "bitmap"             , "Bitmap"             , ""              },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
    { "-command"            , "command"            , "Command"            , ""              },
    { "-compound"           , "compound"           , "Compound"           , "none"          },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-default"            , "default"            , "Default"            , "disabled"      },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"       },
    { "-fg"                 , "-foreground"        , nullptr              , nullptr         },
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
    { nullptr               , nullptr              , nullptr              , nullptr         }
  };

  QString widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *button = new CTkAppButton(tk_, parent, childName);

  if (! tk_->useStyle())
    button->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, button, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppCanvasCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-background"         , "background"         , "Background"         , "#d9d9d9" },
    { "-bg"                 , "-background"         , nullptr              , nullptr  },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "0"       },
    { "-bd"                 , "-borderwidth"        , nullptr              , nullptr  },
    { "-closeenough"        , "closeEnough"        , "CloseEnough"        , "1"       },
    { "-confine"            , "confine"            , "Confine"            , "1"       },
    { "-cursor"             , "cursor"             , "Cursor"             , ""        },
    { "-height"             , "height"             , "Height"             , "7c"      },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9" },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000" },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "1"       },
    { "-insertbackground"   , "insertBackground"   , "Foreground"         , "#000000" },
    { "-insertborderwidth"  , "insertBorderWidth"  , "BorderWidth"        , "0"       },
    { "-insertofftime"      , "insertOffTime"      , "OffTime"            , "300"     },
    { "-insertontime"       , "insertOnTime"       , "OnTime"             , "600"     },
    { "-insertwidth"        , "insertWidth"        , "InsertWidth"        , "2"       },
    { "-offset"             , "offset"             , "Offset"             , "0,0"     },
    { "-relief"             , "relief"             , "Relief"             , "flat"    },
    { "-scrollregion"       , "scrollRegion"       , "ScrollRegion"       , ""        },
    { "-selectbackground"   , "selectBackground"   , "Foreground"         , "#c3c3c3" },
    { "-selectborderwidth"  , "selectBorderWidth"  , "BorderWidth"        , "1"       },
    { "-selectforeground"   , "selectForeground"   , "Background"         , "#000000" },
    { "-state"              , "state"              , "State"              , "normal"  },
    { "-takefocus"          , "takeFocus"          , "TakeFocus"          , ""        },
    { "-width"              , "width"              , "Width"              , "10c"     },
    { "-xscrollcommand"     , "xScrollCommand"     , "ScrollCommand"      , ""        },
    { "-xscrollincrement"   , "xScrollIncrement"   , "ScrollIncrement"    , "0"       },
    { "-yscrollcommand"     , "yScrollCommand"     , "ScrollCommand"      , ""        },
    { "-yscrollincrement"   , "yScrollIncrement"   , "ScrollIncrement"    , "0"       },
    { nullptr               , nullptr              , nullptr              , nullptr   }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *canvas = new CTkAppCanvas(tk_, parent, childName);

  if (! tk_->useStyle())
    canvas->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, canvas, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppCheckButtonCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-activebackground"   , "activeBackground"   , "Foreground"         , "#ececec"       },
    { "-activeforeground"   , "activeForeground"   , "Background"         , "#000000"       },
    { "-anchor"             , "anchor"             , "Anchor"             , "center"        },
    { "-background"         , "background"         , "Background"         , "#d9d9d9"       },
    { "-bd"                 , "-borderwidth"       , nullptr              , nullptr         },
    { "-bg"                 , "-background"        , nullptr              , nullptr         },
    { "-bitmap"             , "bitmap"             , "Bitmap"             , ""              },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
    { "-command"            , "command"            , "Command"            , ""              },
    { "-compound"           , "compound"           , "Compound"           , "none"          },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"       },
    { "-fg"                 , "-foreground"        , nullptr              , nullptr         },
    { "-font"               , "font"               , "Font"               , "TkDefaultFont" },
    { "-foreground"         , "foreground"         , "Foreground"         , "#000000"       },
    { "-height"             , "height"             , "Height"             , "0"             },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"       },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"       },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "1"             },
    { "-image"              , "image"              , "Image"              , ""              },
    { "-indicatoron"        , "indicatorOn"        , "IndicatorOn"        , "1"             },
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
    { nullptr               , nullptr              , nullptr              , nullptr         }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *check = new CTkAppCheckButton(tk_, parent, childName);

  if (! tk_->useStyle())
    check->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, check, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

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
    { "-bd"                 , "-borderwidth"       , nullptr              , nullptr      },
    { "-bg"                 , "-background"        , nullptr              , nullptr      },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"          },
    { "-cursor"             , "cursor"             , "Cursor"             , "xterm"      },
    { "-disabledbackground" , "disabledBackground" , "DisabledBackground" , "#d9d9d9"    },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"    },
    { "-exportselection"    , "exportSelection"    , "ExportSelection"    , "1"          },
    { "-fg"                 , "-foreground"        , nullptr              , nullptr      },
    { "-font"               , "font"               , "Font"               , "TkTextFont" },
    { "-foreground"         , "foreground"         , "Foreground"         , "#000000"    },
    { "-height"             , "height"             , "Height"             , "0"       },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"    },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"    },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "1"          },
    { "-insertbackground"   , "insertBackground"   , "Foreground"         , "#000000"    },
    { "-insertborderwidth"  , "insertBorderWidth"  , "BorderWidth"        , "0"          },
    { "-insertofftime"      , "insertOffTime"      , "OffTime"            , "300"        },
    { "-insertontime"       , "insertOnTime"       , "OnTime"             , "600"        },
    { "-insertwidth"        , "insertWidth"        , "InsertWidth"        , "2"          },
    { "-invalidcommand"     , "invalidCommand"     , "InvalidCommand"     , ""           },
    { "-invcmd"             , "-invalidcommand"    , nullptr              , nullptr      },
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
    { "-vcmd"               , "-validatecommand"   , nullptr              , nullptr      },
    { "-width"              , "width"              , "Width"              , "20"         },
    { "-xscrollcommand"     , "xScrollCommand"     , "ScrollCommand"      , ""           },
    { nullptr               , nullptr              , nullptr              , nullptr      }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *check = new CTkAppComboBox(tk_, parent, childName);

  if (! tk_->useStyle())
    check->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, check, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

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
  if (! tk_->lookupOptionName(optionNames, args[0].toString(), opt))
    return false;

  //---

  auto *clipboard = QApplication::clipboard();

  if      (opt == "clear") {
    // -displayof window
    clipboard->setText("", QClipboard::Clipboard);
  }
  else if (opt == "append") {
    // -option value ... data
    if (numArgs < 2)
      return tk_->wrongNumArgs("clipboard append ?-option value ...? data");

    clipboard->setText(clipboard->text(QClipboard::Clipboard) + args[1].toString());
  }
  else if (opt == "get") {
    setStringResult(clipboard->text(QClipboard::Clipboard));
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
    auto widgetName = args[i].toString();

    auto *w = tk_->lookupWidgetByName(widgetName, /*quiet*/true);
    if (! w) continue;

    w->deleteLater();
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
  if (! tk_->lookupOptionName(optionNames, args[0].toString(), arg))
    return false;

  if      (arg == "add") {
    if (numArgs < 3)
      return tk_->wrongNumArgs("event add virtual sequence ?sequence ...?");

    CTkAppVirtualEventData vdata;
    if (! tk_->stringToVirtualEvent(args[1].toString(), vdata))
      return false;

    CTkAppEventData edata;
    if (! tk_->parseEvent(args[2].toString(), edata))
      (void) tk_->throwError("bad event pattern \"" + args[2].toString() + "\"");

    tk_->addVirtualEventData(vdata, edata);
  }
  else if (arg == "delete") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("event delete virtual ?sequence ...?");

    tk_->TODO(args);
  }
  else if (arg == "generate") {
    if (numArgs < 3)
      return tk_->wrongNumArgs("event generate window event ?-option value ...?");

    auto *w = tk_->lookupWidgetByName(args[1].toString());
    if (! w) return false;

    CTkAppEventData data;
    if (! tk_->parseEvent(args[2].toString(), data))
      (void) tk_->throwError("bad event pattern \"" + args[2].toString() + "\"");

    w->processEvents(nullptr, data);
  }
  else if (arg == "info") {
    if      (numArgs == 1) {
      std::vector<QString> names;
      tk_->virtualEventNames(names);

      tk_->setStringArrayResult(names);
    }
    else if (numArgs == 2) {
      CTkAppVirtualEventData vdata;
      if (! tk_->stringToVirtualEvent(args[1].toString(), vdata))
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
    { "-background"         , "background"         , "Background"         , "#ffffff"    },
    { "-bd"                 , "-borderwidth"       , nullptr              , nullptr      },
    { "-bg"                 , "-background"        , nullptr              , nullptr      },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"          },
    { "-cursor"             , "cursor"             , "Cursor"             , "xterm"      },
    { "-disabledbackground" , "disabledBackground" , "DisabledBackground" , "#d9d9d9"    },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"    },
    { "-exportselection"    , "exportSelection"    , "ExportSelection"    , "1"          },
    { "-fg"                 , "-foreground"        , nullptr              , nullptr      },
    { "-font"               , "font"               , "Font"               , "TkTextFont" },
    { "-foreground"         , "foreground"         , "Foreground"         , "#000000"    },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"    },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"    },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "1"          },
    { "-insertbackground"   , "insertBackground"   , "Foreground"         , "#000000"    },
    { "-insertborderwidth"  , "insertBorderWidth"  , "BorderWidth"        , "0"          },
    { "-insertofftime"      , "insertOffTime"      , "OffTime"            , "300"        },
    { "-insertontime"       , "insertOnTime"       , "OnTime"             , "600"        },
    { "-insertwidth"        , "insertWidth"        , "InsertWidth"        , "2"          },
    { "-invalidcommand"     , "invalidCommand"     , "InvalidCommand"     , ""           },
    { "-invcmd"             , "-invalidcommand"    , nullptr              , nullptr      },
    { "-justify"            , "justify"            , "Justify"            , "left"       },
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
    { "-vcmd"               , "-validatecommand"   , nullptr              , nullptr      },
    { "-width"              , "width"              , "Width"              , "20"         },
    { "-xscrollcommand"     , "xScrollCommand"     , "ScrollCommand"      , ""           },
    { nullptr               , nullptr              , nullptr              , nullptr      }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *entry = new CTkAppEntry(tk_, parent, childName);

  if (! tk_->useStyle())
    entry->setBackground(QColor("#ffffff"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, entry, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

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
    auto widgetName = args[0].toString();

    auto *w = tk_->lookupWidgetByName(widgetName);
    if (! w) return false;

    w->setFocus();
  }
  else {
    uint i = 1;

    for ( ; i < numArgs; ++i) {
      auto arg = args[i].toString();

      if (arg[0] != '-')
        break;

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

    auto widgetName = args[i].toString();

    auto *w = tk_->lookupWidgetByName(widgetName);
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
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  auto processFontOptions = [&](CTkAppFontRef &font, uint i) {
    for ( ; i < numArgs; ++i) {
      if      (args[i] == "-family") {
        ++i; if (i >= numArgs) return false;
        font->setFamily(args[i].toString());
      }
      else if (args[i] == "-size") {
        ++i; if (i >= numArgs) return false;
        double size;
        if (! CTkAppUtil::stringToReal(args[2].toString(), size))
          return tk_->throwError("Bad size");
        if (size >= 0)
          font->setPointSize(size);
        else
          font->setPixelSize(size);
      }
      else if (args[i] == "-weight") {
        ++i; if (i >= numArgs) return false;
        if      (args[i] == "normal")
          font->setNormal();
        else if (args[i] == "bold")
          font->setBold();
        else
          return false;
      }
      else if (args[i] == "-slant") {
        ++i; if (i >= numArgs) return false;
        if      (args[i] == "roman")
          font->setItalic(false);
        else if (args[i] == "italic")
          font->setItalic(true);
        else
          return false;
      }
      else if (args[i] == "-underline") {
        ++i; if (i >= numArgs) return false;
        bool b;
        if (! CTkAppUtil::stringToBool(args[i].toString(), b))
          return tk_->throwError("Invalid bool '" + args[i].toString() + "'");
        font->setUnderline(b);
      }
      else if (args[i] == "-overstrike") {
        ++i; if (i >= numArgs) return false;
        bool b;
        if (! CTkAppUtil::stringToBool(args[i].toString(), b))
          return tk_->throwError("Invalid bool '" + args[i].toString() + "'");
        font->setOverstrike(b);
      }
      else {
        return tk_->throwError("bad option \"" + args[i].toString() + "\": must be "
          "-family, -size, -weight, -slant, -underline, or -overstrike");
      }
    }

    return true;
  };

  if (numArgs == 0)
    return tk_->wrongNumArgs("font option ?arg?");

  static auto optionNames = std::vector<QString>({
    "actual", "configure", "create", "delete", "families", "measure", "metrics", "names"});

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0].toString(), option))
    return false;

  if      (option == "actual") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("font actual font ?-displayof window? ?option? ?--? ?char?");

    auto name = args[1].toString();

    auto font = tk_->getFont(name);

    QFont qfont;

    if (font)
      qfont = font->getQFont();
    else
      CTkAppUtil::stringToQFont(name, qfont);

    CTkApp::FontData fontData;
    tk_->getFontData(qfont, fontData);

     uint i = 2;

    if (i < numArgs && args[i] == "-displayof")
      i += 2;

    if (i >= numArgs) {
      setStringResult("-family {"     + fontData.family + "}" +
                      " -size "       + QString::number(fontData.size) +
                      " -weight "     + fontData.weight +
                      " -slant "      + fontData.slant +
                      " -underline "  + (fontData.underline ? "1" : "0") +
                      " -overstrike " + (fontData.overstrike ? "1" : "0"));
      return true;
    }

    for ( ; i < numArgs; ++i) {
      if      (args[i] == "-family"    ) setStringResult(fontData.family);
      else if (args[i] == "-size"      ) setRealResult  (fontData.size);
      else if (args[i] == "-weight"    ) setStringResult(fontData.weight);
      else if (args[i] == "-slant"     ) setStringResult(fontData.slant);
      else if (args[i] == "-underline" ) setBoolResult  (fontData.underline);
      else if (args[i] == "-overstrike") setBoolResult  (fontData.overstrike);
      else return false;
    }
  }
  else if (option == "configure") {
    if (numArgs < 3)
      return tk_->wrongNumArgs("font configure fontname ?-option value ...?");

    auto name = args[1].toString();

    auto font = tk_->getFont(name);

    if (font)
      return tk_->throwError("invalid font \"" + name + "\"");

    processFontOptions(font, 2);
  }
  else if (option == "create") {
    QString name;

    if (numArgs > 1)
      name = args[1].toString();

    if (name == "")
      name = tk_->getNewFontName();

    auto font = tk_->getFont(name);

    if (font)
      return tk_->throwError("named font \"" + name + "\" already exists");

    font = tk_->createFont(name);

    processFontOptions(font, 2);

    setStringResult(name);
  }
  else if (option == "delete") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("font delete fontname ?fontname ...?");

    uint i = 1;

    for ( ; i < numArgs; ++i) {
      auto font = tk_->getFont(args[i].toString());

      if (! font)
        return tk_->throwError("named font \"" + args[i].toString() +
                               "\" doesn't exist");

      tk_->deleteFont(font);
    }
  }
  else if (option == "families") {
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
    if (numArgs < 3)
      return tk_->wrongNumArgs("font measure font ?-displayof window? text");

    auto qfont = tk_->getQFont(args[1].toString());

    uint i = 2;

    if (i < numArgs && args[i] == "-displayof")
      i += 2;

    if (i >= numArgs)
      return tk_->wrongNumArgs("font measure font ?-displayof window? text");

    auto text = args[i].toString();

    QFontMetrics fm(qfont);

    auto w = fm.horizontalAdvance(text);

    setIntegerResult(w);
  }
  else if (option == "metrics") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("font metrics font ?-displayof window? ?option?");

    auto qfont = tk_->getQFont(args[1].toString());

    uint i = 2;

    if (i < numArgs && args[i] == "-displayof")
      i += 2;

    CTkApp::FontData fontData;
    tk_->getFontData(qfont, fontData);

    if (i >= numArgs) {
      setStringResult("-ascent "     + QString::number(fontData.ascent) +
                      " -descent "   + QString::number(fontData.descent) +
                      " -linespace " + QString::number(fontData.linespace) +
                      " -fixed "     + (fontData.fixed ? "1" : "0"));
      return true;
    }

    for (uint i = 2; i < numArgs; ++i) {
      if      (args[i] == "-ascent"   ) setRealResult(fontData.ascent);
      else if (args[i] == "-descent"  ) setRealResult(fontData.descent);
      else if (args[i] == "-linespace") setRealResult(fontData.linespace);
      else if (args[i] == "-fixed"    ) setBoolResult(fontData.fixed);
      else return false;
    }
  }
  else if (option == "names") {
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
    { "-bd"                 , "-borderwidth"       , nullptr              , nullptr   },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "0"       },
    { "-class"              , "class"              , "Class"              , "Frame"   },
    { "-relief"             , "relief"             , "Relief"             , "flat"    },
    { "-background"         , "background"         , "Background"         , "#d9d9d9" },
    { "-bg"                 , "-background"        , nullptr              , nullptr   },
    { "-colormap"           , "colormap"           , "Colormap"           , ""        },
    { "-container"          , "container"          , "Container"          , "0"       },
    { "-cursor"             , "cursor"             , "Cursor"             , ""        },
    { "-height"             , "height"             , "Height"             , "0"       },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9" },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000" },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "0"       },
    { "-padx"               , "padX"               , "Pad"                , "0"       },
    { "-pady"               , "padY"               , "Pad"                , "0"       },
    { "-takefocus"          , "takeFocus"          , "TakeFocus"          , "0"       },
    { "-visual"             , "visual"             , "Visual"             , ""        },
    { "-width"              , "width"              , "Width"              , "0"       },
    { nullptr               , nullptr              , nullptr              , nullptr   }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *frame = new CTkAppFrame(tk_, parent, childName);

  if (! tk_->useStyle())
    frame->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, frame, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

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
      auto *w = tk_->lookupWidgetByName(args[1].toString());
      if (! w) return false;

      setStringResult("."); // current grab
    }
  }
  else if (args[0] == "release") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("grab release window");

    auto *w = tk_->lookupWidgetByName(args[1].toString());
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

    auto *w = tk_->lookupWidgetByName(args[i].toString());
    if (! w) return false;
  }
  else if (args[0] == "status") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("grab status window");

    auto *w = tk_->lookupWidgetByName(args[1].toString());
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
      w = tk_->lookupWidgetByName(args[i].toString(), /*quiet*/true);

    if (! w)
     return tk_->throwError("bad option \"" + args[i].toString() + "\": must be "
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
    { "-padx"      , CTkAppOptionType::Int   , nullptr },
    { "-pady"      , CTkAppOptionType::Int   , nullptr },
    { "-row"       , CTkAppOptionType::Int   , nullptr },
    { "-rowspan"   , CTkAppOptionType::Int   , nullptr },
    { "-sticky"    , CTkAppOptionType::String, nullptr },
    { nullptr      , CTkAppOptionType::None  , nullptr },
  };

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("grid option arg ?arg ...?");

  bool configure = false;

  static auto optionNames = std::vector<QString>({
    "anchor", "bbox", "columnconfigure", "configure", "content", "forget",
    "info", "location", "propagate", "rowconfigure", "remove", "size", "slaves" });

  QString option;
  (void) tk_->lookupName("option", optionNames, args[0].toString(),
                         option, /*quiet*/true);

  if      (option == "anchor") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs("grid anchor window ?anchor?");

    auto *window = tk_->lookupWidgetByName(args[1].toString());
    if (! window) return false;

    if (numArgs == 2) {
      auto anchor = window->getGridAnchor();

      setStringResult(CTkAppUtil::alignToString(anchor));
    }
    else {
      Qt::Alignment anchor;
      if (! CTkAppUtil::stringToAlign(args[2].toString(), anchor))
        return tk_->throwError("bad anchor \"" + args[2].toString() + "\": must be "
                               "n, ne, e, se, s, sw, w, nw, or center");

      window->setGridAnchor(anchor);
    }
  }
  else if (option == "bbox") {
    if (numArgs != 2 && numArgs != 4 && numArgs != 6)
      return tk_->wrongNumArgs("grid bbox master ?column row? ?column2 row2?");

    auto *window = tk_->lookupWidgetByName(args[1].toString());
    if (! window) return false;

    if      (numArgs == 2) {
      int column1, row1, column2, row2;
      window->getGridBBox(column1, row1, column2, row2);
      setIntegerArrayResult({ column1, row1, column2, row2 });
    }
    else if (numArgs == 4) {
      long column, row;
      if (! CTkAppUtil::stringToInt(args[2].toString(), column) ||
          ! CTkAppUtil::stringToInt(args[3].toString(), row))
        return tk_->throwError("Bad column/row");
      window->setGridBBox(column, row, column, row);
    }
    else if (numArgs == 4) {
      long column1, row1, column2, row2;
      if (! CTkAppUtil::stringToInt(args[2].toString(), column1) ||
          ! CTkAppUtil::stringToInt(args[3].toString(), row1) ||
          ! CTkAppUtil::stringToInt(args[4].toString(), column2) ||
          ! CTkAppUtil::stringToInt(args[5].toString(), row2))
        return tk_->throwError("Bad column/row");
      window->setGridBBox(column1, row1, column2, row2);
    }
  }
  else if (option == "columnconfigure") {
    if (numArgs < 3)
      return tk_->wrongNumArgs("grid columnconfigure master index ?-option value...?");

    auto masterName = args[1].toString();

    auto *master = tk_->lookupWidgetByName(masterName);
    if (! master) return false;

    auto *layout = master->getTkGridLayout();
    if (! layout) return tk_->throwError("no grid layout for \"" + option + "\"");

    long index;
    if (! CTkAppUtil::stringToInt(args[2].toString(), index))
      return tk_->throwError("Bad index");

    for (uint i = 3; i < numArgs - 1; i += 2) {
      auto name  = args[i    ].toString();
      auto value = args[i + 1].toString();

      if      (name == "-minsize") {
        double size;
        if (! CTkAppUtil::stringToDistance(value, size))
          return tk_->throwError("Invalid width \"" + value + "\"");

        layout->setColumnMinSize(index, size);
      }
      else if (name == "-pad") {
        long pad;

        if (! CTkAppUtil::stringToInt(value, pad))
          return tk_->throwError("expected integer but got \"" + value + "\"");

        layout->setColumnPad(index, pad);
      }
      else if (name == "-uniform") {
        layout->setColumnUniform(index, value);
      }
      else if (name == "-weight") {
        long weight;

        if (! CTkAppUtil::stringToInt(value, weight))
          return tk_->throwError("expected integer but got \"" + value + "\"");

        layout->setColumnWeight(index, weight);
      }
      else
        return tk_->throwError("bad option \"" + name + "\": must be "
                               "-minsize, -pad, -uniform, or -weight");
    }
  }
  else if (option == "configure") {
    configure = true;
  }
  else if (option == "forget") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("grid forget slave ?slave ...?");

    uint i = 2;

    for ( ; i < numArgs; ++i) {
      auto *child = tk_->lookupWidgetByName(args[i].toString());
      if (! child) return false;

      auto *layout = child->getParent()->getTkGridLayout();
      if (! layout) return tk_->throwError("no grid layout for \"" + option + "\"");

      (void) layout->removeWidget(child);
    }
  }
  else if (option == "info") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("grid info window");

    auto window = args[1].toString();

    auto *child = tk_->lookupWidgetByName(window);
    if (! child) return false;

    auto *layout = child->getParent()->getTkGridLayout();
    if (! layout) return tk_->throwError("no grid layout for \"" + window + "\"");

    CTkAppGridLayout::Info info;
    layout->getChildInfo(child, info);

    auto res =
      QString("-in %1 -ipadx %2 -ipady %3 -padx %4 -pady %5").
       arg(info.getIn()).arg(info.getIPadX()).arg(info.getIPadY()).
       arg(info.getPadX()).arg(info.getPadY());

    setStringResult(res);
  }
  else if (option == "location") {
    if (numArgs != 4)
      return tk_->wrongNumArgs("grid location master x y");

    auto *master = tk_->lookupWidgetByName(args[1].toString());
    if (! master) return false;

    double x, y;
    if (! CTkAppUtil::stringToDistance(args[2].toString(), x) ||
        ! CTkAppUtil::stringToDistance(args[3].toString(), y))
      return tk_->throwError("bad screen distance \"" + args[2].toString() +
                             "\" or \"" + args[3].toString() + "\"");

    tk_->TODO(args);
  }
  else if (option == "propagate") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs("grid propagate window ?boolean?");

    auto *window = tk_->lookupWidgetByName(args[1].toString());
    if (! window) return false;

    if (numArgs == 2) {
      setBoolResult(window->getGridPropagate());
    }
    else {
      bool b;
      if (! CTkAppUtil::stringToBool(args[2].toString(), b))
        return tk_->throwError("Invalid bool '" + args[2].toString() + "'");
      window->setGridPropagate(b);
    }
  }
  else if (option == "rowconfigure") {
    if (numArgs < 3)
      return tk_->wrongNumArgs("grid rowconfigure master index ?-option value...?");

    auto masterName = args[1].toString();

    auto *master = tk_->lookupWidgetByName(masterName);
    if (! master) return false;

    auto *layout = master->getTkGridLayout();
    if (! layout) return tk_->throwError("no grid layout for \"" + option + "\"");

    long index;
    if (! CTkAppUtil::stringToInt(args[2].toString(), index))
      return tk_->throwError("Bad index");

    for (uint i = 3; i < numArgs - 1; i += 2) {
      auto name = args[i].toString();

      auto value = args[i + 1].toString();

      if      (name == "-minsize") {
        double size;
        if (! CTkAppUtil::stringToDistance(value, size))
          return tk_->throwError("Invalid width \"" + value + "\"");

        layout->setRowMinSize(index, size);
      }
      else if (name == "-pad") {
        long pad;

        if (! CTkAppUtil::stringToInt(value, pad))
          return tk_->throwError("expected integer but got \"" + value + "\"");

        layout->setRowPad(index, pad);
      }
      else if (name == "-uniform") {
        layout->setRowUniform(index, value);
      }
      else if (name == "-weight") {
        long weight;

        if (! CTkAppUtil::stringToInt(value, weight))
          return tk_->throwError("expected integer but got \"" + value + "\"");

        layout->setRowWeight(index, weight);
      }
      else
        return tk_->throwError("bad option \"" + name + "\": must be "
                               "-minsize, -pad, -uniform, or -weight");
    }
  }
  else if (option == "remove") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("grid remove window ?window ...?");

    auto *window = tk_->lookupWidgetByName(args[1].toString());
    if (! window) return tk_->throwError("Invalid window \"" +
                           args[1].toString() + "\"");

    auto *layout = window->getTkGridLayout();
    if (! layout) return tk_->throwError("no grid layout for \"" +
                           args[1].toString() + "\"");

    for (uint i = 2; i < numArgs; ++i) {
      auto *child = tk_->lookupWidgetByName(args[i].toString());
      if (! child) continue;

      layout->removeWidget(child);
    }
  }
  else if (option == "size") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("grid size window");

    auto *window = tk_->lookupWidgetByName(args[1].toString());
    if (! window) return tk_->throwError("Invalid window \"" +
                           args[1].toString() + "\"");

    auto *layout = window->getTkGridLayout();
    if (! layout) return tk_->throwError("no grid layout for \"" +
                           args[1].toString() + "\"");

    CTkAppGridLayout::Info info;
    layout->getChildInfo(window, info);

    setIntegerArrayResult({ info.getColSpan(), info.getRowSpan()});
  }
  else if (option == "slaves" || option == "content") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("grid " + option + " window ?-option value ...?");

    auto *w = tk_->lookupWidgetByName(args[1].toString());
    if (! w) return false;

    auto *layout = w->getTkGridLayout();
    if (! layout) return tk_->throwError("no grid layout for \"" +
                           args[1].toString() + "\"");

    auto widgets = layout->getWidgets();

    long row = -1, col = -1;

    for (uint i = 2; i < numArgs; ++i) {
      if      (args[i] == "-row") {
        if (i + 1 < numArgs) return false;

        if (! CTkAppUtil::stringToInt(args[i].toString(), row))
          return tk_->throwError("bad row \"" + args[i].toString() + "\"");
      }
      else if (args[i] == "-column") {
        if (i + 1 < numArgs) return false;

        if (! CTkAppUtil::stringToInt(args[i].toString(), col))
          return tk_->throwError("bad column \"" + args[i].toString() + "\"");
      }
      else
        return false;
    }

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
    CTkAppGridLayout::WidgetDatas widgetDatas;

    CTkAppOptionValueMap optValues;

    for (uint i = 0; i < numArgs; ++i) {
      auto arg = args[i].toString();
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
          tk_->throwError("Unhandled option '" + arg + "'");
      }
      else {
        CTkAppWidget* parent1;
        QString       childName;

        if (! root()->decodeWidgetName(arg, &parent1, childName))
          return tk_->throwError("Invalid widget name '" + arg + "'");

        auto *child = parent1->getChild(childName);

        if (child) {
          if (parent == nullptr)
            parent = parent1;

          widgetDatas.emplace_back(child);
        }
      }
    }

    CTkAppGridLayout::Info info;

    {
    auto p = optValues.find("-column");
    if (p != optValues.end()) info.setCol((*p).second.getInteger());
    }

    {
    auto p = optValues.find("-columnspan");
    if (p != optValues.end()) info.setColSpan((*p).second.getInteger());
    }

    {
    auto p = optValues.find("-in");
    if (p != optValues.end()) info.setIn((*p).second.getString());
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
    if (p != optValues.end()) info.setPadX((*p).second.getInteger());
    }

    {
    auto p = optValues.find("-pady");
    if (p != optValues.end()) info.setPadY((*p).second.getInteger());
    }

    {
    auto p = optValues.find("-row");
    if (p != optValues.end()) info.setRow((*p).second.getInteger());
    }

    {
    auto p = optValues.find("-rowspan");
    if (p != optValues.end()) info.setRowSpan((*p).second.getInteger());
    }

    {
    auto p = optValues.find("-sticky");
    if (p != optValues.end()) info.setSticky((*p).second.getString());
    }

    if (! parent)
      return false;

    auto *layout = parent->getTkGridLayout();
    if (! layout) return tk_->throwError("no grid layout for \"" + option + "\"");

    layout->addWidgets(widgetDatas, info);

    layout->invalidate();

    if (parent->isTopLevel()) {
      auto *topWidget = qobject_cast<CTkAppTopLevel *>(parent);

      if (topWidget)
        topWidget->setNeedsShow(true);
    }
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

  static auto optionNames = std::vector<QString>({
    "create", "delete", "height", "inuse", "names", "type", "types", "width"});

  QString name;
  if (! tk_->lookupOptionName(optionNames, args[0].toString(), name))
    return false;

  if      (name == "create") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("image create type ?name? ?-option value ...?");

    auto type = args[1].toString();

    if (type != "photo" && type != "bitmap" && type != "svg")
      return tk_->throwError("image type \"" + type + "\" doesn't exist");

    long        width { -1 }, height { -1 };
    QString     name, format, filename, background, foreground;
    std::string data;

    for (uint i = 2; i < numArgs; ++i) {
      auto arg = args[i].toString();

      if      (arg == "-file") {
        ++i;

        if (i >= numArgs)
          return tk_->throwError("value for \"" + arg + "\" missing");

        filename = args[i].toString();
      }
      else if (arg == "-data") {
        ++i;

        if (i >= numArgs)
          return tk_->throwError("value for \"" + arg + "\" missing");

        data = args[i].toString().toStdString();

        if (type == "photo") {
          std::string dataStr;
          for (const auto &c : data) {
            if (std::isspace(c)) continue;
            dataStr += c;
          }
//        std::cerr << dataStr << "\n";
          data = CEncode64Inst->decode(dataStr); // don't use QString
        }
      }
      else if (arg == "-format") {
        ++i;

        if (i >= numArgs)
          return tk_->throwError("value for \"" + arg + "\" missing");

        format = args[i].toString();
      }
      else if (arg == "-background") {
        ++i;

        if (i >= numArgs)
          return tk_->throwError("value for \"" + arg + "\" missing");

        background = args[i].toString();
      }
      else if (arg == "-foreground") {
        ++i;

        if (i >= numArgs)
          return tk_->throwError("value for \"" + arg + "\" missing");

        foreground = args[i].toString();
      }
      else if (arg == "-width") {
        ++i;

        if (i >= numArgs)
          return tk_->throwError("value for \"" + arg + "\" missing");

        if (! CTkAppUtil::stringToInt(args[i].toString(), width))
          return tk_->throwError("bad image width \"" + args[i].toString() + "\"");
      }
      else if (arg == "-height") {
        ++i;

        if (i >= numArgs)
          return tk_->throwError("value for \"" + arg + "\" missing");

        if (! CTkAppUtil::stringToInt(args[i].toString(), height))
          return tk_->throwError("bad image height \"" + args[i].toString() + "\"");
      }
      else if (arg[0] == '-') {
        return tk_->throwError("unknown image create option \"" + arg + "\"");
      }
      else {
        name = arg;
      }
    }

    if (name == "")
      name = tk_->getNewImageName();

    auto image = tk_->createImage(type, format, name, width, height);

    if      (filename != "") {
      if (format == "svg")
        image->loadSVG(filename);
      else
        image->loadFile(filename);
    }
    else if (data != "") {
      if (type == "bitmap" && format == "")
        format = "xbm";

      if (format == "xbm")
        image->loadXBM(name, data);
      else
        image->loadData(name, format, data);
    }

    auto *cmd = new CTkAppImageCommand(tk_, name);
    assert(cmd);

    setStringResult(name);
  }
  else if (name == "delete") {
    for (uint i = 1; i < numArgs; ++i) {
      auto image = tk_->getImage(args[i].toString());
      if (! image) return false;

      tk_->deleteImage(image);
    }
  }
  else if (name == "height") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("image height name");

    auto image = tk_->getImage(args[1].toString());
    if (! image) return false;

    tk_->setIntegerResult(image->height());
  }
  else if (name == "inuse") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("image inuse name");

    auto image = tk_->getImage(args[1].toString());
    if (! image) return false;

    setBoolResult(image->numRefs() > 0 ? true : false);
  }
  else if (name == "names") {
    std::vector<QString> names;
    tk_->getImageNames(names);
    setStringArrayResult(names);
  }
  else if (name == "type") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("image type name");

    auto image = tk_->getImage(args[1].toString());
    if (! image) return false;

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

    auto image = tk_->getImage(args[1].toString());
    if (! image) return false;

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
    { "-bd"                 , "-borderwidth"       , nullptr              , nullptr         },
    { "-bg"                 , "-background"        , nullptr              , nullptr         },
    { "-bitmap"             , "bitmap"             , "Bitmap"             , ""              },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
    { "-compound"           , "compound"           , "Compound"           , "none"          },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"       },
    { "-fg"                 , "-foreground"        , nullptr              , nullptr         },
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
    { nullptr               , nullptr              , nullptr              , nullptr         }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *label = new CTkAppLabel(tk_, parent, childName);

  if (! tk_->useStyle())
    label->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, label, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppLabelFrameCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-bd"                 , "-borderwidth"       , nullptr              , nullptr         },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "2"             },
    { "-class"              , "class"              , "Class"              , "Labelframe"    },
    { "-fg"                 , "-foreground"        , nullptr              , nullptr         },
    { "-font"               , "font"               , "Font"               , "TkDefaultFont" },
    { "-foreground"         , "foreground"         , "Foreground"         , "#000000"       },
    { "-labelanchor"        , "labelAnchor"        , "LabelAnchor"        , "nw"            },
    { "-labelwidget"        , "labelWidget"        , "LabelWidget"        , ""              },
    { "-relief"             , "relief"             , "Relief"             , "groove"        },
    { "-text"               , "text"               , "Text"               , ""              },
    { "-background"         , "background"         , "Background"         , "#d9d9d9"       },
    { "-bg"                 , "-background"        , nullptr              , nullptr         },
    { "-colormap"           , "colormap"           , "Colormap"           , ""              },
    { "-container"          , "container"          , "Container"          , "0"             },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-height"             , "height"             , "Height"             , "0"             },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"       },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"       },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "0"             },
    { "-padx"               , "padX"               , "Pad"                , "0"             },
    { "-pady"               , "padY"               , "Pad"                , "0"             },
    { "-takefocus"          , "takeFocus"          , "TakeFocus"          , "0"             },
    { "-visual"             , "visual"             , "Visual"             , ""              },
    { "-width"              , "width"              , "Width"              , "0"             },
    { nullptr               , nullptr              , nullptr              , nullptr         }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *frame = new CTkAppLabelFrame(tk_, parent, childName);

  if (! tk_->useStyle())
    frame->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, frame, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppListBoxCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-activestyle"        , "activeStyle"        , "ActiveStyle"        , "dotbox"        },
    { "-background"         , "background"         , "Background"         , "#ffffff"       },
    { "-bd"                 , "-borderwidth"       , nullptr              , nullptr         },
    { "-bg"                 , "-background"        , nullptr              , nullptr         },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"       },
    { "-exportselection"    , "exportSelection"    , "ExportSelection"    , "1"             },
    { "-fg"                 , "-foreground"        , nullptr              , nullptr         },
    { "-font"               , "font"               , "Font"               , "TkDefaultFont" },
    { "-foreground"         , "foreground"         , "Foreground"         , "#000000"       },
    { "-height"             , "height"             , "Height"             , "10"            },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"       },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"       },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "1"             },
    { "-relief"             , "relief"             , "Relief"             , "sunken"        },
    { "-selectbackground"   , "selectBackground"   , "Foreground"         , "#c3c3c3"       },
    { "-selectborderwidth"  , "selectBorderWidth"  , "BorderWidth"        , "0"             },
    { "-selectforeground"   , "selectForeground"   , "Background"         , "#000000"       },
    { "-selectmode"         , "selectMode"         , "SelectMode"         , "browse"        },
    { "-setgrid"            , "setGrid"            , "SetGrid"            , "0"             },
    { "-state"              , "state"              , "State"              , "normal"        },
    { "-takefocus"          , "takeFocus"          , "TakeFocus"          , ""              },
    { "-width"              , "width"              , "Width"              , "20"            },
    { "-xscrollcommand"     , "xScrollCommand"     , "ScrollCommand"      , ""              },
    { "-yscrollcommand"     , "yScrollCommand"     , "ScrollCommand"      , ""              },
    { "-listvariable"       , "listVariable"       , "Variable"           , ""              },
    { nullptr               , nullptr              , nullptr              , nullptr         }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *list = new CTkAppListBox(tk_, parent, childName);

  if (! tk_->useStyle())
    list->setBackground(QColor("#ffffff"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, list, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

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

  auto widgetName = args[0].toString();

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *child = parent->getChild(childName);

  if (child == nullptr)
    return tk_->throwError("bad window path name \"" + widgetName + "\"");

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
    { nullptr            , nullptr           , nullptr      , nullptr         }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *menu = new CTkAppMenu(tk_, parent, childName);

  if (! tk_->useStyle())
    menu->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, menu, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppMenuButtonCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-activebackground"   , "activeBackground"   , "Foreground"         , "#ececec"       },
    { "-activeforeground"   , "activeForeground"   , "Background"         , "#000000"       },
    { "-anchor"             , "anchor"             , "Anchor"             , "center"        },
    { "-background"         , "background"         , "Background"         , "#ffffff"       },
    { "-bd"                 , "-borderwidth"       , nullptr              , nullptr         },
    { "-bg"                 , "-background"        , nullptr              , nullptr         },
    { "-bitmap"             , "bitmap"             , "Bitmap"             , ""              },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-direction"          , "direction"          , "Direction"          , ""              },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"       },
    { "-fg"                 , "-foreground"        , nullptr              , nullptr         },
    { "-font"               , "font"               , "Font"               , "TkDefaultFont" },
    { "-foreground"         , "foreground"         , "Foreground"         , "#000000"       },
    { "-height"             , "height"             , "Height"             , "10"            },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"       },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"       },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "1"             },
    { "-image"              , "image"              , "Image"              , ""              },
    { "-indicatoron"        , "indicatorOn"        , "IndicatorOn"        , "1"             },
    { "-justify"            , "justify"            , "Justify"            , "center"        },
    { "-menu"               , "menu"               , "Menu"               , ""              },
    { "-padx"               , "padX"               , "Pad"                , "1"             },
    { "-pady"               , "padY"               , "Pad"                , "1"             },
    { "-relief"             , "relief"             , "Relief"             , "sunken"        },
    { "-compound"           , "compound"           , "Compound"           , "none"          },
    { "-state"              , "state"              , "State"              , "normal"        },
    { "-takefocus"          , "takeFocus"          , "TakeFocus"          , ""              },
    { "-text"               , "text"               , "Text"               , ""              },
    { "-textvariable"       , "textVariable"       , "Variable"           , ""              },
    { "-underline"          , "underline"          , "Underline"          , "-1"            },
    { "-width"              , "width"              , "Width"              , "20"            },
    { "-wraplength"         , "wrapLength"         , "WrapLength"         , "0"             },
    { nullptr               , nullptr              , nullptr              , nullptr         }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *button = new CTkAppMenuButton(tk_, parent, childName);

  if (! tk_->useStyle())
    button->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, button, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

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
    { nullptr     , nullptr    , nullptr    , nullptr  }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *button = new CTkAppNoteBook(tk_, parent, childName);

  if (! tk_->useStyle())
    button->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, button, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

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
    { "-bd"                 , "-borderwidth"       , nullptr              , nullptr         },
    { "-bg"                 , "-background"        , nullptr              , nullptr         },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-fg"                 , "-foreground"        , nullptr              , nullptr         },
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
    { nullptr               , nullptr              , nullptr              , nullptr         }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *message = new CTkAppMessage(tk_, parent, childName);

  if (! tk_->useStyle())
    message->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, message, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

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

  auto cmd = args[0].toString();

  if      (cmd == "add") {
    if (numArgs != 3 && numArgs != 4)
      return tk_->wrongNumArgs("option add pattern value ?priority?");

    auto pattern = args[1].toString();
    auto value   = args[2].toString();

    QString priority;
    int     ipriority = -1;

    if (numArgs == 4) {
      priority = args[3].toString();

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
        if (! CTkAppUtil::stringToInt(priority, i) || i < 0 || i > 100)
          return tk_->throwError("bad priority level \"" + priority + "\": must be "
            "widgetDefault, startupFile, userDefault, interactive, or a number between 0 and 100");

        ipriority = i;
      }
    }

    tk_->addOption(pattern, value, ipriority);
  }
  else if (cmd == "clear") {
    if (numArgs != 1)
      return tk_->wrongNumArgs("option clear");

    tk_->clearOptions();
  }
  else if (cmd == "get") {
    if (numArgs != 4)
      return tk_->wrongNumArgs("option get window name class");

    auto windowName = args[1].toString();

    auto *window = tk_->lookupWidgetByName(windowName);
    if (! window) return false;

    auto optName  = args[2].toString();
    auto optClass = args[3].toString();

    QString optValue;
    if (! window->getOptionValue(optName, optClass, optValue))
      return false;

    setStringResult(optValue);
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

  QString option;
  (void) tk_->lookupName("option", optionNames, args[0].toString(),
                         option, /*quiet*/true);

  int ic = 0;

  if      (option == "configure") {
    configure = true;
    ++ic;
  }
  else if (option == "forget") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("pack forget slave ?slave ...?");

    for (uint i = 1; i < numArgs; ++i) {
      auto *child = tk_->lookupWidgetByName(args[i].toString());
      if (! child) return false;

      auto *layout = child->getParent()->getTkPackLayout();
      if (! layout) return tk_->throwError("no pack layout for \"" + option + "\"");

      layout->removeWidget(child);
    }
  }
  else if (option == "info") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("pack info window");

    auto arg = args[1].toString();

    CTkAppWidget* parent;
    QString       childName;

    if (! root()->decodeWidgetName(arg, &parent, childName))
      return tk_->throwError("Invalid widget name '" + arg + "'");

    auto *child = parent->getChild(childName);

    if (child == nullptr)
      return tk_->throwError("bad window path name \"" + arg + "\"");

    auto *layout = parent->getTkPackLayout();
    if (! layout) return tk_->throwError("no pack layout for \"" + arg + "\"");

    CTkAppPackLayout::Info info;
    layout->getChildInfo(child, info);

    auto res =
      QString("-in %1 -anchor %2 -expand %3 -fill %4 -ipadx %5 -ipady %6 "
              "-padx %7 -pady %8 -side %9").
        arg(".").arg("center").arg(int(info.isExpand())).arg(info.getFillStr()).
        arg(info.ipadX()).arg(info.ipadY()).arg(info.padX()).arg(info.padY()).
        arg(info.getSideStr());

    setStringResult(res);
  }
  else if (option == "propagate") {
    if (numArgs < 2 || numArgs > 3)
      return tk_->wrongNumArgs("pack option arg ?arg ...?");

    auto arg = args[1].toString();

    auto *w = tk_->lookupWidgetByName(arg);
    if (! w) return false;

    auto *layout = w->getTkPackLayout();
    if (! layout) return tk_->throwError("no pack layout for \"" + arg + "\"");

    if (numArgs == 2)
      setIntegerResult(layout->isPropagate());
    else {
      auto value = args[2].toString();

      bool b;
      if (! CTkAppUtil::stringToBool(value, b))
        return tk_->throwError("Invalid bool '" + value + "'");

      layout->setPropagate(b);
    }
  }
  else if (option == "slaves" || option == "content") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("pack " + option + " arg ?arg ...?");

    auto arg = args[1].toString();

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
    else
      tk_->TODO(args);
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

    for (uint i = ic; i < numArgs; ++i) {
      auto arg = args[i].toString();

      if (arg.size() > 0 && arg[0] == '-') {
        if (! tk_->processOption(opts, args, i, optValues))
          tk_->throwError("Unhandled option '" + arg + "'");
      }
      else {
        CTkAppWidget* parent1;
        QString       childName;

        if (! root()->decodeWidgetName(arg, &parent1, childName))
          return tk_->throwError("Invalid widget name '" + arg + "'");

        auto *child = parent1->getChild(childName);

        if (child) {
          if (parent == nullptr)
            parent = parent1;

          children.push_back(child);
        }
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

      if (root()->decodeWidgetName((*p).second.getString(), &parent1, childName)) {
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

    auto *layout = parent->getTkPackLayout();
    if (! layout) return tk_->throwError("no pack layout for \"" + option + "\"");

    if (side == Info::Side::NONE)
      side = Info::Side::TOP;

    Info info(side, fill, expand, padx, pady, ipadx, ipady);

    layout->addWidgets(children, info);

    layout->invalidate();

    if (parent->isTopLevel()) {
      auto *topWidget = qobject_cast<CTkAppTopLevel *>(parent);

      if (topWidget)
        topWidget->setNeedsShow(true);
    }
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
    { "-bd"          , "-borderwidth", nullptr       , nullptr      },
    { "-bg"          , "-background" , nullptr       , nullptr      },
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
    { nullptr        , nullptr       , nullptr       , nullptr      }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *pane = new CTkAppPanedWindow(tk_, parent, childName);

  if (! tk_->useStyle())
    pane->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, pane, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

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
  (void) tk_->lookupName("option", optionNames, args[0].toString(),
                         option, /*quiet*/true);

  auto getPlaceLayout = [&](const QString &name, CTkAppPlaceLayout* &layout,
                            CTkAppWidget* &child) {
    CTkAppWidget* parent;
    QString       childName;

    if (! root()->decodeWidgetName(name, &parent, childName))
      return tk_->throwError("Invalid widget name '" + name + "'");

    child = parent->getChild(childName);

    if (child == nullptr)
      return tk_->throwError("bad window path name \"" + name + "\"");

    layout = parent->getTkPlaceLayout();
    if (! layout) return tk_->throwError("no place layout for \"" + name + "\"");

    return true;
  };

  if      (option == "configure") {
    configure = true;
  }
  else if (option == "forget") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("place forget args");

    CTkAppPlaceLayout *layout;
    CTkAppWidget      *child;
    if (! getPlaceLayout(args[1].toString(), layout, child))
      return false;

    // Causes the placer to stop managing the geometry of window.
    // As a side effect of this command window will be unmapped so that it does not
    // appear on the screen. If window is not currently managed by the placer then
    // the command has no effect. This command returns an empty string.
    tk_->TODO(args);
  }
  else if (option == "info") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("place info window");

    CTkAppPlaceLayout *layout;
    CTkAppWidget      *child;
    if (! getPlaceLayout(args[1].toString(), layout, child))
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

    auto *w = tk_->lookupWidgetByName(args[1].toString());
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

    for (uint i = 0; i < numArgs; ++i) {
      auto arg = args[i].toString();

      if (arg.size() > 0 && arg[0] == '-') {
        if (! tk_->processOption(opts, args, i, optValues))
          tk_->throwError("Unhandled option '" + arg + "'");
      }
      else {
        CTkAppWidget* parent1;
        QString       childName;

        if (! root()->decodeWidgetName(arg, &parent1, childName))
          return tk_->throwError("Invalid widget name '" + arg + "'");

        auto *child = parent1->getChild(childName);

        if (child) {
          if (parent == nullptr)
            parent = parent1;

          children.push_back(child);
        }
      }
    }

    CTkAppPlaceLayout::Info info;

    {
    auto p = optValues.find("-anchor");
    if (p != optValues.end()) {
      Qt::Alignment anchor;
      if (! CTkAppUtil::stringToAlign((*p).second.getString(), anchor))
        return tk_->throwError("bad anchor \"" + (*p).second.getString() + "\": must be "
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
      auto arg = (*p).second.getString();
      double height;
      if (! CTkAppUtil::stringToDistance(arg, height))
        return tk_->throwError("Invalid number \"" + arg + "\"");
      info.setHeight(height);
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
      auto arg = (*p).second.getString();
      double width;
      if (! CTkAppUtil::stringToDistance(arg, width))
        return tk_->throwError("Invalid number \"" + arg + "\"");
      info.setWidth(width);
    }
    }

    {
    auto p = optValues.find("-x");
    if (p != optValues.end()) {
      auto arg = (*p).second.getString();
      double x;
      if (! CTkAppUtil::stringToDistance(arg, x))
        return tk_->throwError("Invalid number \"" + arg + "\"");
      info.setX(x);
    }
    }

    {
    auto p = optValues.find("-y");
    if (p != optValues.end()) {
      auto arg = (*p).second.getString();
      double y;
      if (! CTkAppUtil::stringToDistance(arg, y))
        return tk_->throwError("Invalid number \"" + arg + "\"");
      info.setY(y);
    }
    }

    auto *layout = parent->getTkPlaceLayout();
    if (! layout) return tk_->throwError("no place layout for \"" + option + "\"");

    layout->addWidgets(children, info);

    layout->invalidate();

    if (parent->isTopLevel()) {
      auto *topWidget = qobject_cast<CTkAppTopLevel *>(parent);

      if (topWidget)
        topWidget->setNeedsShow(true);
    }
  }

  return true;
}

bool
CTkAppRadioButtonCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-activebackground"   , "activeBackground"   , "Foreground"         , "#ececec"        },
    { "-activeforeground"   , "activeForeground"   , "Background"         , "#000000"        },
    { "-anchor"             , "anchor"             , "Anchor"             , "center"         },
    { "-background"         , "background"         , "Background"         , "#d9d9d9"        },
    { "-bd"                 , "-borderwidth"       , nullptr              , nullptr          },
    { "-bg"                 , "-background"        , nullptr              , nullptr          },
    { "-bitmap"             , "bitmap"             , "Bitmap"             , ""               },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"              },
    { "-command"            , "command"            , "Command"            , ""               },
    { "-compound"           , "compound"           , "Compound"           , "none"           },
    { "-cursor"             , "cursor"             , "Cursor"             , ""               },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"        },
    { "-fg"                 , "-foreground"        , nullptr              , nullptr          },
    { "-font"               , "font"               , "Font"               , "TkDefaultFont"  },
    { "-foreground"         , "foreground"         , "Foreground"         , "#000000"        },
    { "-height"             , "height"             , "Height"             , "0"              },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"        },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"        },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "1"              },
    { "-image"              , "image"              , "Image"              , ""               },
    { "-indicatoron"        , "indicatorOn"        , "IndicatorOn"        , "1"              },
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
    { nullptr               , nullptr              , nullptr              , nullptr          }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *radio = new CTkAppRadioButton(tk_, parent, childName);

  if (! tk_->useStyle())
    radio->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, radio, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

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

  auto widgetName = args[0].toString();

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *child = parent->getChild(childName);

  if (child == nullptr)
    return tk_->throwError("bad window path name \"" + widgetName + "\"");

  child->raise();

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
    { "-bd"                 , "-borderwidth"       , nullptr              , nullptr         },
    { "-bg"                 , "-background"        , nullptr              , nullptr         },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
    { "-command"            , "command"            , "Command"            , ""              },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-digits"             , "digits"             , "Digits"             , "0"             },
    { "-fg"                 , "-foreground"        , nullptr              , nullptr         },
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
    { nullptr               , nullptr              , nullptr              , nullptr         }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *scale = new CTkAppScale(tk_, parent, childName);

  if (! tk_->useStyle())
    scale->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, scale, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppScrollBarCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-activebackground"   , "activeBackground"   , "Foreground"         , "#ececec"  },
    { "-activerelief"       , "activeRelief"       , "Relief"             , "raised"   },
    { "-background"         , "background"         , "Background"         , "#d9d9d9"  },
    { "-bd"                 , "borderwidth"        , nullptr              , nullptr    },
    { "-bg"                 , "background"         , nullptr              , nullptr    },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"        },
    { "-command"            , "command"            , "Command"            , ""         },
    { "-cursor"             , "cursor"             , "Cursor"             , ""         },
    { "-elementborderwidth" , "elementBorderWidth" , "BorderWidth"        , "-1"       },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"  },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"  },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "0"        },
    { "-jump"               , "jump"               , "Jump"               , "0"        },
    { "-orient"             , "orient"             , "Orient"             , "vertical" },
    { "-relief"             , "relief"             , "Relief"             , "sunken"   },
    { "-repeatdelay"        , "repeatDelay"        , "RepeatDelay"        , "300"      },
    { "-repeatinterval"     , "repeatInterval"     , "RepeatInterval"     , "100"      },
    { "-takefocus"          , "takeFocus"          , "TakeFocus"          , ""         },
    { "-troughcolor"        , "troughColor"        , "Background"         , "#b3b3b3"  },
    { "-width"              , "width"              , "Width"              , "11"       },
    { nullptr               , nullptr              , nullptr              , nullptr    }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *scrollbar = new CTkAppScrollBar(tk_, parent, childName);

  if (! tk_->useStyle())
    scrollbar->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, scrollbar, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

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
    "append", "clear", "get"});

  QString opt;
  if (! tk_->lookupOptionName(optionNames, args[0].toString(), opt))
    return false;

  //---

  auto *clipboard = QApplication::clipboard();

  if      (opt == "clear") {
    // -displayof window
    clipboard->setText("", QClipboard::Selection);
  }
  else if (opt == "append") {
    // -option value ... data
    if (numArgs < 2)
      return tk_->wrongNumArgs("selection append ?-option value ...? data");

    clipboard->setText(clipboard->text(QClipboard::Selection) + args[1].toString());
  }
  else if (opt == "get") {
    setStringResult(clipboard->text(QClipboard::Selection));
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

  static CTkAppOpt opts[] = {
    { "-activebackground"   , "activeBackground"   , "Background"         , "#ececec"    },
    { "-background"         , "background"         , "Background"         , "#ffffff"    },
    { "-bd"                 , "-borderwidth"       , nullptr              , nullptr      },
    { "-bg"                 , "-background"        , nullptr              , nullptr      },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"          },
    { "-buttonbackground"   , "Button.background"  , "Background"         , "#d9d9d9"    },
    { "-buttoncursor"       , "Button.cursor"      , "Cursor"             , ""           },
    { "-buttondownrelief"   , "Button.relief"      , "Relief"             , "raised"     },
    { "-buttonuprelief"     , "Button.relief"      , "Relief"             , "raised"     },
    { "-command"            , "command"            , "Command"            , ""           },
    { "-cursor"             , "cursor"             , "Cursor"             , "xterm"      },
    { "-disabledbackground" , "disabledBackground" , "DisabledBackground" , "#d9d9d9"    },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"    },
    { "-exportselection"    , "exportSelection"    , "ExportSelection"    , "1"          },
    { "-fg"                 , "-foreground"        , nullptr              , nullptr      },
    { "-font"               , "font"               , "Font"               , "TkTextFont" },
    { "-foreground"         , "foreground"         , "Foreground"         , "#000000"    },
    { "-format"             , "format"             , "Format"             , ""           },
    { "-from"               , "from"               , "From"               , "0"          },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"    },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"    },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "1"          },
    { "-increment"          , "increment"          , "Increment"          , "1"          },
    { "-insertbackground"   , "insertBackground"   , "Foreground"         , "#000000"    },
    { "-insertborderwidth"  , "insertBorderWidth"  , "BorderWidth"        , "0"          },
    { "-insertofftime"      , "insertOffTime"      , "OffTime"            , "300"        },
    { "-insertontime"       , "insertOnTime"       , "OnTime"             , "600"        },
    { "-insertwidth"        , "insertWidth"        , "InsertWidth"        , "2"          },
    { "-invalidcommand"     , "invalidCommand"     , "InvalidCommand"     , ""           },
    { "-invcmd"             , "-invalidcommand"    , nullptr              , nullptr      },
    { "-justify"            , "justify"            , "Justify"            , "left"       },
    { "-relief"             , "relief"             , "Relief"             , "sunken"     },
    { "-readonlybackground" , "readonlyBackground" , "ReadonlyBackground" , "#d9d9d9"    },
    { "-repeatdelay"        , "repeatDelay"        , "RepeatDelay"        , "400"        },
    { "-repeatinterval"     , "repeatInterval"     , "RepeatInterval"     , "100"        },
    { "-selectbackground"   , "selectBackground"   , "Foreground"         , "#c3c3c3"    },
    { "-selectborderwidth"  , "selectBorderWidth"  , "BorderWidth"        , "0"          },
    { "-selectforeground"   , "selectForeground"   , "Background"         , "#000000"    },
    { "-state"              , "state"              , "State"              , "normal"     },
    { "-takefocus"          , "takeFocus"          , "TakeFocus"          , ""           },
    { "-textvariable"       , "textVariable"       , "Variable"           , ""           },
    { "-to"                 , "to"                 , "To"                 , "0"          },
    { "-validate"           , "validate"           , "Validate"           , "none"       },
    { "-validatecommand"    , "validateCommand"    , "ValidateCommand"    , ""           },
    { "-values"             , "values"             , "Values"             , ""           },
    { "-vcmd"               , "-validatecommand"   , nullptr              , nullptr      },
    { "-width"              , "width"              , "Width"              , "20"         },
    { "-wrap"               , "wrap"               , "Wrap"               , "0"          },
    { "-xscrollcommand"     , "xScrollCommand"     , "ScrollCommand"      , ""           },
    { nullptr               , nullptr              , nullptr              , nullptr      }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *spin = new CTkAppSpinBox(tk_, parent, childName);

  if (! tk_->useStyle())
    spin->setBackground(QColor("#ffffff"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, spin, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

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
   {"-bd"                      , "-borderwidth"            , nullptr              , nullptr      },
   {"-bg"                      , "-background"             , nullptr              , nullptr      },
   {"-blockcursor"             , "blockCursor"             , "BlockCursor"        , "0"          },
   {"-borderwidth"             , "borderWidth"             , "BorderWidth"        , "1"          },
   {"-cursor"                  , "cursor"                  , "Cursor"             , "xterm"      },
   {"-endline"                 , ""                        , ""                   , ""           },
   {"-exportselection"         , "exportSelection"         , "ExportSelection"    , "1"          },
   {"-fg"                      , "-foreground"             , nullptr              , nullptr      },
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
   {nullptr                    , nullptr                   , nullptr              , nullptr      }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *text = new CTkAppText(tk_, parent, childName);

  if (! tk_->useStyle())
    text->setBackground(QColor("#ffffff"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, text, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

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

  auto arg = args[0].toString();

  if      (arg == "appname") {
    if (numArgs == 1)
      tk_->setStringResult("CTkApp");
    else
      tk_->TODO(args);
  }
  else if (arg == "busy") {
    if (numArgs < 1)
      return tk_->wrongNumArgs("tk busy options ?arg arg ...?");

    auto option = args[1].toString();

    if      (option == "cget") {
      if (numArgs != 4)
        return tk_->wrongNumArgs("tk busy cget window option");

      auto *w = tk_->lookupWidgetByName(args[2].toString());
      if (! w) return false;

      tk_->TODO(args);
    }
    else if (option == "configure") {
      if (numArgs < 3)
        return tk_->wrongNumArgs("tk busy configure window ?option? ?value ...?");

      auto *w = tk_->lookupWidgetByName(args[2].toString());
      if (! w) return false;

      tk_->TODO(args);
    }
    else if (option == "current") {
      tk_->TODO(args);
    }
    else if (option == "forget") {
      if (numArgs != 3)
        return tk_->wrongNumArgs("tk busy forget window");

      auto *w = tk_->lookupWidgetByName(args[2].toString());
      if (! w) return false;

      tk_->TODO(args);
    }
    else if (option == "hold") {
      if (numArgs < 3)
        return tk_->wrongNumArgs("tk busy hold window ?option? ?value ...?");

      auto *w = tk_->lookupWidgetByName(args[2].toString());
      if (! w) return false;

      tk_->TODO(args);
    }
    else if (option == "status") {
      if (numArgs != 3)
        return tk_->wrongNumArgs("tk busy status window");

      auto *w = tk_->lookupWidgetByName(args[2].toString());
      if (! w) return false;

      tk_->setIntegerResult(0);
      tk_->TODO(args);
    }
    else
      return false;
  }
  else if (arg == "caret") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("tk caret window ?-x x? ?-y y? ?-height height?");

    auto *w = tk_->lookupWidgetByName(args[1].toString());
    if (! w) return false;

    if (numArgs == 2)
      tk_->setStringResult("-height 0 -x 0 -y 0");
    else {
      auto opt = args[2].toString();

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
    if (numArgs < 2)
      return tk_->wrongNumArgs("tk fontchooser subcommand ?arg ...?");

    auto subcommand = args[1].toString();

    if      (subcommand == "configure") {
      tk_->TODO(args);
    }
    else if (subcommand == "hide") {
      tk_->showFontDialog(false);
    }
    else if (subcommand == "show") {
      tk_->showFontDialog(true);
    }
    else
      return false;
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
      if (! CTkAppUtil::stringToReal(args[1].toString(), s))
        return false;

      tk_->setScaling(s);
    }
  }
  else if (arg == "useinputmethods") {
    uint i = 1;

    if (i < numArgs && args[i] == "-displayof")
      i += 2;

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

  uint numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      auto value = (i < numArgs - 1 ? args[++i] : "");

      if      (name == "-defaultextension") {
        tk_->TODO(name);
      }
      else if (name == "-filetypes") {
        tk_->TODO(name);
      }
      else if (name == "-initialfile") {
        tk_->TODO(name);
      }
      else
        tk_->TODO(name);
    }
    else
      tk_->TODO(name);
  }

  QWidget *parent = nullptr;

  QString              dir;
  QString              filter;
  QString*             selectedFilter = nullptr;
  QFileDialog::Options options;

  auto file = QFileDialog::getSaveFileName(parent, "Open File", dir, filter,
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
  if (! tk_->lookupOptionName(optionNames, args[0].toString(), arg))
    return false;

  auto name = args[1].toString();

  if      (arg == "variable") {
    auto *varProc = new CTkVarTraceEventLoop(tk_, name);

    tk_->traceVar(name, varProc);

    varProc->startLoop();

    delete varProc;
  }
  else if (arg == "visibility") {
    auto *w = tk_->lookupWidgetByName(name);
    if (! w) return false;

    auto *windowLoop = new CTkWindowVisibleEventLoop(tk_, w->getQWidget());

    windowLoop->startLoop();

    delete windowLoop;
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
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      bool hasValue = (i < numArgs - 1);
      auto value    = (hasValue ? args[++i].toString() : "");

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

  tk_->TODO("tk_popup");

  return true;
}

//---

bool
CTkAppTopLevelCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  static CTkAppOpt opts[] = {
    { "-bd"                 , "-borderwidth"       , nullptr              , nullptr    },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "0"        },
    { "-class"              , "class"              , "Class"              , "Toplevel" },
    { "-menu"               , "menu"               , "Menu"               , ""         },
    { "-relief"             , "relief"             , "Relief"             , "flat"     },
    { "-screen"             , "screen"             , "Screen"             , ""         },
    { "-use"                , "use"                , "Use"                , ""         },
    { "-background"         , "background"         , "Background"         , "#d9d9d9"  },
    { "-bg"                 , "-background"        , nullptr              , nullptr    },
    { "-colormap"           , "colormap"           , "Colormap"           , ""         },
    { "-container"          , "container"          , "Container"          , "0"        },
    { "-cursor"             , "cursor"             , "Cursor"             , ""         },
    { "-height"             , "height"             , "Height"             , "0"        },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"  },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"  },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "0"        },
    { "-padx"               , "padX"               , "Pad"                , "0"        },
    { "-pady"               , "padY"               , "Pad"                , "0"        },
    { "-takefocus"          , "takeFocus"          , "TakeFocus"          , "0"        },
    { "-visual"             , "visual"             , "Visual"             , ""         },
    { "-width"              , "width"              , "Width"              , "0"        },
    { nullptr               , nullptr              , nullptr              , nullptr    }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *toplevel = new CTkAppTopLevel(tk_, parent, childName);

  if (! tk_->useStyle())
    toplevel->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, toplevel, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

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
    { nullptr          , nullptr         , nullptr         , nullptr        }
  };

  uint numArgs = args.size();

  QString widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "")
    return tk_->throwError("No name");

  CTkAppWidget* parent = nullptr;
  QString       childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *toplevel = new CTkAppTreeView(tk_, parent, childName);

  if (! tk_->useStyle())
    toplevel->setBackground(QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, toplevel, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppUpdateCmd::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if      (numArgs == 0)
    tk_->processEvents();
  else if (numArgs == 1) {
    static auto optionNames = std::vector<QString>({
      "idletasks"});

    QString arg;
    if (! tk_->lookupOptionName(optionNames, args[0].toString(), arg))
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
  if (! tk_->lookupOptionName(optionNames, args[0].toString(), arg))
    return false;

  CTkAppWidget *w = nullptr;

  auto getWindow = [&](uint i=1, bool quiet=false) {
    if (numArgs < 2)
      return tk_->wrongNumArgs("winfo " + arg + " window");

    auto widgetName = args[i].toString();

    w = tk_->lookupWidgetByName(widgetName, quiet);

    return (w ? true : false);
  };

  //---

  if      (arg == "atom") {
    uint i = 1;

    if (i < numArgs && args[i] == "-displayof")
      i += 2;

    if (i != numArgs - 1)
      return tk_->wrongNumArgs("winfo atom ?-displayof window? name");

    // get atom id for name
    tk_->TODO(args);
  }
  else if (arg == "atomname") {
    uint i = 1;

    if (i < numArgs && args[i] == "-displayof")
      i += 2;

    if (i != numArgs - 1)
      return tk_->wrongNumArgs("winfo atomname ?-displayof window? id");

    // get name for atom id
    tk_->TODO(args);
  }
  else if (arg == "cells") {
    if (! getWindow()) return false;

    tk_->TODO(args); // window size in cells ?
  }
  else if (arg == "children") {
    if (! getWindow()) return false;

    std::vector<CTkAppWidget *> children;

    w->getChildren(children);

    uint numChildren = children.size();

    std::vector<QString> list;

    for (uint i = 0; i < numChildren; ++i) {
      auto *child = children[i];

      list.push_back(child->getFullName());
    }

    setStringArrayResult(list);
  }
  else if (arg == "class") {
    if (! getWindow()) return false;

    setStringResult(w->getClassName());
  }
  else if (arg == "colormapfull") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("winfo colormapfull window");

    tk_->TODO(args);
  }
  else if (arg == "containing") {
    uint i = 1;

    if (i < numArgs && args[i] == "-displayof")
      i += 2;

    if (i + 2 != numArgs)
      return tk_->wrongNumArgs("winfo containing ?-displayof window? rootX rootY");

    long x;
    if (! CTkAppUtil::stringToInt(args[i].toString(), x))
      return tk_->throwError("expected integer but got \"" +
                             args[i].toString() + "\"");

    ++i;

    long y;
    if (! CTkAppUtil::stringToInt(args[i].toString(), y))
      return tk_->throwError("expected integer but got \"" +
                             args[i].toString() + "\"");

    tk_->TODO(args);
  }
  else if (arg == "depth") {
    if (! getWindow()) return false;

    //tk_->TODO(args);
    setIntegerResult(24);
  }
  else if (arg == "exists") {
    setIntegerResult(getWindow(0, /*quiet*/true) ? 1 : 0);
  }
  else if (arg == "fpixels") {
    if (numArgs != 3)
      return tk_->wrongNumArgs("winfo fpixels window number");

    auto *w = tk_->lookupWidgetByName(args[1].toString());
    if (! w) return false;

    double size;
    if (! CTkAppUtil::stringToDistance(args[2].toString(), size))
      return tk_->throwError("Invalid number \"" + args[2].toString() + "\"");

    setRealResult(size);
  }
  else if (arg == "geometry") {
    if (! getWindow()) return false;

    auto r = w->getQWidget()->geometry();

    auto res = QString("%1x%2+%3+%4").arg(r.width()).arg(r.height()).arg(r.x()).arg(r.y());

    setStringResult(res);
  }
  else if (arg == "height") {
    if (! getWindow()) return false;

    setIntegerResult(w->getQWidget()->height());
  }
  else if (arg == "id") {
    if (! getWindow()) return false;

    setIntegerResult(long(w->getQWidget()));
  }
  else if (arg == "interps") {
    uint i = 1;

    if (i < numArgs && args[i] == "-displayof")
      i += 2;

    setStringResult("CTkApp");
  }
  else if (arg == "ismapped") {
    (void) getWindow();

    setIntegerResult(w ? w->getQWidget()->isVisible() : 0);
  }
  else if (arg == "manager") {
    if (! getWindow()) return false;

    auto *parent = w->getParent();
    auto *layout = (parent ? parent->getTkLayout() : nullptr);

    setStringResult(layout ? layout->name() : "");
  }
  else if (arg == "name") {
    if (! getWindow()) return false;

    setStringResult(w->getName());
  }
  else if (arg == "parent") {
    if (! getWindow()) return false;

    auto *parent = w->getParent();

    setStringResult(parent ? parent->getFullName() : "");
  }
  else if (arg == "pathname") {
    uint i = 1;

    if (i < numArgs && args[i] == "-displayof")
      i += 2;

    long id;
    if (! CTkAppUtil::stringToInt(args[i].toString(), id))
      return tk_->throwError("Invalid number \"" + args[i].toString() + "\"");

    tk_->TODO(args); // window id to widget
  }
  else if (arg == "pixels") {
    if (numArgs != 3)
      return tk_->wrongNumArgs("winfo fpixels window number");

    auto *w = tk_->lookupWidgetByName(args[1].toString());
    if (! w) return false;

    double size;
    if (! CTkAppUtil::stringToDistance(args[2].toString(), size))
      return tk_->throwError("Invalid number \"" + args[2].toString() + "\"");

    setIntegerResult(int(size));
  }
  else if (arg == "pointerx") {
    if (! getWindow()) return false;

    auto pos = QCursor::pos();
    setIntegerResult(pos.x());
  }
  else if (arg == "pointerxy") {
    if (! getWindow()) return false;

    auto pos = QCursor::pos();
    setIntegerArrayResult({ pos.x(), pos.y()});
  }
  else if (arg == "pointery") {
    if (! getWindow()) return false;

    auto pos = QCursor::pos();
    setIntegerResult(pos.y());
  }
  else if (arg == "reqheight") {
    if (! getWindow()) return false;

    setIntegerResult(w->getQWidget()->sizeHint().width());
  }
  else if (arg == "reqwidth") {
    if (! getWindow()) return false;

    setIntegerResult(w->getQWidget()->sizeHint().width());
  }
  else if (arg == "rgb") {
    if (numArgs != 3)
      return tk_->wrongNumArgs("winfo rgb window colorName");

    auto *w = tk_->lookupWidgetByName(args[1].toString());
    if (! w) return false;

    QColor c;
    if (! CTkAppUtil::stringToQColor(args[2].toString(), c))
      return false;

    tk_->setIntegerArrayResult({int(65535*c.redF()), int(65535*c.greenF()), int(65535*c.blueF())});
  }
  else if (arg == "rootx") {
    if (! getWindow()) return false;

    auto r = qApp->primaryScreen()->geometry();
    setIntegerResult(r.x());
  }
  else if (arg == "rooty") {
    if (! getWindow()) return false;

    auto r = qApp->primaryScreen()->geometry();
    setIntegerResult(r.y());
  }
  else if (arg == "screen") {
    if (! getWindow()) return false;

    tk_->TODO(args); // screen name
  }
  else if (arg == "screencells") {
    if (! getWindow()) return false;

    tk_->TODO(args); // window size in cells ?
  }
  else if (arg == "screendepth") {
    if (! getWindow()) return false;

    setIntegerResult(24);
  }
  else if (arg == "screenheight") {
    if (! getWindow()) return false;

    auto r = qApp->primaryScreen()->geometry();
    setIntegerResult(r.height());
  }
  else if (arg == "screenmmheight") {
    if (! getWindow()) return false;

    auto r = qApp->primaryScreen()->geometry();
    tk_->setRealResult(r.height()/CScreenUnitsMgrInst->mmSize());
  }
  else if (arg == "screenmmwidth") {
    if (! getWindow()) return false;

    auto r = qApp->primaryScreen()->geometry();
    tk_->setRealResult(r.width()/CScreenUnitsMgrInst->mmSize());
  }
  else if (arg == "screenvisual") {
    if (! getWindow()) return false;

    setStringResult("truecolor"); // TODO
  }
  else if (arg == "screenwidth") {
    if (! getWindow()) return false;

    auto r = qApp->primaryScreen()->geometry();
    setIntegerResult(r.width());
  }
  else if (arg == "server") {
    if (! getWindow()) return false;

    tk_->TODO(args); // x server name
  }
  else if (arg == "toplevel") {
    if (! getWindow()) return false;

    auto *toplevel = w->toplevel();
    setStringResult(toplevel ? toplevel->getFullName() : ".");
  }
  else if (arg == "viewable") {
    (void) getWindow();

    setIntegerResult(w ? w->getQWidget()->isVisible() : 0);
  }
  else if (arg == "visual") {
    if (! getWindow()) return false;

    setStringResult("truecolor"); // TODO
  }
  else if (arg == "visualid") {
    if (! getWindow()) return false;

    tk_->TODO(args); // screen visualid
  }
  else if (arg == "visualsavailable") {
    if (! getWindow()) return false;

    setStringResult("{truecolor 24}"); // TODO
  }
  else if (arg == "vrootheight") {
    if (! getWindow()) return false;

    auto r = qApp->primaryScreen()->geometry();
    setIntegerResult(r.height());
  }
  else if (arg == "vrootwidth") {
    if (! getWindow()) return false;

    auto r = qApp->primaryScreen()->geometry();
    setIntegerResult(r.width());
  }
  else if (arg == "vrootx") {
    if (! getWindow()) return false;

    auto r = qApp->primaryScreen()->geometry();
    setIntegerResult(r.x());
  }
  else if (arg == "vrooty") {
    if (! getWindow()) return false;

    auto r = qApp->primaryScreen()->geometry();
    setIntegerResult(r.y());
  }
  else if (arg == "width") {
    if (! getWindow()) return false;

    setIntegerResult(w ? w->getQWidget()->width() : 0);
  }
  else if (arg == "x") {
    if (! getWindow()) return false;

    setIntegerResult(w ? w->getQWidget()->x() : 0);
  }
  else if (arg == "y") {
    if (! getWindow()) return false;

    setIntegerResult(w ? w->getQWidget()->y() : 0);
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

  if (numArgs < 2)
    return tk_->wrongNumArgs("wm option window ?arg ...?");

  //---

  static auto optionNames = std::vector<QString>({
    "aspect", "attributes", "client", "colormapwindows", "command", "deiconify", "focusmodel",
    "forget", "frame", "geometry", "grid", "group", "iconbitmap", "iconify", "iconmask",
    "iconname", "iconphoto", "iconposition", "iconwindow", "manage", "maxsize", "minsize",
    "overrideredirect", "positionfrom", "protocol", "resizable", "sizefrom", "stackorder",
    "state", "title", "transient", "withdraw" });

  QString arg;
  if (! tk_->lookupOptionName(optionNames, args[0].toString(), arg))
    return false;

  //---

  auto name = args[1].toString();

  CTkAppWidget* parent;
  QString       childName;

  if (! root()->decodeWidgetName(name, &parent, childName))
    return tk_->throwError("Invalid widget name '" + name + "'");

  CTkAppWidget *w;

  if (childName != "") {
    auto *child = parent->getChild(childName);

    if (child == nullptr)
      return tk_->throwError("Invalid widget name '" + name + "'");

    w = child;
  }
  else
    w = parent;

  //---

  if      (arg == "aspect") {
    tk_->TODO(args);
  }
  else if (arg == "attributes") {
    if (numArgs == 2) {
      tk_->setStringResult("-alpha 1.0 -topmost 0 -zoomed 0 -fullscreen 0 -type {}");
    }
    else {
      auto opt = args[2].toString();

      if      (opt == "-alpha") {
        if (numArgs == 2)
          tk_->setStringResult("1.0");
      }
      else if (opt == "-topmost") {
        if (numArgs == 2)
          tk_->setStringResult("0");
      }
      else if (opt == "-zoomed") {
        if (numArgs == 2)
          tk_->setStringResult("0");
      }
      else if (opt == "-fullscreen") {
        if (numArgs == 2)
          tk_->setStringResult("0");
      }
      else if (opt == "-type") {
        if (numArgs == 2)
          tk_->setStringResult("");
      }
      else
        return tk_->throwError("bad attribute \"" + opt + "\": must be "
          "-alpha, -topmost, -zoomed, -fullscreen, or -type");

      tk_->TODO(args);
    }
  }
  else if (arg == "client") {
    tk_->TODO(args);
  }
  else if (arg == "colormapwindows") {
    tk_->TODO(args);
  }
  else if (arg == "command") {
    tk_->TODO(args);
  }
  else if (arg == "deiconify") {
    w->getQWidget()->show();
  }
  else if (arg == "focusmodel") {
    tk_->setStringResult("passive");
    tk_->TODO(args);
  }
  else if (arg == "forget") {
    tk_->TODO(args);
  }
  else if (arg == "frame") {
    tk_->TODO(args); // toplevel
  }
  else if (arg == "geometry") {
    if (numArgs == 3) {
      if (! w->setGeometry(args[2].toString()))
        return false;
    }
    else
      setStringResult(w->getGeometry());
  }
  else if (arg == "grid") {
    if      (numArgs == 2) {
      int w, h, iw, ih;
      tk_->getWmGrid(w, h, iw, ih);

      setIntegerArrayResult({w, h, iw, ih});
    }
    else if (numArgs == 6) {
      long w, h, iw, ih;
      if (! CTkAppUtil::stringToInt(args[2].toString(), w) ||
          ! CTkAppUtil::stringToInt(args[3].toString(), h) ||
          ! CTkAppUtil::stringToInt(args[4].toString(), iw) ||
          ! CTkAppUtil::stringToInt(args[5].toString(), ih))
        return false;

      tk_->setWmGrid(w, h, iw, ih);
    }
    else
      return tk_->wrongNumArgs("wm grid window ?baseWidth baseHeight widthInc heightInc?");
  }
  else if (arg == "group") {
    if (numArgs > 3)
      return tk_->wrongNumArgs("wm group window ?pathName?");

    if (numArgs > 2) {
      auto *w1 = tk_->lookupWidgetByName(args[2].toString());
      if (! w1) return false;

      tk_->setWmGroup(w, w1);
    }
    else {
      auto *w1 = tk_->getWmGroup(w);

      setStringResult(w1 ? w1->getName() : "");
    }
  }
  else if (arg == "iconbitmap") {
    tk_->TODO(args);
  }
  else if (arg == "iconify") {
    tk_->TODO(args);
  }
  else if (arg == "iconmask") {
    tk_->TODO(args);
  }
  else if (arg == "iconname") {
    if (numArgs == 3)
      w->setIcon(args[2].toString());
    else
      setStringResult(w->getIcon());
  }
  else if (arg == "iconphoto") {
    tk_->TODO(args);
  }
  else if (arg == "iconposition") {
    tk_->TODO(args);
  }
  else if (arg == "iconwindow") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs("wm iconwindow window ?icon_window?");

    if (numArgs == 2)
      setStringResult("");
    else {
      auto *iw = tk_->lookupWidgetByName(args[2].toString());
      if (! iw) return false;

      auto *toplevel = iw->toplevel();
      if (! toplevel) return false;

      toplevel->setIconWindow(name);

      toplevel->getQWidget()->hide();
    }
  }
  else if (arg == "manage") {
    tk_->TODO(args);
  }
  else if (arg == "maxsize") {
    if      (numArgs == 2) {
      auto size = w->getQWidget()->maximumSize();

      tk_->setIntegerArrayResult({size.width(), size.height()});
    }
    else if (numArgs != 4) {
      return tk_->wrongNumArgs("wm maxsize window ?width height?");
    }
    else {
      long sw, sh;

      if (! CTkAppUtil::stringToInt(args[2].toString(), sw))
        return tk_->throwError("expected integer but got \"" +
                                 args[2].toString() + "\"");

      if (! CTkAppUtil::stringToInt(args[3].toString(), sh))
        return tk_->throwError("expected integer but got \"" +
                                 args[3].toString() + "\"");

      sw = std::max(sw, 1L);
      sh = std::max(sh, 1L);

      w->getQWidget()->setMaximumSize(QSize(sw, sh));
    }
  }
  else if (arg == "minsize") {
    if      (numArgs == 2) {
      auto size = w->getQWidget()->minimumSize();

      tk_->setIntegerArrayResult({size.width(), size.height()});
    }
    else if (numArgs != 4) {
      return tk_->wrongNumArgs("wm minsize window ?width height?");
    }
    else {
      long sw, sh;

      if (! CTkAppUtil::stringToInt(args[2].toString(), sw))
        return tk_->throwError("expected integer but got \"" +
                                 args[2].toString() + "\"");

      if (! CTkAppUtil::stringToInt(args[3].toString(), sh))
        return tk_->throwError("expected integer but got \"" +
                                 args[3].toString() + "\"");

      sw = std::max(sw, 1L);
      sh = std::max(sh, 1L);

      w->getQWidget()->setMinimumSize(QSize(sw, sh));
    }
  }
  else if (arg == "overrideredirect") {
    if (numArgs != 3)
      return false;

    bool b;
    if (! CTkAppUtil::stringToBool(args[2].toString(), b))
      return tk_->throwError("Invalid overrideredirect value");

    if (b == 1)
      w->getQWidget()->setWindowFlags(
        Qt::Tool | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    else
      return false;
  }
  else if (arg == "positionfrom") {
    tk_->TODO(args);
  }
  else if (arg == "protocol") {
    QString atomName;

    if (numArgs > 2)
      atomName = args[2].toString();

    if (numArgs > 3)
      tk_->setWmAtomValue(atomName, args[3].toString());
    else
      tk_->setStringResult(tk_->getWmAtomValue(atomName));
  }
  else if (arg == "resizable") {
    if      (numArgs == 2) {
      tk_->setStringResult("1 1");
    }
    else if (numArgs == 4) {
      bool wf, hf;
      if (! CTkAppUtil::stringToBool(args[2].toString(), wf))
        return tk_->throwError("invalid bool \"" + args[2].toString() + "\"");
      if (! CTkAppUtil::stringToBool(args[3].toString(), hf))
        return tk_->throwError("invalid bool \"" + args[3].toString() + "\"");

      w->setResizable(wf, hf);
    }
    else
      return tk_->wrongNumArgs("wm resizable window ?width height?");
  }
  else if (arg == "sizefrom") {
    tk_->TODO(args);
  }
  else if (arg == "stackorder") {
    tk_->TODO(args);
  }
  else if (arg == "state") {
    if (numArgs < 2 || numArgs > 3)
      return tk_->wrongNumArgs("wm state window ?arg ...?");

    if (numArgs == 2) {
      if      (w->getQWidget()->isMinimized())
        setStringResult("iconic");
      else if (! w->getQWidget()->isVisible())
        setStringResult("withdrawn");
      else
        setStringResult("normal");
    }
    else {
      auto state = args[2].toString();

      if      (state == "normal") {
        w->getQWidget()->showNormal();
      }
      else if (state == "iconic") {
        w->getQWidget()->showMinimized();
      }
      else if (state == "withdraw") {
        w->getQWidget()->hide();
      }
      else
        return tk_->throwError("bad argument \"" + state + "\": must be "
                               "normal, iconic, or withdrawn");
    }
  }
  else if (arg == "title") {
    if (numArgs == 3) {
      auto title = args[2].toString();

      w->setTitle(title);
    }
    else {
      const auto &title = w->getTitle();

      setStringResult(title);
    }
  }
  else if (arg == "transient") {
    if (numArgs != 3)
      return false;

    auto *pw = tk_->lookupWidgetByName(args[2].toString());
    if (! pw) return false;

    w->getQWidget()->setWindowFlags(
      Qt::Tool | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);

    //CXMachine::setWMTransientFor();
  }
  else if (arg == "withdraw") {
    w->getQWidget()->hide();
  }

  return true;
}

//---

CTkAppRootCommand::
CTkAppRootCommand(CTkApp *tk) :
 CTkAppCommand(tk, "."), opts_(tk)
{
  static CTkAppOpt opts[] = {
    { "-bd"                 , "-borderwidth"       , nullptr              , nullptr    },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "0"        },
    { "-class"              , "class"              , "Class"              , "Toplevel" },
    { "-menu"               , "menu"               , "Menu"               , ""         },
    { "-relief"             , "relief"             , "Relief"             , "flat"     },
    { "-screen"             , "screen"             , "Screen"             , ""         },
    { "-use"                , "use"                , "Use"                , ""         },
    { "-background"         , "background"         , "Background"         , "#d9d9d9"  },
    { "-bg"                 , "-background"        , nullptr              , nullptr    },
    { "-colormap"           , "colormap"           , "Colormap"           , "",        },
    { "-container"          , "container"          , "Container"          , "0"        },
    { "-cursor"             , "cursor"             , "Cursor"             , ""         },
    { "-height"             , "height"             , "Height"             , "0"        },
    { "-highlightbackground", "highlightBackground", "HighlightBackground", "#d9d9d9"  },
    { "-highlightcolor"     , "highlightColor"     , "HighlightColor"     , "#000000"  },
    { "-highlightthickness" , "highlightThickness" , "HighlightThickness" , "0"        },
    { "-padx"               , "padX"               , "Pad"                , "0"        },
    { "-pady"               , "padY"               , "Pad"                , "0"        },
    { "-takefocus"          , "takeFocus"          , "TakeFocus"          , "0"        },
    { "-visual"             , "visual"             , "Visual"             , "",        },
    { "-width"              , "width"              , "Width"              , "0"        },
    { nullptr               , nullptr              , nullptr              , nullptr    }
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
  if (! tk_->lookupOptionName(optionNames, args[0].toString(), arg))
    return false;

  if      (arg == "configure") {
    // get all options
    if      (numArgs == 1) {
      auto var = opts_.getOpts();

      tk_->setResult(var);
    }
    // get single option
    else if (numArgs == 2) {
      auto name = args[1].toString();

      auto var = opts_.getOpt(name);

      tk_->setResult(var);
    }
    // set option
    else {
      for (uint i = 1; i < numArgs - 1; i += 2) {
        auto name  = args[i + 0].toString();
        auto value = args[i + 1].toString();

        const CTkAppOpt *opt;

        if (! opts_.setOptValue(name, value, &opt))
          return tk_->throwError("unknown config option \"" + name + "\"");

        root()->execConfig(opt->name, value);
      }
    }
  }
  else if (arg == "cget") {
    tk_->TODO(args);
  }

  return true;
}

//---

CTkAppWidgetCommand::
CTkAppWidgetCommand(CTkAppCommand *command, const QString &name,
                    CTkAppWidget *w, const CTkAppOpt *opts) :
 CTkAppCommand(command->getTk(), name), command_(command), w_(w), opts_(command->getTk())
{
  opts_.init(opts);

  w->setWidgetCommand(this);
}

CTkAppWidgetCommand::
CTkAppWidgetCommand(CTkApp *app, const QString &name,
                    CTkAppWidget *w, const CTkAppOpt *opts) :
 CTkAppCommand(app, name), w_(w), opts_(app)
{
  opts_.init(opts);

  w->setWidgetCommand(this);
}

CTkAppWidgetCommand::
~CTkAppWidgetCommand()
{
}

bool
CTkAppWidgetCommand::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg arg ...?");

  auto arg = args[0].toString();

  // set config name/value
  if      (arg == "configure" || arg == "config") {
    // get all options
    if      (numArgs == 1) {
      auto var = opts_.getOpts();

      tk_->setResult(var);
    }
    // get single option
    else if (numArgs == 2) {
      auto name = args[1].toString();

      auto var = opts_.getOpt(name);

      tk_->setResult(var);
    }
    // set option
    else {
      for (uint i = 1; i < numArgs - 1; i += 2) {
        auto name  = args[i + 0].toString();
        auto value = args[i + 1].toString();

        if (! setOptValue(name, value))
          return tk_->throwError("unknown config option \"" + name + "\"");
      }
    }

    return true;
  }
  // get config name/value
  else if (arg == "cget") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " cget option");

    auto name = args[1].toString();

    QString value;

    if (! getOptValue(name, value))
      return tk_->throwError("unknown cget option \"" + name + "\"");

    setStringResult(value);

    return true;
  }
  // apply operation
  else {
    if (! w_->execOp(args))
      return false;

    return true;
  }
}

bool
CTkAppWidgetCommand::
processArgs(const Args &args)
{
  w_->setInitNotify(true);

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    auto name = args[i].toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      if (i < numArgs) {
        auto value = args[i].toString();

        if (! setOptValue(name, value))
          getTk()->throwError("unknown option \"" + name + "\"");
      }
      else
        getTk()->throwError("missing option value for \"" + name + "\"");
    }
  }

  w_->setInitNotify(false);

  return true;
}

bool
CTkAppWidgetCommand::
getOptValue(const QString &name, QString &value)
{
  if (! opts_.getOptValue(name, value))
    return false;

  return true;
}

bool
CTkAppWidgetCommand::
setOptValue(const QString &name, const QString &value)
{
  const CTkAppOpt *opt;

  if (! opts_.setOptValue(name, value, &opt))
    return false;

  w_->execConfig(opt->name, value);

  return true;
}

//---

CTkAppImageCommand::
CTkAppImageCommand(CTkApp *app, const QString &name) :
 CTkAppCommand(app, name)
{
}

CTkAppImageCommand::
~CTkAppImageCommand()
{
}

bool
CTkAppImageCommand::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  auto image = tk_->getImage(getName());
  assert(image);

  auto opt = args[0].toString();

  if      (opt == "blank") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getName() + " blank");

    image->clear();
  }
  else if (opt == "cget") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " cget option");

    auto opt = args[1].toString();

    if      (opt == "-data") {
      tk_->TODO(args);
    }
    else if (opt == "-format") {
      tk_->TODO(args);
    }
    else if (opt == "-file") {
      tk_->setStringResult(image->filename());
    }
    else if (opt == "-gamma") {
      tk_->TODO(args);
    }
    else if (opt == "-height") {
      tk_->setIntegerResult(image->height());
    }
    else if (opt == "-palette") {
      tk_->TODO(args);
    }
    else if (opt == "-width") {
      tk_->setIntegerResult(image->width());
    }
    else
      return false;
  }
  else if (opt == "configure") {
    if      (numArgs == 1) {
      QString res;

      auto addData = [&](const QString &name, const QString &def, const QString &value) {
        if (res != "")
          res += " ";

        res += "{" + name + "{} {} {" + def + "} {" + value + "}}";
      };

      auto filename = image->filename();

      addData("-data"   , "", "");
      addData("-format" , "", "");
      addData("-file"   , "", filename);
      addData("-gamma"  , "1", "1");
      addData("-height" , "0", QString::number(image->height()));
      addData("-palette", "", "");
      addData("-width " , "0", QString::number(image->width()));

      tk_->setStringResult(res);
    }
    else {
      uint i = 1;

      for ( ; i < numArgs; ++i) {
        auto name = args[i++].toString();

        if (i >= numArgs)
          return tk_->throwError("value for \"" + name + "\"missing");

        auto value = args[i].toString();

        if      (name == "-data") {
          tk_->TODO(args);
        }
        else if (name == "-format") {
          tk_->TODO(args);
        }
        else if (name == "-file") {
          tk_->TODO(args);
        }
        else if (name == "-gamma") {
          tk_->TODO(args);
        }
        else if (name == "-height") {
          long h;
          if (! CTkAppUtil::stringToInt(value, h))
            return tk_->throwError("expected integer but got \"" + value + "\"");
          image->setHeight(h);
        }
        else if (name == "-palette") {
          tk_->TODO(args);
        }
        else if (name == "-width") {
          long w;
          if (! CTkAppUtil::stringToInt(value, w))
            return tk_->throwError("expected integer but got \"" + value + "\"");
          image->setWidth(w);
        }
        else
          return false;
      }
    }
  }
  else if (opt == "copy") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getName() +
        " copy source-image ?-compositingrule rule? "
        "?-from x1 y1 x2 y2? ?-to x1 y1 x2 y2? ?-zoom x y? ?-subsample x y?");

    tk_->TODO(args);
  }
  else if (opt == "data") {
    tk_->TODO(args);
  }
  else if (opt == "get") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getName() + " get x y");

    long x;
    if (! CTkAppUtil::stringToInt(args[1].toString(), x))
      return tk_->throwError("expected integer but got \"" +
                               args[1].toString() + "\"");

    long y;
    if (! CTkAppUtil::stringToInt(args[2].toString(), y))
      return tk_->throwError("expected integer but got \"" +
                               args[2].toString() + "\"");

    QColor c;
    if (! image->getPixel(x, y, c))
      return tk_->throwError("invalid position");

    tk_->setIntegerArrayResult({c.red(), c.green(), c.blue()});
  }
  else if (opt == "put") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " put data ?-option value ...?");

    auto data = args[1].toString();

    QColor c;
    if (image->isColor()) {
      (void) CTkAppUtil::stringToQColor(data, c);
    }
    else {
      bool b;
      CTkAppUtil::stringToBool(data, b);
      c = (b ? Qt::white : Qt::black);
    }

    uint i = 2;

    for ( ; i < numArgs; ++i) {
      if      (args[i] == "-format") {
        ++i;
        tk_->TODO(args);
      }
      else if (args[i] == "-to") {
        ++i;

        long x1;
        if (i >= numArgs || ! CTkAppUtil::stringToInt(args[i].toString(), x1))
          return tk_->throwError("expected integer but got \"" +
                                   args[i].toString() + "\"");

        ++i;

        long y1;
        if (i >= numArgs || ! CTkAppUtil::stringToInt(args[i].toString(), y1))
          return tk_->throwError("expected integer but got \"" +
                                   args[i].toString() + "\"");

        if (i < numArgs) {
          ++i;

          long x2;
          if (i >= numArgs || ! CTkAppUtil::stringToInt(args[i].toString(), x2))
            return tk_->throwError("expected integer but got \"" +
                                     args[i].toString() + "\"");

          ++i;

          long y2;
          if (i >= numArgs || ! CTkAppUtil::stringToInt(args[i].toString(), y2))
            return tk_->throwError("expected integer but got \"" +
                                     args[i].toString() + "\"");

          image->setPixels(x1, y1, x2, y2, c);
        }
        else
          image->setPixel(x1, y1, c);
      }
    }
  }
  else if (opt == "read") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " read fileName ?-option value ...?");

    auto filename = args[1].toString();

    uint i = 2;

    for ( ; i < numArgs; ++i) {
      if      (args[i] == "-format") {
        ++i;
        tk_->TODO(args);
      }
      else if (args[i] == "-from") {
        long x1;
        if (i >= numArgs || ! CTkAppUtil::stringToInt(args[i].toString(), x1))
          return tk_->throwError("expected integer but got \"" +
                                   args[i].toString() + "\"");

        ++i;

        long y1;
        if (i >= numArgs || ! CTkAppUtil::stringToInt(args[i].toString(), y1))
          return tk_->throwError("expected integer but got \"" +
                                   args[i].toString() + "\"");

        ++i;

        long x2;
        if (i >= numArgs || ! CTkAppUtil::stringToInt(args[i].toString(), x2))
          return tk_->throwError("expected integer but got \"" +
                                   args[i].toString() + "\"");

        ++i;

        long y2;
        if (i >= numArgs || ! CTkAppUtil::stringToInt(args[i].toString(), y2))
          return tk_->throwError("expected integer but got \"" +
                                   args[i].toString() + "\"");
      }
      else if (args[i] == "-to") {
        long x1;
        if (i >= numArgs || ! CTkAppUtil::stringToInt(args[i].toString(), x1))
          return tk_->throwError("expected integer but got \"" +
                                   args[i].toString() + "\"");

        ++i;

        long y1;
        if (i >= numArgs || ! CTkAppUtil::stringToInt(args[i].toString(), y1))
          return tk_->throwError("expected integer but got \"" +
                                   args[i].toString() + "\"");

        ++i;
      }
      else if (args[i] == "-shrink") {
        tk_->TODO(args);
      }
    }

    if (! image->loadFile(filename))
      return false;
  }
  else if (opt == "redither") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getName() + " redither");

    tk_->TODO(args);
  }
  else if (opt == "transparency") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " transparency option ?arg ...?");

    if      (args[1] == "get") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getName() + " transparency get x y");

      long x;
      if (! CTkAppUtil::stringToInt(args[1].toString(), x))
        return tk_->throwError("expected integer but got \"" +
                                 args[1].toString() + "\"");

      long y;
      if (! CTkAppUtil::stringToInt(args[2].toString(), y))
        return tk_->throwError("expected integer but got \"" +
                                 args[2].toString() + "\"");

      QColor c;
      if (! image->getPixel(x, y, c))
        return tk_->throwError("invalid position");

      tk_->setBoolResult(c.alpha() < 255);
    }
    else if (args[1] == "set") {
      if (numArgs != 5)
        return tk_->wrongNumArgs(getName() + " transparency set x y boolean");

      long x;
      if (! CTkAppUtil::stringToInt(args[1].toString(), x))
        return tk_->throwError("expected integer but got \"" +
                                 args[1].toString() + "\"");

      long y;
      if (! CTkAppUtil::stringToInt(args[2].toString(), y))
        return tk_->throwError("expected integer but got \"" +
                                 args[2].toString() + "\"");

      bool b;
      if (! CTkAppUtil::stringToBool(args[3].toString(), b))
        return tk_->throwError("expected boolean but got \"" +
                                 args[3].toString() + "\"");

      QColor c;
      if (! image->getPixel(x, y, c))
        return tk_->throwError("invalid position");

      c.setAlpha(b ? 0 : 255);

      image->setPixel(x, y, c);
    }
    else
      return false;
  }
  else if (opt == "write") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " write fileName ?-option value ...?");

    auto filename = args[1].toString();

    uint i = 2;

    for ( ; i < numArgs; ++i) {
      if      (args[i] == "-background") {
        ++i;
        tk_->TODO(args);
      }
      else if (args[i] == "-format") {
        ++i;
        tk_->TODO(args);
      }
      else if (args[i] == "-from") {
        long x1;
        if (i >= numArgs || ! CTkAppUtil::stringToInt(args[i].toString(), x1))
          return tk_->throwError("expected integer but got \"" +
                                   args[i].toString() + "\"");

        ++i;

        long y1;
        if (i >= numArgs || ! CTkAppUtil::stringToInt(args[i].toString(), y1))
          return tk_->throwError("expected integer but got \"" +
                                   args[i].toString() + "\"");

        ++i;

        long x2;
        if (i >= numArgs || ! CTkAppUtil::stringToInt(args[i].toString(), x2))
          return tk_->throwError("expected integer but got \"" +
                                   args[i].toString() + "\"");

        ++i;

        long y2;
        if (i >= numArgs || ! CTkAppUtil::stringToInt(args[i].toString(), y2))
          return tk_->throwError("expected integer but got \"" +
                                   args[i].toString() + "\"");
      }
      else if (args[i] == "-grayscale") {
        tk_->TODO(args);
      }
    }

    tk_->TODO(args);
  }
  else {
    tk_->throwError("bad option \"" + opt + "\": must be "
      "blank, cget, configure, copy, data, get, put, read, redither, transparency, or write");
  }

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

  auto opt = args[0].toString();

  auto mname = tk_->newMatrixName();

  CTkApp::MatrixData mdata;

  if      (opt == "rotate") {
    if (numArgs < 4)
      return tk_->wrongNumArgs(getName() + " rotate a x y");

    double a, tx, ty;
    if (! CTkAppUtil::stringToReal(args[1].toString(), a) ||
        ! CTkAppUtil::stringToReal(args[2].toString(), tx) ||
        ! CTkAppUtil::stringToReal(args[3].toString(), ty))
      return false;

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
    if (! CTkAppUtil::stringToReal(args[1].toString(), sx) ||
        ! CTkAppUtil::stringToReal(args[2].toString(), sy))
      return false;

    CMatrix2D m;

    m.setScale(sx, sy);

    mdata.transform = tk_->toQTransform(m);
  }
  else if (opt == "skewx") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " skew x");

    double a;
    if (! CTkAppUtil::stringToReal(args[1].toString(), a))
      return false;

    auto m = CMatrix2D::skewX(a);

    mdata.transform = tk_->toQTransform(m);
  }
  else if (opt == "skewy") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " skew y");

    double a;
    if (! CTkAppUtil::stringToReal(args[1].toString(), a))
      return false;

    auto m = CMatrix2D::skewY(a);

    mdata.transform = tk_->toQTransform(m);
  }
  else if (opt == "translate") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getName() + " translate tx ty");

    double tx, ty;
    if (! CTkAppUtil::stringToReal(args[1].toString(), tx) ||
        ! CTkAppUtil::stringToReal(args[2].toString(), ty))
      return false;

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

//---

CTkAppCommand::
CTkAppCommand(CTkApp *tk, const QString &name) :
 CTclAppCommand(tk, name), tk_(tk)
{
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

CTkAppRoot *
CTkAppCommand::
root() const
{
  return tk_->root();
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

#include <CTkAppCommands.h>
#include <CTkApp.h>
#include <CTkAppWidget.h>
#include <CTkAppPackLayout.h>
#include <CTkAppPlaceLayout.h>
#include <CTkAppGridLayout.h>
#include <CTkAppPackLayout.h>
#include <CTkAppImage.h>
#include <CTkAppOptionValue.h>
#include <CTkAppOptData.h>

#include <CQUtil.h>

#include <CEncode64.h>

#include <QApplication>
#include <QClipboard>
#include <QFontDatabase>
#include <QMessageBox>
#include <QFileDialog>
#include <QScreen>

//---

#define CTkAppCommandDef(NAME, CLASS) \
class CLASS : public CTkAppCommand { \
 public: \
  CLASS(CTkApp *tk) : \
    CTkAppCommand(tk, NAME) { \
  } \
\
 protected: \
  bool run(const Args &args) override; \
};

//---

  // widgets
  CTkAppCommandDef("button"     , CTkAppButtonCmd)
  CTkAppCommandDef("canvas"     , CTkAppCanvasCmd)
  CTkAppCommandDef("checkbutton", CTkAppCheckButtonCmd)
  CTkAppCommandDef("entry"      , CTkAppEntryCmd)
  CTkAppCommandDef("frame"      , CTkAppFrameCmd)
  CTkAppCommandDef("label"      , CTkAppLabelCmd)
  CTkAppCommandDef("labelframe" , CTkAppLabelFrameCmd)
  CTkAppCommandDef("listbox"    , CTkAppListBoxCmd)
  CTkAppCommandDef("menu"       , CTkAppMenuCmd)
  CTkAppCommandDef("menubutton" , CTkAppMenuButtonCmd)
  CTkAppCommandDef("message"    , CTkAppMessageCmd)
  CTkAppCommandDef("panedwindow", CTkAppPanedWindowCmd)
  CTkAppCommandDef("radiobutton", CTkAppRadioButtonCmd)
  CTkAppCommandDef("scale"      , CTkAppScaleCmd)
  CTkAppCommandDef("scrollbar"  , CTkAppScrollBarCmd)
  CTkAppCommandDef("spinbox"    , CTkAppSpinBoxCmd)
  CTkAppCommandDef("text"       , CTkAppTextCmd)
  CTkAppCommandDef("toplevel"   , CTkAppTopLevelCmd)

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

//CTkAppCommandDef("bitmap" , CTkAppBitmapCmd)
//CTkAppCommandDef("console", CTkAppConsoleCmd);
//CTkAppCommandDef("photo"  , CTkAppPhotoCmd)

//CTkAppCommandDef("ttk::button"     , CTkAppTtkButtonCmd)
//CTkAppCommandDef("ttk::checkbutton", CTkAppTtkCheckButtonCmd)
//CTkAppCommandDef("ttk::combobox"   , CTkAppTtkComboBoxCmd)
//CTkAppCommandDef("ttk::entry"      , CTkAppTtkEntryCmd)
//CTkAppCommandDef("ttk::frame"      , CTkAppTtkFrameCmd)
//CTkAppCommandDef("ttk::label"      , CTkAppTtkLabeCmd)
//CTkAppCommandDef("ttk::labelframe" , CTkAppTtkLabeFrameCmd)
//CTkAppCommandDef("ttk::menubutton" , CTkAppTtkMenuButtonCmd)
//CTkAppCommandDef("ttk::notebook"   , CTkAppTtkNoteBookCmd)
//CTkAppCommandDef("ttk::panedwindow", CTkAppTtkPanedWindowCmd)
//CTkAppCommandDef("ttk::progressbar", CTkAppTtkProgressBarCmd)
//CTkAppCommandDef("ttk::radiobutton", CTkAppTtkRadioButtonCmd)
//CTkAppCommandDef("ttk::scale"      , CTkAppTtkScaleCmd)
//CTkAppCommandDef("ttk::scrollbar"  , CTkAppTtkScrollBarCmd)
//CTkAppCommandDef("ttk::separator"  , CTkAppTtkSeparatorCmd)
//CTkAppCommandDef("ttk::sizegrip"   , CTkAppTtkSizGripCmd)
//CTkAppCommandDef("ttk::treeview"   , CTkAppTtkTreeViewCmd)

//---

class CTkAppRootCommand :  public CTkAppCommand {
 public:
  CTkAppRootCommand(CTkApp *tk);

  bool run(const Args &args) override;

 private:
  CTkAppOptData opts_;
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
  new CTkAppTopLevelCmd   (tk);
  new CTkAppUpdateCmd     (tk);
  new CTkAppWInfoCmd      (tk);
  new CTkAppWmCmd         (tk);

  new CTkAppTkCmd           (tk);
//new CTkAppTkErrorCmd      (tk);
  new CTkAppTkGetOpenFileCmd(tk);
  new CTkAppTkGetSaveFileCmd(tk);
  new CTkAppTkMessageBoxCmd (tk);
  new CTkAppTkPopupCmd      (tk);
  new CTkAppTkWaitCmd       (tk);

  //---

  (void) new CTkAppRootCommand(tk);
}

}

//---

bool
CTkAppBellCmd::
run(const Args &args)
{
  uint numArgs = args.size();

  for (uint i = 1; i < numArgs; ++i) {
    auto &arg = args[i];

    if      (arg == "-displayof") {
      tk_->TODO(arg);

      ++i;

      if (i >= numArgs)
        return tk_->wrongNumArgs("bell ?-displayof window? ?-nice?");
    }
    else if (arg == "-nice") {
      tk_->TODO(arg);
    }
    else
      return tk_->throwError("bad option \"" + arg + "\": must be "
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
  uint numArgs = args.size();

  if (numArgs < 1 || numArgs > 3)
    return tk_->wrongNumArgs("bind window ?pattern? ?command?");

  auto name = args[0];

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

  if (numArgs > 1) {
    auto pattern = args[1];

    CTkAppEventData data;

    if (! tk_->parseEvent(pattern, data)) {
      (void) tk_->throwError("bad event pattern \"" + pattern + "\"");
      return true; // TODO
    }

    if (numArgs > 2) {
      data.command = args[2];

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
      tk_->TODO("display pattern bindings");
    }
  }
  else {
    tk_->TODO("display bindings");
  }

  return true;
}

//---

bool
CTkAppBindTagsCmd::
run(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("bindtags window ?tagList?");

  auto widgetName = args[0];

  CTkAppWidget* parent = nullptr;
  std::string   childName;
  CTkAppWidget* child  = nullptr;

  if (root()->decodeWidgetName(widgetName, &parent, childName)) {
    child = parent->getChild(childName);

    if (child == nullptr)
      return tk_->throwError("bad window path name \"" + childName + "\"");
  }

  if (numArgs > 1) {
    // TODO:
  }
  else {
  }

  return true;
}

//---

bool
CTkAppButtonCmd::
run(const Args &args)
{
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

  std::string widgetName;

  auto numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *button = new CTkAppButton(tk_, parent, childName);

  CQUtil::setBackground(button->getQWidget(), QColor("#d9d9d9"));

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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *canvas = new CTkAppCanvas(tk_, parent, childName);

  CQUtil::setBackground(canvas->getQWidget(), QColor("#d9d9d9"));

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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *check = new CTkAppCheckButton(tk_, parent, childName);

  CQUtil::setBackground(check->getQWidget(), QColor("#d9d9d9"));

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
  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("clipboard options ?arg arg ...?");

  static auto optionNames = std::vector<std::string>({
    "append", "clear", "get"});

  std::string opt;
  if (! tk_->lookupOptionName(optionNames, args[0], opt))
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

    clipboard->setText(clipboard->text(QClipboard::Clipboard) + QString::fromStdString(args[1]));
  }
  else if (opt == "get") {
    setStringResult(clipboard->text(QClipboard::Clipboard).toStdString());
  }

  return true;
}

//---

bool
CTkAppDestroyCmd::
run(const Args &args)
{
  uint numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    const auto &widgetName = args[i];

    auto *w = tk_->lookupWidgetByName(widgetName);
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
  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("event option ?arg?");

  static auto optionNames = std::vector<std::string>({
    "add", "delete", "generate", "info" });

  std::string arg;
  if (! tk_->lookupOptionName(optionNames, args[0], arg))
    return false;

  if      (arg == "add") {
    tk_->TODO(arg);
  }
  else if (arg == "delete") {
    tk_->TODO(arg);
  }
  else if (arg == "generate") {
    tk_->TODO(arg);
  }
  else if (arg == "info") {
    tk_->TODO(arg);
  }

  return true;
}

//---

bool
CTkAppEntryCmd::
run(const Args &args)
{
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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *entry = new CTkAppEntry(tk_, parent, childName);

  CQUtil::setBackground(entry->getQWidget(), QColor("#ffffff"));

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
    const auto &widgetName = args[0];

    auto *w = tk_->lookupWidgetByName(widgetName);
    if (! w) return false;

    w->setFocus();
  }
  else {
    for (uint i = 1; i < numArgs; ++i) {
      std::string arg = args[i];

      if      (arg == "-displayof") {
        tk_->TODO(arg);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("focus -displayof window");
      }
      else if (arg == "-force") {
        tk_->TODO(arg);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("focus -force window");
      }
      else if (arg == "-lastfor") {
        tk_->TODO(arg);

        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs("focus -lastfor window");
      }
      else
        return tk_->throwError("bad option \"" + arg + "\": must be "
                               "-displayof, -force, or -lastfor");
    }

    const auto &widgetName = args[numArgs - 1];

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
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs("font option ?arg?");

  static auto optionNames = std::vector<std::string>({
    "actual", "configure", "create", "delete", "families", "measure", "metrics", "names"});

  std::string option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  if      (option == "actual") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("font actual font ?-displayof window? ?option? ?--? ?char?");

    const auto &fontName = args[1];

    auto qfont = CTkAppUtil::stringToQFont(fontName);

    if (numArgs == 2) {
      setStringResult("-family {" + qfont.family().toStdString() + "} "
                      "-size " + std::to_string(qfont.pointSize()) + " "
                      "-weight normal -slant roman "
                      "-underline 0 -overstrike 0");
      return true;
    }

    for (uint i = 2; i < numArgs; ++i) {
      if (args[i] == "-size") {
        setIntegerResult(qfont.pointSize());
        return true;
      }
      else
        tk_->TODO(args[i]);
    }
  }
  else if (option == "configure") {
    tk_->TODO(option);
  }
  else if (option == "create") {
    tk_->TODO(option);
  }
  else if (option == "delete") {
    tk_->TODO(option);
  }
  else if (option == "families") {
    QFontDatabase fdb;

    std::vector<std::string> familyNames;

    const auto families = fdb.families(QFontDatabase::Any);

    for (const QString &family : families) {
      if (fdb.isPrivateFamily(family)) continue;

      //if (fdb.isSmoothlyScalable(family)) continue;
      //if (fdb.isFixedPitch(family)) continue;

      auto familyStr = family.toStdString();

      //if (familyStr.find('[') != std::string::npos) continue;

      familyNames.push_back(familyStr);
    }

    setStringArrayResult(familyNames);
  }
  else if (option == "measure") {
    tk_->TODO(option);
  }
  else if (option == "metrics") {
    tk_->TODO(option);
  }
  else if (option == "names") {
    tk_->TODO(option);
  }

  return true;
}

//---

bool
CTkAppFrameCmd::
run(const Args &args)
{
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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *frame = new CTkAppFrame(tk_, parent, childName);

  CQUtil::setBackground(frame->getQWidget(), QColor("#d9d9d9"));

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
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs("grab ?-global? window\" or \"grab option ?arg ...?");

  tk_->TODO("grab");

  return true;
}

//---

bool
CTkAppGridCmd::
run(const Args &args)
{
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

  const auto &arg = args[0];

  if      (arg == "configure") {
    tk_->TODO(arg);
  }
  else if (arg == "content") {
    tk_->TODO(arg);
  }
  else if (arg == "forget") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("grid forget slave ?slave ...?");

    tk_->TODO(arg);
  }
  else if (arg == "info") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("grid info window");

    const auto &arg = args[1];

    auto *child = tk_->lookupWidgetByName(arg);
    if (! child) return false;

    auto *layout = child->getParent()->getTkGridLayout();
    if (! layout) return tk_->throwError("no grid layout for \"" + arg + "\"");

    CTkAppGridLayout::Info info;

    layout->getChildInfo(child, info);

    auto res =
      CStrUtil::strprintf("-in %s -ipadx %d -ipady %d -padx %d -pady %d",
                          ".", info.getIPadX(), info.getIPadY(),
                          info.getPadX(), info.getPadY());

    setStringResult(res);
  }
  else if (arg == "propagate") {
    tk_->TODO(arg);
  }
  else if (arg == "slaves") {
    tk_->TODO(arg);
  }
  else if (arg == "anchor") {
    tk_->TODO(arg);
  }
  else if (arg == "bbox") {
    tk_->TODO(arg);
  }
  else if (arg == "columnconfigure") {
    if (numArgs < 3)
      return tk_->wrongNumArgs("grid columnconfigure master index ?-option value...?");

    const auto &masterName = args[1];

    auto *master = tk_->lookupWidgetByName(masterName);
    if (! master) return false;

    auto *layout = master->getTkGridLayout();
    if (! layout) return tk_->throwError("no grid layout for \"" + arg + "\"");

    long index;
    if (! CTkAppUtil::stringToInt(args[2], index))
      return tk_->throwError("Bad index");

    for (uint i = 3; i < numArgs - 1; i += 2) {
      const auto &name = args[i];

      auto value = args[i + 1];

      if      (name == "-minsize") {
        tk_->TODO(name);
      }
      else if (name == "-pad") {
        tk_->TODO(name);
      }
      else if (name == "-uniform") {
        tk_->TODO(name);
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
  else if (arg == "location") {
    tk_->TODO(arg);
  }
  else if (arg == "rowconfigure") {
    if (numArgs < 3)
      return tk_->wrongNumArgs("grid rowconfigure master index ?-option value...?");

    const auto &masterName = args[1];

    auto *master = tk_->lookupWidgetByName(masterName);
    if (! master) return false;

    auto *layout = master->getTkGridLayout();
    if (! layout) return tk_->throwError("no grid layout for \"" + arg + "\"");

    long index;
    if (! CTkAppUtil::stringToInt(args[2], index))
      return tk_->throwError("Bad index");

    for (uint i = 3; i < numArgs - 1; i += 2) {
      const auto &name = args[i];

      auto value = args[i + 1];

      if      (name == "-minsize") {
        tk_->TODO(name);
      }
      else if (name == "-pad") {
        tk_->TODO(name);
      }
      else if (name == "-uniform") {
        tk_->TODO(name);
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
  else if (arg == "remove") {
    tk_->TODO(arg);
  }
  else if (arg == "size") {
    tk_->TODO(arg);
  }
  else {
    CTkAppWidget*               parent = nullptr;
    std::vector<CTkAppWidget *> children;

    CTkAppOptionValueMap optValues;

    for (uint i = 0; i < numArgs; ++i) {
      const auto &arg = args[i];
      if (! arg.size()) continue;

      //---

      // check for all dashes
      uint pos = 0;

      while (pos < arg.size() && arg[pos] == '-')
        ++pos;

      bool dashes = (pos > 0 && pos >= arg.size());

      //---

      if      (dashes) {
        tk_->TODO("dashes");
      }
      else if (arg == "X") {
        tk_->TODO("X");
      }
      else if (arg == "^") {
        tk_->TODO("^");
      }
      else if (arg[0] == '-') {
        if (! tk_->processOption(opts, args, i, optValues))
          tk_->throwError("Unhandled option '" + arg + "'");
      }
      else {
        CTkAppWidget* parent1;
        std::string   childName;

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

    CTkAppGridLayout::Info info;

    {
    auto p = optValues.find("-row");
    if (p != optValues.end()) info.setRow((*p).second.i);
    }

    {
    auto p = optValues.find("-column");
    if (p != optValues.end()) info.setCol((*p).second.i);
    }

    {
    auto p = optValues.find("-sticky");
    if (p != optValues.end()) info.setSticky((*p).second.s);
    }

    {
    auto p = optValues.find("-padx");
    if (p != optValues.end()) info.setPadX((*p).second.i);
    }

    {
    auto p = optValues.find("-pady");
    if (p != optValues.end()) info.setPadY((*p).second.i);
    }

    {
    auto p = optValues.find("-ipadx");
    if (p != optValues.end()) info.setIPadX((*p).second.i);
    }

    {
    auto p = optValues.find("-ipady");
    if (p != optValues.end()) info.setIPadY((*p).second.i);
    }

    if (! parent)
      return false;

    auto *layout = parent->getTkGridLayout();
    if (! layout) return tk_->throwError("no grid layout for \"" + arg + "\"");

    layout->addWidgets(children, info);
  }

  return true;
}

//---

bool
CTkAppImageCmd::
run(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("image option ?args?");

  static auto optionNames = std::vector<std::string>({
    "create", "delete", "height", "inuse", "names", "type", "types", "width"});

  std::string name;
  if (! tk_->lookupOptionName(optionNames, args[0], name))
    return false;

  if      (name == "create") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("image create type ?name? ?options?");

    const auto &type = args[1];

    if (type != "photo" && type != "bitmap")
      return tk_->throwError("image type \"" + type + "\" doesn't exist");

    std::string name, filename, data, format, background, foreground;

    for (uint i = 2; i < numArgs; ++i) {
      const auto &arg = args[i];

      if      (arg == "-file") {
        ++i;

        if (i >= numArgs)
          return tk_->throwError("value for \"" + arg + "\" missing");

        filename = args[i];
      }
      else if (arg == "-data") {
        ++i;

        if (i >= numArgs)
          return tk_->throwError("value for \"" + arg + "\" missing");

        data = args[i];

        if (type == "photo")
          data = CEncode64Inst->decode(data);
      }
      else if (arg == "-format") {
        ++i;

        if (i >= numArgs)
          return tk_->throwError("value for \"" + arg + "\" missing");

        format = args[i];
      }
      else if (arg == "-background") {
        ++i;

        if (i >= numArgs)
          return tk_->throwError("value for \"" + arg + "\" missing");

        background = args[i];
      }
      else if (arg == "-foreground") {
        ++i;

        if (i >= numArgs)
          return tk_->throwError("value for \"" + arg + "\" missing");

        foreground = args[i];
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

    auto image = tk_->createImage(type, format, name);

    if      (filename != "") {
      image->loadFile(filename);
    }
    else if (data != "") {
      image->loadData(data);
    }

    setStringResult(name);
  }
  else if (name == "delete") {
    tk_->TODO(name);
  }
  else if (name == "height") {
  tk_->TODO(name);
  }
  else if (name == "inuse") {
    tk_->TODO(name);
  }
  else if (name == "names") {
    tk_->TODO(name);
  }
  else if (name == "type") {
    tk_->TODO(name);
  }
  else if (name == "types") {
    if (numArgs != 1)
      return tk_->wrongNumArgs("image types");

    std::vector<std::string> strs;

    strs.push_back("photo");
    strs.push_back("bitmap");

    setStringArrayResult(strs);

    return true;
  }
  else if (name == "width") {
    tk_->TODO(name);
  }

  return true;
}

//---

bool
CTkAppLabelCmd::
run(const Args &args)
{
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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *label = new CTkAppLabel(tk_, parent, childName);

  CQUtil::setBackground(label->getQWidget(), QColor("#d9d9d9"));

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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *frame = new CTkAppLabelFrame(tk_, parent, childName);

  CQUtil::setBackground(frame->getQWidget(), QColor("#d9d9d9"));

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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *list = new CTkAppListBox(tk_, parent, childName);

  CQUtil::setBackground(list->getQWidget(), QColor("#ffffff"));

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
  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("lower window ?belowThis?");

  const auto &widgetName = args[0];

  CTkAppWidget* parent = nullptr;
  std::string   childName;

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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *menu = new CTkAppMenu(tk_, parent, childName);

  CQUtil::setBackground(menu->getQWidget(), QColor("#d9d9d9"));

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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *button = new CTkAppMenuButton(tk_, parent, childName);

  CQUtil::setBackground(button->getQWidget(), QColor("#d9d9d9"));

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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *message = new CTkAppMessage(tk_, parent, childName);

  CQUtil::setBackground(message->getQWidget(), QColor("#d9d9d9"));

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
  auto numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs("option cmd arg ?arg ...?");

  const auto &cmd = args[0];

  if      (cmd == "add") {
    if (numArgs != 3 && numArgs != 4)
      return tk_->wrongNumArgs("option add pattern value ?priority?");

    const auto &pattern = args[1];
    const auto &value   = args[2];

    std::string priority;
    int         ipriority = -1;

    if (numArgs == 4) {
      priority = args[3];

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

    const auto &windowName = args[1];

    auto *window = tk_->lookupWidgetByName(windowName);
    if (! window) return false;

    const auto &optName  = args[2];
    const auto &optClass = args[3];

    std::string optValue;
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
  static CTkAppOption opts[] = {
    { "-after" , CTkAppOptionType::String, nullptr },
    { "-anchor", CTkAppOptionType::String, nullptr },
    { "-before", CTkAppOptionType::String, nullptr },
    { "-expand", CTkAppOptionType::String, nullptr },
    { "-fill"  , CTkAppOptionType::String, nullptr },
    { "-in"    , CTkAppOptionType::String, nullptr },
    { "-ipadx" , CTkAppOptionType::Int   , nullptr },
    { "-ipady" , CTkAppOptionType::Int   , nullptr },
    { "-padx"  , CTkAppOptionType::Int   , nullptr },
    { "-pady"  , CTkAppOptionType::Int   , nullptr },
    { "-side"  , CTkAppOptionType::String, nullptr },
    { nullptr  , CTkAppOptionType::None  , nullptr },
  };

  auto numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("pack option arg ?arg ...?");

  const auto &arg = args[0];

  if      (arg == "configure") {
    tk_->TODO(arg);
  }
  else if (arg == "content") {
    tk_->TODO(arg);
  }
  else if (arg == "forget") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("pack forget slave ?slave ...?");

    for (uint i = 1; i < numArgs; ++i) {
      auto *child = tk_->lookupWidgetByName(args[i]);
      if (! child) return false;

      auto *layout = child->getParent()->getTkPackLayout();
      if (! layout) return tk_->throwError("no pack layout for \"" + arg + "\"");

      layout->removeWidget(child);
    }
  }
  else if (arg == "info") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("pack info window");

    const auto &arg = args[1];

    CTkAppWidget*  parent;
    std::string childName;

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
      CStrUtil::strprintf("-in %s -anchor %s -expand %d -fill %s -ipadx %d -ipady %d "
                          "-padx %d -pady %d -side %s",
                          ".", "center", int(info.expand), info.getFillStr(),
                          info.ipadx, info.ipady, info.padx, info.pady,
                          info.getSideStr());

    setStringResult(res);
  }
  else if (arg == "propagate") {
    if (numArgs < 2 || numArgs > 3)
      return tk_->wrongNumArgs("pack option arg ?arg ...?");

    const auto &arg = args[1];

    auto *w = tk_->lookupWidgetByName(arg);
    if (! w) return false;

    auto *layout = w->getTkPackLayout();
    if (! layout) return tk_->throwError("no pack layout for \"" + arg + "\"");

    if (numArgs == 2)
      setIntegerResult(layout->isPropagate());
    else {
      const auto &value = args[2];

      bool b;

      if (! CTkAppUtil::stringToBool(value, b))
        return tk_->throwError("Invalid bool '" + value + "'");

      layout->setPropagate(b);
    }
  }
  else if (arg == "slaves") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("pack slaves arg ?arg ...?");

    const auto &arg = args[1];

    auto *w = tk_->lookupWidgetByName(arg);
    if (! w) return false;

    auto *layout = w->getTkPackLayout();
    if (! layout) return tk_->throwError("no pack layout for \"" + arg + "\"");

    if (numArgs == 2) {
      auto widgets = layout->getWidgets();

      std::vector<std::string> names;

      for (auto *w : widgets)
        names.push_back(w->getFullName());

      setStringArrayResult(names);
    }
    else
      tk_->TODO(arg);
  }
  else {
    CTkAppPackLayout::Side      side     = CTkAppPackLayout::SIDE_NONE;
    CTkAppPackLayout::Fill      fill     = CTkAppPackLayout::FILL_NONE;
    bool                        expand   = false;
    CTkAppWidget*               inparent = nullptr;
    int                         padx     = 0;
    int                         pady     = 0;
    int                         ipadx    = 0;
    int                         ipady    = 0;
    CTkAppWidget*               parent   = nullptr;
    std::vector<CTkAppWidget *> children;

    CTkAppOptionValueMap optValues;

    for (uint i = 0; i < numArgs; ++i) {
      const auto &arg = args[i];

      if (arg.size() > 0 && arg[0] == '-') {
        if (! tk_->processOption(opts, args, i, optValues))
          tk_->throwError("Unhandled option '" + arg + "'");
      }
      else {
        CTkAppWidget*  parent1;
        std::string childName;

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
      if      ((*p).second.s == "x"   ) fill = CTkAppPackLayout::FILL_X;
      else if ((*p).second.s == "y"   ) fill = CTkAppPackLayout::FILL_Y;
      else if ((*p).second.s == "both") fill = CTkAppPackLayout::FILL_BOTH;
    }
    }

    {
    auto p = optValues.find("-expand");

    if (p != optValues.end()) {
      if      ((*p).second.s == "x") expand = 2;
      else if ((*p).second.s == "y") expand = 3;
      else {
        if (! CTkAppUtil::stringToBool((*p).second.s, expand))
          return tk_->throwError("Invalid -expand value");
      }
    }
    }

    {
    auto p = optValues.find("-padx");

    if (p != optValues.end())
      padx = (*p).second.i;
    }

    {
    auto p = optValues.find("-pady");

    if (p != optValues.end())
      pady = (*p).second.i;
    }

    {
    auto p = optValues.find("-ipadx");

    if (p != optValues.end())
      ipadx = (*p).second.i;
    }

    {
    auto p = optValues.find("-ipady");

    if (p != optValues.end())
      ipady = (*p).second.i;
    }

    {
    auto p = optValues.find("-side");

    if (p != optValues.end()) {
      if      ((*p).second.s == "left"  ) side = CTkAppPackLayout::SIDE_LEFT;
      else if ((*p).second.s == "right" ) side = CTkAppPackLayout::SIDE_RIGHT;
      else if ((*p).second.s == "bottom") side = CTkAppPackLayout::SIDE_BOTTOM;
      else if ((*p).second.s == "top"   ) side = CTkAppPackLayout::SIDE_TOP;
    }
    else {
      if (! expand && fill == CTkAppPackLayout::FILL_NONE)
        side = CTkAppPackLayout::SIDE_TOP;
    }
    }

    {
    auto p = optValues.find("-in");

    if (p != optValues.end()) {
      CTkAppWidget*  parent1;
      std::string childName;

      if (root()->decodeWidgetName((*p).second.s, &parent1, childName)) {
        auto *child = parent1->getChild(childName);

        if (child)
          inparent = child;
      }
    }
    }

    if (inparent)
      parent = inparent;

    if (! parent)
      return tk_->throwError("no parent for \"" + arg + "\"");

    auto *layout = parent->getTkPackLayout();
    if (! layout) return tk_->throwError("no pack layout for \"" + arg + "\"");

    if (side == CTkAppPackLayout::SIDE_NONE)
      side = CTkAppPackLayout::SIDE_TOP;

    CTkAppPackLayout::Info info(side, fill, expand, padx, pady, ipadx, ipady);

    layout->addWidgets(children, info);

    parent->show();
  }

  return true;
}

//---

bool
CTkAppPanedWindowCmd::
run(const Args &args)
{
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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *pane = new CTkAppPanedWindow(tk_, parent, childName);

  CQUtil::setBackground(pane->getQWidget(), QColor("#d9d9d9"));

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

  const auto &arg = args[0];

  if      (arg == "configure") {
    tk_->TODO(arg);
  }
  else if (arg == "content") {
    tk_->TODO(arg);
  }
  else if (arg == "forget") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("place forget args");

    tk_->TODO(arg);
  }
  else if (arg == "info") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("place info window");

    const auto &arg = args[1];

    CTkAppWidget*  parent;
    std::string childName;

    if (! root()->decodeWidgetName(arg, &parent, childName))
      return tk_->throwError("Invalid widget name '" + arg + "'");

    auto *child = parent->getChild(childName);

    if (child == nullptr)
      return tk_->throwError("bad window path name \"" + arg + "\"");

    auto *layout = parent->getTkPlaceLayout();
    if (! layout) return tk_->throwError("no place layout for \"" + arg + "\"");

    CTkAppPlaceLayout::Info info;

    layout->getChildInfo(child, info);

    std::string res;

    setStringResult(res);
  }
  else if (arg == "propagate") {
    tk_->TODO(arg);
  }
  else if (arg == "slaves") {
    tk_->TODO(arg);
  }
  else {
    CTkAppWidget*               parent = nullptr;
    std::vector<CTkAppWidget *> children;

    CTkAppOptionValueMap optValues;

    for (uint i = 0; i < numArgs; ++i) {
      const auto &arg = args[i];

      if (arg.size() > 0 && arg[0] == '-') {
        if (! tk_->processOption(opts, args, i, optValues))
          tk_->throwError("Unhandled option '" + arg + "'");
      }
      else {
        CTkAppWidget*  parent1;
        std::string childName;

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
    auto p = optValues.find("-relx");
    if (p != optValues.end()) info.setRelX((*p).second.r);
    }

    {
    auto p = optValues.find("-rely");
    if (p != optValues.end()) info.setRelY((*p).second.r);
    }

    auto *layout = parent->getTkPlaceLayout();
    if (! layout) return tk_->throwError("no place layout for \"" + arg + "\"");

    layout->addWidgets(children, info);
  }

  return true;
}

bool
CTkAppRadioButtonCmd::
run(const Args &args)
{
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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *radio = new CTkAppRadioButton(tk_, parent, childName);

  CQUtil::setBackground(radio->getQWidget(), QColor("#d9d9d9"));

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
  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("raise window ?aboveThis?");

  const auto &widgetName = args[0];

  CTkAppWidget* parent = nullptr;
  std::string   childName;

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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *scale = new CTkAppScale(tk_, parent, childName);

  CQUtil::setBackground(scale->getQWidget(), QColor("#d9d9d9"));

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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *scrollbar = new CTkAppScrollBar(tk_, parent, childName);

  CQUtil::setBackground(scrollbar->getQWidget(), QColor("#d9d9d9"));

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
  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("selection options ?arg arg ...?");

  static auto optionNames = std::vector<std::string>({
    "append", "clear", "get"});

  std::string opt;
  if (! tk_->lookupOptionName(optionNames, args[0], opt))
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

    clipboard->setText(clipboard->text(QClipboard::Selection) + QString::fromStdString(args[1]));
  }
  else if (opt == "get") {
    setStringResult(clipboard->text(QClipboard::Selection).toStdString());
  }

  return true;
}

//---

bool
CTkAppSendCmd::
run(const Args &args)
{
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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *spin = new CTkAppSpinBox(tk_, parent, childName);

  CQUtil::setBackground(spin->getQWidget(), QColor("#ffffff"));

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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *text = new CTkAppText(tk_, parent, childName);

  CQUtil::setBackground(text->getQWidget(), QColor("#ffffff"));

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
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs("tk subcommand ?arg ...?");

  const auto &arg = args[0];

  if      (arg == "appname") {
    tk_->setStringResult("CTkApp");
  }
  else if (arg == "busy") {
    tk_->TODO(arg);
  }
  else if (arg == "caret") {
    tk_->TODO(arg);
  }
  else if (arg == "fontchooser") {
    tk_->TODO(arg);
  }
  else if (arg == "inactive") {
    tk_->TODO(arg);
  }
  else if (arg == "scaling") {
    tk_->setRealResult(CScreenUnitsMgrInst->dpi()/72.0);
  }
  else if (arg == "useinputmethods") {
    tk_->TODO(arg);
  }
  else if (arg == "windowingsystem") {
    tk_->setStringResult("Qt");
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
run(const Args &)
{
  QWidget *parent = nullptr;

  QString              dir;
  QString              filter;
  QString*             selectedFilter = nullptr;
  QFileDialog::Options options;

  auto file = QFileDialog::getOpenFileName(parent, "Open File", dir, filter,
                                           selectedFilter, options);

  tk_->setStringResult(file.toStdString());

  return true;
}

//---

bool
CTkAppTkGetSaveFileCmd::
run(const Args &args)
{
  uint numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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

  tk_->setStringResult(file.toStdString());

  return true;
}

//---

bool
CTkAppTkWaitCmd::
run(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs != 2)
    return tk_->wrongNumArgs("tkwait variable|visibility|window name");

  const auto &obj  = args[0];
  const auto &name = args[1];

  if      (obj == "variable") {
    tk_->TODO(obj);
  }
  else if (obj == "visibility") {
    tk_->TODO(obj);
  }
  else if (obj == "window") {
    auto *w = tk_->lookupWidgetByName(name);
    if (! w) return false;

    tk_->TODO(obj);
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
  std::string message;
  QWidget*    parent = nullptr;

  uint numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

    if (name.size() > 0 && name[0] == '-') {
      auto value = (i < numArgs - 1 ? args[++i] : "");

      if      (name == "-type") {
        tk_->TODO(name);
      }
      else if (name == "-icon") {
        tk_->TODO(name);
      }
      else if (name == "-message") {
        message = value;
      }
      else if (name == "-parent") {
        auto *w = tk_->lookupWidgetByName(value);
        if (! w) return false;

        parent = w->qwidget();
      }
      else
        tk_->TODO(name);
    }
    else
      tk_->TODO(name);
  }

  QMessageBox::information(parent, "Information", QString::fromStdString(message));

  return true;
}

//---

bool
CTkAppTkPopupCmd::
run(const Args &)
{
  tk_->TODO("tk_popup");

  return true;
}

//---

bool
CTkAppTopLevelCmd::
run(const Args &args)
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

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i];

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
  std::string   childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid widget name '" + widgetName + "'");

  auto *toplevel = new CTkAppTopLevel(tk_, parent, childName);

  CQUtil::setBackground(toplevel->getQWidget(), QColor("#d9d9d9"));

  auto *cmd = new CTkAppWidgetCommand(this, widgetName, toplevel, opts);

  cmd->processArgs(args);

  tk_->addTopLevel(toplevel);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppUpdateCmd::
run(const Args &args)
{
  uint numArgs = args.size();

  if      (numArgs == 0)
    tk_->processEvents();
  else if (numArgs == 1) {
    static auto optionNames = std::vector<std::string>({
      "idletasks"});

    std::string arg;
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
  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("winfo option ?arg ...?");

  static auto optionNames = std::vector<std::string>({
    "cells", "children", "class", "colormapfull", "depth", "geometry", "height", "id",
    "ismapped", "manager", "" "name", "parent", "pointerx", "pointery", "pointerxy",
    "reqheight", "reqwidth", "rootx", "rooty", "screen", "" "screencells", "screendepth",
    "screenheight", "screenwidth", "screenmmheight", "screenmmwidth", "screenvisual",
    "server", "toplevel", "viewable", "visual", "visualid", "vrootheight", "vrootwidth",
    "vrootx", "vrooty", "width", "x", "y", "atom", "atomname", "containing", "interps",
    "pathname", "exists", "fpixels", "pixels", "rgb", "visualsavailable" });

  std::string arg;
  if (! tk_->lookupOptionName(optionNames, args[0], arg))
    return false;

  CTkAppWidget *w = nullptr;

  auto getWindow = [&](bool quiet=false) {
    if (numArgs < 2)
      return tk_->wrongNumArgs("winfo " + arg + " window");

    std::string widgetName = args[1];

    w = tk_->lookupWidgetByName(widgetName, quiet);

    return (w ? true : false);
  };

  //---

  if      (arg == "atom") {
    tk_->TODO(arg);
  }
  else if (arg == "atomname") {
    tk_->TODO(arg);
  }
  else if (arg == "cells") {
    tk_->TODO(arg);
  }
  else if (arg == "children") {
    if (! getWindow()) return false;

    std::vector<CTkAppWidget *> children;

    w->getChildren(children);

    uint numChildren = children.size();

    std::vector<std::string> list;

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
    tk_->TODO(arg);
  }
  else if (arg == "containing") {
    tk_->TODO(arg);
  }
  else if (arg == "depth") {
    if (! getWindow()) return false;

    //tk_->TODO(arg);
    setIntegerResult(24);
  }
  else if (arg == "exists") {
    setIntegerResult(getWindow(/*quiet*/true) ? 1 : 0);
  }
  else if (arg == "fpixels") {
    tk_->TODO(arg);
  }
  else if (arg == "geometry") {
    if (! getWindow()) return false;

    auto r = w->qwidget()->geometry();

    auto res = QString("%1x%2+%3+%4").arg(r.width()).arg(r.height()).arg(r.x()).arg(r.y());

    setStringResult(res.toStdString());
  }
  else if (arg == "height") {
    if (! getWindow()) return false;

    setIntegerResult(w->qwidget()->height());
  }
  else if (arg == "id") {
    if (! getWindow()) return false;

    setIntegerResult(long(w->qwidget()));
  }
  else if (arg == "interps") {
    tk_->TODO(arg);
  }
  else if (arg == "ismapped") {
    if (! getWindow()) return false;

    setIntegerResult(w ? w->getQWidget()->isVisible() : 0);
  }
  else if (arg == "manager") {
    tk_->TODO(arg);
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
    tk_->TODO(arg);
  }
  else if (arg == "pixels") {
    tk_->TODO(arg);
  }
  else if (arg == "pointerx") {
    tk_->TODO(arg);
  }
  else if (arg == "pointerxy") {
    tk_->TODO(arg);
  }
  else if (arg == "pointery") {
    tk_->TODO(arg);
  }
  else if (arg == "reqheight") {
    if (! getWindow()) return false;

    setIntegerResult(w->qwidget()->sizeHint().width());
  }
  else if (arg == "reqwidth") {
    if (! getWindow()) return false;

    setIntegerResult(w->qwidget()->sizeHint().width());
  }
  else if (arg == "rgb") {
    tk_->TODO(arg);
  }
  else if (arg == "rootx") {
    tk_->TODO(arg);
  }
  else if (arg == "rooty") {
    tk_->TODO(arg);
  }
  else if (arg == "screen") {
    tk_->TODO(arg);
  }
  else if (arg == "screencells") {
    tk_->TODO(arg);
  }
  else if (arg == "screendepth") {
    tk_->TODO(arg);
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
    tk_->TODO(arg);
  }
  else if (arg == "screenwidth") {
    if (! getWindow()) return false;

    auto r = qApp->primaryScreen()->geometry();
    setIntegerResult(r.width());
  }
  else if (arg == "server") {
    tk_->TODO(arg);
  }
  else if (arg == "toplevel") {
    if (! getWindow()) return false;

    auto *toplevel = w->toplevel();
    setStringResult(toplevel ? toplevel->getFullName() : "");
  }
  else if (arg == "viewable") {
    tk_->TODO(arg);
  }
  else if (arg == "visual") {
    if (! getWindow()) return false;

    setStringResult("truecolor"); // TODO
  }
  else if (arg == "visualid") {
    tk_->TODO(arg);
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

    setIntegerResult(w ? w->qwidget()->width() : 0);
  }
  else if (arg == "x") {
    if (! getWindow()) return false;

    setIntegerResult(w ? w->qwidget()->x() : 0);
  }
  else if (arg == "y") {
    if (! getWindow()) return false;

    setIntegerResult(w ? w->qwidget()->y() : 0);
  }

  return true;
}

//---

bool
CTkAppWmCmd::
run(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs < 2)
    return tk_->wrongNumArgs("wm option window ?arg ...?");

  //---

  static auto optionNames = std::vector<std::string>({
    "aspect", "attributes", "client", "colormapwindows", "command", "deiconify", "focusmodel",
    "forget", "frame", "geometry", "grid", "group", "iconbitmap", "iconify", "iconmask",
    "iconname", "iconphoto", "iconposition", "iconwindow", "manage", "maxsize", "minsize",
    "overrideredirect", "positionfrom", "protocol", "resizable", "sizefrom", "stackorder",
    "state", "title", "transient", "withdraw"});

  std::string arg;
  if (! tk_->lookupOptionName(optionNames, args[0], arg))
    return false;

  //---

  const auto &name = args[1];

  CTkAppWidget* parent;
  std::string   childName;

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
    tk_->TODO(arg);
  }
  else if (arg == "attributes") {
    tk_->TODO(arg);
  }
  else if (arg == "client") {
    tk_->TODO(arg);
  }
  else if (arg == "colormapwindows") {
    tk_->TODO(arg);
  }
  else if (arg == "command") {
    tk_->TODO(arg);
  }
  else if (arg == "deiconify") {
    w->getQWidget()->show();
  }
  else if (arg == "focusmodel") {
    tk_->TODO(arg);
  }
  else if (arg == "forget") {
    tk_->TODO(arg);
  }
  else if (arg == "frame") {
    tk_->TODO(arg);
  }
  else if (arg == "geometry") {
    if (numArgs == 3)
      w->setGeometry(args[2]);
    else
      setStringResult(w->getGeometry());
  }
  else if (arg == "grid") {
    tk_->TODO(arg);
  }
  else if (arg == "group") {
    tk_->TODO(arg);
  }
  else if (arg == "iconbitmap") {
    tk_->TODO(arg);
  }
  else if (arg == "iconify") {
    tk_->TODO(arg);
  }
  else if (arg == "iconmask") {
    tk_->TODO(arg);
  }
  else if (arg == "iconname") {
    if (numArgs == 3)
      w->setIcon(args[2]);
    else
      setStringResult(w->getIcon());
  }
  else if (arg == "iconphoto") {
    tk_->TODO(arg);
  }
  else if (arg == "iconposition") {
    tk_->TODO(arg);
  }
  else if (arg == "iconwindow") {
    tk_->TODO(arg);
  }
  else if (arg == "manage") {
    tk_->TODO(arg);
  }
  else if (arg == "maxsize") {
    tk_->TODO(arg);
  }
  else if (arg == "minsize") {
    if      (numArgs == 2) {
      auto size = w->getQWidget()->minimumSize();

      auto res = CStrUtil::strprintf("%d %d", size.width(), size.height());

      setStringResult(res);
    }
    else if (numArgs != 4) {
      return tk_->wrongNumArgs("wm minsize window ?width height?");
    }
    else {
      long sw, sh;

      if (! CTkAppUtil::stringToInt(args[2], sw))
        return tk_->throwError("expected integer but got \"" + args[2] + "\"");

      if (! CTkAppUtil::stringToInt(args[3], sh))
        return tk_->throwError("expected integer but got \"" + args[3] + "\"");

      sw = std::max(sw, 1L);
      sh = std::max(sh, 1L);

      w->getQWidget()->setMinimumSize(QSize(sw, sh));
    }
  }
  else if (arg == "overrideredirect") {
    tk_->TODO(arg);
  }
  else if (arg == "positionfrom") {
    tk_->TODO(arg);
  }
  else if (arg == "protocol") {
    tk_->TODO(arg);
  }
  else if (arg == "resizable") {
    tk_->TODO(arg);
  }
  else if (arg == "sizefrom") {
    tk_->TODO(arg);
  }
  else if (arg == "stackorder") {
    tk_->TODO(arg);
  }
  else if (arg == "state") {
    tk_->TODO(arg);
  }
  else if (arg == "title") {
    if (numArgs == 3) {
      const auto &title = args[2];

      w->setTitle(title);
    }
    else {
      const auto &title = w->getTitle();

      setStringResult(title);
    }
  }
  else if (arg == "transient") {
    tk_->TODO(arg);
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
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(". option ?arg arg ...?");

  static auto optionNames = std::vector<std::string>({
    "cget", "configure"});

  std::string arg;
  if (! tk_->lookupOptionName(optionNames, args[0], arg))
    return false;

  if      (arg == "configure") {
    // get all options
    if      (numArgs == 1) {
      auto *obj = opts_.getOpts();

      setObjResult(obj);
    }
    // get single option
    else if (numArgs == 2) {
      const auto &name = args[1];

      auto *obj = opts_.getOpt(name);

      setObjResult(obj);
    }
    // set option
    else {
      for (uint i = 1; i < numArgs - 1; i += 2) {
        const auto &name  = args[i + 0];
        const auto &value = args[i + 1];

        const CTkAppOpt *opt;

        if (! opts_.setOptValue(name, value, &opt))
          return tk_->throwError("unknown config option \"" + name + "\"");

        root()->execConfig(opt->name, value);
      }
    }
  }
  else if (arg == "cget") {
    tk_->TODO(arg);
  }

  return true;
}

//---

CTkAppWidgetCommand::
CTkAppWidgetCommand(CTkAppCommand *command, const std::string &name,
                    CTkAppWidget *w, const CTkAppOpt *opts) :
 CTkAppCommand(command->getTk(), name), command_(command), w_(w), opts_(command->getTk())
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
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg arg ...?");

  const auto &arg = args[0];

  // set config name/value
  if      (arg == "configure" || arg == "config") {
    // get all options
    if      (numArgs == 1) {
      auto *obj = opts_.getOpts();

      setObjResult(obj);
    }
    // get single option
    else if (numArgs == 2) {
      const auto &name = args[1];

      auto *obj = opts_.getOpt(name);

      setObjResult(obj);
    }
    // set option
    else {
      for (uint i = 1; i < numArgs - 1; i += 2) {
        const auto &name  = args[i + 0];
        const auto &value = args[i + 1];

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

    const auto &name = args[1];

    std::string value;

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
    const auto &name = args[i];

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      if (i < numArgs) {
        const auto &value = args[i];

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
getOptValue(const std::string &name, std::string &value)
{
  if (! opts_.getOptValue(name, value))
    return false;

  return true;
}

bool
CTkAppWidgetCommand::
setOptValue(const std::string &name, const std::string &value)
{
  const CTkAppOpt *opt;

  if (! opts_.setOptValue(name, value, &opt))
    return false;

  w_->execConfig(opt->name, value);

  return true;
}

//---

CTkAppCommand::
CTkAppCommand(CTkApp *tk, const std::string &name) :
 CTclAppCommand(tk, name), tk_(tk)
{
}

bool
CTkAppCommand::
proc(int argc, const char **argv)
{
  tk_->setCurrentCommand(getName());

  assert(argc > 0);

  arg0_ = std::string(argv[0]);

  Args args;

  for (int i = 1; i < argc; ++i)
    args.push_back(std::string(argv[i]));

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

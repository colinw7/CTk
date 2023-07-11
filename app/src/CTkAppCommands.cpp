#include <CTkAppCommands.h>
#include <CTkApp.h>
#include <CTkAppWidget.h>
#include <CTkAppPackLayout.h>
#include <CTkAppPlaceLayout.h>
#include <CTkAppGridLayout.h>
#include <CTkAppLayout.h>
#include <CTkAppImage.h>
#include <CTkAppOptionValue.h>
#include <CTkAppOptData.h>

#include <CTclAppCommand.h>

#include <CQUtil.h>
#include <CStrUniqueMatch.h>

#include <QApplication>
#include <QClipboard>
#include <QFontDatabase>

class CTkAppCommand : public CTclAppCommand {
 public:
  using Args = std::vector<std::string>;

 public:
  CTkAppCommand(CTkApp *tk, const std::string &name) :
   CTclAppCommand(tk, name), tk_(tk) {
  }

  CTkApp *getTk() const { return tk_; }

 protected:
  bool proc(int argc, const char **argv) override {
    assert(argc > 0);

    arg0_ = std::string(argv[0]);

    Args args;

    for (int i = 1; i < argc; ++i)
      args.push_back(std::string(argv[i]));

    return run(args);
  }

  virtual bool run(const Args &args) = 0;

  CTkAppRoot *root() const { return tk_->root(); }

 protected: \
  CTkApp *tk_ { nullptr };

  mutable std::string arg0_;
};

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
//CTkAppCommandDef("send"     , CTkAppSendCmd)
//CTkAppCommandDef("tk"       , CTkAppTkCmd)
//CTkAppCommandDef("tkerror"  , CTkAppTkErrorCmd)
//CTkAppCommandDef("tkwait"   , CTkAppTkWaitCmd)
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
//CTkAppCommandDef("tk_getOpenFile"      , CTkAppTkGetOpenFileCmd)
//CTkAppCommandDef("tk_getSaveFile"      , CTkAppTkGetSaveFileCmd)
//CTkAppCommandDef("tk_messageBox"       , CTkAppTkMessageBoxCmd)
//CTkAppCommandDef("tk_popup"            , CTkAppTkPopupCmd)
//CTkAppCommandDef("tk_setPalette"       , CTkAppTkSetPaletteCmd)

//CTkAppCommandDef("bitmap" , CTkAppBitmapCmd)
//CTkAppCommandDef("console", CTkAppConsoleCmd);
//CTkAppCommandDef("photo"  , CTkAppPhotoCmd)

//---

class CTkWidgetCommand : public CTkAppCommand {
 public:
  CTkWidgetCommand(CTkAppCommand *command, const std::string &name,
                   CTkWidget *w, const CTkOpt *opts = nullptr);

  bool run(const Args &args) override;

 public:
  using Args = std::vector<std::string>;

  bool processArgs(const Args &args);

  bool getOptValue(const std::string &name, std::string &value);
  bool setOptValue(const std::string &name, const std::string &value);

 private:
  CTkAppCommand* command_ { nullptr };
  CTkWidget*     w_       { nullptr };
  CTkOptData     opts_;
};

//---

class CTkAppRootCommand :  public CTkAppCommand {
 public:
  CTkAppRootCommand(CTkApp *tk);

  bool run(const Args &args) override;

 private:
  CTkOptData opts_;
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
  new CTkAppSpinBoxCmd    (tk);
  new CTkAppTextCmd       (tk);
  new CTkAppTopLevelCmd   (tk);
  new CTkAppUpdateCmd     (tk);
  new CTkAppWInfoCmd      (tk);
  new CTkAppWmCmd         (tk);

  //---

  (void) new CTkAppRootCommand(tk);
}

}

//---

bool
CTkAppBellCmd::
run(const Args &)
{
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

  auto widgetName = args[0];

  CTkWidget*  parent = nullptr;
  std::string childName;
  CTkWidget*  child  = nullptr;

  if (root()->decodeWidgetName(widgetName, &parent, childName)) {
    child = parent->getChild(childName);

    if (child == nullptr)
      return tk_->throwError("bad window path name \"" + childName + "\"");
  }

  if (numArgs > 1) {
    auto pattern = args[1];

    if (numArgs > 2) {
      auto command = args[2];

      if (child)
        child->bindEvent(pattern, command);
      else
        tk_->bindEvent(widgetName, pattern, command);
    }
  }
  else {
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

  CTkWidget*  parent = nullptr;
  std::string childName;
  CTkWidget*  child  = nullptr;

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
  static CTkOpt opts[] = {
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *button = new CTkButton(tk_, parent, childName);

  CQUtil::setBackground(button->getQWidget(), QColor("#d9d9d9"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, button, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppCanvasCmd::
run(const Args &args)
{
  static CTkOpt opts[] = {
    { "-background"         , "background"         , "Background"         , "#d9d9d9" },
    { "-bd"                 , "-borderWidth"        , nullptr              , nullptr   },
    { "-bg"                 , "-background"         , nullptr              , nullptr   },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "0"       },
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *canvas = new CTkCanvas(tk_, parent, childName);

  CQUtil::setBackground(canvas->getQWidget(), QColor("#d9d9d9"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, canvas, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppCheckButtonCmd::
run(const Args &args)
{
  static CTkOpt opts[] = {
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *check = new CTkCheckButton(tk_, parent, childName);

  CQUtil::setBackground(check->getQWidget(), QColor("#d9d9d9"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, check, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppClipBoardCmd::
run(const Args &args)
{
  CStrUniqueMatchValues<std::string> optionValues;

  optionValues.addValues({{"clear", "clear"}, {"append", "append"}, {"get", "get"}});

  //---

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("clipboard options ?arg arg ...?");

  auto *clipboard = QApplication::clipboard();

  std::string opt;
  (void) optionValues.match(args[0], opt);

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
  else
    return tk_->throwError("bad option \"" + opt + "\" : must be append, clear, or get");

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

    CTkWidget *child = nullptr;

    if (widgetName != ".") {
      CTkWidget*  parent = nullptr;
      std::string childName;

      if (! root()->decodeWidgetName(widgetName, &parent, childName))
        continue;

      auto *child = parent->getChild(childName);

      if (child == nullptr)
        continue;
    }
    else
      child = root();

    child->deleteLater();
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

  // TODO
  return false;
}

//---

bool
CTkAppEntryCmd::
run(const Args &args)
{
  static CTkOpt opts[] = {
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *entry = new CTkEntry(tk_, parent, childName);

  CQUtil::setBackground(entry->getQWidget(), QColor("#ffffff"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, entry, opts);

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
    QWidget *w = QApplication::focusWidget();
    if (! w) return false;

    auto *tw = tk_->lookupWidget(w);
    if (! tw) return false;

    setStringResult(tw->getFullName());

    return true;
  }
  else if (numArgs == 1) {
    std::string widgetName = args[0];

    auto *w = tk_->lookupWidgetByName(widgetName);

    w->setFocus();
  }
  else {
    for (uint i = 1; i < numArgs - 1; ++i) {
      std::string arg = args[i];

      if      (arg == "-displayof") {
      }
      else if (arg == "-force") {
      }
      else if (arg == "-lastfor") {
      }
      else
        return tk_->throwError("bad option \"" + arg + "\": must be "
                               "-displayof, -force, or -lastfor");
    }

    std::string widgetName = args[numArgs - 1];

    auto *w = tk_->lookupWidgetByName(widgetName);

    if (w)
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

  auto option = args[0];

  if      (option == "actual") {
    // TODO
  }
  else if (option == "configure") {
    // TODO
  }
  else if (option == "create") {
    // TODO
  }
  else if (option == "delete") {
    // TODO
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
    // TODO
  }
  else if (option == "metrics") {
    // TODO
  }
  else if (option == "names") {
    // TODO
  }
  else {
    return tk_->throwError("bad option \"" + option + "\": must be "
                           "actual, configure, create, delete, families, "
                           "measure, metrics, or names");
  }

  return true;
}

//---

bool
CTkAppFrameCmd::
run(const Args &args)
{
  static CTkOpt opts[] = {
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *frame = new CTkFrame(tk_, parent, childName);

  CQUtil::setBackground(frame->getQWidget(), QColor("#d9d9d9"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, frame, opts);

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

  // TODO

  return true;
}

//---

bool
CTkAppGridCmd::
run(const Args &args)
{
  static CTkOption opts[] = {
    { "-column"    , CTkOptionType::Int   , nullptr },
    { "-columnspan", CTkOptionType::Int   , nullptr },
    { "-in"        , CTkOptionType::String, nullptr },
    { "-ipadx"     , CTkOptionType::Int   , nullptr },
    { "-ipady"     , CTkOptionType::Int   , nullptr },
    { "-padx"      , CTkOptionType::Int   , nullptr },
    { "-pady"      , CTkOptionType::Int   , nullptr },
    { "-row"       , CTkOptionType::Int   , nullptr },
    { "-rowspan"   , CTkOptionType::Int   , nullptr },
    { "-sticky"    , CTkOptionType::String, nullptr },
    { nullptr      , CTkOptionType::None  , nullptr },
  };

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("grid option arg ?arg ...?");

  const auto &arg = args[0];

  if      (arg == "anchor") {
    return false;
  }
  else if (arg == "bbox") {
    return false;
  }
  else if (arg == "columnconfigure") {
    if (numArgs < 3)
      return tk_->wrongNumArgs("grid columnconfigure master index ?-option value...?");

    const auto &masterName = args[1];

    auto *master = tk_->lookupWidgetByName(masterName);
    if (master == nullptr) return false;

    auto *layout = master->getTkGridLayout();
    if (layout == nullptr) return false;

    long index;
    if (! CTkAppUtil::stringToInt(args[2], index))
      return tk_->throwError("Bad index");

    for (uint i = 3; i < numArgs - 1; i += 2) {
      const auto &name = args[i];

      auto value = args[i + 1];

      if      (name == "-minsize") {
      }
      else if (name == "-pad") {
      }
      else if (name == "-uniform") {
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

    return true;
  }
  else if (arg == "configure") {
    return false;
  }
  else if (arg == "forget") {
    return false;
  }
  else if (arg == "info") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("grid info window");

    const auto &arg = args[1];

    auto *child = tk_->lookupWidgetByName(arg);
    if (child == nullptr) return false;

    auto *layout = child->getParent()->getTkGridLayout();
    if (layout == nullptr) return false;

    CTkAppGridLayout::Info info;

    layout->getChildInfo(child, info);

    auto res =
      CStrUtil::strprintf("-in %s -ipadx %d -ipady %d -padx %d -pady %d",
                          ".", info.getIPadX(), info.getIPadY(),
                          info.getPadX(), info.getPadY());

    setStringResult(res);

    return true;
  }
  else if (arg == "location") {
    return false;
  }
  else if (arg == "propagate") {
    return false;
  }
  else if (arg == "rowconfigure") {
    if (numArgs < 3)
      return tk_->wrongNumArgs("grid rowconfigure master index ?-option value...?");

    const auto &masterName = args[1];

    auto *master = tk_->lookupWidgetByName(masterName);
    if (master == nullptr) return false;

    auto *layout = master->getTkGridLayout();
    if (layout == nullptr) return false;

    long index;
    if (! CTkAppUtil::stringToInt(args[2], index))
      return tk_->throwError("Bad index");

    for (uint i = 3; i < numArgs - 1; i += 2) {
      const auto &name = args[i];

      auto value = args[i + 1];

      if      (name == "-minsize") {
      }
      else if (name == "-pad") {
      }
      else if (name == "-uniform") {
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

    return true;
  }
  else if (arg == "remove") {
    return false;
  }
  else if (arg == "size") {
    return false;
  }
  else if (arg == "slaves") {
    return false;
  }
  else {
    CTkWidget*               parent = nullptr;
    std::vector<CTkWidget *> children;

    CTkOptionValueMap optValues;

    for (uint i = 0; i < numArgs; ++i) {
      const auto &arg = args[i];

      if (arg.size() > 0 && arg[0] == '-') {
        if (! tk_->processOption(opts, args, i, optValues))
          std::cerr << "Unhandled option '" << arg << "'" << std::endl;
      }
      else {
        CTkWidget*  parent1;
        std::string childName;

        if (! root()->decodeWidgetName(arg, &parent1, childName))
          return tk_->throwError("Invalid name '" + arg + "'");

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

    layout->addWidgets(children, info);

    return true;
  }
}

//---

bool
CTkAppImageCmd::
run(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("image option ?args?");

  const auto &name = args[0];

  if      (name == "create") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("image create type ?name? ?options?");

    const auto &type = args[1];

    if (type != "photo" && type != "bitmap")
      return tk_->throwError("image type \"" + type + "\" doesn't exist");

    uint ind = 2;

    std::string name;

    if (numArgs > 2) {
      name = args[2];

      ++ind;
    }
    else
      name = tk_->getNewImageName();

    auto image = tk_->createImage(type, name);

    for (uint i = ind; i < numArgs; ++i) {
      const auto &name = args[i];

      if (name == "-file") {
        ++i;

        if (i >= numArgs)
          return tk_->throwError("value for \"" + name + "\" missing");

        const auto &value = args[i];

        image->loadFile(value);
      }
      else
        return tk_->throwError("unknown image create option \"" + name + "\"");
    }
  }
  else if (name == "delete") {
  }
  else if (name == "height") {
  }
  else if (name == "inuse") {
  }
  else if (name == "names") {
  }
  else if (name == "type") {
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
  }
  else
    return tk_->throwError("bad option \"" + name + "\": must be "
                           "create, delete, height, inuse, names, type, types, or width");

  return true;
}

//---

bool
CTkAppLabelCmd::
run(const Args &args)
{
  static CTkOpt opts[] = {
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *label = new CTkLabel(tk_, parent, childName);

  CQUtil::setBackground(label->getQWidget(), QColor("#d9d9d9"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, label, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppLabelFrameCmd::
run(const Args &args)
{
  static CTkOpt opts[] = {
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *frame = new CTkLabelFrame(tk_, parent, childName);

  CQUtil::setBackground(frame->getQWidget(), QColor("#d9d9d9"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, frame, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppListBoxCmd::
run(const Args &args)
{
  static CTkOpt opts[] = {
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *list = new CTkListBox(tk_, parent, childName);

  CQUtil::setBackground(list->getQWidget(), QColor("#ffffff"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, list, opts);

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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

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
  static CTkOpt opts[] = {
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *menu = new CTkMenu(tk_, parent, childName);

  CQUtil::setBackground(menu->getQWidget(), QColor("#d9d9d9"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, menu, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppMenuButtonCmd::
run(const Args &args)
{
  static CTkOpt opts[] = {
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *button = new CTkMenuButton(tk_, parent, childName);

  CQUtil::setBackground(button->getQWidget(), QColor("#d9d9d9"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, button, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppMessageCmd::
run(const Args &args)
{
  static CTkOpt opts[] = {
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *message = new CTkMessage(tk_, parent, childName);

  CQUtil::setBackground(message->getQWidget(), QColor("#d9d9d9"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, message, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppOptionCmd::
run(const Args &)
{
  return true;
}

//---

bool
CTkAppPackCmd::
run(const Args &args)
{
  static CTkOption opts[] = {
    { "-after" , CTkOptionType::String, nullptr },
    { "-anchor", CTkOptionType::String, nullptr },
    { "-before", CTkOptionType::String, nullptr },
    { "-expand", CTkOptionType::String, nullptr },
    { "-fill"  , CTkOptionType::String, nullptr },
    { "-in"    , CTkOptionType::String, nullptr },
    { "-ipadx" , CTkOptionType::Int   , nullptr },
    { "-ipady" , CTkOptionType::Int   , nullptr },
    { "-padx"  , CTkOptionType::Int   , nullptr },
    { "-pady"  , CTkOptionType::Int   , nullptr },
    { "-side"  , CTkOptionType::String, nullptr },
    { nullptr  , CTkOptionType::None  , nullptr },
  };

  auto numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("pack option arg ?arg ...?");

  const auto &arg = args[0];

  if      (arg == "propagate") {
    if (numArgs < 2 || numArgs > 3)
      return tk_->wrongNumArgs("pack option arg ?arg ...?");

    const auto &arg = args[1];

    auto *w = tk_->lookupWidgetByName(arg);
    if (! w) return tk_->throwError("Invalid widget '" + arg + "'");

    auto *layout = w->getTkPackLayout();

    if (numArgs == 0)
      setIntegerResult(layout->isPropagate());
    else {
      const auto &value = args[2];

      bool b;

      if (! CTkAppUtil::stringToBool(value, b))
        return tk_->throwError("Invalid bool '" + value + "'");

      layout->setPropagate(b);
    }

    return true;
  }
  else if (arg == "info") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("pack info window");

    const auto &arg = args[1];

    CTkWidget*  parent;
    std::string childName;

    if (! root()->decodeWidgetName(arg, &parent, childName))
      return tk_->throwError("Invalid name '" + arg + "'");

    auto *child = parent->getChild(childName);

    if (child == nullptr)
      return tk_->throwError("bad window path name \"" + arg + "\"");

    auto *layout = parent->getTkPackLayout();

    CTkAppPackLayout::Info info;

    layout->getChildInfo(child, info);

    auto res =
      CStrUtil::strprintf("-in %s -anchor %s -expand %d -fill %s -ipadx %d -ipady %d "
                          "-padx %d -pady %d -side %s",
                          ".", "center", int(info.expand), info.getFillStr(),
                          info.ipadx, info.ipady, info.padx, info.pady,
                          info.getSideStr());

    setStringResult(res);

    return true;
  }
  else {
    CTkAppPackLayout::Side   side     = CTkAppPackLayout::SIDE_NONE;
    CTkAppPackLayout::Fill   fill     = CTkAppPackLayout::FILL_NONE;
    bool                     expand   = false;
    CTkWidget*               inparent = nullptr;
    int                      padx     = 0;
    int                      pady     = 0;
    int                      ipadx    = 0;
    int                      ipady    = 0;
    CTkWidget*               parent   = nullptr;
    std::vector<CTkWidget *> children;

    CTkOptionValueMap optValues;

    for (uint i = 0; i < numArgs; ++i) {
      const auto &arg = args[i];

      if (arg.size() > 0 && arg[0] == '-') {
        if (! tk_->processOption(opts, args, i, optValues))
          std::cerr << "Unhandled option '" << arg << "'" << std::endl;
      }
      else {
        CTkWidget*  parent1;
        std::string childName;

        if (! root()->decodeWidgetName(arg, &parent1, childName))
          return tk_->throwError("Invalid name '" + arg + "'");

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
      if (! CTkAppUtil::stringToBool((*p).second.s, expand))
        return tk_->throwError("Invalid -expand value");
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
      CTkWidget*  parent1;
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

    auto *layout = parent->getTkPackLayout();

    CTkAppPackLayout::Info info(side, fill, expand, padx, pady, ipadx, ipady);

    layout->addWidgets(children, info);

    return true;
  }
}

//---

bool
CTkAppPanedWindowCmd::
run(const Args &args)
{
  static CTkOpt opts[] = {
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *pane = new CTkPanedWindow(tk_, parent, childName);

  CQUtil::setBackground(pane->getQWidget(), QColor("#d9d9d9"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, pane, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppPlaceCmd::
run(const Args &args)
{
  static CTkOption opts[] = {
    { "-anchor"    , CTkOptionType::String, nullptr },
    { "-bordermode", CTkOptionType::String, nullptr },
    { "-height"    , CTkOptionType::Int   , nullptr },
    { "-in"        , CTkOptionType::String, nullptr },
    { "-relheight" , CTkOptionType::String, nullptr },
    { "-relwidth"  , CTkOptionType::String, nullptr },
    { "-relx"      , CTkOptionType::Real  , nullptr },
    { "-rely"      , CTkOptionType::Real  , nullptr },
    { "-width"     , CTkOptionType::Int   , nullptr },
    { "-x"         , CTkOptionType::Int   , nullptr },
    { "-y"         , CTkOptionType::Int   , nullptr },
    { nullptr      , CTkOptionType::None  , nullptr },
  };

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("place option|pathName args");

  const auto &arg = args[0];

  if      (arg == "configure") {
    return false;
  }
  else if (arg == "forget") {
    return false;
  }
  else if (arg == "info") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("place info window");

    const auto &arg = args[1];

    CTkWidget*  parent;
    std::string childName;

    if (! root()->decodeWidgetName(arg, &parent, childName))
      return tk_->throwError("Invalid name '" + arg + "'");

    auto *child = parent->getChild(childName);

    if (child == nullptr)
      return tk_->throwError("bad window path name \"" + arg + "\"");

    auto *layout = parent->getTkPlaceLayout();

    CTkAppPlaceLayout::Info info;

    layout->getChildInfo(child, info);

    std::string res;

    setStringResult(res);

    return true;
  }
  else if (arg == "slaves") {
    return false;
  }
  else {
    CTkWidget*               parent = nullptr;
    std::vector<CTkWidget *> children;

    CTkOptionValueMap optValues;

    for (uint i = 0; i < numArgs; ++i) {
      const auto &arg = args[i];

      if (arg.size() > 0 && arg[0] == '-') {
        if (! tk_->processOption(opts, args, i, optValues))
          std::cerr << "Unhandled option '" << arg << "'" << std::endl;
      }
      else {
        CTkWidget*  parent1;
        std::string childName;

        if (! root()->decodeWidgetName(arg, &parent1, childName))
          return tk_->throwError("Invalid name '" + arg + "'");

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

    layout->addWidgets(children, info);

    return true;
  }
}

bool
CTkAppRadioButtonCmd::
run(const Args &args)
{
  static CTkOpt opts[] = {
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *radio = new CTkRadioButton(tk_, parent, childName);

  CQUtil::setBackground(radio->getQWidget(), QColor("#d9d9d9"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, radio, opts);

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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

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
  static CTkOpt opts[] = {
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *scale = new CTkScale(tk_, parent, childName);

  CQUtil::setBackground(scale->getQWidget(), QColor("#d9d9d9"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, scale, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppScrollBarCmd::
run(const Args &args)
{
  static CTkOpt opts[] = {
    { "-activebackground"   , "activeBackground"   , "Foreground"         , "#ececec"  },
    { "-activerelief"       , "activeRelief"       , "Relief"             , "raised"   },
    { "-background"         , "background"         , "Background"         , "#d9d9d9"  },
    { "-bd"                 , "borderWidth"        , nullptr              , nullptr    },
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *scrollbar = new CTkScrollBar(tk_, parent, childName);

  CQUtil::setBackground(scrollbar->getQWidget(), QColor("#d9d9d9"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, scrollbar, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppSelectionCmd::
run(const Args &args)
{
  CStrUniqueMatchValues<std::string> optionValues;

  optionValues.addValues({{"clear", "clear"}, {"append", "append"}, {"get", "get"}});

  //---

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("selection options ?arg arg ...?");

  auto *clipboard = QApplication::clipboard();

  std::string opt;
  (void) optionValues.match(args[0], opt);

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
  else
    return tk_->throwError("bad option \"" + opt + "\" : must be append, clear, or get");

  return true;
}

//---

bool
CTkAppSpinBoxCmd::
run(const Args &args)
{
  static CTkOpt opts[] = {
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *spin = new CTkSpinBox(tk_, parent, childName);

  CQUtil::setBackground(spin->getQWidget(), QColor("#ffffff"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, spin, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppTextCmd::
run(const Args &args)
{
  static CTkOpt opts[] = {
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *text = new CTkText(tk_, parent, childName);

  CQUtil::setBackground(text->getQWidget(), QColor("#ffffff"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, text, opts);

  cmd->processArgs(args);

  setStringResult(widgetName);

  return true;
}

//---

bool
CTkAppTopLevelCmd::
run(const Args &args)
{
  static CTkOpt opts[] = {
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

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *toplevel = new CTkTopLevel(tk_, parent, childName);

  CQUtil::setBackground(toplevel->getQWidget(), QColor("#d9d9d9"));

  auto *cmd = new CTkWidgetCommand(this, widgetName, toplevel, opts);

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
    const auto &arg = args[0];

    if (arg == "idletasks")
      tk_->processEvents();
    else
      return tk_->throwError("bad option \"" + arg + "\": must be idletasks");
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

  const auto &arg = args[0];

  if      (arg == "atom") {
  }
  else if (arg == "atomname") {
  }
  else if (arg == "cells") {
  }
  else if (arg == "children") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("winfo children window");

    std::string widgetName = args[1];

    auto *w = tk_->lookupWidgetByName(widgetName);

    if (w) {
      std::vector<CTkWidget *> children;

      w->getChildren(children);

      uint numChildren = children.size();

      std::vector<std::string> list;

      for (uint i = 0; i < numChildren; ++i) {
        auto *child = children[i];

        list.push_back(child->getFullName());
      }

      setStringArrayResult(list);

      return true;
    }
  }
  else if (arg == "class") {
  }
  else if (arg == "colormapfull") {
  }
  else if (arg == "containing") {
  }
  else if (arg == "depth") {
  }
  else if (arg == "exists") {
  }
  else if (arg == "fpixels") {
  }
  else if (arg == "geometry") {
  }
  else if (arg == "height") {
  }
  else if (arg == "id") {
  }
  else if (arg == "interps") {
  }
  else if (arg == "ismapped") {
  }
  else if (arg == "manager") {
  }
  else if (arg == "name") {
  }
  else if (arg == "parent") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("winfo parent window");

    std::string widgetName = args[1];

    auto *w = tk_->lookupWidgetByName(widgetName);

    if (w) {
      auto *parent = w->getParent();

      if (parent)
        setStringResult(parent->getFullName());
    }
  }
  else if (arg == "pathname") {
  }
  else if (arg == "pixels") {
  }
  else if (arg == "pointerx") {
  }
  else if (arg == "pointerxy") {
  }
  else if (arg == "pointery") {
  }
  else if (arg == "reqheight") {
  }
  else if (arg == "reqwidth") {
  }
  else if (arg == "rgb") {
  }
  else if (arg == "rootx") {
  }
  else if (arg == "rooty") {
  }
  else if (arg == "screen") {
  }
  else if (arg == "screencells") {
  }
  else if (arg == "screendepth") {
  }
  else if (arg == "screenheight") {
  }
  else if (arg == "screenmmheight") {
  }
  else if (arg == "screenmmwidth") {
  }
  else if (arg == "screenvisual") {
  }
  else if (arg == "screenwidth") {
  }
  else if (arg == "server") {
  }
  else if (arg == "toplevel") {
  }
  else if (arg == "viewable") {
  }
  else if (arg == "visual") {
  }
  else if (arg == "visualid") {
  }
  else if (arg == "visualsavailable") {
  }
  else if (arg == "vrootheight") {
  }
  else if (arg == "vrootwidth") {
  }
  else if (arg == "vrootx") {
  }
  else if (arg == "vrootx") {
  }
  else if (arg == "vrooty") {
  }
  else if (arg == "width") {
  }
  else if (arg == "x") {
  }
  else if (arg == "y") {
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

  const auto &arg  = args[0];
  const auto &name = args[1];

  CTkWidget*  parent;
  std::string childName;

  if (! root()->decodeWidgetName(name, &parent, childName))
    return tk_->throwError("Invalid name '" + name + "'");

  CTkWidget *w;

  if (childName != "") {
    auto *child = parent->getChild(childName);

    if (child == nullptr)
      return tk_->throwError("Invalid name '" + name + "'");

    w = child;
  }
  else
    w = parent;

  if      (arg == "aspect") {
  }
  else if (arg == "attributes") {
  }
  else if (arg == "client") {
  }
  else if (arg == "colormapwindows") {
  }
  else if (arg == "command") {
  }
  else if (arg == "deiconify") {
  }
  else if (arg == "focusmodel") {
  }
  else if (arg == "forget") {
  }
  else if (arg == "frame") {
  }
  else if (arg == "geometry") {
  }
  else if (arg == "grid") {
  }
  else if (arg == "group") {
  }
  else if (arg == "iconbitmap") {
  }
  else if (arg == "iconify") {
  }
  else if (arg == "iconmask") {
  }
  else if (arg == "iconname") {
  }
  else if (arg == "iconphoto") {
  }
  else if (arg == "iconposition") {
  }
  else if (arg == "iconwindow") {
  }
  else if (arg == "manage") {
  }
  else if (arg == "maxsize") {
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
        return tk_->throwError("xpected integer but got \"" + args[2] + "\"");

      if (! CTkAppUtil::stringToInt(args[3], sh))
        return tk_->throwError("xpected integer but got \"" + args[3] + "\"");

      sw = std::max(sw, 1L);
      sh = std::max(sh, 1L);

      w->getQWidget()->setMinimumSize(QSize(sw, sh));
    }

    return true;
  }
  else if (arg == "overrideredirect") {
  }
  else if (arg == "positionfrom") {
  }
  else if (arg == "protocol") {
  }
  else if (arg == "resizable") {
  }
  else if (arg == "sizefrom") {
  }
  else if (arg == "stackorder") {
  }
  else if (arg == "state") {
  }
  else if (arg == "title") {
    if (numArgs == 3) {
      const auto &title = args[2];

      w->setTitle(title);

      return true;
    }
    else {
      const auto &title = w->getTitle();

      setStringResult(title);

      return true;
    }
  }
  else if (arg == "transient") {
  }
  else if (arg == "withdraw") {
  }
  else
    return tk_->throwError("ambiguous option \"" + arg + "\": must be "
      "aspect, attributes, client, colormapwindows, command, deiconify, focusmodel, "
      "forget, frame, geometry, grid, group, iconbitmap, iconify, iconmask, iconname, "
      "iconphoto, iconposition, iconwindow, manage, maxsize, minsize, overrideredirect, "
      "positionfrom, protocol, resizable, sizefrom, stackorder, state, title, "
      "transient, or withdraw");

  return false;
}

//---

CTkAppRootCommand::
CTkAppRootCommand(CTkApp *tk) :
 CTkAppCommand(tk, "."), opts_(tk)
{
  static CTkOpt opts[] = {
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

  const auto &arg = args[0];

  if (arg == "configure" || arg == "config") {
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

        const CTkOpt *opt;

        if (! opts_.setOptValue(name, value, &opt))
          return tk_->throwError("unknown config option \"" + name + "\"");

        root()->execConfig(opt->name, value);
      }
    }
  }
  else
    return tk_->throwError("bad option \"" + arg + "\": must be cget or configure");

  return true;
}

//---

CTkWidgetCommand::
CTkWidgetCommand(CTkAppCommand *command, const std::string &name,
                 CTkWidget *w, const CTkOpt *opts) :
 CTkAppCommand(command->getTk(), name), command_(command), w_(w), opts_(command->getTk())
{
  opts_.init(opts);
}

bool
CTkWidgetCommand::
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
CTkWidgetCommand::
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
CTkWidgetCommand::
getOptValue(const std::string &name, std::string &value)
{
  if (! opts_.getOptValue(name, value))
    return false;

  return true;
}

bool
CTkWidgetCommand::
setOptValue(const std::string &name, const std::string &value)
{
  const CTkOpt *opt;

  if (! opts_.setOptValue(name, value, &opt))
    return false;

  w_->execConfig(opt->name, value);

  return true;
}

#include <CTkCommands.h>
#include <CTk.h>
#include <CTkWidget.h>
#include <CTkLayout.h>
#include <CTkImage.h>

#include <CQUtil.h>

#include <QApplication>

namespace CTkCommands {

void
addCommands(CTk *tk)
{
  auto *tcl = tk->getTcl();

  tcl->addCommand(new CTkBellCommand       (tk));
  tcl->addCommand(new CTkBindCommand       (tk));
//tcl->addCommand(new CTkBindTagsCommand   (tk));
//tcl->addCommand(new CTkBitmapCommand     (tk));
  tcl->addCommand(new CTkButtonCommand     (tk));
  tcl->addCommand(new CTkCanvasCommand     (tk));
  tcl->addCommand(new CTkCheckButtonCommand(tk));
//tcl->addCommand(new CTkClipBoardCommand  (tk));
//tcl->addCommand(new CTkConsoleCommand    (tk));
  tcl->addCommand(new CTkDestroyCommand    (tk));
  tcl->addCommand(new CTkEntryCommand      (tk));
//tcl->addCommand(new CTkEventCommand      (tk));
  tcl->addCommand(new CTkFocusCommand      (tk));
//tcl->addCommand(new CTkFontCommand       (tk));
  tcl->addCommand(new CTkFrameCommand      (tk));
//tcl->addCommand(new CTkGrabCommand       (tk));
  tcl->addCommand(new CTkGridCommand       (tk));
  tcl->addCommand(new CTkImageCommand      (tk));
  tcl->addCommand(new CTkLabelCommand      (tk));
  tcl->addCommand(new CTkLabelFrameCommand (tk));
  tcl->addCommand(new CTkListBoxCommand    (tk));
  tcl->addCommand(new CTkLowerCommand      (tk));
//tcl->addCommand(new CTkMenuCommand       (tk));
//tcl->addCommand(new CTkMenuButtonCommand (tk));
//tcl->addCommand(new CTkMessageCommand    (tk));
//tcl->addCommand(new CTkOptionCommand     (tk));
  tcl->addCommand(new CTkPackCommand       (tk));
  tcl->addCommand(new CTkPanedWindowCommand(tk));
//tcl->addCommand(new CTkPhotoCommand      (tk));
  tcl->addCommand(new CTkPlaceCommand      (tk));
  tcl->addCommand(new CTkRadioButtonCommand(tk));
  tcl->addCommand(new CTkRaiseCommand      (tk));
  tcl->addCommand(new CTkScaleCommand      (tk));
  tcl->addCommand(new CTkScrollBarCommand  (tk));
//tcl->addCommand(new CTkSelectionCommand  (tk));
//tcl->addCommand(new CTkSendCommand       (tk));
  tcl->addCommand(new CTkSpinBoxCommand    (tk));
  tcl->addCommand(new CTkTextCommand       (tk));
//tcl->addCommand(new CTkTkCommand         (tk));
//tcl->addCommand(new CTkTkErrorCommand    (tk));
//tcl->addCommand(new CTkTkPopupCommand    (tk));
//tcl->addCommand(new CTkTkWaitCommand     (tk));
  tcl->addCommand(new CTkTopLevelCommand   (tk));
  tcl->addCommand(new CTkWInfoCommand      (tk));
  tcl->addCommand(new CTkWmCommand         (tk));

  //---

  tcl->addCommand(new CTkRootWidgetCommand(tk));
}

}

//----------

CTkBellCommand::
CTkBellCommand(CTk *tk) :
 CTkCommand(tk, "bell")
{
}

CTclValueRef
CTkBellCommand::
exec(const std::vector<CTclValueRef> &)
{
  QApplication::beep();

  return CTclValueRef();
}

//----------

CTkBindCommand::
CTkBindCommand(CTk *tk) :
 CTkCommand(tk, "bind")
{
}

CTclValueRef
CTkBindCommand::
exec(const std::vector<CTclValueRef> &args)
{
  uint numArgs = args.size();

  if (numArgs < 1 || numArgs > 3)
    return tk_->wrongNumArgs("bind window ?pattern? ?command?");

  auto widgetName = args[0]->toString();

  CTkWidget*  parent = nullptr;
  std::string childName;
  CTkWidget*  child  = nullptr;

  if (root()->decodeWidgetName(widgetName, &parent, childName)) {
    child = parent->getChild(childName);

    if (child == nullptr)
      return tk_->throwError("bad window path name \"" + childName + "\"");
  }

  if (numArgs > 1) {
    auto pattern = args[1]->toString();

    if (numArgs > 2) {
      auto command = args[2]->toString();

      if (child)
        child->bindEvent(pattern, command);
      else
        tk_->bindEvent(widgetName, pattern, command);
    }
  }
  else {
  }

  return CTclValueRef();
}

//----------

CTkButtonCommand::
CTkButtonCommand(CTk *tk) :
 CTkCommand(tk, "button")
{
}

CTclValueRef
CTkButtonCommand::
exec(const std::vector<CTclValueRef> &args)
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

  uint numArgs = args.size();

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i]->toString();

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

  tk_->getTcl()->addCommand(cmd);

  cmd->processArgs(args);

  return CTclValueRef(tk_->getTcl()->createValue(widgetName));
}

//----------

CTkCanvasCommand::
CTkCanvasCommand(CTk *tk) :
 CTkCommand(tk, "canvas")
{
}

CTclValueRef
CTkCanvasCommand::
exec(const std::vector<CTclValueRef> &args)
{
  static CTkOpt opts[] = {
    { "-background"         , "background"         , "Background"         , "#d9d9d9" },
    { "-bd"                 , "borderWidth"        , nullptr              , nullptr   },
    { "-bg"                 , "background"         , nullptr              , nullptr   },
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
    const auto &name = args[i]->toString();

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

  tk_->getTcl()->addCommand(cmd);

  cmd->processArgs(args);

  return CTclValueRef(tk_->getTcl()->createValue(widgetName));
}

//----------

CTkCheckButtonCommand::
CTkCheckButtonCommand(CTk *tk) :
 CTkCommand(tk, "checkbutton")
{
}

CTclValueRef
CTkCheckButtonCommand::
exec(const std::vector<CTclValueRef> &args)
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
    const auto &name = args[i]->toString();

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

  tk_->getTcl()->addCommand(cmd);

  cmd->processArgs(args);

  return CTclValueRef(tk_->getTcl()->createValue(widgetName));
}

//----------

CTkDestroyCommand::
CTkDestroyCommand(CTk *tk) :
 CTkCommand(tk, "destroy")
{
}

CTclValueRef
CTkDestroyCommand::
exec(const std::vector<CTclValueRef> &args)
{
  uint numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    const auto &widgetName = args[i]->toString();

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

  return CTclValueRef();
}

//----------

CTkEntryCommand::
CTkEntryCommand(CTk *tk) :
 CTkCommand(tk, "entry")
{
}

CTclValueRef
CTkEntryCommand::
exec(const std::vector<CTclValueRef> &args)
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
    const auto &name = args[i]->toString();

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

  tk_->getTcl()->addCommand(cmd);

  cmd->processArgs(args);

  return CTclValueRef(tk_->getTcl()->createValue(widgetName));
}

//----------

CTkFocusCommand::
CTkFocusCommand(CTk *tk) :
 CTkCommand(tk, "focus")
{
}

CTclValueRef
CTkFocusCommand::
exec(const std::vector<CTclValueRef> &args)
{
  uint numArgs = args.size();

  if      (numArgs == 0) {
    QWidget *w = QApplication::focusWidget();
    if (! w) return CTclValueRef();

    auto *tw = tk_->lookupWidget(w);
    if (! tw) return CTclValueRef();

    return CTclValueRef(tk_->getTcl()->createValue(tw->getFullName()));
  }
  else if (numArgs == 1) {
    std::string widgetName = args[0]->toString();

    auto *w = tk_->lookupWidgetByName(widgetName);

    w->setFocus();
  }
  else {
    for (uint i = 1; i < numArgs - 1; ++i) {
      std::string arg = args[i]->toString();

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

    std::string widgetName = args[numArgs - 1]->toString();

    auto *w = tk_->lookupWidgetByName(widgetName);

    if (w)
      w->setFocus();
  }

  return CTclValueRef();
}

//----------

CTkFrameCommand::
CTkFrameCommand(CTk *tk) :
 CTkCommand(tk, "frame")
{
}

CTclValueRef
CTkFrameCommand::
exec(const std::vector<CTclValueRef> &args)
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
    const auto &name = args[i]->toString();

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

  tk_->getTcl()->addCommand(cmd);

  cmd->processArgs(args);

  return CTclValueRef(tk_->getTcl()->createValue(widgetName));
}

//----------

CTkGridCommand::
CTkGridCommand(CTk *tk) :
 CTkCommand(tk, "grid")
{
}

CTclValueRef
CTkGridCommand::
exec(const std::vector<CTclValueRef> &args)
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
    return tk_->wrongNumArgs("pack option arg ?arg ...?");

  const auto &arg = args[0]->toString();

  if      (arg == "anchor") {
    return CTclValueRef();
  }
  else if (arg == "bbox") {
    return CTclValueRef();
  }
  else if (arg == "columnconfigure") {
    if (numArgs < 3)
      return tk_->wrongNumArgs("grid columnconfigure master index ?-option value...?");

    const auto &masterName = args[1]->toString();

    auto *master = tk_->lookupWidgetByName(masterName);
    if (master == nullptr) return CTclValueRef();

    auto *layout = master->getTkGridLayout();
    if (layout == nullptr) return CTclValueRef();

    long index;
    bool ok = args[2]->toInt(index);
    if (! ok)
      return tk_->throwError("Bad index");

    for (uint i = 3; i < numArgs - 1; i += 2) {
      const auto &name = args[i]->toString();

      CTclValueRef value = args[i + 1];

      if      (name == "-minsize") {
      }
      else if (name == "-pad") {
      }
      else if (name == "-uniform") {
      }
      else if (name == "-weight") {
        long weight;

        if (! value->checkInt(tk_->getTcl(), weight))
          return CTclValueRef();

        layout->setColumnWeight(index, weight);
      }
      else
        return tk_->throwError("bad option \"" + name + "\": must be "
                               "-minsize, -pad, -uniform, or -weight");
    }

    return CTclValueRef();
  }
  else if (arg == "configure") {
    return CTclValueRef();
  }
  else if (arg == "forget") {
    return CTclValueRef();
  }
  else if (arg == "info") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("grid info window");

    const auto &arg = args[1]->toString();

    auto *child = tk_->lookupWidgetByName(arg);
    if (child == nullptr) return CTclValueRef();

    auto *layout = child->getParent()->getTkGridLayout();
    if (layout == nullptr) return CTclValueRef();

    CTkGridLayout::Info info;

    layout->getChildInfo(child, info);

    std::string res =
      CStrUtil::strprintf("-in %s -ipadx %d -ipady %d -padx %d -pady %d",
                          ".", info.getIPadX(), info.getIPadY(),
                          info.getPadX(), info.getPadY());

    return CTclValueRef(tk_->getTcl()->createValue(res));
  }
  else if (arg == "location") {
    return CTclValueRef();
  }
  else if (arg == "propagate") {
    return CTclValueRef();
  }
  else if (arg == "rowconfigure") {
    return CTclValueRef();
  }
  else if (arg == "remove") {
    return CTclValueRef();
  }
  else if (arg == "size") {
    return CTclValueRef();
  }
  else if (arg == "slaves") {
    return CTclValueRef();
  }
  else {
    CTkWidget*               parent = nullptr;
    std::vector<CTkWidget *> children;

    CTkOptionValueMap optValues;

    for (uint i = 0; i < numArgs; ++i) {
      const auto &arg = args[i]->toString();

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

    CTkGridLayout::Info info;

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

    auto *layout = parent->getTkGridLayout();

    layout->addWidgets(children, info);

    return CTclValueRef();
  }
}

//----------

CTkImageCommand::
CTkImageCommand(CTk *tk) :
 CTkCommand(tk, "image")
{
}

CTclValueRef
CTkImageCommand::
exec(const std::vector<CTclValueRef> &args)
{
  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("image option ?args?");

  const auto &name = args[0]->toString();

  if      (name == "create") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("image create type ?name? ?options?");

    const auto &type = args[1]->toString();

    if (type != "photo" && type != "bitmap")
      return tk_->throwError("image type \"" + type + "\" doesn't exist");

    uint ind = 2;

    std::string name;

    if (numArgs > 2) {
      name = args[2]->toString();

      ++ind;
    }
    else
      name = tk_->getNewImageName();

    auto image = tk_->createImage(type, name);

    for (uint i = ind; i < numArgs; ++i) {
      const auto &name = args[i]->toString();

      if (name == "-file") {
        ++i;

        if (i >= numArgs)
          return tk_->throwError("value for \"" + name + "\" missing");

        const auto &value = args[i]->toString();

        image->loadFile(value);
      }
      else
        return tk_->throwError("unknown option \"" + name + "\"");
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

    auto *list = new CTclList;

    list->addValue(tk_->getTcl()->createValue("photo"));
    list->addValue(tk_->getTcl()->createValue("bitmap"));

    return CTclValueRef(list);
  }
  else if (name == "width") {
  }
  else
    return tk_->throwError("bad option \"" + name + "\": must be "
                           "create, delete, height, inuse, names, type, types, or width");

  return CTclValueRef();
}

//----------

CTkLabelCommand::
CTkLabelCommand(CTk *tk) :
 CTkCommand(tk, "label")
{
}

CTclValueRef
CTkLabelCommand::
exec(const std::vector<CTclValueRef> &args)
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
    const auto &name = args[i]->toString();

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

  tk_->getTcl()->addCommand(cmd);

  cmd->processArgs(args);

  return CTclValueRef(tk_->getTcl()->createValue(widgetName));
}

//----------

CTkLabelFrameCommand::
CTkLabelFrameCommand(CTk *tk) :
 CTkCommand(tk, "labelframe")
{
}

CTclValueRef
CTkLabelFrameCommand::
exec(const std::vector<CTclValueRef> &args)
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
    const auto &name = args[i]->toString();

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

  tk_->getTcl()->addCommand(cmd);

  cmd->processArgs(args);

  return CTclValueRef(tk_->getTcl()->createValue(widgetName));
}

//----------

CTkListBoxCommand::
CTkListBoxCommand(CTk *tk) :
 CTkCommand(tk, "listbox")
{
}

CTclValueRef
CTkListBoxCommand::
exec(const std::vector<CTclValueRef> &args)
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
    const auto &name = args[i]->toString();

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

  tk_->getTcl()->addCommand(cmd);

  cmd->processArgs(args);

  return CTclValueRef(tk_->getTcl()->createValue(widgetName));
}

//----------

CTkLowerCommand::
CTkLowerCommand(CTk *tk) :
 CTkCommand(tk, "lower")
{
}

CTclValueRef
CTkLowerCommand::
exec(const std::vector<CTclValueRef> &args)
{
  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("lower window ?belowThis?");

  const auto &widgetName = args[0]->toString();

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *child = parent->getChild(childName);

  if (child == nullptr)
    return tk_->throwError("bad window path name \"" + widgetName + "\"");

  child->lower();

  return CTclValueRef();
}

//----------

CTkPackCommand::
CTkPackCommand(CTk *tk) :
 CTkCommand(tk, "pack")
{
}

CTclValueRef
CTkPackCommand::
exec(const std::vector<CTclValueRef> &args)
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

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("pack option arg ?arg ...?");

  const auto &arg = args[0]->toString();

  if      (arg == "propagate") {
    return CTclValueRef();
  }
  else if (arg == "info") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("pack info window");

    const auto &arg = args[1]->toString();

    CTkWidget*  parent;
    std::string childName;

    if (! root()->decodeWidgetName(arg, &parent, childName))
      return tk_->throwError("Invalid name '" + arg + "'");

    auto *child = parent->getChild(childName);

    if (child == nullptr)
      return tk_->throwError("bad window path name \"" + arg + "\"");

    auto *layout = parent->getTkPackLayout();

    CTkPackLayout::Info info;

    layout->getChildInfo(child, info);

    std::string res =
      CStrUtil::strprintf("-in %s -anchor %s -expand %d -fill %s -ipadx %d -ipady %d "
                          "-padx %d -pady %d -side %s",
                          ".", "center", int(info.expand), info.getFillStr(),
                          info.ipadx, info.ipady, info.padx, info.pady,
                          info.getSideStr());

    return CTclValueRef(tk_->getTcl()->createValue(res));
  }
  else {
    CTkPackLayout::Side      side     = CTkPackLayout::SIDE_NONE;
    CTkPackLayout::Fill      fill     = CTkPackLayout::FILL_NONE;
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
      const auto &arg = args[i]->toString();

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
      if      ((*p).second.s == "x"   ) fill = CTkPackLayout::FILL_X;
      else if ((*p).second.s == "y"   ) fill = CTkPackLayout::FILL_Y;
      if      ((*p).second.s == "both") fill = CTkPackLayout::FILL_BOTH;
    }
    }

    {
    auto p = optValues.find("-expand");

    if (p != optValues.end()) {
      if ((*p).second.s == "1" || (*p).second.s == "yes" || (*p).second.s == "true")
        expand = true;
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
      if      ((*p).second.s == "left"  ) side = CTkPackLayout::SIDE_LEFT;
      else if ((*p).second.s == "right" ) side = CTkPackLayout::SIDE_RIGHT;
      if      ((*p).second.s == "bottom") side = CTkPackLayout::SIDE_BOTTOM;
      else if ((*p).second.s == "top"   ) side = CTkPackLayout::SIDE_TOP;
    }
    else {
      if (! expand && fill == CTkPackLayout::FILL_NONE)
        side = CTkPackLayout::SIDE_TOP;
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

    CTkPackLayout::Info info(side, fill, expand, padx, pady, ipadx, ipady);

    layout->addWidgets(children, info);

    return CTclValueRef();
  }
}

//----------

CTkPanedWindowCommand::
CTkPanedWindowCommand(CTk *tk) :
 CTkCommand(tk, "panedwindow")
{
}

CTclValueRef
CTkPanedWindowCommand::
exec(const std::vector<CTclValueRef> &args)
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
    const auto &name = args[i]->toString();

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

  tk_->getTcl()->addCommand(cmd);

  cmd->processArgs(args);

  return CTclValueRef(tk_->getTcl()->createValue(widgetName));
}

//----------

CTkPlaceCommand::
CTkPlaceCommand(CTk *tk) :
 CTkCommand(tk, "place")
{
}

CTclValueRef
CTkPlaceCommand::
exec(const std::vector<CTclValueRef> &args)
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

  const auto &arg = args[0]->toString();

  if      (arg == "configure") {
    return CTclValueRef();
  }
  else if (arg == "forget") {
    return CTclValueRef();
  }
  else if (arg == "info") {
    if (numArgs != 2)
      return tk_->wrongNumArgs("place info window");

    const auto &arg = args[1]->toString();

    CTkWidget*  parent;
    std::string childName;

    if (! root()->decodeWidgetName(arg, &parent, childName))
      return tk_->throwError("Invalid name '" + arg + "'");

    auto *child = parent->getChild(childName);

    if (child == nullptr)
      return tk_->throwError("bad window path name \"" + arg + "\"");

    auto *layout = parent->getTkPlaceLayout();

    CTkPlaceLayout::Info info;

    layout->getChildInfo(child, info);

    std::string res;

    return CTclValueRef(tk_->getTcl()->createValue(res));
  }
  else if (arg == "slaves") {
    return CTclValueRef();
  }
  else {
    CTkWidget*               parent = nullptr;
    std::vector<CTkWidget *> children;

    CTkOptionValueMap optValues;

    for (uint i = 0; i < numArgs; ++i) {
      const auto &arg = args[i]->toString();

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

    CTkPlaceLayout::Info info;

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

    return CTclValueRef();
  }
}

//----------

CTkRadioButtonCommand::
CTkRadioButtonCommand(CTk *tk) :
 CTkCommand(tk, "radiobutton")
{
}

CTclValueRef
CTkRadioButtonCommand::
exec(const std::vector<CTclValueRef> &args)
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
    const auto &name = args[i]->toString();

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

  tk_->getTcl()->addCommand(cmd);

  cmd->processArgs(args);

  return CTclValueRef(tk_->getTcl()->createValue(widgetName));
}

//----------

CTkRaiseCommand::
CTkRaiseCommand(CTk *tk) :
 CTkCommand(tk, "raise")
{
}

CTclValueRef
CTkRaiseCommand::
exec(const std::vector<CTclValueRef> &args)
{
  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("raise window ?aboveThis?");

  const auto &widgetName = args[0]->toString();

  CTkWidget*  parent = nullptr;
  std::string childName;

  if (! root()->decodeWidgetName(widgetName, &parent, childName))
    return tk_->throwError("Invalid name '" + widgetName + "'");

  auto *child = parent->getChild(childName);

  if (child == nullptr)
    return tk_->throwError("bad window path name \"" + widgetName + "\"");

  child->raise();

  return CTclValueRef();
}

//----------

CTkScaleCommand::
CTkScaleCommand(CTk *tk) :
 CTkCommand(tk, "scale")
{
}

CTclValueRef
CTkScaleCommand::
exec(const std::vector<CTclValueRef> &args)
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
    const auto &name = args[i]->toString();

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

  tk_->getTcl()->addCommand(cmd);

  cmd->processArgs(args);

  return CTclValueRef(tk_->getTcl()->createValue(widgetName));
}

//----------

CTkScrollBarCommand::
CTkScrollBarCommand(CTk *tk) :
 CTkCommand(tk, "scrollbar")
{
}

CTclValueRef
CTkScrollBarCommand::
exec(const std::vector<CTclValueRef> &args)
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
    const auto &name = args[i]->toString();

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

  tk_->getTcl()->addCommand(cmd);

  cmd->processArgs(args);

  return CTclValueRef(tk_->getTcl()->createValue(widgetName));
}

//----------

CTkSpinBoxCommand::
CTkSpinBoxCommand(CTk *tk) :
 CTkCommand(tk, "spinbox")
{
}

CTclValueRef
CTkSpinBoxCommand::
exec(const std::vector<CTclValueRef> &args)
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
    const auto &name = args[i]->toString();

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

  tk_->getTcl()->addCommand(cmd);

  cmd->processArgs(args);

  return CTclValueRef(tk_->getTcl()->createValue(widgetName));
}

//----------

CTkTextCommand::
CTkTextCommand(CTk *tk) :
 CTkCommand(tk, "text")
{
}

CTclValueRef
CTkTextCommand::
exec(const std::vector<CTclValueRef> &args)
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
    const auto &name = args[i]->toString();

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

  tk_->getTcl()->addCommand(cmd);

  cmd->processArgs(args);

  return CTclValueRef(tk_->getTcl()->createValue(widgetName));
}

//----------

CTkTopLevelCommand::
CTkTopLevelCommand(CTk *tk) :
 CTkCommand(tk, "toplevel")
{
}

CTclValueRef
CTkTopLevelCommand::
exec(const std::vector<CTclValueRef> &args)
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
    const auto &name = args[i]->toString();

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

  tk_->getTcl()->addCommand(cmd);

  cmd->processArgs(args);

  tk_->addTopLevel(toplevel);

  return CTclValueRef(tk_->getTcl()->createValue(widgetName));
}

//----------

CTkWInfoCommand::
CTkWInfoCommand(CTk *tk) :
 CTkCommand(tk, "winfo")
{
}

CTclValueRef
CTkWInfoCommand::
exec(const std::vector<CTclValueRef> &args)
{
  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs("winfo option ?arg ...?");

  const auto &arg = args[0]->toString();

  if      (arg == "atom") {
  }
  else if (arg == "atomname") {
  }
  else if (arg == "cells") {
  }
  else if (arg == "children") {
    if (numArgs < 2)
      return tk_->wrongNumArgs("winfo children window");

    std::string widgetName = args[1]->toString();

    auto *w = tk_->lookupWidgetByName(widgetName);

    if (w) {
      std::vector<CTkWidget *> children;

      w->getChildren(children);

      uint numChildren = children.size();

      auto *list = new CTclList;

      for (uint i = 0; i < numChildren; ++i) {
        auto *child = children[i];

        list->addValue(tk_->getTcl()->createValue(child->getFullName()));
      }

      return CTclValueRef(list);
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

    std::string widgetName = args[1]->toString();

    auto *w = tk_->lookupWidgetByName(widgetName);

    if (w) {
      auto *parent = w->getParent();

      if (parent)
        return CTclValueRef(tk_->getTcl()->createValue(parent->getFullName()));
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
  else if (arg == "vrooty") {
  }
  else if (arg == "width") {
  }
  else if (arg == "x") {
  }
  else if (arg == "y") {
  }

  return CTclValueRef();
}

//----------

CTkWmCommand::
CTkWmCommand(CTk *tk) :
 CTkCommand(tk, "wm")
{
}

CTclValueRef
CTkWmCommand::
exec(const std::vector<CTclValueRef> &args)
{
  uint numArgs = args.size();

  if (numArgs < 2)
    return tk_->wrongNumArgs("wm option window ?arg ...?");

  const auto &arg  = args[0]->toString();
  const auto &name = args[1]->toString();

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
  else if (arg == "title") {
    if (numArgs == 3) {
      const auto &title = args[2]->toString();

      w->setTitle(title);

      return CTclValueRef();
    }
    else {
      const auto &title = w->getTitle();

      return CTclValueRef(tk_->getTcl()->createValue(title));
    }
  }
  else
    return tk_->throwError("ambiguous option \"" + arg + "\": must be "
      "aspect, attributes, client, colormapwindows, command, deiconify, focusmodel, "
      "forget, frame, geometry, grid, group, iconbitmap, iconify, iconmask, iconname, "
      "iconphoto, iconposition, iconwindow, manage, maxsize, minsize, overrideredirect, "
      "positionfrom, protocol, resizable, sizefrom, stackorder, state, title, "
      "transient, or withdraw");

  return CTclValueRef();
}

//----------

CTkRootWidgetCommand::
CTkRootWidgetCommand(CTk *tk) :
 CTkCommand(tk, "."), opts_(tk)
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

CTclValueRef
CTkRootWidgetCommand::
exec(const std::vector<CTclValueRef> &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(". option ?arg arg ...?");

  const auto &arg = args[0]->toString();

  if (arg == "configure" || arg == "config") {
    if (numArgs == 1)
      return opts_.getOpts();
    else {
      for (uint i = 1; i < numArgs; i += 2) {
        const auto &name  = args[i + 0]->toString();
        const auto &value = args[i + 1]->toString();

        const CTkOpt *opt;

        if (! opts_.setOptValue(name, value, &opt))
          return tk_->throwError("unknown option \"" + name + "\"");

        root()->notifyValueChanged(opt->name, value);
      }
    }

    return CTclValueRef();
  }
  else
    return tk_->throwError("bad option \"" + arg + "\": must be cget or configure");
}

//----------

CTkWidgetCommand::
CTkWidgetCommand(CTkCommand *command, const std::string &name, CTkWidget *w, const CTkOpt *opts) :
 CTkCommand(command->getTk(), name), command_(command), w_(w), opts_(command->getTk())
{
  if (opts) opts_.init(opts);
}

CTclValueRef
CTkWidgetCommand::
exec(const std::vector<CTclValueRef> &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg arg ...?");

  const auto &arg = args[0]->toString();

  if      (arg == "configure" || arg == "config") {
    if (numArgs == 1)
      return opts_.getOpts();
    else {
      for (uint i = 1; i < numArgs; i += 2) {
        const auto &name  = args[i + 0]->toString();
        const auto &value = args[i + 1]->toString();

        if (! setOptValue(name, value))
          return tk_->throwError("unknown option \"" + name + "\"");
      }
    }

    return CTclValueRef();
  }
  else if (arg == "cget") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " cget option");

    const auto &name = args[1]->toString();

    std::string value;

    if (! getOptValue(name, value))
      return tk_->throwError("unknown option \"" + name + "\"");

    return CTclValueRef(tk_->getTcl()->createValue(value));
  }
  else {
    return w_->iexec(args);
  }
}

bool
CTkWidgetCommand::
processArgs(const std::vector<CTclValueRef> &args)
{
  auto *tk = command_->getTk();

  uint numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    const auto &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      if (i < numArgs) {
        const auto &value = args[i]->toString();

        if (! setOptValue(name, value))
          tk->throwError("unknown option \"" + name + "\"");
      }
      else
        tk->throwError("missing option value for \"" + name + "\"");
    }
  }

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

  if (! w_->notifyValueChanged(opt->name, value))
    w_->CTkWidget::notifyValueChanged(opt->name, value);

  return true;
}

//----------

CTkCommand::
CTkCommand(CTk *tk, const std::string &name) :
 CTclCommand(tk->getTcl(), name), tk_(tk)
{
}

CTkRootWidget *
CTkCommand::
root() const
{
  return tk_->getRootWidget();
}

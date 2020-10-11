#include <CTk.h>
#include <CTkLayout.h>
#include <CStrUtil.h>
#include <CQImage.h>
#include <CQUtil.h>
#include <CStrMap.h>
#include <CRGBName.h>

#include <QApplication>
#include <QCheckBox>
#include <QFrame>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QKeyEvent>

CTk::
CTk(CTcl *tcl) :
 tcl_(tcl)
{
  tcl_->addCommand(new CTkBellCommand       (this));
  tcl_->addCommand(new CTkBindCommand       (this));
//tcl_->addCommand(new CTkBindTagsCommand   (this));
//tcl_->addCommand(new CTkBitmapCommand     (this));
  tcl_->addCommand(new CTkButtonCommand     (this));
  tcl_->addCommand(new CTkCanvasCommand     (this));
  tcl_->addCommand(new CTkCheckButtonCommand(this));
//tcl_->addCommand(new CTkClipBoardCommand  (this));
//tcl_->addCommand(new CTkConsoleCommand    (this));
  tcl_->addCommand(new CTkDestroyCommand    (this));
  tcl_->addCommand(new CTkEntryCommand      (this));
//tcl_->addCommand(new CTkEventCommand      (this));
  tcl_->addCommand(new CTkFocusCommand      (this));
//tcl_->addCommand(new CTkFontCommand       (this));
  tcl_->addCommand(new CTkFrameCommand      (this));
//tcl_->addCommand(new CTkGrabCommand       (this));
  tcl_->addCommand(new CTkGridCommand       (this));
  tcl_->addCommand(new CTkImageCommand      (this));
  tcl_->addCommand(new CTkLabelCommand      (this));
  tcl_->addCommand(new CTkLabelFrameCommand (this));
  tcl_->addCommand(new CTkListBoxCommand    (this));
  tcl_->addCommand(new CTkLowerCommand      (this));
//tcl_->addCommand(new CTkMenuCommand       (this));
//tcl_->addCommand(new CTkMenuButtonCommand (this));
//tcl_->addCommand(new CTkMessageCommand    (this));
//tcl_->addCommand(new CTkOptionCommand     (this));
  tcl_->addCommand(new CTkPackCommand       (this));
  tcl_->addCommand(new CTkPanedWindowCommand(this));
//tcl_->addCommand(new CTkPhotoCommand      (this));
  tcl_->addCommand(new CTkPlaceCommand      (this));
  tcl_->addCommand(new CTkRadioButtonCommand(this));
  tcl_->addCommand(new CTkRaiseCommand      (this));
  tcl_->addCommand(new CTkScaleCommand      (this));
  tcl_->addCommand(new CTkScrollBarCommand  (this));
//tcl_->addCommand(new CTkSelectionCommand  (this));
//tcl_->addCommand(new CTkSendCommand       (this));
  tcl_->addCommand(new CTkSpinBoxCommand    (this));
  tcl_->addCommand(new CTkTextCommand       (this));
//tcl_->addCommand(new CTkTkCommand         (this));
//tcl_->addCommand(new CTkTkErrorCommand    (this));
//tcl_->addCommand(new CTkTkPopupCommand    (this));
//tcl_->addCommand(new CTkTkWaitCommand     (this));
  tcl_->addCommand(new CTkTopLevelCommand   (this));
  tcl_->addCommand(new CTkWInfoCommand      (this));
  tcl_->addCommand(new CTkWmCommand         (this));

  //----

  root_ = new CTkRootWidget(this);

  tcl_->addCommand(new CTkRootWidgetCommand(this));
}

void
CTk::
exec()
{
  if (toplevels_.empty())
    root_->show();
  else {
    uint num = toplevels_.size();

    for (uint i = 0; i < num; ++i)
      toplevels_[i]->show();
  }
}

void
CTk::
processEvents()
{
  while (QCoreApplication::hasPendingEvents())
    QCoreApplication::processEvents();

  purgeWidgets();
}

CTkWidget *
CTk::
lookupWidgetByName(const std::string &widgetName)
{
  if (widgetName == ".")
    return getRootWidget();

  CTkWidget*  parent = 0;
  std::string childName;

  if (! decodeWidgetName(widgetName, &parent, childName)) {
    getTcl()->throwError("Invalid name '" + widgetName + "'");
    return 0;
  }

  CTkWidget *child = parent->getChild(childName);

  if (child == 0) {
    getTcl()->throwError("bad window path name \"" + childName + "\"");
    return 0;
  }

  return child;
}

bool
CTk::
decodeWidgetName(const std::string &name, CTkWidget **parent, std::string &childName)
{
  uint len = name.size();

  if (len == 0) return false;

  if (name[0] != '.') return false;

  if (len == 1) {
    *parent = root_;

    childName = "";

    return true;
  }

  CTkWidget *parent1 = root_;
  CTkWidget *child1  = 0;

  uint pos1 = 1;
  uint pos2 = pos1;

  while (pos2 < len) {
    while (pos2 < len) {
      if (name[pos2] == '.')
        break;

      ++pos2;
    }

    if (pos2 < len)
      childName = name.substr(pos1, pos2 - pos1);
    else
      childName = name.substr(pos1);

    if (childName == "")
      return false;

    if (pos2 >= len)
      break;

    child1 = parent1->getChild(childName);

    if (child1 == 0)
      return false;

    parent1 = child1;
    child1  = 0;

    pos1 = pos2 + 1;
    pos2 = pos1;
  }

  *parent = parent1;

  return true;
}

bool
CTk::
processOption(CTkOption *opts, const std::vector<CTclValueRef> &args, uint &i,
              CTkOptionValueMap &values)
{
  uint numArgs = args.size();

  const std::string &arg = args[i]->toString();

  for (uint j = 0; opts[j].name != 0; ++j) {
    const CTkOption &option = opts[j];

    if (arg != option.name && (option.alias == 0 || arg != option.alias))
      continue;

    CTkOptionValue &value = values[arg];

    if      (option.type == CTkOptionType::Flag)
      value.i = 1;
    else if (option.type == CTkOptionType::String) {
      if (i < numArgs - 1)
        value.s = args[++i]->toString();
      else
        return false;
    }
    else if (option.type == CTkOptionType::Int) {
      if (i < numArgs - 1) {
        bool ok = args[++i]->toInt(value.i);

        if (! ok)
          return false;
      }
      else
        return false;
    }
    else if (option.type == CTkOptionType::Real) {
      if (i < numArgs - 1) {
        bool ok = args[++i]->toReal(value.r);

        if (! ok)
          return false;
      }
      else
        return false;
    }

    return true;
  }

  return false;
}

void
CTk::
addTopLevel(CTkTopLevel *toplevel)
{
  toplevels_.push_back(toplevel);
}

std::string
CTk::
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
CTk::
createImage(const std::string &, const std::string &name)
{
  CTkImage *image = new CTkImage(name);

  CTkImageRef pimage(image);

  images_[name] = pimage;

  return pimage;
}

CTkImageRef
CTk::
getImage(const std::string &name) const
{
  ImageMap::const_iterator p = images_.find(name);

  if (p == images_.end())
    return CTkImageRef();

  return (*p).second;
}

void
CTk::
bindEvent(const std::string &tagName, const std::string &pattern, const std::string &command)
{
  events_[tagName][pattern] = command;
}

bool
CTk::
triggerEnterEvents(const char *className, CTkWidget *w, QEvent *e)
{
  TagEventMap::const_iterator p1 = events_.find(className);

  if (p1 != events_.end()) {
    EventMap::const_iterator p2 = (*p1).second.find("<Enter>");

    if (p2 != (*p1).second.end())
      return execEvent(w, e, (*p2).second);
  }

  return false;
}

bool
CTk::
triggerLeaveEvents(const char *className, CTkWidget *w, QEvent *e)
{
  TagEventMap::const_iterator p1 = events_.find(className);

  if (p1 != events_.end()) {
    EventMap::const_iterator p2 = (*p1).second.find("<Leave>");

    if (p2 != (*p1).second.end())
      return execEvent(w, e, (*p2).second);
  }

  return false;
}

bool
CTk::
triggerKeyPressEvents(const char *className, CTkWidget *w, QEvent *e, const std::string &str)
{
  TagEventMap::const_iterator p1 = events_.find(className);

  if (p1 != events_.end()) {
    EventMap::const_iterator p2 = (*p1).second.find(str);

    if (p2 != (*p1).second.end())
      return execEvent(w, e, (*p2).second);
  }

  p1 = events_.find("all");

  if (p1 != events_.end()) {
    EventMap::const_iterator p2 = (*p1).second.find(str);

    if (p2 != (*p1).second.end())
      return execEvent(w, e, (*p2).second);
  }

  return false;
}

bool
CTk::
execEvent(CTkWidget *w, QEvent *e, const std::string &str)
{
  std::string str1 = str;

  std::string::size_type pos1 = str1.find("%W");

  if (pos1 != std::string::npos)
    str1 = str1.substr(0, pos1) + w->getFullName() + str1.substr(pos1 + 2);

  QEvent::Type type = e->type();

  if (type == QEvent::MouseButtonPress) {
    QMouseEvent *me = dynamic_cast<QMouseEvent *>(e);

    std::string::size_type pos1 = str1.find("%x");

    if (pos1 != std::string::npos)
      str1 = str1.substr(0, pos1) + CStrUtil::toString(me->x()) + str1.substr(pos1 + 2);

    std::string::size_type pos2 = str1.find("%y");

    if (pos2 != std::string::npos)
      str1 = str1.substr(0, pos2) + CStrUtil::toString(me->y()) + str1.substr(pos2 + 2);

    std::string::size_type pos3 = str1.find("%b");

    if (pos3 != std::string::npos)
      str1 = str1.substr(0, pos3) + CStrUtil::toString(me->button()) + str1.substr(pos3 + 2);
  }

  getTcl()->parseLine(str1);

  return true;
}

void
CTk::
addWidget(CTkWidget *w)
{
  widgets_.insert(w);
}

CTkWidget *
CTk::
lookupWidget(QWidget *w) const
{
  WidgetSet::const_iterator p1, p2;

  for (p1 = widgets_.begin(), p2 = widgets_.end(); p1 != p2; ++p1) {
    CTkWidget *tw = *p1;

    if (tw->getQWidget() == w)
      return tw;
  }

  return 0;
}

void
CTk::
removeWidget(CTkWidget *w)
{
  WidgetSet::iterator p = widgets_.find(w);

  if (p != widgets_.end())
    widgets_.erase(p);
}

void
CTk::
addDeleteWidget(CTkWidget *w)
{
  deleteWidgets_.push_back(w);
}

void
CTk::
purgeWidgets()
{
  uint num = deleteWidgets_.size();

  for (uint i = 0; i < num; ++i)
    delete deleteWidgets_[i];

  deleteWidgets_.clear();
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

  if (numArgs < 1 || numArgs > 3) {
    tk_->getTcl()->wrongNumArgs("bind window ?pattern? ?command?");
    return CTclValueRef();
  }

  std::string widgetName = args[0]->toString();

  CTkWidget*  parent = 0;
  std::string childName;
  CTkWidget*  child  = 0;

  if (tk_->decodeWidgetName(widgetName, &parent, childName)) {
    child = parent->getChild(childName);

    if (child == 0) {
      tk_->getTcl()->throwError("bad window path name \"" + childName + "\"");
      return CTclValueRef();
    }
  }

  if (numArgs > 1) {
    std::string pattern = args[1]->toString();

    if (numArgs > 2) {
      std::string command = args[2]->toString();

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
    { "-bd"                 , "-borderwidth"       , 0                    , 0               },
    { "-bg"                 , "-background"        , 0                    , 0               },
    { "-bitmap"             , "bitmap"             , "Bitmap"             , ""              },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
    { "-command"            , "command"            , "Command"            , ""              },
    { "-compound"           , "compound"           , "Compound"           , "none"          },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-default"            , "default"            , "Default"            , "disabled"      },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"       },
    { "-fg"                 , "-foreground"        , 0                    , 0               },
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
    { 0                     , 0                    , 0                    , 0               }
  };

  uint numArgs = args.size();

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const std::string &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "") {
    tk_->getTcl()->throwError("No name");
    return CTclValueRef();
  }

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkButton *button = new CTkButton(tk_, parent, childName);

  CQUtil::setBackground(button->getQWidget(), QColor("#d9d9d9"));

  CTkWidgetCommand *cmd = new CTkWidgetCommand(this, widgetName, button, opts);

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
    { "-bd"                 , "borderWidth"        , 0                    , 0         },
    { "-bg"                 , "background"         , 0                    , 0         },
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
    { 0                     , 0                    , 0                    , 0         }
  };

  uint numArgs = args.size();

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const std::string &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "") {
    tk_->getTcl()->throwError("No name");
    return CTclValueRef();
  }

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkCanvas *canvas = new CTkCanvas(tk_, parent, childName);

  CQUtil::setBackground(canvas->getQWidget(), QColor("#d9d9d9"));

  CTkWidgetCommand *cmd = new CTkWidgetCommand(this, widgetName, canvas, opts);

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
    { "-bd"                 , "-borderwidth"       , 0                    , 0               },
    { "-bg"                 , "-background"        , 0                    , 0               },
    { "-bitmap"             , "bitmap"             , "Bitmap"             , ""              },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
    { "-command"            , "command"            , "Command"            , ""              },
    { "-compound"           , "compound"           , "Compound"           , "none"          },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"       },
    { "-fg"                 , "-foreground"        , 0                    , 0               },
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
    { 0                     , 0                    , 0                    , 0               }
  };

  uint numArgs = args.size();

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const std::string &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "") {
    tk_->getTcl()->throwError("No name");
    return CTclValueRef();
  }

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkCheckButton *check = new CTkCheckButton(tk_, parent, childName);

  CQUtil::setBackground(check->getQWidget(), QColor("#d9d9d9"));

  CTkWidgetCommand *cmd = new CTkWidgetCommand(this, widgetName, check, opts);

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
    const std::string &widgetName = args[i]->toString();

    CTkWidget *child = 0;

    if (widgetName != ".") {
      CTkWidget*  parent = 0;
      std::string childName;

      if (! tk_->decodeWidgetName(widgetName, &parent, childName))
        continue;

      CTkWidget *child = parent->getChild(childName);

      if (child == 0)
        continue;
    }
    else
      child = tk_->getRootWidget();

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
    { "-bd"                 , "-borderwidth"       , 0                    , 0            },
    { "-bg"                 , "-background"        , 0                    , 0            },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"          },
    { "-cursor"             , "cursor"             , "Cursor"             , "xterm"      },
    { "-disabledbackground" , "disabledBackground" , "DisabledBackground" , "#d9d9d9"    },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"    },
    { "-exportselection"    , "exportSelection"    , "ExportSelection"    , "1"          },
    { "-fg"                 , "-foreground"        , 0                    , 0            },
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
    { "-invcmd"             , "-invalidcommand"    , 0                    , 0            },
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
    { "-vcmd"               , "-validatecommand"   , 0                    , 0            },
    { "-width"              , "width"              , "Width"              , "20"         },
    { "-xscrollcommand"     , "xScrollCommand"     , "ScrollCommand"      , ""           },
    { 0                     , 0                    , 0                    , 0            }
  };

  uint numArgs = args.size();

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const std::string &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "") {
    tk_->getTcl()->throwError("No name");
    return CTclValueRef();
  }

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkEntry *entry = new CTkEntry(tk_, parent, childName);

  CQUtil::setBackground(entry->getQWidget(), QColor("#ffffff"));

  CTkWidgetCommand *cmd = new CTkWidgetCommand(this, widgetName, entry, opts);

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

    if (! w)
      return CTclValueRef();

    CTkWidget *tw = tk_->lookupWidget(w);

    if (! tw)
      return CTclValueRef();

    return CTclValueRef(tk_->getTcl()->createValue(tw->getFullName()));
  }
  else if (numArgs == 1) {
    std::string widgetName = args[0]->toString();

    CTkWidget *w = tk_->lookupWidgetByName(widgetName);

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
      else {
        tk_->getTcl()->throwError("bad option \"" + arg + "\": must be "
                                  "-displayof, -force, or -lastfor");
        return CTclValueRef();
      }
    }

    std::string widgetName = args[numArgs - 1]->toString();

    CTkWidget *w = tk_->lookupWidgetByName(widgetName);

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
    { "-bd"                 , "-borderwidth"       , 0                    , 0         },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "0"       },
    { "-class"              , "class"              , "Class"              , "Frame"   },
    { "-relief"             , "relief"             , "Relief"             , "flat"    },
    { "-background"         , "background"         , "Background"         , "#d9d9d9" },
    { "-bg"                 , "-background"        , 0                    , 0         },
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
    { 0                     , 0                    , 0                    , 0         }
  };

  uint numArgs = args.size();

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const std::string &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "") {
    tk_->getTcl()->throwError("No name");
    return CTclValueRef();
  }

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkFrame *frame = new CTkFrame(tk_, parent, childName);

  CQUtil::setBackground(frame->getQWidget(), QColor("#d9d9d9"));

  CTkWidgetCommand *cmd = new CTkWidgetCommand(this, widgetName, frame, opts);

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
    { "-column"    , CTkOptionType::Int   , 0 },
    { "-columnspan", CTkOptionType::Int   , 0 },
    { "-in"        , CTkOptionType::String, 0 },
    { "-ipadx"     , CTkOptionType::Int   , 0 },
    { "-ipady"     , CTkOptionType::Int   , 0 },
    { "-padx"      , CTkOptionType::Int   , 0 },
    { "-pady"      , CTkOptionType::Int   , 0 },
    { "-row"       , CTkOptionType::Int   , 0 },
    { "-rowspan"   , CTkOptionType::Int   , 0 },
    { "-sticky"    , CTkOptionType::String, 0 },
    { 0            , CTkOptionType::None  , 0 },
  };

  uint numArgs = args.size();

  if (numArgs < 1) {
    tk_->getTcl()->wrongNumArgs("pack option arg ?arg ...?");
    return CTclValueRef();
  }

  const std::string &arg = args[0]->toString();

  if      (arg == "anchor") {
    return CTclValueRef();
  }
  else if (arg == "bbox") {
    return CTclValueRef();
  }
  else if (arg == "columnconfigure") {
    if (numArgs < 3) {
      tk_->getTcl()->wrongNumArgs("grid rowconfigure master index ?-option value...?");
      return CTclValueRef();
    }

    const std::string &masterName = args[1]->toString();
    const std::string &indexName  = args[2]->toString();

    CTkWidget *master = tk_->lookupWidgetByName(masterName);
    if (master == 0) return CTclValueRef();

    CTkWidget *index = tk_->lookupWidgetByName(indexName);
    if (index == 0) return CTclValueRef();

    CTkGridLayout *layout = master->getTkGridLayout();
    if (layout == 0) return CTclValueRef();

    for (uint i = 3; i < numArgs - 1; i += 2) {
      const std::string &name = args[i]->toString();

      CTclValueRef value = args[i + 1];

      if      (name == "-minsize") {
      }
      else if (name == "-pad") {
      }
      else if (name == "-uniform") {
      }
      else if (name == "-weight") {
        int weight;

        if (! value->checkInt(tk_->getTcl(), weight))
          return CTclValueRef();

        layout->setChildWeight(index, weight);
      }
      else {
        tk_->getTcl()->throwError("bad option \"" + name + "\": must be "
                                  "-minsize, -pad, -uniform, or -weight");
        return CTclValueRef();
      }
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
    if (numArgs != 2) {
      tk_->getTcl()->wrongNumArgs("grid info window");
      return CTclValueRef();
    }

    const std::string &arg = args[1]->toString();

    CTkWidget *child = tk_->lookupWidgetByName(arg);
    if (child == 0) return CTclValueRef();

    CTkGridLayout *layout = child->getParent()->getTkGridLayout();
    if (layout == 0) return CTclValueRef();

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
    CTkWidget*               parent   = 0;
    std::vector<CTkWidget *> children;

    CTkOptionValueMap optValues;

    for (uint i = 0; i < numArgs; ++i) {
      const std::string &arg = args[i]->toString();

      if (arg.size() > 0 && arg[0] == '-') {
        if (! tk_->processOption(opts, args, i, optValues))
          std::cerr << "Unhandled option '" << arg << "'" << std::endl;
      }
      else {
        CTkWidget*  parent1;
        std::string childName;

        if (! tk_->decodeWidgetName(arg, &parent1, childName)) {
          tk_->getTcl()->throwError("Invalid name '" + arg + "'");
          continue;
        }

        CTkWidget *child = parent1->getChild(childName);

        if (child) {
          if (parent == 0)
            parent = parent1;

          children.push_back(child);
        }
      }
    }

    CTkGridLayout::Info info;

    {
    CTkOptionValueMap::const_iterator p = optValues.find("-row");
    if (p != optValues.end()) info.setRow((*p).second.i);
    }

    {
    CTkOptionValueMap::const_iterator p = optValues.find("-column");
    if (p != optValues.end()) info.setCol((*p).second.i);
    }

    {
    CTkOptionValueMap::const_iterator p = optValues.find("-sticky");
    if (p != optValues.end()) info.setSticky((*p).second.s);
    }

    {
    CTkOptionValueMap::const_iterator p = optValues.find("-padx");
    if (p != optValues.end()) info.setPadX((*p).second.i);
    }

    {
    CTkOptionValueMap::const_iterator p = optValues.find("-pady");
    if (p != optValues.end()) info.setPadY((*p).second.i);
    }

    {
    CTkOptionValueMap::const_iterator p = optValues.find("-ipadx");
    if (p != optValues.end()) info.setIPadX((*p).second.i);
    }

    {
    CTkOptionValueMap::const_iterator p = optValues.find("-ipady");
    if (p != optValues.end()) info.setIPadY((*p).second.i);
    }

    CTkGridLayout *layout = parent->getTkGridLayout();

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

  if (numArgs < 1) {
    tk_->getTcl()->wrongNumArgs("image option ?args?");
    return CTclValueRef();
  }

  const std::string &name = args[0]->toString();

  if      (name == "create") {
    if (numArgs < 2) {
      tk_->getTcl()->wrongNumArgs("image create type ?name? ?options?");
      return CTclValueRef();
    }

    const std::string &type = args[1]->toString();

    if (type != "photo" && type != "bitmap") {
      tk_->getTcl()->throwError("image type \"" + type + "\" doesn't exist");
      return CTclValueRef();
    }

    uint ind = 2;

    std::string name;

    if (numArgs > 2) {
      name = args[2]->toString();

      ++ind;
    }
    else
      name = tk_->getNewImageName();

    CTkImageRef image = tk_->createImage(type, name);

    for (uint i = ind; i < numArgs; ++i) {
      const std::string &name = args[i]->toString();

      if (name == "-file") {
        ++i;

        if (i >= numArgs) {
          tk_->getTcl()->throwError("value for \"" + name + "\" missing");
          return CTclValueRef();
        }

        const std::string &value = args[i]->toString();

        image->loadFile(value);
      }
      else {
        tk_->getTcl()->throwError("unknown option \"" + name + "\"");
        return CTclValueRef();
      }
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
    if (numArgs != 1) {
      tk_->getTcl()->wrongNumArgs("image types");
      return CTclValueRef();
    }

    CTclList *list = new CTclList;

    list->addValue(tk_->getTcl()->createValue("photo"));
    list->addValue(tk_->getTcl()->createValue("bitmap"));

    return CTclValueRef(list);
  }
  else if (name == "width") {
  }
  else {
    tk_->getTcl()->throwError("bad option \"" + name + "\": must be "
                              "create, delete, height, inuse, names, type, types, or width");
    return CTclValueRef();
  }

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
    { "-bd"                 , "-borderwidth"       , 0                    , 0               },
    { "-bg"                 , "-background"        , 0                    , 0               },
    { "-bitmap"             , "bitmap"             , "Bitmap"             , ""              },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
    { "-compound"           , "compound"           , "Compound"           , "none"          },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"       },
    { "-fg"                 , "-foreground"        , 0                    , 0               },
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
    { 0                     , 0                    , 0                    , 0               }
  };

  uint numArgs = args.size();

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const std::string &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "") {
    tk_->getTcl()->throwError("No name");
    return CTclValueRef();
  }

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkLabel *label = new CTkLabel(tk_, parent, childName);

  CQUtil::setBackground(label->getQWidget(), QColor("#d9d9d9"));

  CTkWidgetCommand *cmd = new CTkWidgetCommand(this, widgetName, label, opts);

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
    { "-bd"                 , "-borderwidth"       , 0                    , 0               },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "2"             },
    { "-class"              , "class"              , "Class"              , "Labelframe"    },
    { "-fg"                 , "-foreground"        , 0                    , 0               },
    { "-font"               , "font"               , "Font"               , "TkDefaultFont" },
    { "-foreground"         , "foreground"         , "Foreground"         , "#000000"       },
    { "-labelanchor"        , "labelAnchor"        , "LabelAnchor"        , "nw"            },
    { "-labelwidget"        , "labelWidget"        , "LabelWidget"        , ""              },
    { "-relief"             , "relief"             , "Relief"             , "groove"        },
    { "-text"               , "text"               , "Text"               , ""              },
    { "-background"         , "background"         , "Background"         , "#d9d9d9"       },
    { "-bg"                 , "-background"        , 0                    , 0               },
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
    { 0                     , 0                    , 0                    , 0               }
  };

  uint numArgs = args.size();

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const std::string &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "") {
    tk_->getTcl()->throwError("No name");
    return CTclValueRef();
  }

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkLabelFrame *frame = new CTkLabelFrame(tk_, parent, childName);

  CQUtil::setBackground(frame->getQWidget(), QColor("#d9d9d9"));

  CTkWidgetCommand *cmd = new CTkWidgetCommand(this, widgetName, frame, opts);

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
    { "-bd"                 , "-borderwidth"       , 0                    , 0               },
    { "-bg"                 , "-background"        , 0                    , 0               },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"       },
    { "-exportselection"    , "exportSelection"    , "ExportSelection"    , "1"             },
    { "-fg"                 , "-foreground"        , 0                    , 0               },
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
    { 0                     , 0                    , 0                    , 0               }
  };

  uint numArgs = args.size();

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const std::string &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "") {
    tk_->getTcl()->throwError("No name");
    return CTclValueRef();
  }

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkListBox *list = new CTkListBox(tk_, parent, childName);

  CQUtil::setBackground(list->getQWidget(), QColor("#ffffff"));

  CTkWidgetCommand *cmd = new CTkWidgetCommand(this, widgetName, list, opts);

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

  if (numArgs < 1) {
    tk_->getTcl()->wrongNumArgs("lower window ?belowThis?");
    return CTclValueRef();
  }

  const std::string &widgetName = args[0]->toString();

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkWidget *child = parent->getChild(childName);

  if (child == 0) {
    tk_->getTcl()->throwError("bad window path name \"" + widgetName + "\"");
    return CTclValueRef();
  }

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
    { "-after" , CTkOptionType::String, 0 },
    { "-anchor", CTkOptionType::String, 0 },
    { "-before", CTkOptionType::String, 0 },
    { "-expand", CTkOptionType::String, 0 },
    { "-fill"  , CTkOptionType::String, 0 },
    { "-in"    , CTkOptionType::String, 0 },
    { "-ipadx" , CTkOptionType::Int   , 0 },
    { "-ipady" , CTkOptionType::Int   , 0 },
    { "-padx"  , CTkOptionType::Int   , 0 },
    { "-pady"  , CTkOptionType::Int   , 0 },
    { "-side"  , CTkOptionType::String, 0 },
    { 0        , CTkOptionType::None  , 0 },
  };

  uint numArgs = args.size();

  if (numArgs < 1) {
    tk_->getTcl()->wrongNumArgs("pack option arg ?arg ...?");
    return CTclValueRef();
  }

  const std::string &arg = args[0]->toString();

  if      (arg == "propagate") {
    return CTclValueRef();
  }
  else if (arg == "info") {
    if (numArgs != 2) {
       tk_->getTcl()->wrongNumArgs("pack info window");
      return CTclValueRef();
    }

    const std::string &arg = args[1]->toString();

    CTkWidget*  parent;
    std::string childName;

    if (! tk_->decodeWidgetName(arg, &parent, childName)) {
      tk_->getTcl()->throwError("Invalid name '" + arg + "'");
      return CTclValueRef();
    }

    CTkWidget *child = parent->getChild(childName);

    if (child == 0) {
      tk_->getTcl()->throwError("bad window path name \"" + arg + "\"");
      return CTclValueRef();
    }

    CTkPackLayout *layout = parent->getTkPackLayout();

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
    CTkWidget*               inparent = 0;
    int                      padx     = 0;
    int                      pady     = 0;
    int                      ipadx    = 0;
    int                      ipady    = 0;
    CTkWidget*               parent   = 0;
    std::vector<CTkWidget *> children;

    CTkOptionValueMap optValues;

    for (uint i = 0; i < numArgs; ++i) {
      const std::string &arg = args[i]->toString();

      if (arg.size() > 0 && arg[0] == '-') {
        if (! tk_->processOption(opts, args, i, optValues))
          std::cerr << "Unhandled option '" << arg << "'" << std::endl;
      }
      else {
        CTkWidget*  parent1;
        std::string childName;

        if (! tk_->decodeWidgetName(arg, &parent1, childName)) {
          tk_->getTcl()->throwError("Invalid name '" + arg + "'");
          continue;
        }

        CTkWidget *child = parent1->getChild(childName);

        if (child) {
          if (parent == 0)
            parent = parent1;

          children.push_back(child);
        }
      }
    }

    {
    CTkOptionValueMap::const_iterator p = optValues.find("-fill");

    if (p != optValues.end()) {
      if      ((*p).second.s == "x"   ) fill = CTkPackLayout::FILL_X;
      else if ((*p).second.s == "y"   ) fill = CTkPackLayout::FILL_Y;
      if      ((*p).second.s == "both") fill = CTkPackLayout::FILL_BOTH;
    }
    }

    {
    CTkOptionValueMap::const_iterator p = optValues.find("-expand");

    if (p != optValues.end()) {
      if ((*p).second.s == "1" || (*p).second.s == "yes" || (*p).second.s == "true")
        expand = true;
    }
    }

    {
    CTkOptionValueMap::const_iterator p = optValues.find("-padx");

    if (p != optValues.end())
      padx = (*p).second.i;
    }

    {
    CTkOptionValueMap::const_iterator p = optValues.find("-pady");

    if (p != optValues.end())
      pady = (*p).second.i;
    }

    {
    CTkOptionValueMap::const_iterator p = optValues.find("-ipadx");

    if (p != optValues.end())
      ipadx = (*p).second.i;
    }

    {
    CTkOptionValueMap::const_iterator p = optValues.find("-ipady");

    if (p != optValues.end())
      ipady = (*p).second.i;
    }

    {
    CTkOptionValueMap::const_iterator p = optValues.find("-side");

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
    CTkOptionValueMap::const_iterator p = optValues.find("-in");

    if (p != optValues.end()) {
      CTkWidget*  parent1;
      std::string childName;

      if (tk_->decodeWidgetName((*p).second.s, &parent1, childName)) {
        CTkWidget *child = parent1->getChild(childName);

        if (child)
          inparent = child;
      }
    }
    }

    if (inparent)
      parent = inparent;

    CTkPackLayout *layout = parent->getTkPackLayout();

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
    { "-bd"          , "-borderwidth", 0             , 0            },
    { "-bg"          , "-background" , 0             , 0            },
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
    { 0              , 0             , 0             , 0            }
  };

  uint numArgs = args.size();

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const std::string &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "") {
    tk_->getTcl()->throwError("No name");
    return CTclValueRef();
  }

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkPanedWindow *pane = new CTkPanedWindow(tk_, parent, childName);

  CQUtil::setBackground(pane->getQWidget(), QColor("#d9d9d9"));

  CTkWidgetCommand *cmd = new CTkWidgetCommand(this, widgetName, pane, opts);

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
    { "-anchor"    , CTkOptionType::String, 0 },
    { "-bordermode", CTkOptionType::String, 0 },
    { "-height"    , CTkOptionType::Int   , 0 },
    { "-in"        , CTkOptionType::String, 0 },
    { "-relheight" , CTkOptionType::String, 0 },
    { "-relwidth"  , CTkOptionType::String, 0 },
    { "-relx"      , CTkOptionType::Real  , 0 },
    { "-rely"      , CTkOptionType::Real  , 0 },
    { "-width"     , CTkOptionType::Int   , 0 },
    { "-x"         , CTkOptionType::Int   , 0 },
    { "-y"         , CTkOptionType::Int   , 0 },
    { 0            , CTkOptionType::None  , 0 },
  };

  uint numArgs = args.size();

  if (numArgs < 1) {
    tk_->getTcl()->wrongNumArgs("place option|pathName args");
    return CTclValueRef();
  }

  const std::string &arg = args[0]->toString();

  if      (arg == "configure") {
    return CTclValueRef();
  }
  else if (arg == "forget") {
    return CTclValueRef();
  }
  else if (arg == "info") {
    if (numArgs != 2) {
       tk_->getTcl()->wrongNumArgs("place info window");
      return CTclValueRef();
    }

    const std::string &arg = args[1]->toString();

    CTkWidget*  parent;
    std::string childName;

    if (! tk_->decodeWidgetName(arg, &parent, childName)) {
      tk_->getTcl()->throwError("Invalid name '" + arg + "'");
      return CTclValueRef();
    }

    CTkWidget *child = parent->getChild(childName);

    if (child == 0) {
      tk_->getTcl()->throwError("bad window path name \"" + arg + "\"");
      return CTclValueRef();
    }

    CTkPlaceLayout *layout = parent->getTkPlaceLayout();

    CTkPlaceLayout::Info info;

    layout->getChildInfo(child, info);

    std::string res;

    return CTclValueRef(tk_->getTcl()->createValue(res));
  }
  else if (arg == "slaves") {
    return CTclValueRef();
  }
  else {
    CTkWidget*               parent   = 0;
    std::vector<CTkWidget *> children;

    CTkOptionValueMap optValues;

    for (uint i = 0; i < numArgs; ++i) {
      const std::string &arg = args[i]->toString();

      if (arg.size() > 0 && arg[0] == '-') {
        if (! tk_->processOption(opts, args, i, optValues))
          std::cerr << "Unhandled option '" << arg << "'" << std::endl;
      }
      else {
        CTkWidget*  parent1;
        std::string childName;

        if (! tk_->decodeWidgetName(arg, &parent1, childName)) {
          tk_->getTcl()->throwError("Invalid name '" + arg + "'");
          continue;
        }

        CTkWidget *child = parent1->getChild(childName);

        if (child) {
          if (parent == 0)
            parent = parent1;

          children.push_back(child);
        }
      }
    }

    CTkPlaceLayout::Info info;

    {
    CTkOptionValueMap::const_iterator p = optValues.find("-relx");
    if (p != optValues.end()) info.setRelX((*p).second.r);
    }

    {
    CTkOptionValueMap::const_iterator p = optValues.find("-rely");
    if (p != optValues.end()) info.setRelY((*p).second.r);
    }

    CTkPlaceLayout *layout = parent->getTkPlaceLayout();

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
    { "-bd"                 , "-borderwidth"       , 0                    , 0                },
    { "-bg"                 , "-background"        , 0                    , 0                },
    { "-bitmap"             , "bitmap"             , "Bitmap"             , ""               },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"              },
    { "-command"            , "command"            , "Command"            , ""               },
    { "-compound"           , "compound"           , "Compound"           , "none"           },
    { "-cursor"             , "cursor"             , "Cursor"             , ""               },
    { "-disabledforeground" , "disabledForeground" , "DisabledForeground" , "#a3a3a3"        },
    { "-fg"                 , "-foreground"        , 0                    , 0                },
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
    { 0                     , 0                    , 0                    , 0                }
  };

  uint numArgs = args.size();

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const std::string &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "") {
    tk_->getTcl()->throwError("No name");
    return CTclValueRef();
  }

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkRadioButton *radio = new CTkRadioButton(tk_, parent, childName);

  CQUtil::setBackground(radio->getQWidget(), QColor("#d9d9d9"));

  CTkWidgetCommand *cmd = new CTkWidgetCommand(this, widgetName, radio, opts);

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

  if (numArgs < 1) {
    tk_->getTcl()->wrongNumArgs("raise window ?aboveThis?");
    return CTclValueRef();
  }

  const std::string &widgetName = args[0]->toString();

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkWidget *child = parent->getChild(childName);

  if (child == 0) {
    tk_->getTcl()->throwError("bad window path name \"" + widgetName + "\"");
    return CTclValueRef();
  }

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
    { "-bd"                 , "-borderwidth"       , 0                    , 0               },
    { "-bg"                 , "-background"        , 0                    , 0               },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "1"             },
    { "-command"            , "command"            , "Command"            , ""              },
    { "-cursor"             , "cursor"             , "Cursor"             , ""              },
    { "-digits"             , "digits"             , "Digits"             , "0"             },
    { "-fg"                 , "-foreground"        , 0                    , 0               },
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
    { 0                     , 0                    , 0                    , 0               }
  };

  uint numArgs = args.size();

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const std::string &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "") {
    tk_->getTcl()->throwError("No name");
    return CTclValueRef();
  }

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkScale *scale = new CTkScale(tk_, parent, childName);

  CQUtil::setBackground(scale->getQWidget(), QColor("#d9d9d9"));

  CTkWidgetCommand *cmd = new CTkWidgetCommand(this, widgetName, scale, opts);

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
    { "-bd"                 , "borderWidth"        , 0                    , 0          },
    { "-bg"                 , "background"         , 0                    , 0          },
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
    { 0                     , 0                    , 0                    , 0          }
  };

  uint numArgs = args.size();

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const std::string &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "") {
    tk_->getTcl()->throwError("No name");
    return CTclValueRef();
  }

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkScrollBar *scrollbar = new CTkScrollBar(tk_, parent, childName);

  CQUtil::setBackground(scrollbar->getQWidget(), QColor("#d9d9d9"));

  CTkWidgetCommand *cmd = new CTkWidgetCommand(this, widgetName, scrollbar, opts);

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
    { "-bd"                 , "-borderwidth"       , 0                    , 0            },
    { "-bg"                 , "-background"        , 0                    , 0            },
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
    { "-fg"                 , "-foreground"        , 0                    , 0            },
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
    { "-invcmd"             , "-invalidcommand"    , 0                    , 0            },
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
    { "-vcmd"               , "-validatecommand"   , 0                    , 0            },
    { "-width"              , "width"              , "Width"              , "20"         },
    { "-wrap"               , "wrap"               , "Wrap"               , "0"          },
    { "-xscrollcommand"     , "xScrollCommand"     , "ScrollCommand"      , ""           },
    { 0                     , 0                    , 0                    , 0            }
  };

  uint numArgs = args.size();

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const std::string &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "") {
    tk_->getTcl()->throwError("No name");
    return CTclValueRef();
  }

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkSpinBox *spin = new CTkSpinBox(tk_, parent, childName);

  CQUtil::setBackground(spin->getQWidget(), QColor("#ffffff"));

  CTkWidgetCommand *cmd = new CTkWidgetCommand(this, widgetName, spin, opts);

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
   {"-bd"                      , "-borderwidth"            , 0                    , 0            },
   {"-bg"                      , "-background"             , 0                    , 0            },
   {"-blockcursor"             , "blockCursor"             , "BlockCursor"        , "0"          },
   {"-borderwidth"             , "borderWidth"             , "BorderWidth"        , "1"          },
   {"-cursor"                  , "cursor"                  , "Cursor"             , "xterm"      },
   {"-endline"                 , ""                        , ""                   , ""           },
   {"-exportselection"         , "exportSelection"         , "ExportSelection"    , "1"          },
   {"-fg"                      , "-foreground"             , 0                    , 0            },
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
   {0                          , 0                         , 0                    , 0            }
  };

  uint numArgs = args.size();

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const std::string &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "") {
    tk_->getTcl()->throwError("No name");
    return CTclValueRef();
  }

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkText *text = new CTkText(tk_, parent, childName);

  CQUtil::setBackground(text->getQWidget(), QColor("#ffffff"));

  CTkWidgetCommand *cmd = new CTkWidgetCommand(this, widgetName, text, opts);

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
    { "-bd"                 , "-borderwidth"       , 0                    , 0          },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "0"        },
    { "-class"              , "class"              , "Class"              , "Toplevel" },
    { "-menu"               , "menu"               , "Menu"               , ""         },
    { "-relief"             , "relief"             , "Relief"             , "flat"     },
    { "-screen"             , "screen"             , "Screen"             , ""         },
    { "-use"                , "use"                , "Use"                , ""         },
    { "-background"         , "background"         , "Background"         , "#d9d9d9"  },
    { "-bg"                 , "-background"        , 0                    , 0          },
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
    { 0                     , 0                    , 0                    , 0          }
  };

  uint numArgs = args.size();

  std::string widgetName;

  for (uint i = 0; i < numArgs; ++i) {
    const std::string &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      continue;
    }
    else
      widgetName = name;
  }

  if (widgetName == "") {
    tk_->getTcl()->throwError("No name");
    return CTclValueRef();
  }

  CTkWidget*  parent = 0;
  std::string childName;

  if (! tk_->decodeWidgetName(widgetName, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + widgetName + "'");
    return CTclValueRef();
  }

  CTkTopLevel *toplevel = new CTkTopLevel(tk_, parent, childName);

  CQUtil::setBackground(toplevel->getQWidget(), QColor("#d9d9d9"));

  CTkWidgetCommand *cmd = new CTkWidgetCommand(this, widgetName, toplevel, opts);

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

  if (numArgs < 1) {
    tk_->getTcl()->wrongNumArgs("winfo option ?arg ...?");
    return CTclValueRef();
  }

  const std::string &arg = args[0]->toString();

  if      (arg == "atom") {
  }
  else if (arg == "atomname") {
  }
  else if (arg == "cells") {
  }
  else if (arg == "children") {
    if (numArgs < 2) {
      tk_->getTcl()->wrongNumArgs("winfo children window");
      return CTclValueRef();
    }

    std::string widgetName = args[1]->toString();

    CTkWidget *w = tk_->lookupWidgetByName(widgetName);

    if (w) {
      std::vector<CTkWidget *> children;

      w->getChildren(children);

      uint numChildren = children.size();

      CTclList *list = new CTclList;

      for (uint i = 0; i < numChildren; ++i) {
        CTkWidget *child = children[i];

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
    if (numArgs < 2) {
      tk_->getTcl()->wrongNumArgs("winfo parent window");
      return CTclValueRef();
    }

    std::string widgetName = args[1]->toString();

    CTkWidget *w = tk_->lookupWidgetByName(widgetName);

    if (w) {
      CTkWidget *parent = w->getParent();

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

  if (numArgs < 2) {
    tk_->getTcl()->wrongNumArgs("wm option window ?arg ...?");
    return CTclValueRef();
  }

  const std::string &arg  = args[0]->toString();
  const std::string &name = args[1]->toString();

  CTkWidget*  parent;
  std::string childName;

  if (! tk_->decodeWidgetName(name, &parent, childName)) {
    tk_->getTcl()->throwError("Invalid name '" + name + "'");
    return CTclValueRef();
  }

  CTkWidget *w;

  if (childName != "") {
    CTkWidget *child = parent->getChild(childName);

    if (child == 0) {
      tk_->getTcl()->throwError("Invalid name '" + name + "'");
      return CTclValueRef();
    }

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
      const std::string &title = args[2]->toString();

      w->setTitle(title);

      return CTclValueRef();
    }
    else {
      const std::string &title = w->getTitle();

      return CTclValueRef(tk_->getTcl()->createValue(title));
    }
  }
  else {
    tk_->getTcl()->throwError("ambiguous option \"" + arg + "\": must be "
      "aspect, attributes, client, colormapwindows, command, deiconify, focusmodel, "
      "forget, frame, geometry, grid, group, iconbitmap, iconify, iconmask, iconname, "
      "iconphoto, iconposition, iconwindow, manage, maxsize, minsize, overrideredirect, "
      "positionfrom, protocol, resizable, sizefrom, stackorder, state, title, "
      "transient, or withdraw");
    return CTclValueRef();
  }

  return CTclValueRef();
}

//----------

CTkRootWidgetCommand::
CTkRootWidgetCommand(CTk *tk) :
 CTkCommand(tk, "."), opts_(tk)
{
  static CTkOpt opts[] = {
    { "-bd"                 , "-borderwidth"       , 0                    , 0          },
    { "-borderwidth"        , "borderWidth"        , "BorderWidth"        , "0"        },
    { "-class"              , "class"              , "Class"              , "Toplevel" },
    { "-menu"               , "menu"               , "Menu"               , ""         },
    { "-relief"             , "relief"             , "Relief"             , "flat"     },
    { "-screen"             , "screen"             , "Screen"             , ""         },
    { "-use"                , "use"                , "Use"                , ""         },
    { "-background"         , "background"         , "Background"         , "#d9d9d9"  },
    { "-bg"                 , "-background"        , 0                    , 0          },
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
    { 0                     , 0                    , 0                    , 0          }
  };

  opts_.init(opts);
}

CTclValueRef
CTkRootWidgetCommand::
exec(const std::vector<CTclValueRef> &args)
{
  uint numArgs = args.size();

  if (numArgs == 0) {
    tk_->getTcl()->wrongNumArgs(". option ?arg arg ...?");
    return CTclValueRef();
  }

  const std::string &arg = args[0]->toString();

  if (arg == "configure" || arg == "config") {
    if (numArgs == 1)
      return opts_.getOpts();
    else {
      for (uint i = 1; i < numArgs; i += 2) {
        const std::string &name  = args[i + 0]->toString();
        const std::string &value = args[i + 1]->toString();

        const CTkOpt *opt;

        if (! opts_.setOptValue(name, value, &opt)) {
          tk_->getTcl()->throwError("unknown option \"" + name + "\"");
          return CTclValueRef();
        }

        tk_->getRootWidget()->notifyValueChanged(opt->name, value);
      }
    }

    return CTclValueRef();
  }
  else {
    tk_->getTcl()->throwError("bad option \"" + arg + "\": must be cget or configure");
    return CTclValueRef();
  }
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

  if (numArgs == 0) {
    tk_->getTcl()->wrongNumArgs(getName() + " option ?arg arg ...?");
    return CTclValueRef();
  }

  const std::string &arg = args[0]->toString();

  if      (arg == "configure" || arg == "config") {
    if (numArgs == 1)
      return opts_.getOpts();
    else {
      for (uint i = 1; i < numArgs; i += 2) {
        const std::string &name  = args[i + 0]->toString();
        const std::string &value = args[i + 1]->toString();

        if (! setOptValue(name, value)) {
          tk_->getTcl()->throwError("unknown option \"" + name + "\"");
          return CTclValueRef();
        }
      }
    }

    return CTclValueRef();
  }
  else if (arg == "cget") {
    if (numArgs != 2) {
      tk_->getTcl()->wrongNumArgs(getName() + " cget option");
      return CTclValueRef();
    }

    const std::string &name = args[1]->toString();

    std::string value;

    if (! getOptValue(name, value)) {
      tk_->getTcl()->throwError("unknown option \"" + name + "\"");
      return CTclValueRef();
    }

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
  CTk *tk = command_->getTk();

  uint numArgs = args.size();

  for (uint i = 0; i < numArgs; ++i) {
    const std::string &name = args[i]->toString();

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      if (i < numArgs) {
        const std::string &value = args[i]->toString();

        if (! setOptValue(name, value))
          tk->getTcl()->throwError("unknown option \"" + name + "\"");
      }
      else
        tk->getTcl()->throwError("missing option value for \"" + name + "\"");
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

//----------

CTkRootWidget::
CTkRootWidget(CTk *tk) :
 CTkWidget(tk, 0, "")
{
  qframe_ = new QFrame(0);

  qframe_->resize(1, 1);

  setQWidget(qframe_);
}

bool
CTkRootWidget::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if      (name == "-background") {
    double r, g, b, a;

    CRGBName::lookup(value, &r, &g, &b, &a);

    QColor c(255*r, 255*g, 255*b, 255*a);

    CQUtil::setBackground(qframe_, c);
  }
  else if (name == "-borderwidth") {
    int w;

    if (CStrUtil::toInteger(value, &w))
      qframe_->setLineWidth(w);
  }
  else
    return false;

  return true;
}

//----------

CTkButton::
CTkButton(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qbutton_ = new QPushButton(parent_->getQWidget());
  else
    qbutton_ = new QPushButton(0);

  setQWidget(qbutton_);

  connect(qbutton_, SIGNAL(clicked()), this, SLOT(runCommandSlot()));
}

bool
CTkButton::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if      (name == "-command")
    setCommand(value);
  else if (name == "-text")
    setText(value);
  else if (name == "-image") {
    CTkImageRef image = tk_->getImage(value);

    if (image.isValid())
      setImage(image->getImage());
  }
  else
    return false;

  return true;
}

void
CTkButton::
setCommand(const std::string &command)
{
  command_ = command;
}

void
CTkButton::
setText(const std::string &text)
{
  qbutton_->setText(text.c_str());
}

void
CTkButton::
setImage(CImagePtr image)
{
  QImage &qimage = image.cast<CQImage>()->getQImage();

  QPixmap pixmap;

  pixmap.convertFromImage(qimage);

  qbutton_->setIcon(QIcon(pixmap));
}

CTclValueRef
CTkButton::
iexec(const std::vector<CTclValueRef> &args)
{
  const std::string &arg = args[0]->toString();

  if (arg == "invoke")
    invoke();

  return CTclValueRef();
}

void
CTkButton::
runCommandSlot()
{
  invoke();
}

void
CTkButton::
invoke()
{
  if (command_ == "") return;

  tk_->getTcl()->parseLine(command_);
}

//----------

CTkCanvas::
CTkCanvas(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qcanvas_ = new QWidget(parent_->getQWidget());
  else
    qcanvas_ = new QWidget(0);

  setQWidget(qcanvas_);

  qwidget_->setFocusPolicy(Qt::StrongFocus);
}

bool
CTkCanvas::
notifyValueChanged(const std::string &, const std::string &)
{
  return false;
}

//----------

CTkCheckButton::
CTkCheckButton(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qcheck_ = new QCheckBox(parent_->getQWidget());
  else
    qcheck_ = new QCheckBox(0);

  setQWidget(qcheck_);
}

bool
CTkCheckButton::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if (name == "-text")
    setText(value);
  else
    return false;

  return true;
}

void
CTkCheckButton::
setText(const std::string &text)
{
  qcheck_->setText(text.c_str());
}

//----------

class CTkEntryVarProc : public CTclVariableProc {
 public:
  CTkEntryVarProc(CTkEntry *entry) :
   entry_(entry) {
  }

  void notify(CTclVariable *var) {
    entry_->setText(var->getValue()->toString());
  }

 private:
  CTkEntry *entry_;
};

CTkEntry::
CTkEntry(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qedit_ = new QLineEdit(parent_->getQWidget());
  else
    qedit_ = new QLineEdit(0);

  setQWidget(qedit_);

  qwidget_->setFocusPolicy(Qt::StrongFocus);

  connect(qedit_, SIGNAL(editingFinished()), this, SLOT(valueChangedSlot()));
}

bool
CTkEntry::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if (name == "-textvariable") {
    varName_ = value;

    CTclVariableRef var = tk_->getTcl()->getVariable(varName_);

    if (! var.isValid())
      var = tk_->getTcl()->addVariable(varName_, tk_->getTcl()->createValue(""));

    setText(var->getValue()->toString());

    var->addNotifyProc(new CTkEntryVarProc(this));
  }
  else
    return false;

  return true;
}

void
CTkEntry::
setText(const std::string &text)
{
  qedit_->setText(text.c_str());
}

void
CTkEntry::
valueChangedSlot()
{
  if (varName_ != "") {
    CTclVariableRef var = tk_->getTcl()->getVariable(varName_);

    if (! var.isValid())
      var = tk_->getTcl()->addVariable(varName_, CTclValueRef());

    var->setValue(tk_->getTcl()->createValue(qedit_->text().toStdString()));
  }
}

//----------

CTkFrame::
CTkFrame(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qframe_ = new QFrame(parent_->getQWidget());
  else
    qframe_ = new QFrame(0);

  setQWidget(qframe_);
}

bool
CTkFrame::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if      (name == "-relief") {
    if      (value == "raised") {
      qframe_->setFrameShadow(QFrame::Raised);
      qframe_->setFrameShape (QFrame::Panel);
    }
    else if (value == "sunken") {
      qframe_->setFrameShadow(QFrame::Sunken);
      qframe_->setFrameShape (QFrame::Panel);
    }
    else if (value == "flat") {
      qframe_->setFrameShadow(QFrame::Plain);
      qframe_->setFrameShape (QFrame::NoFrame);
    }
    else if (value == "ridge") {
      qframe_->setFrameShadow(QFrame::Raised);
      qframe_->setFrameShape (QFrame::Box);
    }
    else if (value == "solid") {
      qframe_->setFrameShadow(QFrame::Plain);
      qframe_->setFrameShape (QFrame::Panel);
    }
    else if (value == "groove") {
      qframe_->setFrameShadow(QFrame::Sunken);
      qframe_->setFrameShape (QFrame::Box);
    }
  }
  else if (name == "-borderwidth") {
    int w;

    if (CStrUtil::toInteger(value, &w))
      qframe_->setLineWidth(w);
  }

  return false;
}

//----------

CTkLabel::
CTkLabel(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qlabel_ = new QLabel(parent_->getQWidget());
  else
    qlabel_ = new QLabel(0);

  setQWidget(qlabel_);
}

bool
CTkLabel::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if      (name == "-text")
    setText(value);
  else if (name == "-image") {
    CTkImageRef image = tk_->getImage(value);

    if (image.isValid())
      setImage(image->getImage());
  }
  else
    return false;

  return true;
}

void
CTkLabel::
setText(const std::string &text)
{
  qlabel_->setText(text.c_str());
}

void
CTkLabel::
setImage(CImagePtr image)
{
  QImage &qimage = image.cast<CQImage>()->getQImage();

  QPixmap pixmap;

  pixmap.convertFromImage(qimage);

  qlabel_->setPixmap(pixmap);
}

//----------

CTkLabelFrame::
CTkLabelFrame(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qframe_ = new QGroupBox(parent_->getQWidget());
  else
    qframe_ = new QGroupBox(0);

  setQWidget(qframe_);
}

bool
CTkLabelFrame::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if (name == "-text")
    setText(value);
  else
    return false;

  return true;
}

void
CTkLabelFrame::
setText(const std::string &text)
{
  qframe_->setTitle(text.c_str());
}

//----------

CTkListBox::
CTkListBox(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qlist_ = new QListWidget(parent_->getQWidget());
  else
    qlist_ = new QListWidget(0);

  setQWidget(qlist_);
}

bool
CTkListBox::
notifyValueChanged(const std::string &, const std::string &)
{
  return false;
}

//----------

CTkPanedWindow::
CTkPanedWindow(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qpane_ = new QSplitter(parent_->getQWidget());
  else
    qpane_ = new QSplitter(0);

  qpane_->setChildrenCollapsible(false);

  setQWidget(qpane_);
}

bool
CTkPanedWindow::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if (name == "-orient") {
    static CStrMap<Qt::Orientation,int> orientMap(
      "horizontal", Qt::Horizontal,
      "vertical"  , Qt::Vertical  ,
      0);

    Qt::Orientation orient;

    bool rc = orientMap.map(value, orient);

    if (rc)
      qpane_->setOrientation(orient);

    return true;
  }
  else
    return false;
}

CTclValueRef
CTkPanedWindow::
iexec(const std::vector<CTclValueRef> &args)
{
  uint numArgs = args.size();

  const std::string &arg = args[0]->toString();

  if (arg == "add") {
    if (numArgs < 2) {
      tk_->getTcl()->wrongNumArgs(getName() + " add widget ?widget ...?");
      return CTclValueRef();
    }

    int min_size = -1;
    int padx     = -1;
    int pady     = -1;

    for (uint i = 1; i < numArgs; ++i) {
      const std::string &name = args[i]->toString();

      if (name.size() > 0 && name[0] == '-') {
        if      (name == "-minsize") {
          ++i;

          if (i >= numArgs) {
            tk_->getTcl()->throwError("Missing value for \"-minsize\"");
            return CTclValueRef();
          }

          std::string value = args[i]->toString();

          int i;

          if (CStrUtil::toInteger(value, &i))
            min_size = i;
        }
        else if (name == "-padx") {
          ++i;

          if (i >= numArgs) {
            tk_->getTcl()->throwError("Missing value for \"-padx\"");
            return CTclValueRef();
          }

          std::string value = args[i]->toString();

          int i;

          if (CStrUtil::toInteger(value, &i))
            padx = i;

          std::cerr << "padx " << padx << " not supported" << std::endl;
        }
        else if (name == "-pady") {
          ++i;

          if (i >= numArgs) {
            tk_->getTcl()->throwError("Missing value for \"-pady\"");
            return CTclValueRef();
          }

          std::string value = args[i]->toString();

          int i;

          if (CStrUtil::toInteger(value, &i))
            pady = i;

          std::cerr << "pady " << pady << " not supported" << std::endl;
        }
      }
      else {
        CTkWidget*  parent;
        std::string childName;

        if (! tk_->decodeWidgetName(name, &parent, childName)) {
          tk_->getTcl()->throwError("Invalid name '" + name + "'");
          return CTclValueRef();
        }

        CTkWidget *child = parent->getChild(childName);

        if (child == 0) {
          tk_->getTcl()->throwError("Invalid name '" + name + "'");
          return CTclValueRef();
        }

        QWidget *qwidget = child->getQWidget();

        if (min_size > 0) {
          if (qpane_->orientation() == Qt::Horizontal)
            qwidget->setMinimumHeight(min_size);
          else
            qwidget->setMinimumWidth (min_size);
        }

        qpane_->addWidget(qwidget);
      }
    }
  }

  return CTclValueRef();
}

//----------

CTkRadioButton::
CTkRadioButton(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qradio_ = new QRadioButton(parent_->getQWidget());
  else
    qradio_ = new QRadioButton(0);

  setQWidget(qradio_);
}

bool
CTkRadioButton::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if (name == "-text")
    setText(value);
  else
    return false;

  return true;
}

void
CTkRadioButton::
setText(const std::string &text)
{
  qradio_->setText(text.c_str());
}

//----------

CTkScale::
CTkScale(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qscale_ = new QSlider(parent_->getQWidget());
  else
    qscale_ = new QSlider(0);

  setQWidget(qscale_);
}

bool
CTkScale::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if (name == "-text")
    setText(value);
  else
    return false;

  return true;
}

void
CTkScale::
setText(const std::string &)
{
  //qscale_->setLabel(text.c_str());
}

//----------

CTkScrollBar::
CTkScrollBar(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qscrollbar_ = new QScrollBar(parent_->getQWidget());
  else
    qscrollbar_ = new QScrollBar(0);

  setQWidget(qscrollbar_);
}

bool
CTkScrollBar::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if (name == "-orient") {
    static CStrMap<Qt::Orientation,int> orientMap(
      "horizontal", Qt::Horizontal,
      "vertical"  , Qt::Vertical  ,
      0);

    Qt::Orientation orient;

    bool rc = orientMap.map(value, orient);

    if (rc)
      qscrollbar_->setOrientation(orient);

    return true;
  }
  else
    return false;
}

//----------

CTkSpinBox::
CTkSpinBox(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qspin_ = new QSpinBox(parent_->getQWidget());
  else
    qspin_ = new QSpinBox(0);

  setQWidget(qspin_);

  qwidget_->setFocusPolicy(Qt::StrongFocus);
}

bool
CTkSpinBox::
notifyValueChanged(const std::string &, const std::string &)
{
  return false;
}

//----------

CTkText::
CTkText(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  if (parent)
    qtext_ = new QTextEdit(parent_->getQWidget());
  else
    qtext_ = new QTextEdit(0);

  setQWidget(qtext_);

  qwidget_->setFocusPolicy(Qt::StrongFocus);
}

bool
CTkText::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if (name == "-text")
    setText(value);
  else
    return false;

  return true;
}

void
CTkText::
setText(const std::string &text)
{
  qtext_->setText(text.c_str());
}

//----------

CTkTopLevel::
CTkTopLevel(CTk *tk, CTkWidget *parent, const std::string &name) :
 CTkWidget(tk, parent, name)
{
  qframe_ = new QFrame(0);

  setQWidget(qframe_);
}

bool
CTkTopLevel::
notifyValueChanged(const std::string &, const std::string &)
{
  return false;
}

//----------

CTkWidget::
CTkWidget(CTk *tk, CTkWidget *parent, const std::string &name) :
 tk_(tk), parent_(parent), name_(name), w_(0), h_(0), deleted_(false), qwidget_(0)
{
  if (parent_)
    parent_->addChild(this);

  tk_->addWidget(this);
}

CTkWidget::
~CTkWidget()
{
  if (parent_)
    parent_->removeChild(this);

  tk_->removeWidget(this);

  delete qwidget_;
}

std::string
CTkWidget::
getFullName() const
{
  std::string name = getName();

  if (parent_ != 0) {
    std::string pname = parent_->getFullName();

    if (pname != ".")
      pname += ".";

    name = pname + name;
  }
  else {
    if (name == "")
      name = ".";
  }

  return name;
}

void
CTkWidget::
setQWidget(QWidget *w)
{
  qwidget_ = w;

  if (name_ != "")
    qwidget_->setObjectName(name_.c_str());
  else
    qwidget_->setObjectName(".");

  qwidget_->installEventFilter(new CTkWidgetEventFilter(this));

  qwidget_->setFocusPolicy(Qt::ClickFocus);
}

QWidget *
CTkWidget::
getQWidget() const
{
  return qwidget_;
}

int
CTkWidget::
getWidth() const
{
  return (w_ > 0 ? w_ : qwidget_->sizeHint().width());
}

int
CTkWidget::
getHeight() const
{
  return (h_ > 0 ? h_ : qwidget_->sizeHint().height());
}

void
CTkWidget::
setWidth(int w)
{
  w_ = w;
}

void
CTkWidget::
setHeight(int h)
{
  h_ = h;
}

void
CTkWidget::
addChild(CTkWidget *w)
{
  children_[w->getName()] = w;
}

void
CTkWidget::
removeChild(CTkWidget *w)
{
  WidgetMap::iterator p = children_.find(w->getName());

  if (p != children_.end())
    children_.erase(p);
}

void
CTkWidget::
getChildren(std::vector<CTkWidget *> &children) const
{
  WidgetMap::const_iterator p1, p2;

  for (p1 = children_.begin(), p2 = children_.end(); p1 != p2; ++p1)
    children.push_back((*p1).second);
}

CTkWidget *
CTkWidget::
getChild(const std::string &name) const
{
  WidgetMap::const_iterator p = children_.find(name);

  if (p == children_.end())
    return 0;

  return (*p).second;
}

void
CTkWidget::
setFocus()
{
  QWidget *w = getQWidget();
  assert(w);

  w->setFocus();
}

void
CTkWidget::
lower()
{
  QWidget *w = getQWidget();
  assert(w);

  w->lower();
}

void
CTkWidget::
raise()
{
  QWidget *w = getQWidget();
  assert(w);

  w->raise();
}

CTkPackLayout *
CTkWidget::
getTkPackLayout()
{
  QWidget *w = getQWidget();
  assert(w);

  QLayout *l = w->layout();

  CTkPackLayout *l1 = qobject_cast<CTkPackLayout *>(l);

  if (! l1)
    l1 = new CTkPackLayout(w);

  return l1;
}

CTkGridLayout *
CTkWidget::
getTkGridLayout()
{
  QWidget *w = getQWidget();
  assert(w);

  QLayout *l = w->layout();

  CTkGridLayout *l1 = qobject_cast<CTkGridLayout *>(l);

  if (! l1)
    l1 = new CTkGridLayout(w);

  return l1;
}

CTkPlaceLayout *
CTkWidget::
getTkPlaceLayout()
{
  QWidget *w = getQWidget();
  assert(w);

  QLayout *l = w->layout();

  CTkPlaceLayout *l1 = qobject_cast<CTkPlaceLayout *>(l);

  if (! l1)
    l1 = new CTkPlaceLayout(w);

  return l1;
}

bool
CTkWidget::
notifyValueChanged(const std::string &name, const std::string &value)
{
  if      (name == "-width") {
    int w;

    if (CStrUtil::toInteger(value, &w)) {
      setWidth(w);

      qwidget_->resize(getWidth(), getHeight());
    }
  }
  else if (name == "-height") {
    int h;

    if (CStrUtil::toInteger(value, &h)) {
      setHeight(h);

      qwidget_->resize(getWidth(), getHeight());
    }
  }
  else if (name == "-background") {
    double r, g, b, a;

    CRGBName::lookup(value, &r, &g, &b, &a);

    QColor c(255*r, 255*g, 255*b, 255*a);

    CQUtil::setBackground(qwidget_, c);
  }
  else
    return false;

  return true;
}

void
CTkWidget::
show()
{
  if (qwidget_)
    qwidget_->show();
}

void
CTkWidget::
setTitle(const std::string &title)
{
  qwidget_->setWindowTitle(title.c_str());
}

std::string
CTkWidget::
getTitle() const
{
  return qwidget_->windowTitle().toStdString();
}

void
CTkWidget::
bindEvent(const std::string &pattern, const std::string &command)
{
  events_[pattern] = command;
}

bool
CTkWidget::
triggerEnterEvents(QEvent *e)
{
  EventMap::const_iterator p = events_.find("<Enter>");

  if (p != events_.end())
    return tk_->execEvent(this, e, (*p).second);

  return tk_->triggerEnterEvents(getClassName(), this, e);
}

bool
CTkWidget::
triggerLeaveEvents(QEvent *e)
{
  EventMap::const_iterator p = events_.find("<Leave>");

  if (p != events_.end())
    return tk_->execEvent(this, e, (*p).second);

  return tk_->triggerLeaveEvents(getClassName(), this, e);
}

bool
CTkWidget::
triggerMousePressEvents(QEvent *e)
{
  EventMap::const_iterator p = events_.find("<Button>");

  if (p == events_.end())
    p = events_.find("<ButtonPress>");

  if (p != events_.end())
    return tk_->execEvent(this, e, (*p).second);

  return false;
}

bool
CTkWidget::
triggerKeyPressEvents(QEvent *e)
{
  QKeyEvent *ke = dynamic_cast<QKeyEvent *>(e);

  int k = ke->key();

  std::string str("<");

  if (ke->modifiers() & Qt::ControlModifier)
    str += "Control-";

  if (ke->modifiers() & Qt::ShiftModifier)
    str += "Shift-";

  if (k > 32 && k < 0x7f)
    str += tolower(char(k));
  else
    return false;

  str += ">";

  EventMap::const_iterator p = events_.find(str);

  if (p != events_.end())
    return tk_->execEvent(this, e, (*p).second);

  return tk_->triggerKeyPressEvents(getClassName(), this, e, str);
}

void
CTkWidget::
deleteLater()
{
  deleted_ = true;

  tk_->addDeleteWidget(this);
}

//-----------

bool
CTkWidgetEventFilter::
eventFilter(QObject *obj, QEvent *event)
{
  if (w_->isDeleted()) return false;

  switch (event->type()) {
    case QEvent::Enter           : w_->triggerEnterEvents(event); break;
    case QEvent::Leave           : w_->triggerLeaveEvents(event); break;
    case QEvent::MouseButtonPress: w_->triggerMousePressEvents(event); break;
    case QEvent::KeyPress        : w_->triggerKeyPressEvents(event); break;
    default                      : break;
  }

  // standard event processing
  return QObject::eventFilter(obj, event);
}

//-----------

CTkOptData::
CTkOptData(CTk *tk) :
 tk_(tk)
{
}

CTclValueRef
CTkOptData::
getOpts()
{
  CTcl *tcl = tk_->getTcl();

  CTclList *list = new CTclList;

  for (uint i = 0; opts_[i].name != 0; ++i) {
    const CTkOpt &opt = opts_[i];

    CTclList *list1 = new CTclList;

    list1->addValue(tcl->createValue(opt.name ));
    list1->addValue(tcl->createValue(opt.dname));

    if (opt.cname != 0) {
      list1->addValue(tcl->createValue(opt.cname));
      list1->addValue(tcl->createValue(opt.def  ));

      CTkOptionValueMap::const_iterator p = values_.find(opt.name);

      if (p != values_.end())
        list1->addValue(tcl->createValue((*p).second.s));
      else
        list1->addValue(tcl->createValue(opt.def));
    }

    list->addValue(CTclValueRef(list1));
  }

  return CTclValueRef(list);
}

bool
CTkOptData::
getOptValue(const std::string &name, std::string &value)
{
  for (uint i = 0; opts_[i].name != 0; ++i) {
    const CTkOpt &opt = opts_[i];

    if (opt.name != name) continue;

    if (opt.cname == 0)
      return getOptValue(opt.dname, value);

    CTkOptionValueMap::const_iterator p = values_.find(name);

    if (p != values_.end())
      value = (*p).second.s;
    else
      value = opts_[i].def;

    return true;
  }

  return false;
}

bool
CTkOptData::
setOptValue(const std::string &name, const std::string &value, const CTkOpt **opt)
{
  // exact match
  for (uint i = 0; opts_[i].name != 0; ++i) {
    const CTkOpt &opt1 = opts_[i];

    if (opt1.name != name) continue;

    if (opt1.cname == 0)
      return setOptValue(opt1.dname, value, opt);

    *opt = &opt1;

    values_[name].s = value;

    return true;
  }

  // abbrev
  *opt = 0;

  for (uint i = 0; opts_[i].name != 0; ++i) {
    const CTkOpt &opt1 = opts_[i];

    std::string::size_type pos = std::string(opt1.name).find(name);

    if (pos != 0) continue;

    if (opt)
      *opt = &opt1;
    else
      return false;
  }

  if (! *opt)
    return false;

  values_[name].s = value;

  return true;
}

//------

bool
CTkImage::
loadFile(const std::string &filename)
{
  CImageFileSrc src(filename);

  image_ = CImageMgrInst->createImage(src);

  return image_.isValid();
}

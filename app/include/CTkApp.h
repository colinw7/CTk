#ifndef CTkApp_H
#define CTkApp_H

#include <CTkAppOptionValue.h>
#include <CTkAppEventData.h>

#include <CTclApp.h>
#include <CImagePtr.h>
#include <CStrParse.h>
#include <CRGBName.h>
#include <CScreenUnits.h>
#include <CStrMap.h>

#include <QColor>
#include <QFont>

#include <memory>
#include <vector>
#include <set>
#include <map>

class CTkAppRoot;
class CTkAppTopLevel;
class CTkAppWidget;
class CTkAppImage;

using CTkAppImageRef = std::shared_ptr<CTkAppImage>;

class QWidget;
class QEvent;
class QKeyEvent;
class QMouseEvent;

//---

class CTkApp : public CTclApp {
 public:
  CTkApp(int argc, char **argv);

  std::string getTclStr() override;

  CTkAppRoot *root() const { return root_; }

  //---

  void processEvents();

  void show();

  //---

  CTkAppWidget *lookupWidgetByName(const std::string &widgetName, bool quiet=false) const;

  //---

  bool processOption(CTkAppOption *opts, const std::vector<std::string> &args, uint &i,
                     CTkAppOptionValueMap &values);

  //---

  std::string getNewImageName() const;

  CTkAppImageRef createImage(const std::string &type, const std::string &format,
                             const std::string &name);

  CTkAppImageRef getImage(const std::string &name) const;

  CImagePtr getBitmap(const std::string &name) const;

  //---

  void *getWidgetClassData(const std::string &name) const;

  //---

  bool bindAllEvent  (const CTkAppEventData &data);
  bool bindTagEvent  (const std::string &tagName, const CTkAppEventData &data);
  bool bindClassEvent(const std::string &tagName, const CTkAppEventData &data);

  bool triggerEvents(const std::string &, CTkAppWidget *, QEvent *e,
                     const CTkAppEventData &matchEventData);

  bool triggerKeyPressEvents(const std::string &, CTkAppWidget *, QEvent *e);

  bool execEvent(CTkAppWidget *, QEvent *e, const CTkAppEventData &data);

  //---

  void addTopLevel(CTkAppTopLevel *toplevel);

  void addWidget(CTkAppWidget *w);

  CTkAppWidget *lookupWidget(QWidget *w) const;

  void removeWidget(CTkAppWidget *w);

  void addDeleteWidget(CTkAppWidget *w);

  void purgeWidgets();

  //---

  void encodeEvent(QKeyEvent *e, bool press, CTkAppEventData &data) const;
  void encodeEvent(QMouseEvent *e, CTkAppEventMode mode, int button, CTkAppEventData &data) const;

  bool parseEvent(const std::string &str, CTkAppEventData &data);

  //---

  void addOption(const std::string &pattern, const std::string &value, int priority);

  bool matchOption(const std::string &widgetClass, const std::string &optName,
                   const std::string &optClass, std::string &optValue) const;

  void clearOptions();

  //---

  bool lookupOptionName(const std::vector<std::string> &names,
                        const std::string &arg, std::string &opt) const;

  bool lookupName(const std::string &msg, const std::vector<std::string> &names,
                  const std::string &arg, std::string &opt) const;

  bool getOptionInt (const std::string &name, const std::string &value, long &i) const;
  bool getOptionReal(const std::string &name, const std::string &value, double &r) const;

  //---

  const std::string &currentCommand() const { return currentCommand_; }
  void setCurrentCommand(const std::string &s) { currentCommand_ = s; }

  //---

  bool wrongNumArgs(const std::string &msg) const;

  bool throwError(const std::string &msg) const;

  bool TODO(const std::string &msg="") const;

 protected:
  void addCommands() override;

  void addWidgetClass(const std::string &name);

 private:
  using WidgetClasses   = std::set<std::string>;
  using ImageMap        = std::map<std::string, CTkAppImageRef>;
  using TopLevelArray   = std::vector<CTkAppTopLevel *>;
  using EventDatas      = std::vector<CTkAppEventData>;
  using ClassEventDatas = std::map<std::string, EventDatas>;
  using TagEventDatas   = std::map<std::string, EventDatas>;
  using WidgetSet       = std::set<CTkAppWidget *>;
  using WidgetArray     = std::vector<CTkAppWidget *>;

  struct OptionData {
    std::string pattern;
    std::string value;
    int         priority { -1 };

    OptionData() = default;

    OptionData(const std::string &pattern1, const std::string &value1, int priority1) :
     pattern(pattern1), value(value1), priority(priority1) {
    }
  };

  using OptionDatas = std::vector<OptionData>;

  WidgetClasses widgetClasses_;

  CTkAppRoot*     root_ { nullptr };
  TopLevelArray   toplevels_;
  ImageMap        images_;
  ClassEventDatas classEvents_;
  TagEventDatas   tagEvents_;
  EventDatas      allEvents_;
  WidgetSet       widgets_;
  WidgetArray     deleteWidgets_;

  OptionDatas options_;

  std::string currentCommand_;
};

//---

namespace CTkAppUtil {

inline bool stringToInt(const std::string &str, long &i) {
  try {
    i = std::stol(str);
  }
    catch (...) {
    return false;
  }

  return true;
}

inline bool stringToReal(const std::string &str, double &r) {
  try {
    r = std::stod(str);
  }
    catch (...) {
    return false;
  }

  return true;
}

inline bool stringToBool(const std::string &str, bool &b) {
  if      (str == "1" || str == "yes" || str == "true" || str == "y")
    b = true;
  else if (str == "0" || str == "no" || str == "false" || str == "n")
    b = false;
  else
    return false;

  return true;
}

inline QColor stringToQColor(const std::string &str) {
  double r = 0.0, g = 0.0, b = 0.0, a = 0.0;

  if (str != "")
    CRGBName::lookup(str, &r, &g, &b, &a);

  return QColor(255*r, 255*g, 255*b, 255*a);
}

inline QFont stringToQFont(const std::string &str) {
  QFont f;

  CStrParse parse(str);

  parse.skipSpace();

  std::string family;

  if (! parse.readNonSpace(family))
    return f;

  //std::cerr << family << " ";

  f.setFamily(QString::fromStdString(family));

  //---

  parse.skipSpace();

  int  size   = 0;
  bool points = true;

  if (! parse.readInteger(&size))
    return f;

  if (size < 0) {
    size   = -size;
    points = false;
  }

  //std::cerr << size << (points ? "pt" : "px") << " ";

  if (points)
    f.setPointSize(size);
  else
    f.setPixelSize(size);

  //---

  parse.skipSpace();

  std::vector<std::string> styles;

  while (! parse.eof()) {
    std::string style;

    if (! parse.readNonSpace(style))
      return f;

    styles.push_back(style);
  }

  //for (const auto &style : styles)
  //  std::cerr << style << " ";

  for (const auto &style : styles) {
    if      (style == "bold")
      f.setBold(true);
    else if (style == "italic")
      f.setItalic(true);
    else
      std::cerr << "Unhandled style '" << style << "'\n";
  }

  //---

  //std::cerr << "\n";

  return f;
}

inline bool stringToDistance(const std::string &str, double &r) {
  CStrParse parse(str);

  parse.skipSpace();

  double r1;

  if (! parse.readReal(&r1))
    return false;

  parse.skipSpace();

  if (parse.eof()) {
    r = r1;
    return true;
  }

  auto c = parse.readChar();

  double f = 1.0;

  if      (c == 'c')
    f = 1.0/2.54;
  else if (c == 'i')
    f = 1.0;
  else if (c == 'm')
    f = 1.0/25.4;
  else if (c == 'p')
    f = 1.0/72.0;
  else
    return false;

  auto dpi = CScreenUnitsMgrInst->dpi();

  r = r1*dpi*f;

  return true;
}

inline bool stringToDistanceI(const std::string &str, int &i) {
  double r;
  if (! stringToDistance(str, r))
    return false;
  i = int(r);
  return true;
}

inline bool stringToOrient(const std::string &str, Qt::Orientation &orient) {
  static CStrMap<Qt::Orientation, int> orientMap(
    "horizontal", Qt::Horizontal,
    "vertical"  , Qt::Vertical  ,
    0);

  return orientMap.map(str, orient);
}

}

#endif

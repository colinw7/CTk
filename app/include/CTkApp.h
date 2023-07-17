#ifndef CTkApp_H
#define CTkApp_H

#include <CTkAppOptionValue.h>
#include <CTkAppEventData.h>

#include <CTclApp.h>
#include <CImagePtr.h>

#include <memory>
#include <vector>
#include <set>
#include <map>

class CTkAppRoot;
class CTkTopLevel;
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

  bool processOption(CTkOption *opts, const std::vector<std::string> &args, uint &i,
                     CTkOptionValueMap &values);

  //---

  std::string getNewImageName() const;

  CTkAppImageRef createImage(const std::string &type, const std::string &name);

  CTkAppImageRef getImage(const std::string &name) const;

  CImagePtr loadImage(const std::string &name) const;

  //---

  void *getWidgetClassData(const std::string &name) const;

  //---

  bool bindAllEvent  (const CTkAppEventData &data);
  bool bindTagEvent  (const std::string &tagName, const CTkAppEventData &data);
  bool bindClassEvent(const std::string &tagName, const CTkAppEventData &data);

  bool triggerEnterEvents(const std::string &, CTkAppWidget *, QEvent *e);
  bool triggerLeaveEvents(const std::string &, CTkAppWidget *, QEvent *e);

  bool triggerKeyPressEvents(const std::string &, CTkAppWidget *, QEvent *e);

  bool execEvent(CTkAppWidget *, QEvent *e, const CTkAppEventData &data);

  //---

  void addTopLevel(CTkTopLevel *toplevel);

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

  bool lookupOptionName(const std::vector<std::string> &names,
                        const std::string &arg, std::string &opt) const;

  bool getOptionInt (const std::string &name, const std::string &value, int &i) const;
  bool getOptionReal(const std::string &name, const std::string &value, double &r) const;

  bool wrongNumArgs(const std::string &msg) const;

  bool throwError(const std::string &msg) const;

  bool TODO(const std::string &msg="") const;

 protected:
  void addCommands() override;

  void addWidgetClass(const std::string &name);

 private:
  using WidgetClasses   = std::set<std::string>;
  using ImageMap        = std::map<std::string, CTkAppImageRef>;
  using TopLevelArray   = std::vector<CTkTopLevel *>;
  using EventDatas      = std::vector<CTkAppEventData>;
  using ClassEventDatas = std::map<std::string, EventDatas>;
  using TagEventDatas   = std::map<std::string, EventDatas>;
  using WidgetSet       = std::set<CTkAppWidget *>;
  using WidgetArray     = std::vector<CTkAppWidget *>;

  WidgetClasses widgetClasses_;

  CTkAppRoot*     root_ { nullptr };
  TopLevelArray   toplevels_;
  ImageMap        images_;
  ClassEventDatas classEvents_;
  TagEventDatas   tagEvents_;
  EventDatas      allEvents_;
  WidgetSet       widgets_;
  WidgetArray     deleteWidgets_;
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
  if      (str == "1" || str == "yes" || str == "true")
    b = true;
  else if (str == "0" || str == "no" || str == "false")
    b = false;
  else
    return false;

  return true;
}

}

#endif

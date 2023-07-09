#ifndef CTkApp_H
#define CTkApp_H

#include <CTkAppOptionValue.h>

#include <CTclApp.h>
#include <CImagePtr.h>

#include <memory>
#include <vector>
#include <set>
#include <map>

class CTkAppRoot;
class CTkTopLevel;
class CTkWidget;
class CTkImage;

using CTkImageRef = std::shared_ptr<CTkImage>;

class QWidget;
class QEvent;

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

  CTkWidget *lookupWidgetByName(const std::string &widgetName) const;

  //---

  bool processOption(CTkOption *opts, const std::vector<std::string> &args, uint &i,
                     CTkOptionValueMap &values);

  //---

  std::string getNewImageName() const;

  CTkImageRef createImage(const std::string &type, const std::string &name);

  CTkImageRef getImage(const std::string &name) const;

  CImagePtr loadImage(const std::string &name) const;

  //---

  void bindEvent(const std::string &tagName, const std::string &pattern,
                 const std::string &command);

  bool triggerEnterEvents(const std::string &, CTkWidget *, QEvent *e);
  bool triggerLeaveEvents(const std::string &, CTkWidget *, QEvent *e);

  bool triggerKeyPressEvents(const std::string &, CTkWidget *, QEvent *e, const std::string &);

  bool execEvent(CTkWidget *, QEvent *e, const std::string &desc);

  //---

  void addTopLevel(CTkTopLevel *toplevel);

  void addWidget(CTkWidget *w);

  CTkWidget *lookupWidget(QWidget *w) const;

  void removeWidget(CTkWidget *w);

  void addDeleteWidget(CTkWidget *w);

  void purgeWidgets();

  //---

  bool wrongNumArgs(const std::string &msg) const;

  bool throwError(const std::string &msg) const;

  bool TODO(const std::string &msg="") const;

 protected:
  void addCommands() override;

 private:
  using ImageMap      = std::map<std::string, CTkImageRef>;
  using TopLevelArray = std::vector<CTkTopLevel *>;
  using EventMap      = std::map<std::string, std::string>;
  using TagEventMap   = std::map<std::string, EventMap>;
  using WidgetSet     = std::set<CTkWidget *>;
  using WidgetArray   = std::vector<CTkWidget *>;

  CTkAppRoot*   root_ { nullptr };
  TopLevelArray toplevels_;
  ImageMap      images_;
  TagEventMap   events_;
  WidgetSet     widgets_;
  WidgetArray   deleteWidgets_;
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

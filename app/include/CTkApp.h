#ifndef CTkApp_H
#define CTkApp_H

#include <CTkAppOptionValue.h>
#include <CTkAppEventData.h>

#include <CTclApp.h>
#include <CImagePtr.h>
#include <CStrParse.h>
#include <CRGBName.h>

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

class QFrame;
class QWidget;
class QEvent;
class QKeyEvent;
class QMouseEvent;

//---

class CTkApp : public CTclApp {
 public:
  explicit CTkApp(Tcl_Interp *interp);
  explicit CTkApp(int argc, const char **argv);

  std::string getTclStr() override;

  //---

  CTkAppRoot *root() const { return root_; }

  void setRootFrame(QFrame *frame);

  //---

  bool useStyle() const { return useStyle_; }
  void setUseStyle(bool b) { useStyle_ = b; }

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

  bool execEvent(CTkAppWidget *, QEvent *e, const CTkAppEventData &data,
                 const std::string &command);

  //---

  CTkAppTopLevel *installToplevel(const std::string &id, QFrame *frame);

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
  void construct(int argc, const char **argv);

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
  bool            useStyle_ { false };
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

#endif

#ifndef CTK_H
#define CTK_H

#include <CTkOptionValue.h>

#include <CTcl.h>
#include <CImageLib.h>
#include <QWidget>

#include <set>
#include <memory>

class CTkWidget;
class CTkRootWidget;
class CTkTopLevel;
class CTkImage;
class CTk;

using CTkImageRef = std::shared_ptr<CTkImage>;

class QWidget;

//---

class CTk {
 public:
  CTk(CTcl *tcl);

  CTcl *getTcl() const { return tcl_; }

  CTkRootWidget *getRootWidget() const { return root_; }

  //---

  void exec();

  void processEvents();

  //---

  CTkWidget *lookupWidgetByName(const std::string &widgetName) const;

  //---

  bool processOption(CTkOption *opts, const std::vector<CTclValueRef> &args, uint &i,
                     CTkOptionValueMap &values);

  //---

  std::string getNewImageName() const;

  CTkImageRef createImage(const std::string &type, const std::string &name);

  CTkImageRef getImage(const std::string &name) const;

  //---

  void bindEvent(const std::string &tagName, const std::string &pattern,
                 const std::string &command);

  bool triggerEnterEvents(const char *className, CTkWidget *w, QEvent *e);
  bool triggerLeaveEvents(const char *className, CTkWidget *w, QEvent *e);

  bool triggerKeyPressEvents(const char *className, CTkWidget *w, QEvent *e, const std::string &s);

  bool execEvent(CTkWidget *w, QEvent *e, const std::string &str);

  //---

  void addTopLevel(CTkTopLevel *toplevel);

  void addWidget(CTkWidget *w);

  CTkWidget *lookupWidget(QWidget *w) const;

  void removeWidget(CTkWidget *w);

  void addDeleteWidget(CTkWidget *w);

  void purgeWidgets();

  //---

  CTclValueRef wrongNumArgs(const std::string &msg) const;

  CTclValueRef throwError(const std::string &msg) const;

 private:
  using ImageMap      = std::map<std::string, CTkImageRef>;
  using TopLevelArray = std::vector<CTkTopLevel *>;
  using EventMap      = std::map<std::string, std::string>;
  using TagEventMap   = std::map<std::string, EventMap>;
  using WidgetSet     = std::set<CTkWidget *>;
  using WidgetArray   = std::vector<CTkWidget *>;

  CTcl*          tcl_  { nullptr };
  CTkRootWidget* root_ { nullptr };
  TopLevelArray  toplevels_;
  ImageMap       images_;
  TagEventMap    events_;
  WidgetSet      widgets_;
  WidgetArray    deleteWidgets_;
};

//-----

#endif

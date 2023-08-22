#ifndef CTkApp_H
#define CTkApp_H

#include <CTkAppOptionValue.h>
#include <CTkAppEventData.h>

#include <CTclApp.h>
#include <CMatrix2D.h>

#include <QPointer>
#include <QTransform>
#include <QFont>

#include <memory>
#include <vector>
#include <set>
#include <map>

class CTkAppRoot;
class CTkAppTopLevel;
class CTkAppWidget;
class CTkAppImage;
class CTkAppFont;

using CTkAppImageRef = std::shared_ptr<CTkAppImage>;
using CTkAppFontRef  = std::shared_ptr<CTkAppFont>;

class QFrame;
class QWidget;
class QEvent;
class QKeyEvent;
class QMouseEvent;
class QTimer;

//---

enum class CTkAppCompoundType {
  NONE,
  BOTTOM,
  TOP,
  LEFT,
  RIGHT,
  CENTER
};

//---

class CTkApp : public QObject, public CTclApp {
  Q_OBJECT

 public:
  struct MatrixData {
    QTransform transform;
  };

  using Args = std::vector<QVariant>;

 public:
  explicit CTkApp(Tcl_Interp *interp, const QString &context="");
  explicit CTkApp(int argc, const char **argv, const QString &context="");

  //---

  CTkAppRoot *root() const { return root_; }

  void setRootFrame(QFrame *frame);

  QFont appFont() const { return appFont_; }
  void setAppFont(const QFont &f) { appFont_ = f; }

  //---

  bool useStyle() const { return useStyle_; }
  void setUseStyle(bool b) { useStyle_ = b; }

  QString mapName(const QString &name) const {
    if (context_ != "")
      return "::" + context_ + "::" + name;
    else
      return name;
  }

  //---

  int waitForEventProc();

  void processEvents();

  void show();

  //---

  CTkAppWidget *lookupWidgetByName(const QString &widgetName, bool quiet=false) const;

  //---

  bool processOption(CTkAppOption *opts, const Args &args, uint &i,
                     CTkAppOptionValueMap &values);

  //---

  QString getNewImageName() const;

  CTkAppImageRef createImage(const QString &type, const QString &format,
                             const QString &name, int width=-1, int height=-1);

  void deleteImage(const CTkAppImageRef &image);

  CTkAppImageRef getImage(const QString &name) const;

  CTkAppImageRef getBitmap(const QString &name) const;

  void getImageNames(std::vector<QString> &names) const;

  //---

  QString getNewFontName() const;

  CTkAppFontRef createFont(const QString &name);
  void deleteFont(const CTkAppFontRef &font);

  CTkAppFontRef getFont(const QString &name) const;

  QFont getQFont(const QString &name) const;

  struct FontData {
    QString family     { };
    double  size       { 12.0 };
    QString weight     { "normal" };
    QString slant      { "roman" };
    bool    underline  { false };
    bool    overstrike { false };

    double ascent    { 10 };
    double descent   { 2 };
    double linespace { 12 };
    bool   fixed     { false };
  };

  void getFontData(const QFont &qfont, FontData &data) const;

  void getFontNames(std::vector<QString> &names) const;

  void showFontDialog(bool b);

  //---

  void *getWidgetClassData(const QString &name) const;

  //---

  bool bindAllEvent  (const CTkAppEventData &data);
  bool bindTagEvent  (const QString &tagName, const CTkAppEventData &data);
  bool bindClassEvent(const QString &tagName, const CTkAppEventData &data);

  void getClassBindings(const QString &name, const CTkAppEventData &data,
                        std::vector<QString> &bindings);
  void getTagBindings(const QString &name, const CTkAppEventData &data,
                      std::vector<QString> &bindings);
  void getAllBindings(const CTkAppEventData &data,
                      std::vector<QString> &bindings);

  void getClassBindings(const QString &name, std::vector<QString> &bindings);
  void getTagBindings(const QString &name, std::vector<QString> &bindings);
  void getAllBindings(std::vector<QString> &bindings);

  //---

  bool triggerEvents(const QString &, CTkAppWidget *, QEvent *e,
                     const CTkAppEventData &matchEventData);
  bool triggerVirtualEvents(const QString &, CTkAppWidget *,
                            const CTkAppEventData &matchEventData);

  bool execEvent(CTkAppWidget *, QEvent *e, const CTkAppEventData &data,
                 const QString &command);
  bool execVirtualEvent(CTkAppWidget *, const CTkAppEventData &data,
                        const QString &command);

  //---

  CTkAppTopLevel *installToplevel(const QString &id, QFrame *frame);

  void addTopLevel(CTkAppTopLevel *toplevel);

  int addWidget(CTkAppWidget *w);

  CTkAppWidget *lookupWidget(QWidget *w) const;

  void removeWidget(CTkAppWidget *w);

  void addDeleteWidget(CTkAppWidget *w);

  void purgeWidgets();

  //---

  void encodeEvent(QKeyEvent *e, bool press, CTkAppEventData &data) const;
  void encodeEvent(QMouseEvent *e, CTkAppEventMode mode, int button, CTkAppEventData &data) const;

  bool stringToVirtualEvent(const QString &str, CTkAppVirtualEventData &data,
                            bool quiet=false) const;

  bool parseEvent(const QString &str, CTkAppEventData &data);

  void addVirtualEventData(const CTkAppVirtualEventData &vdata, const CTkAppEventData &edata);

  void virtualEventNames(std::vector<QString> &names) const;
  void virtualEventNames(const CTkAppVirtualEventData &vdata,
                         std::vector<QString> &names) const;

  //---

  void addOption(const QString &pattern, const QString &value, int priority);

  bool matchOption(const QString &widgetClass, const QString &optName,
                   const QString &optClass, QString &optValue) const;

  void clearOptions();

  //---

  bool lookupOptionName(const std::vector<QString> &names,
                        const QString &arg, QString &opt, bool quiet=false) const;

  bool lookupName(const QString &msg, const std::vector<QString> &names,
                  const QString &arg, QString &opt, bool quiet=false) const;

  bool getOptionInt (const QString &name, const QString &value, long &i) const;
  bool getOptionReal(const QString &name, const QString &value, double &r) const;

  //---

  const QString &currentCommand() const { return currentCommand_; }
  void setCurrentCommand(const QString &s) { currentCommand_ = s; }

  //---

  QString newMatrixName() const {
    return QString("matrix_%1").arg(nameMatrices_.size() + 1);
  }

  void setNamedMatrix(const QString &name, const MatrixData &d) {
    nameMatrices_[name] = d;
  }

  bool getNamedMatrix(const QString &name, MatrixData &d) {
    auto pm = nameMatrices_.find(name);
    if (pm == nameMatrices_.end()) return false;
    d = (*pm).second;
    return true;
  }

  QTransform toQTransform(const CMatrix2D &m) const;

  //---

  bool wrongNumArgs(const QString &msg) const;

  bool throwError(const QString &msg) const;

  void debugCmd(const QString &cmd, const Args &args) const;

  bool TODO(const QString &msg="") const;
  bool TODO(const Args &args) const;
  bool TODO(const QString &arg, const Args &args) const;

 protected:
  void construct(int argc, const char **argv);

  void addCommands() override;

  void addWidgetClass(const QString &name);

 private Q_SLOTS:
  void timerSlot();

 private:
  using WidgetClasses    = std::set<QString>;
  using ImageMap         = std::map<QString, CTkAppImageRef>;
  using FontMap          = std::map<QString, CTkAppFontRef>;
  using TopLevelArray    = std::vector<CTkAppTopLevel *>;
  using EventDatas       = std::vector<CTkAppEventData>;
  using ClassEventDatas  = std::map<QString, EventDatas>;
  using TagEventDatas    = std::map<QString, EventDatas>;
  using WidgetSet        = std::set<CTkAppWidget *>;
  using WidgetP          = QPointer<CTkAppWidget>;
  using WidgetArray      = std::vector<WidgetP>;
  using VirtualEventData = std::map<CTkAppVirtualEventData, EventDatas>;

  //--

  struct OptionData {
    QString pattern;
    QString value;
    int     priority { -1 };

    OptionData() = default;

    OptionData(const QString &pattern1, const QString &value1, int priority1) :
     pattern(pattern1), value(value1), priority(priority1) {
    }
  };

  using OptionDatas = std::vector<OptionData>;

  //--

  using NamedMatrices = std::map<QString, MatrixData>;

  //--

  QString context_;

  WidgetClasses widgetClasses_;
  NamedMatrices nameMatrices_;

  CTkAppRoot* root_ { nullptr };

  QTimer *timer_ { nullptr };

  bool            useStyle_ { false };
  TopLevelArray   toplevels_;
  ClassEventDatas classEvents_;
  TagEventDatas   tagEvents_;
  EventDatas      allEvents_;
  WidgetSet       widgets_;
  WidgetArray     deleteWidgets_;

  ImageMap         images_;
  mutable ImageMap bitmaps_;

  QFont   appFont_;
  FontMap fonts_;

  OptionDatas options_;

  QString currentCommand_;

  VirtualEventData virtualEventData_;
};

#endif

#ifndef CTkApp_H
#define CTkApp_H

#include <CTkAppOptionValue.h>
#include <CTkAppEventData.h>
#include <CTkAppTypes.h>
#include <CTkAppUtil.h>

#include <CTclApp.h>
#include <CMatrix2D.h>

#include <QPointer>
#include <QTransform>
#include <QFont>

#include <memory>
#include <vector>
#include <set>
#include <map>
#include <optional>

class CTkAppRoot;
class CTkAppTopLevel;
class CTkAppWidget;
class CTkAppImage;
class CTkAppFont;
class CTkAppTopLevelWidget;
class CTkAppCursorMgr;
class CTkAppCursor;
class CTkAppImageCommand;

using CTkAppImageRef = std::shared_ptr<CTkAppImage>;
using CTkAppFontRef  = std::shared_ptr<CTkAppFont>;

enum class CTkAppClipboard {
  Clipboard,
  Selection
};

class QFrame;
class QWidget;
class QEvent;
class QKeyEvent;
class QMouseEvent;
class QTimer;

struct CTkAppCursorData {
  bool    isAt { false };
  QString name;
  QString maskName;
  QString fgColor;
  QString bgColor;
};

//---

class CTkApp : public QObject, public CTclApp {
  Q_OBJECT

 public:
  class Msg {
   public:
    explicit Msg(const CTkApp *app) :
     app_(app) {
    }

    Msg &operator+(const char *str) { str_ += str; return *this; }
    Msg &operator+(const QString &str) { str_ += str; return *this; }
    Msg &operator+(const QVariant &var) { str_ += app_->variantToString(var); return *this; }

    const QString &str() const { return str_; }

   private:
    const CTkApp* app_ { nullptr };
    QString       str_;
  };

 public:
  struct MatrixData {
    QTransform transform;
  };

  using Args = std::vector<QVariant>;

  using WidgetP    = QPointer<CTkAppWidget>;
  using WidgetSet  = std::set<WidgetP>;
  using Distance   = CTkAppDistance;
  using CursorData = CTkAppCursorData;

 public:
  explicit CTkApp(Tcl_Interp *interp, const QString &context="");
  explicit CTkApp(int argc, const char **argv, const QString &context="");

  //---

  CTkAppRoot *root() const { return root_; }

  void setRootFrame(QFrame *frame);

  //---

  const QString &getAppName() const { return appName_; }
  void setAppName(const QString &s) { appName_ = s; }

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
  void showToplevels();

  int numShowToplevels() const;

  //---

  CTkAppWidget *lookupWidgetByName(const QVariant &widgetName, bool quiet=false) const;

  //---

  bool processOption(CTkAppOption *opts, const Args &args, uint &i,
                     CTkAppOptionValueMap &values);

  //---

  bool commandExists(const QString &name) const;

  //---

  QString getNewImageName() const;

  CTkAppImageRef createImage(const QString &type, const QString &format, const QString &name,
                             int width=0, int height=0);

  void deleteImage(const CTkAppImageRef &image);

  void renameImage(const QString &oldName, const QString &newName);

  CTkAppImageRef getImage(const QVariant &var) const;

  void getImageNames(std::vector<QString> &names) const;

  CTkAppImageRef getBitmap(const QString &name, bool create=false) const;
  void addBitmap(const QString &name, CTkAppImageRef &image);

  void getBitmapNames(std::vector<QString> &names) const;

  //---

  CTkAppImageCommand *addImageCommand(const CTkAppImageRef &image);

  //---

  QString getNewFontName() const;

  CTkAppFontRef createFont(const QVariant &var);
  void deleteFont(const CTkAppFontRef &font);

  CTkAppFontRef getFont(const QVariant &var) const;

  QFont getQFont(const QVariant &var) const;

  void getFontNames(std::vector<QString> &names) const;

  void showFontDialog(bool b);

  //---

  CTkAppCursor *getCursor(const QString &name);

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

  CTkAppTopLevel *installToplevel(const QString &id, CTkAppTopLevelWidget *frame);

  void addTopLevel(CTkAppTopLevel *toplevel);
  void removeTopLevel(CTkAppTopLevel *toplevel);

  int addWidget(CTkAppWidget *w);

  CTkAppWidget *lookupWidget(QWidget *w) const;
  CTkAppWidget *lookupWidgetId(ulong id) const;

  CTkAppWidget *getWidgetAt(long x, long y) const;

  CTkAppWidget *getWidgetForWin(ulong xwin) const;

  const WidgetSet &widgets() const { return widgets_; }

  //---

  void removeWidget(CTkAppWidget *w);

  uint numDeleteWidgets() const;

  void addDeleteWidget(CTkAppWidget *w);

  void purgeWidgets();

  //---

  void encodeEvent(QKeyEvent *e, bool press, CTkAppEventData &data) const;
  void encodeEvent(QMouseEvent *e, CTkAppEventMode mode, int button, CTkAppEventData &data) const;

  bool stringToVirtualEvent(const QVariant &var, CTkAppVirtualEventData &data,
                            bool quiet=false) const;

  bool parseEvent(const QVariant &var, CTkAppEventData &data);

  int stringToKeySym(const QString &key) const;

  void addVirtualEventData(const CTkAppVirtualEventData &vdata, const CTkAppEventData &edata);

  void deleteAllVirtualEventData(const CTkAppVirtualEventData &vdata);
  void deleteVirtualEventData(const CTkAppVirtualEventData &vdata, const CTkAppEventData &edata);

  void virtualEventNames(std::vector<QString> &names) const;
  void virtualEventNames(const CTkAppVirtualEventData &vdata,
                         std::vector<QString> &names) const;

  //---

  void addOption(const QString &pattern, const QVariant &value, int priority);

  bool matchOption(const QString &widgetClass, const QString &optName,
                   const QString &optClass, QVariant &optValue) const;

  void clearOptions();

  //---

  bool lookupOptionName(const std::vector<QString> &names,
                        const QVariant &arg, QString &opt, bool quiet=false) const;
  bool lookupOptionName(const std::vector<QString> &names,
                        const QString &arg, QString &opt, bool quiet=false) const;

  bool lookupName(const QString &msg, const std::vector<QString> &names,
                  const QVariant &arg, QString &opt, bool quiet=false) const;
  bool lookupName(const QString &msg, const std::vector<QString> &names,
                  const QString &arg, QString &opt, bool quiet=false) const;

  QString concatOptionNames(const std::vector<QString> &names) const;

  //---

  bool getOptionInt (const QString &name, const QVariant &value, long &i) const;
  bool getOptionReal(const QString &name, const QVariant &value, double &r) const;

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

  CTkAppWidget *getWmGroup(CTkAppWidget *group) const;
  void setWmGroup(CTkAppWidget *group, CTkAppWidget *child);

  void getWmGrid(int &w, int &h, int &iw, int &ih);
  void setWmGrid(int w, int h, int iw, int ih);

  //---

  QString getClipboardText(CTkAppClipboard type) const;
  void    setClipboardText(const QString &text, CTkAppClipboard type);

  //---

  double getScaling() const;
  void setScaling(double r) { scaling_ = r; }

  //---

  bool variantIsValid(const QVariant &var) const;

  bool variantToCursor(const QVariant &var, CursorData &c) const;

  bool variantToQColor(const QVariant &var, QColor &c) const;

  bool variantToDistance (const QVariant &var, Distance &r) const;
  bool variantToDistanceI(const QVariant &var, Distance &r) const;

  bool variantToInt (const QVariant &var, long &i) const;
  bool variantToReal(const QVariant &var, double &r) const;
  bool variantToBool(const QVariant &var, bool &b) const;

  QString variantToString(const QVariant &var) const;

  bool variantToAlign(const QVariant &var, Qt::Alignment &align) const;

  //---

  QVariant boolToVariant(bool b) const;
  QVariant intToVariant(long i) const;
  QVariant realToVariant(double r) const;
  QVariant colorToVariant(const QColor &c) const;
  QVariant fontToVariant(const QFont &f) const;
  QVariant imageToVariant(const CTkAppImageRef &image) const;

  //---

  bool fileExists(const QString &filename) const;

  //---

  QString geometryStr(const QRect &r) const;

  //---

  Msg msg() const { return Msg(this); }

  bool wrongNumArgs(const QString &msg) const;

  bool invalidBool    (const QVariant &var) const;
  bool invalidInteger (const QVariant &var) const;
  bool invalidReal    (const QVariant &var) const;
  bool invalidDistance(const QVariant &var) const;
  bool invalidQColor  (const QVariant &var) const;
  bool invalidQColor1 (const QVariant &var) const;
  bool invalidCursor  (const QVariant &var) const;
  bool unknownOption  (const QVariant &var) const;

  bool throwError(const Msg &msg) const;
  bool throwError(const QString &msg) const;

  void debugCmd(const QString &cmd, const Args &args) const;

  bool TODO(const Msg &msg) const;
  bool TODO(const QString &msg="") const;
  bool TODO(const Args &args) const;
  bool TODO(const QString &arg, const Args &args) const;

  //---

  void showDebug();

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
  using TopLevelP        = QPointer<CTkAppTopLevel>;
  using TopLevelArray    = std::vector<TopLevelP>;
  using TopLevelSet      = std::set<TopLevelP>;
  using EventDatas       = std::vector<CTkAppEventData>;
  using ClassEventDatas  = std::map<QString, EventDatas>;
  using TagEventDatas    = std::map<QString, EventDatas>;
  using WidgetArray      = std::vector<WidgetP>;
  using VirtualEventData = std::map<CTkAppVirtualEventData, EventDatas>;
  using ImageCommands    = std::map<uint, CTkAppImageCommand *>;

  //--

  struct OptionData {
    QString  pattern;
    QVariant value;
    int      priority { -1 };

    OptionData() = default;

    OptionData(const QString &pattern1, const QVariant &value1, int priority1) :
     pattern(pattern1), value(value1), priority(priority1) {
    }
  };

  using OptionDatas = std::vector<OptionData>;

  //--

  using NamedMatrices = std::map<QString, MatrixData>;

  using WmGroups = std::map<CTkAppWidget *, CTkAppWidget *>;

  //--

  CTkAppRoot* root_ { nullptr };

  QString context_;
  QString appName_;
  bool    useStyle_ { false };
  double  scaling_  { -1.0 };

  QTimer *timer_ { nullptr };

  // widget class names
  WidgetClasses widgetClasses_;

  QString currentCommand_;

  // named matrices
  NamedMatrices nameMatrices_;

  TopLevelSet toplevels_; // toplevels

  // event data
  ClassEventDatas classEvents_;
  TagEventDatas   tagEvents_;
  EventDatas      allEvents_;

  VirtualEventData virtualEventData_;

  // widgets
  WidgetSet   widgets_;
  WidgetArray deleteWidgets_;

  // images and bitmaps
  ImageMap         images_;
  mutable ImageMap bitmaps_;

  // fonts
  QFont   appFont_;
  FontMap fonts_;

  OptionDatas options_;

  WmGroups wmGroups_;

  struct WmGrid {
    int w  { -1 };
    int h  { -1 };
    int iw { -1 };
    int ih { -1 };
  };

  WmGrid wmGrid_;

  CTkAppCursorMgr *cursorMgr_ { nullptr };
};

#endif

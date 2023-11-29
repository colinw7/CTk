#ifndef CTkAppWidget_H
#define CTkAppWidget_H

#include <CTkAppEventData.h>
#include <CTkApp.h>
#include <CTkAppUtil.h>

#include <CQLabelImage.h>

#include <QBrush>
#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPainterPath>
#include <QPen>
#include <QPushButton>
#include <QScrollBar>
#include <QToolButton>
#include <QTreeWidget>
#include <QWidgetAction>

#include <optional>
#include <cmath>

class CTkApp;
class CTkAppRoot;
class CTkAppTopLevel;
class CTkAppWidgetCommand;
class CTkAppOptData;

class CTkAppLayout;
class CTkAppPackLayout;
class CTkAppGridLayout;
class CTkAppPlaceLayout;

class CQRealSlider;

class QBoxLayout;
class QFrame;
class QGroupBox;
class QMenuBar;
class QSplitter;
class QTabWidget;
class QTextCursor;
class QTextEdit;
class QTreeWidget;

//---

class CTkAppWidgetEventFilter;

class CTkAppWidget : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString type     READ getType)
  Q_PROPERTY(QString name     READ getName)
  Q_PROPERTY(QString fullName READ getFullName)

  Q_PROPERTY(QColor background READ background WRITE setBackground)
  Q_PROPERTY(QColor foreground READ foreground WRITE setForeground)

  Q_PROPERTY(QColor activeBackground READ activeBackground WRITE setActiveBackground)
  Q_PROPERTY(QColor activeForeground READ activeForeground WRITE setActiveForeground)

  Q_PROPERTY(QColor disabledBackground READ disabledBackground WRITE setDisabledBackground)
  Q_PROPERTY(QColor disabledForeground READ disabledForeground WRITE setDisabledForeground)

  Q_PROPERTY(QColor highlightBackground READ highlightBackground WRITE setHighlightBackground)
  Q_PROPERTY(QColor highlightForeground READ highlightForeground WRITE setHighlightForeground)

  Q_PROPERTY(QColor insertBackground READ insertBackground WRITE setInsertBackground)

  Q_PROPERTY(QString relief READ reliefStr WRITE setReliefStr)

  Q_PROPERTY(int repeatDelay    READ repeatDelay    WRITE setRepeatDelay)
  Q_PROPERTY(int repeatInterval READ repeatInterval WRITE setRepeatInterval)

  Q_PROPERTY(QString       anchorStr READ anchorStr WRITE setAnchorStr)
  Q_PROPERTY(Qt::Alignment anchor    READ anchor    WRITE setAnchor)

  Q_PROPERTY(double borderWidth       READ borderWidth       WRITE setBorderWidth)
  Q_PROPERTY(double selectBorderWidth READ selectBorderWidth WRITE setSelectBorderWidth)
  Q_PROPERTY(double insertBorderWidth READ insertBorderWidth WRITE setInsertBorderWidth)
  Q_PROPERTY(double insertWidth       READ insertWidth       WRITE setInsertWidth)

  Q_PROPERTY(QString bitmap READ bitmap WRITE setBitmap)

  Q_PROPERTY(int highlightThickness READ highlightThickness WRITE setHighlightThickness)

  Q_PROPERTY(QString text     READ getText     WRITE setText)
  Q_PROPERTY(double  width    READ getWidth    WRITE setWidth)
  Q_PROPERTY(double  height   READ getHeight   WRITE setHeight)
  Q_PROPERTY(QString title    READ getTitle    WRITE setTitle)
  Q_PROPERTY(QString icon     READ getIcon     WRITE setIcon)
  Q_PROPERTY(QString geometry READ getGeometry WRITE setGeometry)

  Q_PROPERTY(int padX READ padX WRITE setPadX)
  Q_PROPERTY(int padY READ padY WRITE setPadY)

  Q_PROPERTY(Qt::Alignment justify READ justify WRITE setJustify)

  Q_PROPERTY(QString command        READ getCommand        WRITE setCommand)
  Q_PROPERTY(QString xScrollCommand READ getXScrollCommand WRITE setXScrollCommand)
  Q_PROPERTY(QString yScrollCommand READ getYScrollCommand WRITE setYScrollCommand)

  Q_PROPERTY(bool gridWidget READ isGridWidget WRITE setGridWidget)

  Q_PROPERTY(QString deleteWindowCmd READ deleteWindowCmd WRITE setDeleteWindowCmd)
  Q_PROPERTY(QString wmTransientFor  READ wmTransientFor  WRITE setWmTransientFor)

 public:
  using Relief   = CTkAppWidgetRelief;
  using Args     = std::vector<QVariant>;
  using Children = std::vector<CTkAppWidget *>;

  using OptPointI = std::optional<QPoint>;
//using OptPointF = std::optional<QPointF>;

  enum class FocusModel {
    Active,
    Passive
  };

  enum class PositionFrom {
    NONE,
    USER,
    PROGRAM
  };

  enum class SizeFrom {
    NONE,
    USER,
    PROGRAM
  };

 public:
  explicit CTkAppWidget(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  virtual ~CTkAppWidget();

  //---

  CTkApp *app() const { return tk_; }

  CTkAppWidget *getParent() const { return parent_; }

  QWidget *parentWidget() const { return parent_->getQWidget(); }
  void setParentWidget(QWidget *w);

  CTkAppRoot *root() const;

  CTkAppTopLevel *toplevel() const;

  virtual bool isRoot() const { return false; }
  virtual bool isTopLevel() const { return false; }

  virtual void setNeedsShow(bool) { assert(false); }

  //---

  virtual const char *getClassName() const { return ""; }

  //---

  QString getType() const { return QString(getClassName()); }

  const QString &getName() const { return name_; }

  QString getFullName() const;

  //---

  void setQWidget(QWidget *w);
  QWidget *getQWidget() const;

  int parentDepth() const {
    return (parent_ ? parent_->parentDepth() + 1 : 1);
  }

  //---

  const CTkAppWidgetCommand *getWidgetCommand() const { return widgetCommand_; }
  void setWidgetCommand(CTkAppWidgetCommand *p) { widgetCommand_ = p; }

  const CTkAppOptData &getOpts() const;

  //---

  const QColor &background() const { return background_; }
  void setBackground(const QColor &c);

  const QColor &foreground() const { return foreground_; }
  void setForeground(const QColor &c);

  const QColor &activeBackground() const { return activeBackground_; }
  void setActiveBackground(const QColor &c);

  const QColor &activeForeground() const { return activeForeground_; }
  void setActiveForeground(const QColor &c);

  const QColor &disabledBackground() const { return disabledBackground_; }
  void setDisabledBackground(const QColor &c);

  const QColor &disabledForeground() const { return disabledForeground_; }
  void setDisabledForeground(const QColor &c);

  const QColor &highlightBackground() const { return highlightBackground_; }
  void setHighlightBackground(const QColor &c);

  const QColor &highlightForeground() const { return highlightForeground_; }
  void setHighlightForeground(const QColor &c);

  const QColor &insertBackground() const { return insertBackground_; }
  void setInsertBackground(const QColor &c);

  void updatePalette();

  //---

  int highlightThickness() const { return highlightThickness_; }
  void setHighlightThickness(int i) { highlightThickness_ = i; }

  //---

  const QString &getText() const { return text_; }
  virtual void setText(const QString &s) { text_ = s; }

  //---

  const QString &bitmap() const { return bitmap_; }
  bool setBitmap(const QString &s);

  const QString &image() const { return image_; }
  bool setImage(const QString &s);

  const CTkAppImageRef &getImageRef() const { return imageRef_; }
  virtual void setImageRef(const CTkAppImageRef &i);

  const CTkAppCursorData &cursor() const { return cursor_; }
  bool setCursor(const CTkAppCursorData &c);

  const CTkAppCompoundType &compoundType() const { return compoundType_; }
  void setCompoundType(const CTkAppCompoundType &v) { compoundType_ = v; }

  //---

  virtual double getWidth() const;
  virtual void setWidth(double w);

  virtual double getHeight() const;
  virtual void setHeight(double h);

  //---

  const OptPointI &wmMinAspect() const { return wmMinAspect_; }
  void setWmMinAspect(const OptPointI &v) { wmMinAspect_ = v; }

  const OptPointI &wmMaxAspect() const { return wmMaxAspect_; }
  void setWmMaxAspect(const OptPointI &v) { wmMaxAspect_ = v; }

  void updateWmSizeHints();

  //---

  virtual QSize sizeHint() const;

  virtual QSize minimumSizeHint() const;

  QMargins contentsMargins() const;

  //---

  bool isDeleted() const { return deleted_; }

  //---

  void addChild   (CTkAppWidget *w);
  void removeChild(CTkAppWidget *w);

  void getChildren(Children &children) const;

  CTkAppWidget *getChild(const QString &name) const;

  void getColormapChildren(Children &children) const;

  //---

  bool isInitNotify() const { return initNotify_; }
  void setInitNotify(bool b) { initNotify_ = b; }

  //---

  void setFocus();

  const FocusModel &focusModel() const { return focusModel_; }
  void setFocusModel(const FocusModel &v) { focusModel_ = v; }

  //---

  void lower();
  void raise();

  //---

  bool hasColormap() const { return colormap_; }
  void setColormap(bool b, CTkAppWidget *) { colormap_ = b; }

  const QString &visual() const { return visual_; }
  void setVisual(const QString &s) { visual_ = s; }

  //---

  const QString &getXScrollCommand() const { return xScrollCommand_; }
  void setXScrollCommand(const QString &s) { xScrollCommand_ = s; }

  const QString &getYScrollCommand() const { return yScrollCommand_; }
  void setYScrollCommand(const QString &s) { yScrollCommand_ = s; }

  //---

  CTkAppLayout *getTkLayout();

  CTkAppPackLayout  *getTkPackLayout(bool create=true);
  CTkAppGridLayout  *getTkGridLayout(bool create=true);
  CTkAppPlaceLayout *getTkPlaceLayout(bool create=true);

  const Qt::Alignment &getGridAnchor() const { return gridAnchor_; }
  void setGridAnchor(const Qt::Alignment &a) { gridAnchor_ = a; }

  void getGridBBox(int &column1, int &row1, int &column2, int &row2) const {
    column1 = gridColumn1_; row1 = gridRow1_;
    column2 = gridColumn2_; row2 = gridRow2_;
  }
  void setGridBBox(int column1, int row1, int column2, int row2) {
    gridColumn1_ = column1; gridRow1_ = row1;
    gridColumn2_ = column2; gridRow2_ = row2;
  }

  bool getGridPropagate() const { return gridPropagate_; }
  void setGridPropagate(bool b) { gridPropagate_ = b; }

  //---

  // handle name/value change (configure)
  virtual bool execConfig(const QString &name, const QVariant &value);

  bool execDefaultOp(const QString &option, const Args &args);

  // handle widget operation (<widget> <operation>)
  virtual bool execOp(const Args &);

  virtual bool show();
  virtual void hide();

  //---

  QString getTitle() const;
  void setTitle(const QString &s);

  //---

  QString getIcon() const;
  void setIcon(const QString &s);

  const QPoint &iconPosition() const { return iconPosition_; }
  void setIconPosition(const QPoint &p) { iconPosition_ = p; }

  const CTkAppImageRef &iconMask() const { return iconMask_; }
  void setIconMask(const CTkAppImageRef &i) { iconMask_ = i; }

  //---

  QString getGeometry() const;
  bool setGeometry(const QString &s);

  bool setTransientGeometry(CTkAppWidget *pw, const QString &s);

  //---

  int padX() const { return padx_; }
  virtual void setPadX(int i);

  int padY() const { return pady_; }
  virtual void setPadY(int i);

  //---

  const Qt::Alignment &justify() const { return justify_; }
  virtual void setJustify(const Qt::Alignment &a) { justify_ = a; }

  //--

  void setWidgetAnchor(const QString &value);

  //---

  double borderWidth() const { return borderWidth_; }
  void setBorderWidth(double width);

  double selectBorderWidth() const { return selectBorderWidth_; }
  void setSelectBorderWidth(double width);

  double insertBorderWidth() const { return insertBorderWidth_; }
  void setInsertBorderWidth(double width);

  int insertOffTime() const { return insertOffTime_; }
  void setInsertOffTime(int i) { insertOffTime_ = i; }

  int insertOnTime() const { return insertOnTime_; }
  void setInsertOnTime(int i) { insertOnTime_ = i; }

  double insertWidth() const { return insertWidth_; }
  void setInsertWidth(double width);

  //---

  const QString &getOptionClass() const { return optionClass_; }
  void setOptionClass(const QString &s) { optionClass_ = s; }

  //---

  const Relief &relief() const { return relief_; }
  void setRelief(const Relief &v) { relief_ = v; }

  QString reliefStr() const { return reliefVar_.toString(); }
  bool setReliefStr(const QString &s);

  bool setReliefFromVar(const QVariant &s);

  //---

  int repeatDelay() const { return repeatDelay_; }
  void setRepeatDelay(int i) { repeatDelay_ = i; }

  int repeatInterval() const { return repeatInterval_; }
  void setRepeatInterval(int i) { repeatInterval_ = i; }

  //---

  const Qt::Alignment &anchor() const { return anchor_; }
  virtual void setAnchor(const Qt::Alignment &a) { anchor_ = a; }

  const QString &anchorStr() const { return anchorStr_; }
  bool setAnchorStr(const QString &s);

  bool isGridWidget() const { return gridWidget_; }
  void setGridWidget(bool b) { gridWidget_ = b; }

  const QString &deleteWindowCmd() const { return deleteWindowCmd_; }
  void setDeleteWindowCmd(const QString &s) { deleteWindowCmd_ = s; }

  //---

  std::vector<QString> bindtags() const;
  void setBindtags(const std::vector<QString> &strs) { bindtags_ = strs; }

  bool bindEvent(const CTkAppEventData &data);

  void getBindings(const CTkAppEventData &data, std::vector<QString> &bindings);
  void getBindings(std::vector<QString> &bindings);

  virtual bool triggerMousePressEvents  (QEvent *e, int button);
  virtual bool triggerMouseMoveEvents   (QEvent *e, int button, bool pressed);
  virtual bool triggerMouseReleaseEvents(QEvent *e, int button);

  virtual bool triggerKeyPressEvents(QEvent *e);

  virtual bool triggerEvents(QEvent *e, const CTkAppEventData &matchEventData);
  virtual bool triggerVirtualEvents(const CTkAppEventData &matchEventData);

  void processEvents(QEvent *e, const CTkAppEventData &matchEventData);
  void processVirtualEvents(const CTkAppEventData &matchEventData);

  void generateEvent(const CTkAppEventData &eventData);

  //---

  virtual void appearanceChanged() { }

  //---

  void deleteLater();

  bool getOptionValue(const QString &optName, const QString &optClass,
                      QVariant &optValue) const;

  //---

  void isResizable(bool &x, bool &y) { x = xResizable_; y = yResizable_; }
  void setResizable(bool x, bool y) { xResizable_ = x; yResizable_ = y; }

  //---

  void setWmAtomValue(const QString &atomName, const QString &atomValue);
  QString getWmAtomValue(const QString &atomName) const;

  //---

  const QString &wmTransientFor() const { return wmTransientFor_; }
  void setWmTransientFor(const QString &name);

  bool isOverrideRedirect() const { return overrideRedirect_; }
  void setOverrideRedirect(bool b);

  const PositionFrom &positionFrom() const { return positionFrom_; }
  void setPositionFrom(const PositionFrom &p) { positionFrom_ = p; }

  const SizeFrom &sizeFrom() const { return sizeFrom_; }
  void setSizeFrom(const SizeFrom &p) { sizeFrom_ = p; }

  bool setWidgetStateFromString(const QString &value);
  QString widgetStateToString(const CTkAppWidgetState &state) const;

  const CTkAppWidgetState &widgetState() const { return state_; }
  void setWidgetState(const CTkAppWidgetState &state) { state_ = state; }

 protected:
  const QString &getCommand() const { return command_; }
  void setCommand(const QString &command) { command_ = command; }

  void runCommand(const Args &args=Args());

 protected Q_SLOTS:
  void destroySlot(QObject *);

 protected:
  using WidgetP    = QPointer<QWidget>;
  using AppWidgetP = QPointer<CTkAppWidget>;
  using WidgetMap  = std::map<QString, AppWidgetP>;
  using EventDatas = std::vector<CTkAppEventData>;
  using WmAtoms    = std::map<QString, QString>;

  CTkApp*       tk_     { nullptr };
  CTkAppWidget* parent_ { nullptr };
  QString       name_;

  CTkAppWidgetCommand *widgetCommand_ { nullptr };

  int        w_       { 0 };
  int        h_       { 0 };
  bool       deleted_ { false };
  WidgetP    qwidget_;
  WidgetMap  children_;
  EventDatas eventDatas_;

  bool colormap_ { false };

  QString visual_;

  QString xScrollCommand_;
  QString yScrollCommand_;

  Relief   relief_ { Relief::NONE };
  QVariant reliefVar_;

  int repeatDelay_    { -1 };
  int repeatInterval_ { -1 };

  Qt::Alignment anchor_ { Qt::AlignCenter };
  QString       anchorStr_;

  double borderWidth_       { 0.0 };
  double selectBorderWidth_ { 0.0 };
  double insertBorderWidth_ { 0.0 };
  int    insertOffTime_     { -1 };
  int    insertOnTime_      { -1 };
  double insertWidth_       { 0.0 };

  int padx_ { 0 };
  int pady_ { 0 };

  Qt::Alignment justify_ { Qt::AlignLeft | Qt::AlignTop };

  Qt::Alignment gridAnchor_    { Qt::AlignTop | Qt::AlignLeft };
  int           gridColumn1_   { 0 };
  int           gridRow1_      { 0 };
  int           gridColumn2_   { 0 };
  int           gridRow2_      { 0 };
  bool          gridPropagate_ { false };

  QIcon          icon_;
  QString        iconName_;
  QPoint         iconPosition_;
  CTkAppImageRef iconMask_;

  FocusModel focusModel_ { FocusModel::Passive };

  QString command_;

  QString optionClass_;

  QColor background_;
  QColor foreground_;
  QColor activeBackground_;
  QColor activeForeground_;
  QColor disabledBackground_;
  QColor disabledForeground_;
  QColor highlightBackground_;
  QColor highlightForeground_;
  QColor insertBackground_;

  int highlightThickness_ { 0 };

  QString text_;

  QString        bitmap_;
  QString        image_;
  CTkAppImageRef imageRef_;

  std::vector<QString> bindtags_;

  QColor selectBackground_;
  QColor selectForeground_;

  bool gridWidget_ { false }; // is size based on number of grid cells (chars)

  CTkAppCursorData cursor_;

  bool initNotify_ { false };

  CTkAppCompoundType compoundType_ { CTkAppCompoundType::NONE };

  CTkAppWidgetEventFilter *eventFilter_ { nullptr };

  bool xResizable_ { true };
  bool yResizable_ { true };

  WmAtoms wmAtoms_;
  QString deleteWindowCmd_;
  QString wmTransientFor_;

  bool overrideRedirect_ { false };

  PositionFrom positionFrom_ { PositionFrom::NONE };
  SizeFrom     sizeFrom_     { SizeFrom::NONE };

  CTkAppWidgetState state_ { CTkAppWidgetState::NORMAL };

  OptPointI wmMinAspect_;
  OptPointI wmMaxAspect_;
};

//---

class CTkAppWidgetEventFilter :  public QObject {
  Q_OBJECT

 public:
  explicit CTkAppWidgetEventFilter(CTkAppWidget *w) : w_(w) { }

 protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

 private:
  CTkAppWidget* w_ { nullptr };

  mutable bool pressed_     { false };
  mutable int  pressButton_ { 0 };
};

#endif

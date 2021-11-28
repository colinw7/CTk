#ifndef CTK_H
#define CTK_H

#include <CTcl.h>
#include <CImageLib.h>
#include <QWidget>

#include <set>

class CTkWidget;
class CTkRootWidget;
class CTkTopLevel;
class CTkPackLayout;
class CTkGridLayout;
class CTkPlaceLayout;

class QWidget;
class QCheckBox;
class QFrame;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QRadioButton;
class QScrollBar;
class QSlider;
class QSpinBox;
class QSplitter;
class QTextEdit;
class QBoxLayout;

//-----

class CTk;
class CTkImage;

using CTkImageRef = CRefPtr<CTkImage>;

enum class CTkOptionType {
  None,
  Flag,
  String,
  Int,
  Real
};

struct CTkOptionValue {
  int         i { 0 };
  double      r { 0.0 };
  std::string s;
};

struct CTkOption {
  const char*   name  { nullptr };
  CTkOptionType type  { CTkOptionType::None };
  const char*   alias { nullptr };
};

struct CTkOpt {
  const char*name  { nullptr }; // option name
  const char*dname { nullptr }; // database name
  const char*cname { nullptr }; // class name
  const char*def   { nullptr }; // default value
};

using CTkOptionValueMap = std::map<std::string,CTkOptionValue>;

class CTkOptData {
 public:
  CTkOptData(CTk *tk);

  void init(const CTkOpt *opts) {
   opts_ = opts;
  }

  CTclValueRef getOpts();

  bool getOptValue(const std::string &name, std::string &value);

  bool setOptValue(const std::string &name, const std::string &value, const CTkOpt **opt);

 private:
  CTk*              tk_   { nullptr };
  const CTkOpt*     opts_ { nullptr };
  CTkOptionValueMap values_;
};

//-----

class CTk {
 public:
  CTk(CTcl *tcl);

  void exec();

  CTcl *getTcl() const { return tcl_; }

  CTkRootWidget *getRootWidget() const { return root_; }

  void processEvents();

  CTkWidget *lookupWidgetByName(const std::string &widgetName);

  bool decodeWidgetName(const std::string &name, CTkWidget **parent, std::string &childName);

  bool processOption(CTkOption *opts, const std::vector<CTclValueRef> &args, uint &i,
                     CTkOptionValueMap &values);

  void addTopLevel(CTkTopLevel *toplevel);

  std::string getNewImageName() const;

  CTkImageRef createImage(const std::string &type, const std::string &name);

  CTkImageRef getImage(const std::string &name) const;

  void bindEvent(const std::string &tagName, const std::string &pattern,
                 const std::string &command);

  bool triggerEnterEvents(const char *className, CTkWidget *w, QEvent *e);
  bool triggerLeaveEvents(const char *className, CTkWidget *w, QEvent *e);
  bool triggerKeyPressEvents(const char *className, CTkWidget *w, QEvent *e, const std::string &s);

  bool execEvent(CTkWidget *w, QEvent *e, const std::string &str);

  void addWidget(CTkWidget *w);

  CTkWidget *lookupWidget(QWidget *w) const;

  void removeWidget(CTkWidget *w);

  void addDeleteWidget(CTkWidget *w);

  void purgeWidgets();

 private:
  using ImageMap      = std::map<std::string,CTkImageRef>;
  using TopLevelArray = std::vector<CTkTopLevel *>;
  using EventMap      = std::map<std::string,std::string>;
  using TagEventMap   = std::map<std::string,EventMap>;
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

class CTkImage {
 public:
  CTkImage(const std::string &name) :
   name_(name) {
  }

  bool loadFile(const std::string &filename);

  CImagePtr getImage() const { return image_; }

 private:
  std::string name_;
  CImagePtr   image_;
};

//-----

class CTkCommand : public CTclCommand {
 public:
  CTkCommand(CTk *tk, const std::string &name);

  virtual ~CTkCommand() { }

  CTk *getTk() const { return tk_; }

 protected:
  CTk* tk_ { nullptr };
};

class CTkBellCommand : public CTkCommand {
 public:
  CTkBellCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkBindCommand : public CTkCommand {
 public:
  CTkBindCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkButtonCommand : public CTkCommand {
 public:
  CTkButtonCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkCanvasCommand : public CTkCommand {
 public:
  CTkCanvasCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkCheckButtonCommand : public CTkCommand {
 public:
  CTkCheckButtonCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkDestroyCommand : public CTkCommand {
 public:
  CTkDestroyCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkEntryCommand : public CTkCommand {
 public:
  CTkEntryCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkFocusCommand : public CTkCommand {
 public:
  CTkFocusCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkFrameCommand : public CTkCommand {
 public:
  CTkFrameCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkGridCommand : public CTkCommand {
 public:
  CTkGridCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkImageCommand : public CTkCommand {
 public:
  CTkImageCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkLabelCommand : public CTkCommand {
 public:
  CTkLabelCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkLabelFrameCommand : public CTkCommand {
 public:
  CTkLabelFrameCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkListBoxCommand : public CTkCommand {
 public:
  CTkListBoxCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkLowerCommand : public CTkCommand {
 public:
  CTkLowerCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkPackCommand : public CTkCommand {
 public:
  CTkPackCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkPanedWindowCommand : public CTkCommand {
 public:
  CTkPanedWindowCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkPlaceCommand : public CTkCommand {
 public:
  CTkPlaceCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkRadioButtonCommand : public CTkCommand {
 public:
  CTkRadioButtonCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkRaiseCommand : public CTkCommand {
 public:
  CTkRaiseCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkScaleCommand : public CTkCommand {
 public:
  CTkScaleCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkScrollBarCommand : public CTkCommand {
 public:
  CTkScrollBarCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkSpinBoxCommand : public CTkCommand {
 public:
  CTkSpinBoxCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkTextCommand : public CTkCommand {
 public:
  CTkTextCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkTopLevelCommand : public CTkCommand {
 public:
  CTkTopLevelCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkWmCommand : public CTkCommand {
 public:
  CTkWmCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

class CTkWInfoCommand : public CTkCommand {
 public:
  CTkWInfoCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);
};

//-----

class CTkRootWidgetCommand : public CTkCommand {
 public:
  CTkRootWidgetCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);

 private:
  CTkOptData opts_;
};

//-----

class CTkWidgetCommand : public CTkCommand {
 public:
  CTkWidgetCommand(CTkCommand *command, const std::string &name,
                   CTkWidget *w, const CTkOpt *opts=NULL);

  CTclValueRef exec(const std::vector<CTclValueRef> &args);

  bool processArgs(const std::vector<CTclValueRef> &args);

  bool getOptValue(const std::string &name, std::string &value);
  bool setOptValue(const std::string &name, const std::string &value);

 private:
  CTkCommand* command_ { nullptr };
  CTkWidget*  w_       { nullptr };
  CTkOptData  opts_;
};

//-----

class CTkWidget : public QObject {
  Q_OBJECT

 public:
  CTkWidget(CTk *tk, CTkWidget *parent=NULL, const std::string &name="");

  virtual ~CTkWidget();

  CTkWidget *getParent() const { return parent_; }

  virtual const char *getClassName() const = 0;

  const std::string &getName() const { return name_; }

  std::string getFullName() const;

  void setQWidget(QWidget *w);

  QWidget *getQWidget() const;

  int getWidth () const;
  int getHeight() const;

  void setWidth (int w);
  void setHeight(int h);

  bool isDeleted() const { return deleted_; }

  void addChild   (CTkWidget *w);
  void removeChild(CTkWidget *w);

  void getChildren(std::vector<CTkWidget *> &children) const;

  CTkWidget *getChild(const std::string &name) const;

  void setFocus();

  void lower();
  void raise();

  CTkPackLayout  *getTkPackLayout();
  CTkGridLayout  *getTkGridLayout();
  CTkPlaceLayout *getTkPlaceLayout();

  virtual bool notifyValueChanged(const std::string &name, const std::string &value);

  virtual CTclValueRef iexec(const std::vector<CTclValueRef> &) { return CTclValueRef(); }

  void show();

  void setTitle(const std::string &title);

  std::string getTitle() const;

  void bindEvent(const std::string &pattern, const std::string &command);

  bool triggerEnterEvents(QEvent *e);
  bool triggerLeaveEvents(QEvent *e);
  bool triggerMousePressEvents(QEvent *e);
  bool triggerKeyPressEvents(QEvent *e);

  void deleteLater();

 protected:
  using WidgetMap = std::map<std::string, CTkWidget *>;
  using EventMap  = std::map<std::string, std::string>;

  CTk*        tk_      { nullptr };
  CTkWidget*  parent_  { nullptr };
  std::string name_;
  int         w_       { 0 };
  int         h_       { 0 };
  bool        deleted_ { false };
  QWidget*    qwidget_ { nullptr };
  WidgetMap   children_;
  EventMap    events_;
};

//---

class CTkWidgetEventFilter :  public QObject {
  Q_OBJECT

 public:
  CTkWidgetEventFilter(CTkWidget *w) : w_(w) { }

 protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

 private:
  CTkWidget* w_ { nullptr };
};

//---

class CTkRootWidget : public CTkWidget {
 public:
  CTkRootWidget(CTk *tk);

  const char *getClassName() const { return "Window"; }

  bool notifyValueChanged(const std::string &name, const std::string &value);

 private:
  QFrame* qframe_ { nullptr };
};

//---

class CTkButton : public CTkWidget {
  Q_OBJECT

 public:
  CTkButton(CTk *tk, CTkWidget *parent=NULL, const std::string &name="");

  const char *getClassName() const { return "Button"; }

  bool notifyValueChanged(const std::string &name, const std::string &value);

  void setCommand(const std::string &text);

  void setText(const std::string &text);

  void setImage(CImagePtr image);

  CTclValueRef iexec(const std::vector<CTclValueRef> &args);

  void invoke();

 private slots:
  void runCommandSlot();

 private:
  QPushButton* qbutton_ { nullptr };
  std::string  command_;
};

//---

class CTkCanvas : public CTkWidget {
 public:
  CTkCanvas(CTk *tk, CTkWidget *parent=NULL, const std::string &name="");

  const char *getClassName() const { return "Canvas"; }

  bool notifyValueChanged(const std::string &name, const std::string &value);

 private:
  QWidget* qcanvas_ { nullptr };
};

//---

class CTkCheckButton : public CTkWidget {
 public:
  CTkCheckButton(CTk *tk, CTkWidget *parent=NULL, const std::string &name="");

  const char *getClassName() const { return "CheckButton"; }

  bool notifyValueChanged(const std::string &name, const std::string &value);

  void setText(const std::string &text);

 private:
  QCheckBox* qcheck_ { nullptr };
};

//---

class CTkEntry : public CTkWidget {
  Q_OBJECT

 public:
  CTkEntry(CTk *tk, CTkWidget *parent=NULL, const std::string &name="");

  const char *getClassName() const { return "Entry"; }

  bool notifyValueChanged(const std::string &name, const std::string &value);

  void setText(const std::string &text);

 private slots:
  void valueChangedSlot();

 private:
  QLineEdit*  qedit_ { nullptr };
  std::string varName_;
};

//---

class CTkFrame : public CTkWidget {
 public:
  CTkFrame(CTk *tk, CTkWidget *parent=NULL, const std::string &name="");

  const char *getClassName() const { return "Frame"; }

  bool notifyValueChanged(const std::string &name, const std::string &value);

 private:
  QFrame* qframe_ { nullptr };
};

//---

class CTkLabel : public CTkWidget {
 public:
  CTkLabel(CTk *tk, CTkWidget *parent=NULL, const std::string &name="");

  const char *getClassName() const { return "Label"; }

  bool notifyValueChanged(const std::string &name, const std::string &value);

  void setText(const std::string &text);

  void setImage(CImagePtr image);

 private:
  QLabel* qlabel_ { nullptr };
};

//---

class CTkLabelFrame : public CTkWidget {
 public:
  CTkLabelFrame(CTk *tk, CTkWidget *parent=NULL, const std::string &name="");

  const char *getClassName() const { return "LabelFrame"; }

  bool notifyValueChanged(const std::string &name, const std::string &value);

  void setText(const std::string &text);

 private:
  QGroupBox* qframe_ { nullptr };
};

//---

class CTkListBox : public CTkWidget {
 public:
  CTkListBox(CTk *tk, CTkWidget *parent=NULL, const std::string &name="");

  const char *getClassName() const { return "ListBox"; }

  bool notifyValueChanged(const std::string &name, const std::string &value);

 private:
  QListWidget* qlist_ { nullptr };
};

//---

class CTkPanedWindow : public CTkWidget {
 public:
  CTkPanedWindow(CTk *tk, CTkWidget *parent=NULL, const std::string &name="");

  const char *getClassName() const { return "PanedWindow"; }

  bool notifyValueChanged(const std::string &name, const std::string &value);

  CTclValueRef iexec(const std::vector<CTclValueRef> &args);

 private:
  QSplitter* qpane_ { nullptr };
};

//---

class CTkRadioButton : public CTkWidget {
 public:
  CTkRadioButton(CTk *tk, CTkWidget *parent=NULL, const std::string &name="");

  const char *getClassName() const { return "RadioButton"; }

  bool notifyValueChanged(const std::string &name, const std::string &value);

  void setText(const std::string &text);

 private:
  QRadioButton* qradio_ { nullptr };
};

//---

class CTkScale : public CTkWidget {
 public:
  CTkScale(CTk *tk, CTkWidget *parent=NULL, const std::string &name="");

  const char *getClassName() const { return "Scale"; }

  bool notifyValueChanged(const std::string &name, const std::string &value);

  void setText(const std::string &text);

 private:
  QSlider* qscale_ { nullptr };
};

//---

class CTkScrollBar : public CTkWidget {
 public:
  CTkScrollBar(CTk *tk, CTkWidget *parent=NULL, const std::string &name="");

  const char *getClassName() const { return "ScrollBar"; }

  bool notifyValueChanged(const std::string &name, const std::string &value);

 private:
  QScrollBar* qscrollbar_ { nullptr };
};

//---

class CTkSpinBox : public CTkWidget {
 public:
  CTkSpinBox(CTk *tk, CTkWidget *parent=NULL, const std::string &name="");

  const char *getClassName() const { return "SpinBox"; }

  bool notifyValueChanged(const std::string &name, const std::string &value);

 private:
  QSpinBox* qspin_ { nullptr };
};

//---

class CTkText : public CTkWidget {
 public:
  CTkText(CTk *tk, CTkWidget *parent=NULL, const std::string &name="");

  const char *getClassName() const { return "Text"; }

  bool notifyValueChanged(const std::string &name, const std::string &value);

  void setText(const std::string &text);

 private:
  QTextEdit* qtext_ { nullptr };
};

//---

class CTkTopLevel : public CTkWidget {
 public:
  CTkTopLevel(CTk *tk, CTkWidget *parent=NULL, const std::string &name="");

  const char *getClassName() const { return "TopLevel"; }

  bool notifyValueChanged(const std::string &name, const std::string &value);

 private:
  QFrame* qframe_ { nullptr };
};

#endif

#ifndef CTkWidget_H
#define CTkWidget_H

#include <CImageLib.h>

#include <QWidget>

class CTkApp;
class CTkRootWidget;

class CTkPackLayout;
class CTkGridLayout;
class CTkPlaceLayout;

class QFrame;
class QPushButton;
class QRadioButton;
class QCheckBox;
class QLineEdit;
class QGroupBox;
class QLabel;
class QListWidget;
class QMenu;
class QScrollBar;
class QSlider;
class QSpinBox;
class QSplitter;
class QTextEdit;
class QToolButton;
class QBoxLayout;

class CTkWidget : public QObject {
  Q_OBJECT

 public:
  CTkWidget(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  virtual ~CTkWidget();

  //---

  CTkWidget *getParent() const { return parent_; }

  CTkRootWidget *root() const;

  //---

  virtual const char *getClassName() const = 0;

  const std::string &getName() const { return name_; }

  std::string getFullName() const;

  //---

  void setQWidget(QWidget *w);
  QWidget *getQWidget() const;

  //---

  int getWidth () const;
  int getHeight() const;

  void setWidth (int w);
  void setHeight(int h);

  //---

  bool isDeleted() const { return deleted_; }

  //---

  void addChild   (CTkWidget *w);
  void removeChild(CTkWidget *w);

  void getChildren(std::vector<CTkWidget *> &children) const;

  CTkWidget *getChild(const std::string &name) const;

  //---

  bool isInitNotify() const { return initNotify_; }
  void setInitNotify(bool b) { initNotify_ = b; }

  //---

  void setFocus();

  //---

  void lower();
  void raise();

  //---

  CTkPackLayout  *getTkPackLayout();
  CTkGridLayout  *getTkGridLayout();
  CTkPlaceLayout *getTkPlaceLayout();

  //---

  // handle name/value change (configure)
  virtual bool notifyValueChanged(const std::string &name, const std::string &value);

  // handle widget operation (<widget> <operation>)
  virtual std::string iexec(const std::vector<std::string> &);

  void show();

  //---

  void setTitle(const std::string &title);
  std::string getTitle() const;

  //---

  void bindEvent(const std::string &pattern, const std::string &command);

  bool triggerEnterEvents(QEvent *e);
  bool triggerLeaveEvents(QEvent *e);

  bool triggerMousePressEvents  (QEvent *e);
  bool triggerMouseMoveEvents   (QEvent *e, int button);
  bool triggerMouseReleaseEvents(QEvent *e);

  bool triggerKeyPressEvents(QEvent *e);

  //---

  void deleteLater();

 protected:
  using WidgetMap = std::map<std::string, CTkWidget *>;
  using EventMap  = std::map<std::string, std::string>;

  CTkApp*     tk_      { nullptr };
  CTkWidget*  parent_  { nullptr };
  std::string name_;
  int         w_       { 0 };
  int         h_       { 0 };
  bool        deleted_ { false };
  QWidget*    qwidget_ { nullptr };
  WidgetMap   children_;
  EventMap    events_;

  bool initNotify_ { false };
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

  mutable bool pressed_     { false };
  mutable int  pressButton_ { 0 };
};

//---

class CTkRootWidget : public CTkWidget {
 public:
  CTkRootWidget(CTkApp *tk);

  const char *getClassName() const override { return "Window"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

  bool decodeWidgetName(const std::string &name, CTkWidget **parent, std::string &childName) const;

 private:
  QFrame* qframe_ { nullptr };
};

//---

class CTkButton : public CTkWidget {
  Q_OBJECT

 public:
  CTkButton(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Button"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

  void setCommand(const std::string &text);

  void setText(const std::string &text);

  void setImage(CImagePtr image);

  std::string iexec(const std::vector<std::string> &args) override;

  void invoke();
  void flash();

 private slots:
  void runCommandSlot();

 private:
  QPushButton* qbutton_ { nullptr };
  std::string  command_;
};

//---

class CTkCanvas : public CTkWidget {
 public:
  CTkCanvas(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Canvas"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

 private:
  QWidget* qcanvas_ { nullptr };
};

class CTkCanvasWidget : public QWidget {
 public:
  CTkCanvasWidget(QWidget *parent=nullptr);

  QSize sizeHint() const override;
};

//---

class CTkCheckButton : public CTkWidget {
 public:
  CTkCheckButton(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "CheckButton"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

  void setText(const std::string &text);

 private:
  QCheckBox* qcheck_ { nullptr };
};

//---

class CTkEntry : public CTkWidget {
  Q_OBJECT

 public:
  CTkEntry(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Entry"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

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
  CTkFrame(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Frame"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

 private:
  QFrame* qframe_ { nullptr };
};

//---

class CTkLabel : public CTkWidget {
 public:
  CTkLabel(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Label"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

  void setText(const std::string &text);

  void setImage(CImagePtr image);

 private:
  QLabel* qlabel_ { nullptr };
};

//---

class CTkLabelFrame : public CTkWidget {
 public:
  CTkLabelFrame(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "LabelFrame"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

  void setText(const std::string &text);

 private:
  QGroupBox* qframe_ { nullptr };
};

//---

class CTkListBox : public CTkWidget {
 public:
  CTkListBox(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "ListBox"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

 private:
  QListWidget* qlist_ { nullptr };
};

//---

class CTkMenu : public CTkWidget {
  Q_OBJECT

 public:
  CTkMenu(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Menu"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

  std::string iexec(const std::vector<std::string> &args) override;

 private Q_SLOTS:
  void actionPressedSlot();

 private:
  QMenu*      qmenu_ { nullptr };
  std::string command_;
};

//---

class CTkMenuButton : public CTkWidget {
  Q_OBJECT

 public:
  CTkMenuButton(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "MenuButton"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

  void setCommand(const std::string &text);

  void setText(const std::string &text);

  void setImage(CImagePtr image);

  void setMenu(CTkMenu *menu);

  std::string iexec(const std::vector<std::string> &args) override;

  void invoke();

 private slots:
  void runCommandSlot();

 private:
  QToolButton* qbutton_ { nullptr };
  std::string  command_;
};

//---

class CTkMessage : public CTkWidget {
  Q_OBJECT

 public:
  CTkMessage(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Message"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

  void setText(const std::string &text);

 private:
  QTextEdit*  qedit_ { nullptr };
  std::string varName_;
};

//---

class CTkPanedWindow : public CTkWidget {
 public:
  CTkPanedWindow(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "PanedWindow"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

  std::string iexec(const std::vector<std::string> &args) override;

 private:
  QSplitter* qpane_ { nullptr };
};

//---

class CTkRadioButton : public CTkWidget {
 public:
  CTkRadioButton(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "RadioButton"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

  void setText(const std::string &text);

 private:
  QRadioButton* qradio_ { nullptr };
};

//---

class CTkScale : public CTkWidget {
 public:
  CTkScale(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Scale"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

  void setText(const std::string &text);

 private:
  QSlider* qscale_ { nullptr };
};

//---

class CTkScrollBar : public CTkWidget {
 public:
  CTkScrollBar(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "ScrollBar"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

 private:
  QScrollBar* qscrollbar_ { nullptr };
};

//---

class CTkSpinBox : public CTkWidget {
 public:
  CTkSpinBox(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "SpinBox"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

 private:
  QSpinBox* qspin_ { nullptr };
};

//---

class CTkText : public CTkWidget {
 public:
  CTkText(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Text"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

  void setText(const std::string &text);

 private:
  QTextEdit* qtext_ { nullptr };
};

//---

class CTkTopLevel : public CTkWidget {
 public:
  CTkTopLevel(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "TopLevel"; }

  bool notifyValueChanged(const std::string &name, const std::string &value) override;

 private:
  QFrame* qframe_ { nullptr };
};

#endif

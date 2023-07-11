#ifndef CTkWidget_H
#define CTkWidget_H

#include <CImageLib.h>

#include <QFrame>
#include <QWidgetAction>
#include <QCheckBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QMenu>
#include <QLabel>

class CTkApp;
class CTkAppRoot;

class CTkAppPackLayout;
class CTkAppGridLayout;
class CTkAppPlaceLayout;

class CQRealSlider;

class QFrame;
class QPushButton;
class QRadioButton;
class QCheckBox;
class QLineEdit;
class QGroupBox;
class QListWidget;
class QScrollBar;
class QSplitter;
class QTextEdit;
class QToolButton;
class QBoxLayout;

class CTkWidget : public QObject {
  Q_OBJECT

 public:
  using Args = std::vector<std::string>;

 public:
  CTkWidget(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  virtual ~CTkWidget();

  //---

  CTkWidget *getParent() const { return parent_; }

  CTkAppRoot *root() const;

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

  CTkAppPackLayout  *getTkPackLayout();
  CTkAppGridLayout  *getTkGridLayout();
  CTkAppPlaceLayout *getTkPlaceLayout();

  //---

  // handle name/value change (configure)
  virtual bool execConfig(const std::string &name, const std::string &value);

  // handle widget operation (<widget> <operation>)
  virtual bool execOp(const Args &);

  virtual void show();

  //---

  void setTitle(const std::string &title);
  std::string getTitle() const;

  //---

  void setPadX(int i);
  void setPadY(int i);

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
  const std::string &getCommand() const { return command_; }
  void setCommand(const std::string &command) { command_ = command; }

  void runCommand(const Args &args=Args());

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

  std::string command_;

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

class CTkRootWidget;

class CTkAppRoot : public CTkWidget {
  Q_OBJECT

 public:
  CTkAppRoot(CTkApp *tk);

  const char *getClassName() const override { return "Root"; }

  void show() override;

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  bool decodeWidgetName(const std::string &name, CTkWidget **parent, std::string &childName) const;

 private:
  CTkRootWidget* qframe_ { nullptr };
};

class CTkRootWidget : public QFrame {
 public:
  CTkRootWidget(QWidget *parent=nullptr);

  QSize sizeHint() const override;
};

//---

class CTkButton : public CTkWidget {
  Q_OBJECT

 public:
  CTkButton(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Button"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void setImage(CImagePtr image);

  void flash();

 private:
  void connectSlots(bool b);

 private Q_SLOTS:
  void clickSlot();

 private:
  QPushButton* qbutton_ { nullptr };
};

//---

class CTkCanvas : public CTkWidget {
  Q_OBJECT

 public:
  CTkCanvas(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Canvas"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  QWidget* qcanvas_ { nullptr };
};

class CTkCanvasWidget : public QWidget {
 public:
  CTkCanvasWidget(QWidget *parent=nullptr);

  QSize sizeHint() const override;
};

//---

class CTkCheckButtonVarProc;

class CTkCheckButton : public CTkWidget {
  Q_OBJECT

 public:
  CTkCheckButton(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "CheckButton"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void updateVariable();

  void setChecked(bool b);

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void stateChangedSlot(int);

 private:
  QCheckBox*             qcheck_  { nullptr };
  std::string            varName_;
  CTkCheckButtonVarProc* varProc_ { nullptr };
};

//---

class CTkEntryVarProc;

class CTkEntry : public CTkWidget {
  Q_OBJECT

 public:
  CTkEntry(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Entry"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void valueChangedSlot();

 private:
  QLineEdit*       qedit_ { nullptr };
  std::string      varName_;
  CTkEntryVarProc* varProc_ { nullptr };
};

//---

class CTkFrame : public CTkWidget {
  Q_OBJECT

 public:
  CTkFrame(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Frame"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  QFrame* qframe_ { nullptr };
};

//---

class CTkLabelWidget : public QFrame {
  Q_OBJECT

 public:
  enum class Type {
    NONE,
    BOTTOM,
    TOP,
    LEFT,
    RIGHT,
    CENTER
  };

 public:
  CTkLabelWidget(QWidget *parent=nullptr);

  const Type &type() const { return type_; }
  void setType(const Type &t) { type_ = t; update(); }

  const QString &label() const { return label_; }
  void setLabel(const QString &s) { label_ = s; update(); }

  const QImage &image() const { return image_; }
  void setImage(const QImage &i) { image_ = i; update(); }

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override;

 private:
  Type    type_ { Type::NONE };
  QString label_;
  QImage  image_;
};

class CTkLabelVarProc;

class CTkLabel : public CTkWidget {
  Q_OBJECT

 public:
  CTkLabel(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Label"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void setImage(CImagePtr image);

 private:
  CTkLabelWidget*  qlabel_ { nullptr };
  std::string      varName_;
  CTkLabelVarProc* varProc_ { nullptr };
};

//---

class CTkLabelFrame : public CTkWidget {
  Q_OBJECT

 public:
  CTkLabelFrame(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "LabelFrame"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

 private:
  QGroupBox* qframe_ { nullptr };
};

//---

class CTkListBoxVarProc;

class CTkListBox : public CTkWidget {
  Q_OBJECT

 public:
  CTkListBox(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "ListBox"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void updateVariable();

 private:
  void connectSlots(bool b);

 private Q_SLOTS:
  void vscrollSlot(int value);
  void hscrollSlot(int value);

 private:
  QListWidget*       qlist_ { nullptr };
  std::string        varName_;
  CTkListBoxVarProc* varProc_ { nullptr };
  std::string        xScrollCommand_;
  std::string        yScrollCommand_;
};

//---

class CTkCheckAction : public QWidgetAction {
  Q_OBJECT

 public:
  CTkCheckAction() : QWidgetAction(nullptr) { }

  QWidget *createWidget(QWidget *parent) override {
    auto *check = new QCheckBox(parent);

    if (text() != "")
      check->setText(text());

    connect(check, SIGNAL(stateChanged(int)), this, SIGNAL(stateChanged(int)));

    return check;
  }

  const QString &text() const { return text_; }
  void setText(const QString &s) { text_ = s; }

 Q_SIGNALS:
  void stateChanged(int);

 private:
  QString text_;
};

class CTkRadioAction : public QWidgetAction {
  Q_OBJECT

 public:
  CTkRadioAction(CTkApp* tk) :
   QWidgetAction(nullptr), tk_(tk) {
  }

  QWidget *createWidget(QWidget *parent) override {
    radio_ = new QRadioButton(parent);

    if (text() != "")
      radio_->setText(QString::fromStdString(text()));

    connect(radio_, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));

    return radio_;
  }

  const std::string &text() const { return text_; }
  void setText(const std::string &s) { text_ = s; }

  const std::string &value() const { return value_; }
  void setValue(const std::string &s) { value_ = s; }

  const std::string &varName() const { return varName_; }
  void setVarName(const std::string &s) { varName_ = s; }

  void updateVariable();

 Q_SIGNALS:
  void toggled(bool);

 private:
  CTkApp*       tk_ { nullptr };
  QRadioButton* radio_ { nullptr };
  std::string   text_;
  std::string   value_;
  std::string   varName_;
};

class CTkSubMenu : public QMenu {
  Q_OBJECT

 public:
  CTkSubMenu(CTkApp* tk);

  const std::string &menu() const { return menu_; }
  void setMenu(const std::string &s) { menu_ = s; }

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void showSlot();
  void hideSlot();

 private:
  CTkApp*     tk_ { nullptr };
  std::string menu_;
};

class CTkMenu : public CTkWidget {
  Q_OBJECT

 public:
  CTkMenu(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Menu"; }

  QMenu *qmenu() { return qmenu_; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void actionPressedSlot();

  void radioToggledSlot(bool);

 private:
  QMenu*      qmenu_ { nullptr };
  std::string command_;
};

//---

class CTkMenuButtonVarProc;

class CTkMenuButton : public CTkWidget {
  Q_OBJECT

 public:
  CTkMenuButton(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "MenuButton"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void setImage(CImagePtr image);

  void updateMenu();

  void setMenu(CTkMenu *menu);

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void clickSlot();

 private:
  QToolButton*          qbutton_ { nullptr };
  std::string           menuName_;
  std::string           varName_;
  CTkMenuButtonVarProc* varProc_ { nullptr };
};

//---

class CTkMessageVarProc;

class CTkMessage : public CTkWidget {
  Q_OBJECT

 public:
  CTkMessage(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Message"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

 private:
  QTextEdit*         qedit_ { nullptr };
  std::string        varName_;
  CTkMessageVarProc* varProc_ { nullptr };
};

//---

class CTkPanedWindow : public CTkWidget {
  Q_OBJECT

 public:
  CTkPanedWindow(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "PanedWindow"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  QSplitter* qpane_ { nullptr };
};

//---

class CTkRadioButtonVarProc;

class CTkRadioButton : public CTkWidget {
  Q_OBJECT

 public:
  CTkRadioButton(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "RadioButton"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void updateVariable();

  void setValue(const std::string &value);

  void setChecked(bool b);

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void toggleSlot(bool);

 private:
  QRadioButton*          qradio_ { nullptr };
  std::string            varName_;
  std::string            value_ { "1" };
  CTkRadioButtonVarProc* varProc_ { nullptr };
};

//---

class CTkScaleVarProc;

class CTkScale : public CTkWidget {
  Q_OBJECT

 public:
  CTkScale(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Scale"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void updateVariable();

 private:
  void connectSlots(bool);

  void updateSize();

 private Q_SLOTS:
  void valueSlot(int);

 private:
  CQRealSlider*    qscale_  { nullptr };
  std::string      varName_;
  CTkScaleVarProc* varProc_ { nullptr };
  int              length_  { 0 };
  int              width_   { 0 };
};

//---

class CTkScrollBar : public CTkWidget {
  Q_OBJECT

 public:
  CTkScrollBar(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "ScrollBar"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void scrollSlot(int);

 private:
  QScrollBar* qscrollbar_ { nullptr };
};

//---

class CTkSpinBoxWidget : public QSpinBox {
  Q_OBJECT

 public:
  CTkSpinBoxWidget(QWidget *parent=nullptr) :
   QSpinBox(parent) {
  }

  const QStringList &strings() const { return strings_; }

  void setStrings(const QStringList &s) {
    strings_ = s;

    setRange(0, strings_.length() - 1);
  }

  QString textFromValue(int value) const override {
    if (value < 0 || value >= strings_.length())
      return "";

    return strings_[value];
  }

  int valueFromText(const QString &text) const override {
    for (int i = 0; i < strings_.length(); ++i)
      if (strings_[i] == text)
        return i;

    return -1;
  }

 private:
  QStringList strings_;
};

class CTkSpinBox : public CTkWidget {
  Q_OBJECT

 public:
  CTkSpinBox(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "SpinBox"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  CTkSpinBoxWidget* qspin_ { nullptr };
};

//---

class CTkText : public CTkWidget {
  Q_OBJECT

 public:
  CTkText(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Text"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

 private:
  QTextEdit*  qtext_ { nullptr };
  std::string xScrollCommand_;
  std::string yScrollCommand_;
};

//---

class CTkTopLevel : public CTkWidget {
  Q_OBJECT

 public:
  CTkTopLevel(CTkApp *tk, CTkWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "TopLevel"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  QFrame* qframe_ { nullptr };
};

#endif

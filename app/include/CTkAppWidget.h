#ifndef CTkAppWidget_H
#define CTkAppWidget_H

#include <CImageLib.h>

#include <QFrame>
#include <QWidgetAction>
#include <QCheckBox>
#include <QRadioButton>
#include <QMenu>
#include <QLabel>

class CTkApp;
class CTkAppRoot;

class CTkAppPackLayout;
class CTkAppGridLayout;
class CTkAppPlaceLayout;

class CQRealSlider;
class CQSpinList;
class CQLabelImage;

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
class QMenuBar;

class CTkAppWidget : public QObject {
  Q_OBJECT

 public:
  using Args = std::vector<std::string>;

 public:
  CTkAppWidget(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  virtual ~CTkAppWidget();

  //---

  CTkAppWidget *getParent() const { return parent_; }

  CTkAppRoot *root() const;

  QWidget *qwidget() const { return qwidget_; }

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

  void addChild   (CTkAppWidget *w);
  void removeChild(CTkAppWidget *w);

  void getChildren(std::vector<CTkAppWidget *> &children) const;

  CTkAppWidget *getChild(const std::string &name) const;

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

  //--

  const Qt::Alignment &align() const { return align_; }
  void setAlign(const Qt::Alignment &v) { align_ = v; }

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
  using WidgetMap = std::map<std::string, CTkAppWidget *>;
  using EventMap  = std::map<std::string, std::string>;

  CTkApp*       tk_      { nullptr };
  CTkAppWidget* parent_  { nullptr };
  std::string   name_;
  int           w_       { 0 };
  int           h_       { 0 };
  bool          deleted_ { false };
  QWidget*      qwidget_ { nullptr };
  WidgetMap     children_;
  EventMap      events_;

  Qt::Alignment align_ { Qt::AlignCenter };

  std::string command_;

  bool initNotify_ { false };
};

//---

class CTkWidgetEventFilter :  public QObject {
  Q_OBJECT

 public:
  CTkWidgetEventFilter(CTkAppWidget *w) : w_(w) { }

 protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

 private:
  CTkAppWidget* w_ { nullptr };

  mutable bool pressed_     { false };
  mutable int  pressButton_ { 0 };
};

//---

class CTkRootWidget;

class CTkAppRoot : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkAppRoot(CTkApp *tk);

  const char *getClassName() const override { return "Root"; }

  void show() override;

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  bool decodeWidgetName(const std::string &name, CTkAppWidget **parent,
                        std::string &childName) const;

 private:
  CTkRootWidget* qframe_ { nullptr };
};

class CTkRootWidget : public QFrame {
  Q_OBJECT

 public:
  CTkRootWidget(QWidget *parent=nullptr);

  void setMenu(QMenu *menu);

  QSize sizeHint() const override;

 private:
  QMenu*    menu_    { nullptr };
  QMenuBar* menuBar_ { nullptr };
};

//---

class CTkButton : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

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

class CTkAppCanvasWidget;

class CTkAppCanvas : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkAppCanvas(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Canvas"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  CTkAppCanvasWidget* qcanvas_ { nullptr };
};

class CTkAppCanvasWidget : public QWidget {
  Q_OBJECT

 public:
  enum class ShapeType {
    NONE,
    RECTANGLE,
    LINE,
    TEXT
  };

  struct Point {
    Point() = default;

    Point(double x1, double y1) :
     x(x1), y(y1) {
    }

    double x { 0.0 };
    double y { 0.0 };
  };

  using Points = std::vector<Point>;

  class Shape {
   public:
    Shape(ShapeType type) :
     type_(type) {
    }

    virtual ~Shape() = default;

    ShapeType type() const { return type_; }

   protected:
    ShapeType type_ { ShapeType::NONE };
  };

  class Rectangle : public Shape {
   public:
    Rectangle(double x1, double y1, double x2, double y2) :
     Shape(ShapeType::RECTANGLE), x1_(x1), y1_(y1), x2_(x2), y2_(y2) {
    }

    double x1() const { return x1_; }
    double y1() const { return y1_; }
    double x2() const { return x2_; }
    double y2() const { return y2_; }

   protected:
    double x1_ { 0.0 };
    double y1_ { 0.0 };
    double x2_ { 1.0 };
    double y2_ { 1.0 };
  };

  class Line : public Shape {
   public:
    Line(const Points &points) :
     Shape(ShapeType::LINE), points_(points) {
   }

   const Points &points() const { return points_; }

  protected:
   Points points_;
  };

  class Text : public Shape {
   public:
    Text(const Point &p, const std::string &text) :
     Shape(ShapeType::TEXT), p_(p), text_(text) {
    }

   const Point &pos() const { return p_; }

   const std::string &text() const { return text_; }

  protected:
   Point       p_;
   std::string text_;
  };

 public:
  CTkAppCanvasWidget(QWidget *parent=nullptr);

  void addRectangle(double x1, double y1, double x2, double y2) {
    auto *rectangleShape = new Rectangle(x1, y1, x2, y2);

    addShape(rectangleShape);
  }

  void addLine(const Points &points) {
    auto *lineShape = new Line(points);

    addShape(lineShape);
  }

  void addText(const Point &pos, const std::string &text) {
    auto *textShape = new Text(pos, text);

    addShape(textShape);
  }

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override;

 private:
  void addShape(Shape *s) { shapes_.push_back(s); }

  void drawShape(QPainter *p, Shape *s);

 private:
  using Shapes = std::vector<Shape *>;

  Shapes shapes_;
};

//---

class CTkCheckButtonVarProc;

class CTkCheckButton : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkCheckButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

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
class CTkEntryValidator;

class CTkEntry : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkEntry(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Entry"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void updateVariable();

  bool validate(const std::string &) const;

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void valueChangedSlot();

 private:
  enum class ValidateMode {
    NONE,
    FOCUS,
    FOCUSIN,
    FOCUSOUT,
    KEY,
    ALL
  };

  QLineEdit*         qedit_ { nullptr };
  std::string        varName_;
  CTkEntryVarProc*   varProc_ { nullptr };
  ValidateMode       validateMode_ { ValidateMode::NONE };
  std::string        validateCmd_;
  CTkEntryValidator* validateProc_ { nullptr };
};

//---

class CTkFrame : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkFrame(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Frame"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  QFrame* qframe_ { nullptr };
};

//---

class CTkLabelVarProc;

class CTkLabel : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkLabel(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Label"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void setImage(CImagePtr image);

  void updateVariable();

 private:
  CQLabelImage*    qlabel_ { nullptr };
  std::string      varName_;
  CTkLabelVarProc* varProc_ { nullptr };
};

//---

class CTkLabelFrame : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkLabelFrame(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "LabelFrame"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

 private:
  QGroupBox* qframe_ { nullptr };
};

//---

class CTkListBoxVarProc;

class CTkListBox : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkListBox(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

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
  CTkCheckAction(CTkApp* tk) :
   QWidgetAction(nullptr), tk_(tk) {
  }

  QWidget *createWidget(QWidget *parent) override {
    check_ = new QCheckBox(parent);

    if (text() != "")
      check_->setText(QString::fromStdString(text()));

    connect(check_, SIGNAL(stateChanged(int)), this, SIGNAL(stateChanged(int)));

    return check_;
  }

  const std::string &text() const { return text_; }
  void setText(const std::string &s) { text_ = s; }

  const std::string &varName() const { return varName_; }
  void setVarName(const std::string &s) { varName_ = s; }

  void updateVariable();

 Q_SIGNALS:
  void stateChanged(int);

 private:
  CTkApp*     tk_ { nullptr };
  QCheckBox*  check_ { nullptr };
  std::string text_;
  std::string varName_;
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

class CTkMenu : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkMenu(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Menu"; }

  QMenu *qmenu() { return qmenu_; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  void connectSlots(bool);

 private Q_SLOTS:
  void actionPressedSlot();

  void checkChangedSlot(int);

  void radioToggledSlot(bool);

 private:
  QMenu*      qmenu_ { nullptr };
  std::string command_;
};

//---

class CTkMenuButtonVarProc;

class CTkMenuButton : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkMenuButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "MenuButton"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void setImage(CImagePtr image);

  void updateMenu();

  void setMenu(CTkMenu *menu);

  void updateVariable();

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

class CTkMessage : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkMessage(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "Message"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void updateVariable();

 private:
  QTextEdit*         qedit_ { nullptr };
  std::string        varName_;
  CTkMessageVarProc* varProc_ { nullptr };
};

//---

class CTkPanedWindow : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkPanedWindow(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "PanedWindow"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  QSplitter* qpane_ { nullptr };
};

//---

class CTkRadioButtonVarProc;

class CTkRadioButton : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkRadioButton(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "RadioButton"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setText(const std::string &text);

  void setValue(const std::string &value);

  void setChecked(bool b);

  void updateVariable();

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

class CTkScale : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkScale(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

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

class CTkScrollBar : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkScrollBar(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

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

class CTkSpinBoxVarProc;

class CTkSpinBox : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkSpinBox(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "SpinBox"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

  void setValue(const std::string &);

  void updateVariable();

 private:
  CQSpinList*        qspin_ { nullptr };
  std::string        varName_;
  CTkSpinBoxVarProc* varProc_;
};

//---

class CTkText : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkText(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

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

class CTkTopLevel : public CTkAppWidget {
  Q_OBJECT

 public:
  CTkTopLevel(CTkApp *tk, CTkAppWidget *parent=nullptr, const std::string &name="");

  const char *getClassName() const override { return "TopLevel"; }

  bool execConfig(const std::string &name, const std::string &value) override;

  bool execOp(const Args &args) override;

 private:
  QFrame* qframe_ { nullptr };
};

#endif

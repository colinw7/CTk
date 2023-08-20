#ifndef CTkAppPackLayout_H
#define CTkAppPackLayout_H

#include <CTkAppLayout.h>
#include <CTkAppLayoutWidget.h>

class CTkAppWidget;

class QPainter;

//---

class CTkAppPackLayoutInfo {
 public:
  enum class Side { LEFT, RIGHT, BOTTOM, TOP, NONE };
  enum class Fill { X, Y, NONE, BOTH };

 public:
  explicit CTkAppPackLayoutInfo() { }

  explicit CTkAppPackLayoutInfo(Side side, Fill fill, bool expand,
                                int padx, int pady, int ipadx, int ipady) :
   side_(side), fill_(fill), expand_(expand),
   padx_(padx), pady_(pady), ipadx_(ipadx), ipady_(ipady) {
  }

  const Side &side() const { return side_; }
  void setSide(const Side &v) { side_ = v; }

  const Fill &fill() const { return fill_; }
  void setFill(const Fill &v) { fill_ = v; }

  bool isExpand() const { return expand_; }
  void setExpand(bool b) { expand_ = b; }

  int padX() const { return padx_; }
  void setPadX(int i) { padx_ = i; }

  int padY() const { return pady_; }
  void setPadY(int i) { pady_ = i; }

  int ipadX() const { return ipadx_; }
  void setIPadX(int i) { ipadx_ = i; }

  int ipadY() const { return ipady_; }
  void setIPadY(int i) { ipady_ = i; }

  const char *getSideStr() const {
    switch (side_) {
      case Side::LEFT  : return "left";
      case Side::RIGHT : return "right";
      case Side::BOTTOM: return "bottom";
      case Side::TOP   : return "top";
      default          : return "none";
    }
  }

  const char *getFillStr() const {
    switch (fill_) {
      case Fill::X   : return "x";
      case Fill::Y   : return "y";
      case Fill::BOTH: return "both";
      default        : return "none";
    }
  }

 private:
  Side side_   { Side::TOP };
  Fill fill_   { Fill::NONE };
  bool expand_ { false };
  int  padx_   { 0 };
  int  pady_   { 0 };
  int  ipadx_  { 0 };
  int  ipady_  { 0 };
};

//---

class CTkAppPackLayoutWidget : public CTkAppLayoutWidget {
 public:
  using Info = CTkAppPackLayoutInfo;

  explicit CTkAppPackLayoutWidget(TkWidget *widget, const Info &info) :
   CTkAppLayoutWidget(widget), info_(info) {
  }

  explicit CTkAppPackLayoutWidget(QLayout *layout, const Info &info) :
   CTkAppLayoutWidget(layout), info_(info) {
  }

  const Info &info() const { return info_; }
  Info &info() { return info_; }

 private:
  Info info_;
};

//---

class CTkAppPackLayout : public CTkAppLayout {
  Q_OBJECT

  Q_PROPERTY(bool propagate READ isPropagate WRITE setPropagate)

 public:
  using Info = CTkAppPackLayoutInfo;

 public:
  explicit CTkAppPackLayout(QWidget *parent, int margin=0, int spacing=0);
  explicit CTkAppPackLayout(int spacing = -1);

 ~CTkAppPackLayout();

  QString name() const override { return "pack"; }

  bool isPropagate() const { return propagate_; }
  void setPropagate(bool b) { propagate_ = b; }

  void addItem(QLayoutItem *item) override;

  void addWidgets(const std::vector<CTkAppWidget *> &widgets, const Info &info);
  void addWidget(CTkAppWidget *widget, const Info &info);

  void removeWidget(CTkAppWidget *widget);

  std::vector<CTkAppLayoutWidget *> getLayoutWidgets() const override;

  std::vector<CTkAppWidget *> getWidgets() const;

  QLayoutItem *getItem(CTkAppWidget *widget) const;

  bool getChildInfo(CTkAppWidget *widget, Info &info);

  //---

  Qt::Orientations expandingDirections() const override;

  bool hasHeightForWidth() const override;

  int count() const override;

  QLayoutItem *itemAt(int index) const override;

  QSize minimumSize() const override;

  void setGeometry(const QRect &rect) override;

  QSize widgetSizeHint(CTkAppPackLayoutWidget *widget) const;

  QSize sizeHint() const override;

  QLayoutItem *takeAt(int index) override;

  void add(QLayoutItem *item);

  void invalidate() override;

 private:
  void setItemGeometry(QLayoutItem *item, const QRect &r);

 private:
  QSize calculateSize(SizeType sizeType) const;

  QList<QLayoutItem *> list_;
  bool                 propagate_ { true };
};

#endif

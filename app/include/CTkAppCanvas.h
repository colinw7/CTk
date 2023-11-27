#ifndef CTkAppCanvas_H
#define CTkAppCanvas_H

#include <CTkAppWidget.h>

class CTkAppCanvasWidget;
class CTkAppCanvasShape;

class CTkAppCanvas : public CTkAppWidget {
  Q_OBJECT

 public:
  using Shape   = CTkAppCanvasShape;
  using OptReal = std::optional<double>;

 public:
  explicit CTkAppCanvas(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Canvas"; }

  CTkAppCanvasWidget *canvasWidget() const { return qcanvas_; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  //---

  double getWidth() const override { return width_; }
  void setWidth(double r) override { width_ = r; }

  double getHeight() const override { return height_; }
  void setHeight(double r) override { height_ = r; }

  //---

  bool triggerMousePressEvents(QEvent *e, int button) override;
  bool triggerMouseMoveEvents (QEvent *e, int button, bool pressed) override;

  void processShapeEvents(Shape *shape, QEvent *w, const CTkAppEventData &matchEventData);

  //---

  QString newGradientName() const {
    return QString("gradient%1").arg(gradients_.size() + 1);
  }

  void addGradient(const QString &name, QGradient *g) {
    gradients_[name] = g;
  }

  bool getGradient(const QString &name, QGradient* &g) {
    auto pg = gradients_.find(name);
    if (pg == gradients_.end()) return false;

    g = (*pg).second;

    return true;
  }

 private:
  CTkAppCanvasWidget* qcanvas_ { nullptr };

  using IdEventDatas = std::map<QString, EventDatas>;
  using Gradients    = std::map<QString, QGradient *>;

  IdEventDatas idEventDatas_;
  double       width_  { 400 };
  double       height_ { 400 };
  Gradients    gradients_;
};

#endif

#include <CTkAppDebug.h>
#include <CTkApp.h>
#include <CTkAppWidget.h>
#include <CTkAppLayout.h>
#include <CTkAppPackLayout.h>
#include <CTkAppGridLayout.h>
#include <CTkAppPlaceLayout.h>

#include <CQIntegerSpin.h>
#include <CQRealSpin.h>
#include <CQOptEdit.h>

#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

CTkAppDebug::
CTkAppDebug(CTkApp *tk) :
 QFrame(nullptr), tk_(tk)
{
  setObjectName("appDebug");

  auto *layout = new QHBoxLayout(this);

  //---

  auto *widgetFrame = new QFrame;
  auto *widgetLayout = new QVBoxLayout(widgetFrame);

  layout->addWidget(widgetFrame);

  //---

  childList_ = new QListWidget;

  currentWidget_ = tk_->root();
  currentLayout_ = (currentWidget_ ? currentWidget_->getTkLayout() : nullptr);
  currentChild_  = nullptr;

  layoutWidget_ = nullptr;

  connect(childList_, SIGNAL(itemSelectionChanged()), this, SLOT(selectionSlot()));

  widgetLayout->addWidget(childList_);

  auto *buttonFrame  = new QFrame;
  auto *buttonLayout = new QHBoxLayout(buttonFrame);

  widgetLayout->addWidget(buttonFrame);

  auto *parentButton  = new QPushButton("Parent");
  auto *currentButton = new QPushButton("Set Current");

  connect(parentButton, SIGNAL(clicked()), this, SLOT(parentSlot()));
  connect(currentButton, SIGNAL(clicked()), this, SLOT(currentSlot()));

  buttonLayout->addWidget(parentButton);
  buttonLayout->addWidget(currentButton);
  buttonLayout->addStretch(1);

  //---

  auto *dataFrame = new QFrame;

  dataLayout_ = new QVBoxLayout(dataFrame);

  layout->addWidget(dataFrame);

  typeLabel_   = new QLabel;
  layoutLabel_ = new QLabel;

  dataLayout_->addWidget(typeLabel_);
  dataLayout_->addWidget(layoutLabel_);

  //---

  updateChildren();
}

void
CTkAppDebug::
updateChildren()
{
  childList_->clear();

  if (currentWidget_) {
    CTkAppWidget::Children children;

    currentWidget_->getChildren(children);

    for (auto *child : children)
      childList_->addItem(child->getName());
  }

  layoutLabel_->setText(currentLayout_ ? currentLayout_->name() : "");

  createLayoutWidgets();
}

void
CTkAppDebug::
createLayoutWidgets()
{
  packLayout_  = dynamic_cast<CTkAppPackLayout  *>(currentLayout_);
  gridLayout_  = dynamic_cast<CTkAppGridLayout  *>(currentLayout_);
  placeLayout_ = dynamic_cast<CTkAppPlaceLayout *>(currentLayout_);

  delete layoutFrame_;

  //---

  layoutFrame_  = new QFrame;
  layoutLayout_ = new QVBoxLayout(layoutFrame_);

  layoutWidgets_ = LayoutWidgets();

  dataLayout_->addWidget(layoutFrame_);

  //---

  auto addBoolEdit = [&](const QString &name, const QString &id) {
    auto *frame  = new QFrame;
    auto *layout = new QHBoxLayout(frame);

    layout->addWidget(new QLabel(name));

    auto *edit = new QCheckBox;

    edit->setProperty("id", id);

    layout->addWidget(edit);

    layoutLayout_->addWidget(frame);

    connect(edit, SIGNAL(stateChanged(int)), this, SLOT(boolChanged(int)));

    return edit;
  };

  auto addIntEdit = [&](const QString &name, const QString &id) {
    auto *frame  = new QFrame;
    auto *layout = new QHBoxLayout(frame);

    layout->addWidget(new QLabel(name));

    auto *edit = new CQIntegerSpin;

    edit->setProperty("id", id);

    layout->addWidget(edit);

    layoutLayout_->addWidget(frame);

    connect(edit, SIGNAL(valueChanged(int)), this, SLOT(intChanged(int)));

    return edit;
  };

  auto addOptIntEdit = [&](const QString &name, const QString &id, CQOptEdit* &optEdit) {
    auto *frame  = new QFrame;
    auto *layout = new QHBoxLayout(frame);

    layout->addWidget(new QLabel(name));

    optEdit = new CQOptEdit;

    auto *edit = new CQIntegerSpin;

    edit->setProperty("id", id);

    optEdit->setWidget(edit);

    layout->addWidget(optEdit);

    layoutLayout_->addWidget(frame);

    connect(optEdit, SIGNAL(enabledChanged(bool)), this, SLOT(optIntChanged(bool)));
    connect(edit   , SIGNAL(valueChanged(int)), this, SLOT(intChanged(int)));

    return edit;
  };

#if 0
  auto addRealEdit = [&](const QString &name, const QString &id) {
    auto *frame  = new QFrame;
    auto *layout = new QHBoxLayout(frame);

    layout->addWidget(new QLabel(name));

    auto *edit = new CQRealSpin;

    edit->setProperty("id", id);

    layout->addWidget(edit);

    layoutLayout_->addWidget(frame);

    connect(edit, SIGNAL(valueChanged(double)), this, SLOT(realChanged(double)));

    return edit;
  };
#endif

  auto addOptRealEdit = [&](const QString &name, const QString &id, CQOptEdit* &optEdit) {
    auto *frame  = new QFrame;
    auto *layout = new QHBoxLayout(frame);

    layout->addWidget(new QLabel(name));

    optEdit = new CQOptEdit;

    auto *edit = new CQRealSpin;

    edit->setProperty("id", id);

    optEdit->setWidget(edit);

    layout->addWidget(optEdit);

    layoutLayout_->addWidget(frame);

    connect(optEdit, SIGNAL(enabledChanged(bool)), this, SLOT(optRealChanged(bool)));
    connect(edit   , SIGNAL(valueChanged(double)), this, SLOT(realChanged(double)));

    return edit;
  };

  auto addStringEdit = [&](const QString &name, const QString &id) {
    auto *frame  = new QFrame;
    auto *layout = new QHBoxLayout(frame);

    layout->addWidget(new QLabel(name));

    auto *edit = new QLineEdit;

    edit->setProperty("id", id);

    layout->addWidget(edit);

    layoutLayout_->addWidget(frame);

    connect(edit, SIGNAL(editingFinished()), this, SLOT(stringChanged()));

    return edit;
  };

  auto addComboEdit = [&](const QString &name, const QString &id, const QStringList &items) {
    auto *frame  = new QFrame;
    auto *layout = new QHBoxLayout(frame);

    layout->addWidget(new QLabel(name));

    auto *edit = new QComboBox;

    for (const auto &item : items)
      edit->addItem(item);

    edit->setProperty("id", id);

    layout->addWidget(edit);

    layoutLayout_->addWidget(frame);

    connect(edit, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged(int)));

    return edit;
  };

  if      (packLayout_) {
    layoutWidgets_.sideCombo = addComboEdit("Side", "side",
      QStringList() << "left" << "right" << "bottom" << "top" << "none");
    layoutWidgets_.fillCombo = addComboEdit("Fill", "fill",
      QStringList() << "x" << "y" << "none" << "both");

    layoutWidgets_.expandCheck = addBoolEdit("Expand", "expand");

    layoutWidgets_.padXSpin  = addIntEdit("Pad X" , "padx");
    layoutWidgets_.padYSpin  = addIntEdit("Pad Y" , "pady");
    layoutWidgets_.ipadXSpin = addIntEdit("IPad X", "ipadx");
    layoutWidgets_.ipadYSpin = addIntEdit("IPad Y", "ipady");
  }
  else if (gridLayout_) {
    layoutWidgets_.rowSpin     = addOptIntEdit("Row"     , "row", layoutWidgets_.rowOptEdit);
    layoutWidgets_.colSpin     = addOptIntEdit("Col"     , "col", layoutWidgets_.colOptEdit);
    layoutWidgets_.rowSpanSpin =
      addOptIntEdit("Row Span", "rowspan", layoutWidgets_.rowSpanOptEdit);
    layoutWidgets_.colSpanSpin =
      addOptIntEdit("Col Span", "colspan", layoutWidgets_.colSpanOptEdit);

    layoutWidgets_.stickySidesEdit = addStringEdit("Sticky Sides", "stickySides");

    layoutWidgets_.padXSpin  = addIntEdit("Pad X" , "padx");
    layoutWidgets_.padYSpin  = addIntEdit("Pad Y" , "pady");
    layoutWidgets_.ipadXSpin = addIntEdit("IPad X", "ipadx");
    layoutWidgets_.ipadYSpin = addIntEdit("IPad Y", "ipady");

    layoutWidgets_.inEdit = addStringEdit("In", "in");

    layoutWidgets_.weightSpin = addIntEdit("Weight", "weight");
  }
  else if (placeLayout_) {
    // TODO: anchorEdit

    layoutWidgets_.borderModeCombo = addComboEdit("Border Mode", "borderMode",
      QStringList() << "inside" << "outside" << "ignore");

    layoutWidgets_.xSpin = addOptRealEdit("X", "x", layoutWidgets_.xOptEdit);
    layoutWidgets_.ySpin = addOptRealEdit("Y", "y", layoutWidgets_.yOptEdit);

    layoutWidgets_.widthSpin  = addOptRealEdit("Width" , "width" , layoutWidgets_.widthOptEdit );
    layoutWidgets_.heightSpin = addOptRealEdit("Height", "height", layoutWidgets_.heightOptEdit);

    layoutWidgets_.relXSpin = addOptRealEdit("Rel X", "relX", layoutWidgets_.relXOptEdit);
    layoutWidgets_.relYSpin = addOptRealEdit("Rel Y", "relY", layoutWidgets_.relYOptEdit);

    layoutWidgets_.relWidthSpin  =
      addOptRealEdit("Rel Width" , "relWidth" , layoutWidgets_.relWidthOptEdit );
    layoutWidgets_.relHeightSpin =
      addOptRealEdit("Rel Height", "relHeight", layoutWidgets_.relHeightOptEdit);

    layoutWidgets_.inEdit = addStringEdit("In", "in");
  }

  layoutLayout_->addStretch(1);
}

void
CTkAppDebug::
parentSlot()
{
  currentWidget_ = (currentWidget_ ? currentWidget_->getParent() : nullptr);
  currentLayout_ = (currentWidget_ ? currentWidget_->getTkLayout() : nullptr);
  currentChild_  = nullptr;

  updateChildren();
}

void
CTkAppDebug::
currentSlot()
{
  currentWidget_ = currentChild_;
  currentLayout_ = (currentWidget_ ? currentWidget_->getTkLayout() : nullptr);
  currentChild_  = nullptr;

  updateChildren();
}

void
CTkAppDebug::
selectionSlot()
{
  auto items = childList_->selectedItems();

  int row = -1;

  for (auto *item : items)
    row = childList_->row(item);

  CTkAppWidget::Children children;

  currentWidget_->getChildren(children);

  currentChild_ = nullptr;

  int i = 0;

  for (auto *w : children) {
    if (i == row) {
      currentChild_ = w;
      break;
    }

    ++i;
  }

  typeLabel_->setText(currentChild_ ? currentChild_->getClassName() : "");

  updateLayoutWidgets();
}

void
CTkAppDebug::
updateLayoutWidgets()
{
  layoutWidget_ = nullptr;

  if (currentLayout_) {
    for (auto *w : currentLayout_->getLayoutWidgets(/*flat*/true)) {
      if (w->getTkWidget() == currentChild_) {
        layoutWidget_ = w;
        break;
      }
    }
  }

  if (! layoutWidget_)
    return;

  //---

  updating_ = true;

  if      (packLayout_) {
    const auto &info = dynamic_cast<CTkAppPackLayoutWidget *>(layoutWidget_)->info();

    std::cerr << layoutWidget_->name().toStdString() << "\n";

    // left, right, bottom, top, none;
    uint sideInd = 0;
    if      (info.side() == CTkAppPackLayout::Info::Side::LEFT  ) sideInd = 0;
    else if (info.side() == CTkAppPackLayout::Info::Side::RIGHT ) sideInd = 1;
    else if (info.side() == CTkAppPackLayout::Info::Side::BOTTOM) sideInd = 2;
    else if (info.side() == CTkAppPackLayout::Info::Side::TOP   ) sideInd = 3;
    else if (info.side() == CTkAppPackLayout::Info::Side::NONE  ) sideInd = 4;
    else std::cerr << "Unknown info side " << int(info.side()) << "\n";
    layoutWidgets_.sideCombo->setCurrentIndex(sideInd);

    // x, y, none, both
    uint fillInd = 0;
    if      (info.fill() == CTkAppPackLayout::Info::Fill::X   ) fillInd = 0;
    else if (info.fill() == CTkAppPackLayout::Info::Fill::Y   ) fillInd = 1;
    else if (info.fill() == CTkAppPackLayout::Info::Fill::NONE) fillInd = 2;
    else if (info.fill() == CTkAppPackLayout::Info::Fill::BOTH) fillInd = 3;
    else std::cerr << "Unknown info fill " << int(info.fill()) << "\n";
    layoutWidgets_.fillCombo->setCurrentIndex(fillInd);

    layoutWidgets_.expandCheck->setChecked(info.isExpand());
  }
  else if (gridLayout_) {
    const auto &info = dynamic_cast<CTkAppGridLayoutWidget *>(layoutWidget_)->info();

    std::cerr << layoutWidget_->name().toStdString() << "\n";

    layoutWidgets_.rowOptEdit    ->setEnabled(info.isRowValid    ());
    layoutWidgets_.rowSpin       ->setValue(info.getRow    ());
    layoutWidgets_.colOptEdit    ->setEnabled(info.isColValid    ());
    layoutWidgets_.colSpin       ->setValue(info.getCol    ());
    layoutWidgets_.rowSpanOptEdit->setEnabled(info.isRowSpanValid());
    layoutWidgets_.rowSpanSpin   ->setValue(info.getRowSpan());
    layoutWidgets_.colSpanOptEdit->setEnabled(info.isColSpanValid());
    layoutWidgets_.colSpanSpin   ->setValue(info.getColSpan());

    layoutWidgets_.stickySidesEdit->setText(info.getSticky());

    layoutWidgets_.padXSpin ->setValue(info.getPadX());
    layoutWidgets_.padYSpin ->setValue(info.getPadY());
    layoutWidgets_.ipadXSpin->setValue(info.getIPadX());
    layoutWidgets_.ipadYSpin->setValue(info.getIPadY());
  }
  else if (placeLayout_) {
    using Info = CTkAppPlaceLayoutInfo;

    const auto &info = dynamic_cast<CTkAppPlaceLayoutWidget *>(layoutWidget_)->info();

    std::cerr << layoutWidget_->name().toStdString() << "\n";

    // inside, outside, ignore
    uint borderModeInd = 0;
    if      (info.borderMode() == Info::BorderMode::Inside ) borderModeInd = 0;
    else if (info.borderMode() == Info::BorderMode::Outside) borderModeInd = 1;
    else if (info.borderMode() == Info::BorderMode::Ignore ) borderModeInd = 2;
    layoutWidgets_.borderModeCombo->setCurrentIndex(borderModeInd);

    layoutWidgets_.xOptEdit     ->setEnabled(info.isXValid     ());
    layoutWidgets_.xSpin        ->setValue(info.getX     ());
    layoutWidgets_.yOptEdit     ->setEnabled(info.isYValid     ());
    layoutWidgets_.ySpin        ->setValue(info.getY     ());
    layoutWidgets_.widthOptEdit ->setEnabled(info.isWidthValid ());
    layoutWidgets_.widthSpin    ->setValue(info.getWidth ());
    layoutWidgets_.heightOptEdit->setEnabled(info.isHeightValid());
    layoutWidgets_.heightSpin   ->setValue(info.getHeight());

    layoutWidgets_.relXOptEdit     ->setEnabled(info.isRelXValid());
    layoutWidgets_.relXSpin        ->setValue  (info.getRelX());
    layoutWidgets_.relYOptEdit     ->setEnabled(info.isRelYValid());
    layoutWidgets_.relYSpin        ->setValue  (info.getRelY());
    layoutWidgets_.relWidthOptEdit ->setEnabled(info.isRelWidthValid());
    layoutWidgets_.relWidthSpin    ->setValue  (info.getRelWidth());
    layoutWidgets_.relHeightOptEdit->setEnabled(info.isRelHeightValid());
    layoutWidgets_.relHeightSpin   ->setValue  (info.getRelHeight());
  }

  updating_ = false;
}

void
CTkAppDebug::
boolChanged(int)
{
  if (updating_) return;

  if (! layoutWidget_)
    return;

  auto *check = qobject_cast<QCheckBox *>(sender());
  if (! check) return;

  auto id = check->property("id").toString();

  if (packLayout_) {
    auto &info = dynamic_cast<CTkAppPackLayoutWidget *>(layoutWidget_)->info();

    if (id == "expand") info.setExpand(check->isChecked());
    else std::cerr << "Invalid id: " << id.toStdString() << "\n";

    packLayout_->invalidate();
  }
}

void
CTkAppDebug::
optIntChanged(bool)
{
  if (updating_) return;

  if (! layoutWidget_)
    return;

  auto *optEdit = qobject_cast<CQOptEdit *>(sender());
  if (! optEdit) return;

  auto enabled = optEdit->isEnabled();

  auto *ispin = qobject_cast<CQIntegerSpin *>(optEdit->widget());
  if (! ispin) return;

  auto i = ispin->value();

  auto id = ispin->property("id").toString();

  if      (packLayout_) {
  }
  else if (gridLayout_) {
    auto &info = dynamic_cast<CTkAppGridLayoutWidget *>(layoutWidget_)->info();

    if      (id == "row"    ) { (enabled ? info.setRow    (i) : info.unsetRow    ()); }
    else if (id == "col"    ) { (enabled ? info.setCol    (i) : info.unsetCol    ()); }
    else if (id == "rowspan") { (enabled ? info.setRowSpan(i) : info.unsetRowSpan()); }
    else if (id == "colspan") { (enabled ? info.setColSpan(i) : info.unsetColSpan()); }
    else std::cerr << "Invalid id: " << id.toStdString() << "\n";

    gridLayout_->invalidate();
  }
  else if (placeLayout_) {
  }
}

void
CTkAppDebug::
intChanged(int i)
{
  if (updating_) return;

  if (! layoutWidget_)
    return;

  auto *ispin = qobject_cast<CQIntegerSpin *>(sender());
  if (! ispin) return;

  if (! ispin->isEnabled())
    return;

  auto id = ispin->property("id").toString();

  if      (packLayout_) {
    auto &info = dynamic_cast<CTkAppPackLayoutWidget *>(layoutWidget_)->info();

    if      (id == "padx" ) info.setPadX(i);
    else if (id == "pady" ) info.setPadY(i);
    else if (id == "ipadx") info.setIPadX(i);
    else if (id == "ipady") info.setIPadY(i);
    else std::cerr << "Invalid id: " << id.toStdString() << "\n";

    packLayout_->invalidate();
  }
  else if (gridLayout_) {
    auto &info = dynamic_cast<CTkAppGridLayoutWidget *>(layoutWidget_)->info();

    if      (id == "row"    ) { info.setRow(i); }
    else if (id == "col"    ) { info.setCol(i); }
    else if (id == "rowspan") { info.setRowSpan(i); }
    else if (id == "colspan") { info.setColSpan(i); }
    else if (id == "padx"   ) info.setPadX(i);
    else if (id == "pady"   ) info.setPadY(i);
    else if (id == "ipadx"  ) info.setIPadX(i);
    else if (id == "ipady"  ) info.setIPadY(i);
    else std::cerr << "Invalid id: " << id.toStdString() << "\n";

    gridLayout_->invalidate();
  }
}

void
CTkAppDebug::
optRealChanged(bool)
{
  if (updating_) return;

  if (! layoutWidget_)
    return;

  auto *optEdit = qobject_cast<CQOptEdit *>(sender());
  if (! optEdit) return;

  auto enabled = optEdit->isEnabled();

  auto *rspin = qobject_cast<CQRealSpin *>(optEdit->widget());
  if (! rspin) return;

  auto r = rspin->value();

  auto id = rspin->property("id").toString();

  if      (packLayout_) {
  }
  else if (gridLayout_) {
  }
  else if (placeLayout_) {
    auto &info = dynamic_cast<CTkAppPlaceLayoutWidget *>(layoutWidget_)->info();

    if      (id == "x"        ) { (enabled ? info.setX        (r) : info.unsetX        ()); }
    else if (id == "y"        ) { (enabled ? info.setY        (r) : info.unsetY        ()); }
    else if (id == "width"    ) { (enabled ? info.setWidth    (r) : info.unsetWidth    ()); }
    else if (id == "height"   ) { (enabled ? info.setHeight   (r) : info.unsetHeight   ()); }
    else if (id == "relX"     ) { (enabled ? info.setRelX     (r) : info.unsetRelX     ()); }
    else if (id == "relY"     ) { (enabled ? info.setRelY     (r) : info.unsetRelY     ()); }
    else if (id == "relWidth" ) { (enabled ? info.setRelWidth (r) : info.unsetRelWidth ()); }
    else if (id == "relHeight") { (enabled ? info.setRelHeight(r) : info.unsetRelHeight()); }
    else std::cerr << "Invalid id: " << id.toStdString() << "\n";

    placeLayout_->invalidate();
  }
}

void
CTkAppDebug::
realChanged(double r)
{
  if (updating_) return;

  if (! layoutWidget_)
    return;

  auto *rspin = qobject_cast<CQRealSpin *>(sender());
  if (! rspin) return;

  if (! rspin->isEnabled())
    return;

  auto id = rspin->property("id").toString();

  if      (packLayout_) {
  }
  else if (gridLayout_) {
  }
  else if (placeLayout_) {
    auto &info = dynamic_cast<CTkAppPlaceLayoutWidget *>(layoutWidget_)->info();

    if      (id == "x"        ) info.setX(r);
    else if (id == "y"        ) info.setY(r);
    else if (id == "width"    ) info.setWidth(r);
    else if (id == "height"   ) info.setHeight(r);
    else if (id == "relX"     ) info.setRelX(r);
    else if (id == "relY"     ) info.setRelY(r);
    else if (id == "relWidth" ) info.setRelWidth(r);
    else if (id == "relHeight") info.setRelHeight(r);
    else std::cerr << "Invalid id: " << id.toStdString() << "\n";

    placeLayout_->invalidate();
  }
}

void
CTkAppDebug::
stringChanged()
{
  if (updating_) return;

  if (! layoutWidget_)
    return;

  auto *edit = qobject_cast<QLineEdit *>(sender());
  if (! edit) return;

  auto str = edit->text();

  auto id = edit->property("id").toString();

  if      (packLayout_) {
  }
  else if (gridLayout_) {
    auto &info = dynamic_cast<CTkAppGridLayoutWidget *>(layoutWidget_)->info();

    if      (id == "stickySides") info.setSticky(str);
    else if (id == "in"         ) info.setIn(str);
    else std::cerr << "Invalid id: " << id.toStdString() << "\n";

    gridLayout_->invalidate();
  }
}

void
CTkAppDebug::
comboChanged(int)
{
  if (updating_) return;

  if (! layoutWidget_)
    return;

  auto *combo = qobject_cast<QComboBox *>(sender());
  if (! combo) return;

  auto id  = combo->property("id").toString();
  auto ind = combo->currentIndex();

  if (packLayout_) {
    auto &info = dynamic_cast<CTkAppPackLayoutWidget *>(layoutWidget_)->info();

    if      (id == "side") {
      if      (ind == 0) info.setSide(CTkAppPackLayout::Info::Side::LEFT);
      else if (ind == 1) info.setSide(CTkAppPackLayout::Info::Side::RIGHT);
      else if (ind == 2) info.setSide(CTkAppPackLayout::Info::Side::BOTTOM);
      else if (ind == 3) info.setSide(CTkAppPackLayout::Info::Side::TOP);
      else if (ind == 4) info.setSide(CTkAppPackLayout::Info::Side::NONE);
      else std::cerr << "Invalid side index: " << ind << "\n";
    }
    else if (id == "fill") {
      if      (ind == 0) info.setFill(CTkAppPackLayout::Info::Fill::X);
      else if (ind == 1) info.setFill(CTkAppPackLayout::Info::Fill::Y);
      else if (ind == 2) info.setFill(CTkAppPackLayout::Info::Fill::NONE);
      else if (ind == 3) info.setFill(CTkAppPackLayout::Info::Fill::BOTH);
      else std::cerr << "Invalid fill index: " << ind << "\n";
    }
    else std::cerr << "Invalid id: " << id.toStdString() << "\n";

    packLayout_->invalidate();
  }
}

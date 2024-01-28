#include <CTkAppDebug.h>
#include <CTkApp.h>

#include <CTkAppWidget.h>
#include <CTkAppRoot.h>
#include <CTkAppButton.h>
#include <CTkAppCanvas.h>
#include <CTkAppCanvasWidget.h>
#include <CTkAppCanvasShape.h>
#include <CTkAppCheckButton.h>
#include <CTkAppComboBox.h>
#include <CTkAppEntry.h>
#include <CTkAppLabel.h>
#include <CTkAppListBox.h>
#include <CTkAppMenuButton.h>
#include <CTkAppMessage.h>
#include <CTkAppRadioButton.h>
#include <CTkAppScale.h>
#include <CTkAppSpinBox.h>
#include <CTkAppTopLevel.h>

#include <CTkAppLayout.h>
#include <CTkAppPackLayout.h>
#include <CTkAppGridLayout.h>
#include <CTkAppPlaceLayout.h>

#include <CTkAppImage.h>
#include <CTkAppFont.h>
#include <CTkAppOptData.h>

#include <CQPropertyViewTree.h>
#include <CQIntegerSpin.h>
#include <CQRealSpin.h>
#include <CQOptEdit.h>

#include <QGroupBox>
#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QListWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QRubberBand>
#include <QPainter>
#include <QApplication>

CTkAppDebug::
CTkAppDebug(CTkApp *tk) :
 QFrame(nullptr), tk_(tk)
{
  setObjectName("appDebug");

  //---

  currentWidget_ = tk_->root();
  currentLayout_ = (currentWidget_ ? currentWidget_->getTkLayout() : nullptr);
  currentChild_  = nullptr;
  layoutWidget_  = nullptr;

  //---

  auto *layout = new QHBoxLayout(this);

  //---

  tab_ = new QTabWidget;
  tab_->setObjectName("tab");

  layout->addWidget(tab_);

  //---

  auto *widgetLayoutFrame = new QFrame;
  widgetLayoutFrame->setObjectName("widgetLayoutFrame");

  auto *widgetLayoutLayout = new QHBoxLayout(widgetLayoutFrame);

  tab_->addTab(widgetLayoutFrame, "Widget/Layout");

  //---

  auto *widgetGroup = new QGroupBox("Widget");
  widgetGroup->setObjectName("widgetGroup");

  auto *widgetLayout = new QVBoxLayout(widgetGroup);

  widgetLayoutLayout->addWidget(widgetGroup);

  //--

  currentLabel_ = new QLabel;
  currentLabel_->setObjectName("currentLabel");

  widgetLayout->addWidget(currentLabel_);

  childList_ = new QListWidget;
  childList_->setObjectName("childList");

  connect(childList_, SIGNAL(itemSelectionChanged()), this, SLOT(childSelectionSlot()));

  widgetLayout->addWidget(childList_);

  auto *buttonFrame = new QFrame;
  buttonFrame->setObjectName("buttonFrame");

  auto *buttonLayout = new QHBoxLayout(buttonFrame);

  widgetLayout->addWidget(buttonFrame);

  auto *parentButton = new QPushButton("Parent");
  parentButton->setObjectName("parentButton");
  auto *currentButton = new QPushButton("Set Current");
  currentButton->setObjectName("currentButton");

  connect(parentButton, SIGNAL(clicked()), this, SLOT(parentSlot()));
  connect(currentButton, SIGNAL(clicked()), this, SLOT(currentSlot()));

  buttonLayout->addWidget(parentButton);
  buttonLayout->addWidget(currentButton);
  buttonLayout->addStretch(1);

  //---

  auto *layoutDataGroup = new QGroupBox("Layout");
  layoutDataGroup->setObjectName("layoutDataGroup");

  layoutDataLayout_ = new QVBoxLayout(layoutDataGroup);

  widgetLayoutLayout->addWidget(layoutDataGroup);

  //---

  layoutWidgetLabel_ = new QLabel;
  layoutWidgetLabel_->setObjectName("layoutWidgetLabel");

  layoutDataLayout_->addWidget(layoutWidgetLabel_);

  //---

  optionsTable_ = new QTableWidget;

  tab_->addTab(optionsTable_, "Options");

  auto *delegate = new CTkAppOptionTableDelegate(this);

  optionsTable_->setItemDelegate(delegate);

  //---

  propertyView_ = new CQPropertyViewTree;

  tab_->addTab(propertyView_, "Properties");

  //---

  auto *imagesFrame = new QFrame;
  imagesFrame->setObjectName("imagesFrame");
  auto *imagesLayout = new QVBoxLayout(imagesFrame);

  tab_->addTab(imagesFrame, "Images");

  //---

  auto *imageGroup = new QGroupBox("Images");
  imageGroup->setObjectName("imageGroup");

  auto *imageLayout = new QHBoxLayout(imageGroup);

  imagesLayout->addWidget(imageGroup);

  imageList_ = new QListWidget;
  imageList_->setObjectName("imageList");

  imageLabel_ = new QLabel;
  imageLabel_->setObjectName("imageLabel");

  connect(imageList_, SIGNAL(itemSelectionChanged()), this, SLOT(imageSelectionSlot()));

  imageLayout->addWidget(imageList_);
  imageLayout->addWidget(imageLabel_);

  //---

  auto *bitmapGroup  = new QGroupBox("Bitmaps");
  bitmapGroup->setObjectName("bitmapGroup");

  auto *bitmapLayout = new QHBoxLayout(bitmapGroup);

  imagesLayout->addWidget(bitmapGroup);

  bitmapList_ = new QListWidget;
  bitmapList_->setObjectName("bitmapList");

  bitmapLabel_ = new QLabel;
  bitmapLabel_->setObjectName("bitmapLabel");

  connect(bitmapList_, SIGNAL(itemSelectionChanged()), this, SLOT(bitmapSelectionSlot()));

  bitmapLayout->addWidget(bitmapList_);
  bitmapLayout->addWidget(bitmapLabel_);

  imageProperties_ = new CQPropertyViewTree;

  imagesLayout->addWidget(imageProperties_);

  //---

  auto *fontsFrame = new QFrame;
  fontsFrame->setObjectName("fontsFrame");

  auto *fontsLayout = new QVBoxLayout(fontsFrame);

  tab_->addTab(fontsFrame, "Fonts");

  auto *fontGroup = new QGroupBox("Fonts");
  fontGroup->setObjectName("fontGroup");

  auto *fontLayout = new QHBoxLayout(fontGroup);

  fontsLayout->addWidget(fontGroup);

  fontList_ = new QListWidget;
  fontList_->setObjectName("fontList");

  fontLabel_ = new QLabel;
  fontLabel_->setObjectName("fontLabel");

  connect(fontList_, SIGNAL(itemSelectionChanged()), this, SLOT(fontSelectionSlot()));

  fontLayout->addWidget(fontList_);
  fontLayout->addWidget(fontLabel_);

  fontProperties_ = new CQPropertyViewTree;

  fontsLayout->addWidget(fontProperties_);

  //---

  updateChildren();

  updateImages();
  updateFonts();

  updateProperties();
}

void
CTkAppDebug::
updateChildren()
{
  QString currentName;

  if (currentWidget_)
    currentName = QString("%1 (%2)").
      arg(currentWidget_->getFullName()).arg(currentWidget_->getType());

  currentLabel_->setText(currentName);

  childList_->clear();

  if (currentWidget_) {
    CTkAppWidget::Children children;

    currentWidget_->getChildren(children);

    for (auto *child : children)
      childList_->addItem(child->getName());
  }

  createLayoutWidgets();

  layoutGroup_->setTitle(currentLayout_ ? currentLayout_->name() : "");

  if (rubberBand_)
    rubberBand_->hide();
}

void
CTkAppDebug::
updateImages()
{
  imageList_->clear();

  std::vector<QString> imageNames;

  tk_->getImageNames(imageNames);

  for (const auto &imageName : imageNames)
    imageList_->addItem(imageName);

  std::vector<QString> bitmapNames;

  tk_->getBitmapNames(bitmapNames);

  for (const auto &bitmapName : bitmapNames)
    bitmapList_->addItem(bitmapName);
}

void
CTkAppDebug::
updateFonts()
{
  fontList_->clear();

  std::vector<QString> fontNames;

  tk_->getFontNames(fontNames);

  for (const auto &fontName : fontNames)
    fontList_->addItem(fontName);
}

void
CTkAppDebug::
createLayoutWidgets()
{
  packLayout_  = dynamic_cast<CTkAppPackLayout  *>(currentLayout_);
  gridLayout_  = dynamic_cast<CTkAppGridLayout  *>(currentLayout_);
  placeLayout_ = dynamic_cast<CTkAppPlaceLayout *>(currentLayout_);

  //---

  delete layoutGroup_;
  delete noLayoutLabel_;

  //---

  layoutGroup_ = new QGroupBox("Layout");
  layoutGroup_->setObjectName("layoutGroup");

  layoutLayout_ = new QVBoxLayout(layoutGroup_);

  layoutWidgets_ = LayoutWidgets();

  layoutDataLayout_->addWidget(layoutGroup_);

  //---

  noLayoutLabel_ = new QLabel;
  noLayoutLabel_->setObjectName("noLayoutLabel");

  noLayoutLabel_->setText("No Layout");

  layoutDataLayout_->addWidget(noLayoutLabel_);

  //---

  auto addBoolEdit = [&](const QString &name, const QString &id) {
    auto *frame = new QFrame;
    frame->setObjectName("frame");

    auto *layout = new QHBoxLayout(frame);

    auto *label = new QLabel(name);
    label->setObjectName("label");

    layout->addWidget(label);

    auto *edit = new QCheckBox;
    edit->setObjectName("edit");

    edit->setProperty("id", id);

    layout->addWidget(edit);

    layoutLayout_->addWidget(frame);

    connect(edit, SIGNAL(stateChanged(int)), this, SLOT(boolChanged(int)));

    return edit;
  };

  auto addIntEdit = [&](const QString &name, const QString &id) {
    auto *frame = new QFrame;
    frame->setObjectName("frame");

    auto *layout = new QHBoxLayout(frame);

    auto *label = new QLabel(name);
    label->setObjectName("label");

    layout->addWidget(label);

    auto *edit = new CQIntegerSpin;
    edit->setObjectName("edit");

    edit->setProperty("id", id);

    layout->addWidget(edit);

    layoutLayout_->addWidget(frame);

    connect(edit, SIGNAL(valueChanged(int)), this, SLOT(intChanged(int)));

    return edit;
  };

  auto addOptIntEdit = [&](const QString &name, const QString &id, CQOptEdit* &optEdit) {
    auto *frame = new QFrame;
    frame->setObjectName("frame");

    auto *layout = new QHBoxLayout(frame);

    auto *label = new QLabel(name);
    label->setObjectName("label");

    layout->addWidget(label);

    optEdit = new CQOptEdit;

    auto *edit = new CQIntegerSpin;
    edit->setObjectName("edit");

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
    auto *frame = new QFrame;
    frame->setObjectName("frame");

    auto *layout = new QHBoxLayout(frame);

    auto *label = new QLabel(name);
    label->setObjectName("label");

    layout->addWidget(label);

    auto *edit = new CQRealSpin;
    edit->setObjectName("edit");

    edit->setProperty("id", id);

    layout->addWidget(edit);

    layoutLayout_->addWidget(frame);

    connect(edit, SIGNAL(valueChanged(double)), this, SLOT(realChanged(double)));

    return edit;
  };
#endif

  auto addOptRealEdit = [&](const QString &name, const QString &id, CQOptEdit* &optEdit) {
    auto *frame = new QFrame;
    frame->setObjectName("frame");

    auto *layout = new QHBoxLayout(frame);

    auto *label = new QLabel(name);
    label->setObjectName("label");

    layout->addWidget(label);

    optEdit = new CQOptEdit;

    auto *edit = new CQRealSpin;
    edit->setObjectName("edit");

    edit->setProperty("id", id);

    optEdit->setWidget(edit);

    layout->addWidget(optEdit);

    layoutLayout_->addWidget(frame);

    connect(optEdit, SIGNAL(enabledChanged(bool)), this, SLOT(optRealChanged(bool)));
    connect(edit   , SIGNAL(valueChanged(double)), this, SLOT(realChanged(double)));

    return edit;
  };

  auto addStringEdit = [&](const QString &name, const QString &id) {
    auto *frame = new QFrame;
    frame->setObjectName("frame");

    auto *layout = new QHBoxLayout(frame);

    auto *label = new QLabel(name);
    label->setObjectName("label");

    layout->addWidget(label);

    auto *edit = new QLineEdit;
    edit->setObjectName("edit");

    edit->setProperty("id", id);

    layout->addWidget(edit);

    layoutLayout_->addWidget(frame);

    connect(edit, SIGNAL(editingFinished()), this, SLOT(stringChanged()));

    return edit;
  };

  auto addComboEdit = [&](const QString &name, const QString &id, const QStringList &items) {
    auto *frame = new QFrame;
    frame->setObjectName("frame");

    auto *layout = new QHBoxLayout(frame);

    auto *label = new QLabel(name);
    label->setObjectName("label");

    layout->addWidget(label);

    auto *edit = new QComboBox;
    edit->setObjectName("edit");

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
    layoutWidgets_.rowSpin = addOptIntEdit("Row", "row", layoutWidgets_.rowOptEdit);
    layoutWidgets_.colSpin = addOptIntEdit("Col", "col", layoutWidgets_.colOptEdit);

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
  auto *parentWidget = (currentWidget_ ? currentWidget_->getParent() : nullptr);
  if (! parentWidget) parentWidget = tk_->root();

  currentWidget_ = parentWidget;
  currentLayout_ = (currentWidget_ ? currentWidget_->getTkLayout() : nullptr);
  currentChild_  = nullptr;

  updateChildren();

  updateOptions();
}

void
CTkAppDebug::
currentSlot()
{
  currentWidget_ = currentChild_;
  currentLayout_ = (currentWidget_ ? currentWidget_->getTkLayout() : nullptr);
  currentChild_  = nullptr;

  updateChildren();

  updateOptions();
}

void
CTkAppDebug::
childSelectionSlot()
{
  // get current child
  auto items = childList_->selectedItems();

  int row = -1;

  for (auto *item : items)
    row = childList_->row(item);

  CTkAppWidget::Children children;

  if (currentWidget_)
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

  //---

  QString childName;

  if (currentChild_)
    childName = QString("%1 (%2)").
      arg(currentChild_->getName()).arg(currentChild_->calcClassName());

  layoutWidgetLabel_->setText(childName);

  //---

  updateLayoutWidgets();

  if (currentChild_ && currentChild_->getQWidget())
    showWidget(currentChild_->getQWidget());
}

void
CTkAppDebug::
imageSelectionSlot()
{
  imageProperties_->clear();

  //---

  auto items = imageList_->selectedItems();

  int row = -1;

  for (auto *item : items)
    row = imageList_->row(item);

  if (row < 0) return;

  std::vector<QString> imageNames;

  tk_->getImageNames(imageNames);

  auto *image = tk_->getImage(imageNames[row]).get();

  //---

  QPixmap pixmap;

  pixmap.convertFromImage(image->getQImage());

  imageLabel_->setPixmap(pixmap);

  //---

  imageProperties_->addProperty("", image, "name");
  imageProperties_->addProperty("", image, "filename");
  imageProperties_->addProperty("", image, "type");
  imageProperties_->addProperty("", image, "format");
  imageProperties_->addProperty("", image, "gamma");
  imageProperties_->addProperty("", image, "width");
  imageProperties_->addProperty("", image, "height");
}

void
CTkAppDebug::
bitmapSelectionSlot()
{
  imageProperties_->clear();

  //---

  auto items = bitmapList_->selectedItems();

  int row = -1;

  for (auto *item : items)
    row = bitmapList_->row(item);

  if (row < 0) return;

  std::vector<QString> bitmapNames;

  tk_->getBitmapNames(bitmapNames);

  auto *image = tk_->getBitmap(bitmapNames[row]).get();

  //---

  QPixmap pixmap;

  pixmap.convertFromImage(image->getQImage());

  bitmapLabel_->setPixmap(pixmap);

  //---

  imageProperties_->addProperty("", image, "name");
  imageProperties_->addProperty("", image, "filename");
  imageProperties_->addProperty("", image, "type");
  imageProperties_->addProperty("", image, "format");
  imageProperties_->addProperty("", image, "gamma");
  imageProperties_->addProperty("", image, "width");
  imageProperties_->addProperty("", image, "height");
}

void
CTkAppDebug::
fontSelectionSlot()
{
  fontProperties_->clear();

  //---

  auto items = fontList_->selectedItems();

  int row = -1;

  for (auto *item : items)
    row = fontList_->row(item);

  if (row < 0) return;

  std::vector<QString> fontNames;

  tk_->getFontNames(fontNames);

  auto *font = tk_->getFont(fontNames[row]).get();

  //---

  fontProperties_->addProperty("", font, "family");
  fontProperties_->addProperty("", font, "bold");
  fontProperties_->addProperty("", font, "italic");
  fontProperties_->addProperty("", font, "underline");
  fontProperties_->addProperty("", font, "overstrike");
  fontProperties_->addProperty("", font, "height");
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

  if (layoutWidget_) {
    layoutGroup_  ->setVisible(true);
    noLayoutLabel_->setVisible(false);
  }
  else {
    layoutGroup_  ->setVisible(false);
    noLayoutLabel_->setVisible(true);
    return;
  }

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
updateOptions()
{
  optionsTable_->clear();

  if (! currentWidget_)
    return;

  const auto &opts = currentWidget_->getOpts();

  std::vector<QString> names;
  opts.getNames(names, /*alias*/false);

  optionsTable_->setColumnCount(5);
  optionsTable_->setRowCount(names.size());

  optionsTable_->setHorizontalHeaderLabels(QStringList() <<
    "Name" << "Data Name" << "Class Name" << "Default" << "Value");

  int row = 0;

  auto setTableItem = [&](int col, const QString &str) {
    auto *item = new QTableWidgetItem(str);
    optionsTable_->setItem(row, col, item);
  };

  for (const auto &name : names) {
    auto *nameItem = new QTableWidgetItem(name);

    optionsTable_->setItem(row, 0, nameItem);

    const auto *opt = opts.opt(name);

    QVariant var;
    opts.getOptValue(opt, var);

    auto valueStr = tk_->variantToString(var);

    setTableItem(1, QString(opt->dname()));
    setTableItem(2, QString(opt->cname()));
    setTableItem(3, QString(opt->def()));
    setTableItem(4, valueStr);

    ++row;
  }
}

const CTkAppOpt *
CTkAppDebug::
getRowOption(int row) const
{
  if (! currentWidget_) return nullptr;

  const auto &opts = currentWidget_->getOpts();

  std::vector<QString> names;
  opts.getNames(names, /*alias*/false);

  if (row < 0 || row >= int(names.size())) return nullptr;

  return opts.opt(names[row]);
}

QVariant
CTkAppDebug::
getOptValue(const CTkAppOpt *opt)
{
  if (! currentWidget_) return QVariant();

  const auto &opts = currentWidget_->getOpts();

  QVariant value;
  opts.getOptValue(opt, value);

  return value;
}

void
CTkAppDebug::
updateProperties()
{
  propertyView_->clear();

  for (const auto &w : tk_->widgets()) {
    auto parentName = "widgets/" + widgetPropPath(w) + "/.DATA";

    propertyView_->addProperty(parentName, w, "type");
    propertyView_->addProperty(parentName, w, "name");
    propertyView_->addProperty(parentName, w, "fullName");

    propertyView_->addProperty(parentName, w, "background");
    propertyView_->addProperty(parentName, w, "foreground");

    propertyView_->addProperty(parentName, w, "activeBackground");
    propertyView_->addProperty(parentName, w, "activeForeground");

    propertyView_->addProperty(parentName, w, "disabledBackground");
    propertyView_->addProperty(parentName, w, "disabledForeground");

    propertyView_->addProperty(parentName, w, "highlightBackground");
    propertyView_->addProperty(parentName, w, "highlightForeground");

    propertyView_->addProperty(parentName, w, "insertBackground");

    propertyView_->addProperty(parentName, w, "relief");

    propertyView_->addProperty(parentName, w, "anchorStr");
    propertyView_->addProperty(parentName, w, "anchor");

    propertyView_->addProperty(parentName, w, "borderWidth");
    propertyView_->addProperty(parentName, w, "selectBorderWidth");
    propertyView_->addProperty(parentName, w, "insertBorderWidth");
    propertyView_->addProperty(parentName, w, "insertWidth");

    propertyView_->addProperty(parentName, w, "bitmap");

    propertyView_->addProperty(parentName, w, "highlightThickness");

    propertyView_->addProperty(parentName, w, "text");
    propertyView_->addProperty(parentName, w, "width");
    propertyView_->addProperty(parentName, w, "height");
    propertyView_->addProperty(parentName, w, "title");
    propertyView_->addProperty(parentName, w, "icon");
    propertyView_->addProperty(parentName, w, "geometry");

    propertyView_->addProperty(parentName, w, "command");
    propertyView_->addProperty(parentName, w, "xScrollCommand");
    propertyView_->addProperty(parentName, w, "yScrollCommand");

    propertyView_->addProperty(parentName, w, "gridWidget");
    propertyView_->addProperty(parentName, w, "deleteWindowCmd");

    //---

    auto *button = qobject_cast<CTkAppButton *>(w);

    if (button) {
      propertyView_->addProperty(parentName, button, "overRaised");
    }

    auto *canvas = qobject_cast<CTkAppCanvas *>(w);

    if (canvas) {
      auto *cw = canvas->canvasWidget();

      for (auto *shape : cw->getShapes()) {
        auto shapeName = parentName + "/shapes/" + QString::number(shape->id());

        propertyView_->addProperty(shapeName, shape, "id");

        propertyView_->addProperty(shapeName, shape, "enabled");
        propertyView_->addProperty(shapeName, shape, "visible");

        propertyView_->addProperty(shapeName, shape, "strokeColor");
        propertyView_->addProperty(shapeName, shape, "strokeAlpha");
        propertyView_->addProperty(shapeName, shape, "strokeWidth");
        propertyView_->addProperty(shapeName, shape, "strokeCap");

        propertyView_->addProperty(shapeName, shape, "fillColor");
        propertyView_->addProperty(shapeName, shape, "fillAlpha");

        auto *rectShape = qobject_cast<CTkAppCanvasRectangleShape *>(shape);
        auto *ovalShape = qobject_cast<CTkAppCanvasOvalShape      *>(shape);

        if (rectShape) {
          propertyView_->addProperty(shapeName, rectShape, "x1");
          propertyView_->addProperty(shapeName, rectShape, "y1");
          propertyView_->addProperty(shapeName, rectShape, "x2");
          propertyView_->addProperty(shapeName, rectShape, "y2");
        }

        if (ovalShape) {
          propertyView_->addProperty(shapeName, ovalShape, "x1");
          propertyView_->addProperty(shapeName, ovalShape, "y1");
          propertyView_->addProperty(shapeName, ovalShape, "x2");
          propertyView_->addProperty(shapeName, ovalShape, "y2");
        }
      }
    }

    auto *check = qobject_cast<CTkAppCheckButton *>(w);

    if (check) {
      propertyView_->addProperty(parentName, check, "varName");
      propertyView_->addProperty(parentName, check, "onValue");
      propertyView_->addProperty(parentName, check, "offValue");
      propertyView_->addProperty(parentName, check, "showIndicator");
      propertyView_->addProperty(parentName, check, "overRaised");
      propertyView_->addProperty(parentName, check, "justify");
      propertyView_->addProperty(parentName, check, "underlinePos");
    }

    auto *combo = qobject_cast<CTkAppComboBox *>(w);

    if (combo) {
      propertyView_->addProperty(parentName, combo, "varName");
    }

    auto *entry = qobject_cast<CTkAppEntry *>(w);

    if (entry) {
      propertyView_->addProperty(parentName, entry, "varName");
    }

    auto *label = qobject_cast<CTkAppLabel *>(w);

    if (label) {
      propertyView_->addProperty(parentName, label, "varName");
    }

    auto *listbox = qobject_cast<CTkAppListBox *>(w);

    if (listbox) {
      propertyView_->addProperty(parentName, listbox, "varName");
    }

    auto *menubutton = qobject_cast<CTkAppMenuButton *>(w);

    if (menubutton) {
      propertyView_->addProperty(parentName, menubutton, "varName");
      propertyView_->addProperty(parentName, menubutton, "showIndicator");
    }

    auto *message = qobject_cast<CTkAppMessage *>(w);

    if (message) {
      propertyView_->addProperty(parentName, message, "varName");
    }

    auto *radio = qobject_cast<CTkAppRadioButton *>(w);

    if (radio) {
      propertyView_->addProperty(parentName, radio, "varName");
      propertyView_->addProperty(parentName, radio, "value");
      propertyView_->addProperty(parentName, radio, "selectColor");
      propertyView_->addProperty(parentName, radio, "showIndicator");
      propertyView_->addProperty(parentName, radio, "userWidth");
      propertyView_->addProperty(parentName, radio, "userHeight");
    }

    auto *scale = qobject_cast<CTkAppScale *>(w);

    if (scale) {
      propertyView_->addProperty(parentName, scale, "varName");
    }

    auto *spinbox = qobject_cast<CTkAppSpinBox *>(w);

    if (spinbox) {
      propertyView_->addProperty(parentName, spinbox, "varName");
      propertyView_->addProperty(parentName, spinbox, "invalidCommand");
    }

    auto *toplevel = qobject_cast<CTkAppTopLevel *>(w);

    if (toplevel) {
      propertyView_->addProperty(parentName, toplevel, "iconWindow");
    }
  }
}

QString
CTkAppDebug::
widgetPropPath(CTkAppWidget *w) const
{
  if (w->getParent())
    return widgetPropPath(w->getParent()) + "/" + w->getName();
  else
    return w->getName();
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
    else if (id == "padx"   ) info.setPadX(i, i);
    else if (id == "pady"   ) info.setPadY(i, i);
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

void
CTkAppDebug::
showWidget(QWidget *widget)
{
  auto rect  = widget->rect();
  auto rtl   = widget->mapToGlobal(rect.topLeft());
  auto rbr   = widget->mapToGlobal(rect.bottomRight());
  auto rrect = QRect(rtl, rbr);

  if (! rubberBand_)
    rubberBand_ = new QRubberBand(QRubberBand::Rectangle);

  rubberBand_->setGeometry(rrect);

  rubberBand_->show();
}

//---

CTkAppOptionTableDelegate::
CTkAppOptionTableDelegate(CTkAppDebug *debug) :
 debug_(debug)
{
}

void
CTkAppOptionTableDelegate::
paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  auto *opt = debug_->getRowOption(index.row());

  int col = index.column();

  if ((col == 3 || col == 4) && opt->isColor()) {
    QVariant value;

    if (col == 3)
      value = QString(opt->def());
    else
      value = debug_->getOptValue(opt);

    return drawColor(painter, option, QColor(value.toString()), index);
  }

  return QItemDelegate::paint(painter, option, index);
}

void
CTkAppOptionTableDelegate::
drawColor(QPainter *painter, const QStyleOptionViewItem &option,
          const QColor &color, const QModelIndex &index) const
{
  QItemDelegate::drawBackground(painter, option, index);

  //---

  int margin = qApp->style()->pixelMetric(QStyle::PM_HeaderMargin);

  //---

  // draw color swatch
  auto colorRect = option.rect;

  int s = colorRect.height();

  colorRect.setWidth(s);

  colorRect.adjust(0, 1, -3, -2);

  colorRect.setLeft(colorRect.left() + margin);

  painter->save();

  painter->setBrush(QBrush(color));
  painter->setPen(QColor(Qt::black)); // TODO: contrast border

  painter->drawRect(colorRect);

  painter->restore();

  //---

  // draw label
  int x = colorRect.right() + margin;

  QRect rect1;

  rect1.setCoords(x, option.rect.top(), option.rect.right(), option.rect.bottom());

  QItemDelegate::drawDisplay(painter, option, rect1, color.name());
}

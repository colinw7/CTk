#ifndef CTkAppDebug_H
#define CTkAppDebug_H

#include <QFrame>

class CTkApp;
class CTkAppWidget;
class CTkAppLayout;

class CTkAppPackLayout;
class CTkAppGridLayout;
class CTkAppPlaceLayout;
class CTkAppLayoutWidget;

class CQPropertyViewTree;
class CQIntegerSpin;
class CQRealSpin;
class CQOptEdit;

class QTabWidget;
class QListWidget;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QLabel;

class QVBoxLayout;

class CTkAppDebug : public QFrame {
  Q_OBJECT

 public:
  CTkAppDebug(CTkApp *tk);

 private:
  void updateChildren();

  void createLayoutWidgets();
  void updateLayoutWidgets();

  void updateImages();
  void updateFonts();

  void updateProperties();

  QString widgetPropPath(CTkAppWidget *w) const;

 private Q_SLOTS:
  void parentSlot();
  void currentSlot();

  void childSelectionSlot();
  void imageSelectionSlot();
  void bitmapSelectionSlot();
  void fontSelectionSlot();

  void boolChanged(int i);
  void optIntChanged(bool);
  void intChanged(int i);
  void optRealChanged(bool);
  void realChanged(double r);
  void stringChanged();
  void comboChanged(int i);

 private:
  CTkApp* tk_ { nullptr };

  CTkAppWidget* currentWidget_ { nullptr };
  CTkAppWidget* currentChild_  { nullptr };

  CTkAppLayoutWidget* layoutWidget_ { nullptr };

  QTabWidget *tab_ { nullptr };

  QListWidget* childList_ { nullptr };

  QVBoxLayout* dataLayout_  { nullptr };
  QLabel*      typeLabel_   { nullptr };
  QLabel*      layoutLabel_ { nullptr };

  QListWidget*        imageList_       { nullptr };
  QLabel*             imageLabel_      { nullptr };
  QListWidget*        bitmapList_      { nullptr };
  QLabel*             bitmapLabel_     { nullptr };
  CQPropertyViewTree* imageProperties_ { nullptr };

  QListWidget*        fontList_       { nullptr };
  QLabel*             fontLabel_      { nullptr };
  CQPropertyViewTree* fontProperties_ { nullptr };

  //---

  CTkAppLayout* currentLayout_ { nullptr };

  CTkAppPackLayout*  packLayout_  { nullptr };
  CTkAppGridLayout*  gridLayout_  { nullptr };
  CTkAppPlaceLayout* placeLayout_ { nullptr };

  //---

  QFrame*      layoutFrame_  { nullptr };
  QVBoxLayout* layoutLayout_ { nullptr };

  struct LayoutWidgets {
    // pack
    QComboBox *sideCombo { nullptr };
    QComboBox *fillCombo { nullptr };

    QCheckBox *expandCheck { nullptr };

    CQIntegerSpin *padXSpin  { nullptr };
    CQIntegerSpin *padYSpin  { nullptr };
    CQIntegerSpin *ipadXSpin { nullptr };
    CQIntegerSpin *ipadYSpin { nullptr };

    // grid
    CQOptEdit* rowOptEdit     { nullptr }; CQIntegerSpin *rowSpin     { nullptr };
    CQOptEdit* colOptEdit     { nullptr }; CQIntegerSpin *colSpin     { nullptr };
    CQOptEdit* rowSpanOptEdit { nullptr }; CQIntegerSpin *rowSpanSpin { nullptr };
    CQOptEdit* colSpanOptEdit { nullptr }; CQIntegerSpin *colSpanSpin { nullptr };

    QLineEdit *stickySidesEdit { nullptr };
    QLineEdit *inEdit          { nullptr };

    CQIntegerSpin *weightSpin { nullptr };

    // place
    QComboBox *borderModeCombo { nullptr };

    CQOptEdit* xOptEdit         { nullptr }; CQRealSpin* xSpin         { nullptr };
    CQOptEdit* yOptEdit         { nullptr }; CQRealSpin* ySpin         { nullptr };
    CQOptEdit* widthOptEdit     { nullptr }; CQRealSpin* widthSpin     { nullptr };
    CQOptEdit* heightOptEdit    { nullptr }; CQRealSpin* heightSpin    { nullptr };
    CQOptEdit* relXOptEdit      { nullptr }; CQRealSpin* relXSpin      { nullptr };
    CQOptEdit* relYOptEdit      { nullptr }; CQRealSpin* relYSpin      { nullptr };
    CQOptEdit* relWidthOptEdit  { nullptr }; CQRealSpin* relWidthSpin  { nullptr };
    CQOptEdit* relHeightOptEdit { nullptr }; CQRealSpin* relHeightSpin { nullptr };
  };

  LayoutWidgets layoutWidgets_;

  CQPropertyViewTree *propertyView_ { nullptr };

  bool updating_ { false };
};

#endif

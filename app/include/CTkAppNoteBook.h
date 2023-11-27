#ifndef CTkAppNoteBook_H
#define CTkAppNoteBook_H

#include <CTkAppWidget.h>

class CTkAppNoteBookWidget;

class CTkAppNoteBook : public CTkAppWidget {
  Q_OBJECT

 public:
  explicit CTkAppNoteBook(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "NoteBook"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

 private:
  QTabWidget* qtab_ { nullptr };
};

class CTkAppNoteBookWidget : public QTabWidget {
  Q_OBJECT

 public:
  CTkAppNoteBookWidget(CTkAppNoteBook *notebook);

 private:
  CTkAppNoteBook *notebook_ { nullptr };
};

#endif

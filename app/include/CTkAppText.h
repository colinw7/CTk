#ifndef CTkAppText_H
#define CTkAppText_H

#include <CTkAppWidget.h>

class CTkAppTextWidget;

class CTkAppText : public CTkAppWidget {
  Q_OBJECT

 public:
  using TextInd = CTkAppTextInd;

  struct TextIndRange {
    TextIndRange() { }

    TextIndRange(const TextInd &start1, const TextInd &end1) :
     start(start1), end(end1) {
    }

    TextInd start;
    TextInd end;

    QString toString() const {
      return start.toString() + " " + end.toString();
    }
  };

  using TextIndRanges = std::vector<TextIndRange>;

  struct TagData {
    QColor        foreground;
    QColor        background;
    QFont         font;
    double        borderWidth { 0.0 };
    bool          underline { false };
    TextIndRanges indRanges;
  };

 public:
  explicit CTkAppText(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "Text"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

  void setCurrentInd(const TextInd &ind,
                     const QTextCursor::MoveMode &modeMode=QTextCursor::MoveAnchor) const;
  void setCurrentInd(QTextCursor &cursor, const TextInd &ind,
                     const QTextCursor::MoveMode &modeMode=QTextCursor::MoveAnchor) const;

  void getCurrentInd(TextInd &ind) const;
  void getCurrentInd(QTextCursor &cursor, TextInd &ind) const;

  bool stringToTextInd(const QVariant &str, TextInd &ind) const;

  void setMark(const QString &mark, const TextInd &ind);
  bool getMark(const QString &mark, TextInd &ind) const;
  void getMarkNames(std::vector<QString> &names) const;

  bool getMarkGravity(const QString &name, QString &gravity) const;
  bool setMarkGravity(const QString &name, const QString &gravity);

  TextIndRange remapIndRange(const TextIndRange &ind) const;
  TextInd remapInd(const TextInd &ind) const;

  void setText(const QString &text) override;

  void lowerTag(const QString &tag, const QString &aboveTag);
  void raiseTag(const QString &tag, const QString &aboveTag);

  void applyTag(const TagData &tagData);

  void deleteTag(const QString &tag);

  void addTagRanges(const QString &tag, const TextIndRanges &indRanges);

  void getTagRanges(const QString &tag, TextIndRanges &indRanges) const;

  void tagRemove(const QString &tag);

  void getTagNames(std::vector<QString> &names) const;

  TagData &tagData(const QString &name);

 private:
  void connectSlots(bool b);

 private Q_SLOTS:
  void vscrollSlot(int value);
  void hscrollSlot(int value);

 private:
  struct MarkData {
    TextInd textInd;
    QString gravity { "right" };

    MarkData() { }

    explicit MarkData(const TextInd &ind) :
     textInd(ind) {
    }
  };

  using Marks = std::map<QString, MarkData>;
  using Tags  = std::map<QString, TagData>;

  CTkAppTextWidget* qtext_ { nullptr };
  Marks             marks_;
  Tags              tags_;
};

#endif

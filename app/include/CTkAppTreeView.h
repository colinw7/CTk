#ifndef CTkAppTreeView_H
#define CTkAppTreeView_H

#include <CTkAppWidget.h>

class CTkAppTreeViewWidget;

class CTkAppTreeView : public CTkAppWidget {
  Q_OBJECT

 public:
  class Index {
   public:
    static Index root() { Index ind; ind.root_ = true; return ind; }

    Index() { }

   ~Index() { delete parent_; }

    Index(const Index &i) {
      root_   = i.root_;
      row_    = i.row_;
      col_    = i.col_;
      parent_ = (i.parent_ ? new Index(*i.parent_) : nullptr);
    }

    Index &operator=(const Index &i) {
      delete parent_;

      root_   = i.root_;
      row_    = i.row_;
      col_    = i.col_;
      parent_ = (i.parent_ ? new Index(*i.parent_) : nullptr);

      return *this;
    }

    bool operator==(const Index &rhs) const {
      if (root_ && rhs.root_)
        return true;

      if (row_ != rhs.row_ || col_ != rhs.col_)
        return false;

      if (! parent_ || ! rhs.parent_)
        return (parent_ == rhs.parent_);

      return (*parent_ == *rhs.parent_);
    }

    bool isRoot() const { return root_; }
    void setRoot(bool b) { root_ = b; }

    int row() const { return row_; }
    void setRow(int i) { row_ = i; }

    int col() const { return col_; }
    void setCol(int i) { col_ = i; }

    bool hasParent() const { return parent_; }

    Index parent() const { return *parent_; }

    void setParent(const Index &ind) {
      parent_ = new Index(ind);
    }

   private:
    bool   root_   { false };
    int    row_    { -1 };
    int    col_    { -1 };
    Index *parent_ { nullptr };
  };

 public:
  explicit CTkAppTreeView(CTkApp *tk, CTkAppWidget *parent=nullptr, const QString &name="");

  const char *getClassName() const override { return "TreeView"; }

  bool execConfig(const QString &name, const QVariant &value) override;

  bool execOp(const Args &args) override;

 private:
  void connectSlots(bool b);

  void setIdIndex(const QString &id, const Index &ind);
  bool getIdIndex(const QString &id, Index &ind) const;
  bool getIndexId(const Index &ind, QString &id) const;

 private Q_SLOTS:
  void selectionSlot();

 private:
  using Ids = std::map<QString, Index>;

  CTkAppTreeViewWidget* qtree_ { nullptr };
  Ids                   ids_;
};

//---

class CTkAppTreeViewWidget : public QTreeWidget {
  Q_OBJECT

 public:
  using OptReal = std::optional<double>;

 public:
  CTkAppTreeViewWidget(CTkAppTreeView *listbox);

  const OptReal &height() const { return height_; }
  void setHeight(const OptReal &h);

  QSize sizeHint() const override;

 private:
  CTkAppTreeView* treeview_ { nullptr };

  OptReal height_;
};

#endif

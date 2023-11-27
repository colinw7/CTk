#include <CTkAppTreeView.h>

#include <CQUtil.h>
#include <CQStrParse.h>

#include <deque>

CTkAppTreeView::
CTkAppTreeView(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qtree_ = new CTkAppTreeViewWidget(this);

  qtree_->setColumnCount(1);

  setQWidget(qtree_);

  connectSlots(true);
}

void
CTkAppTreeView::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qtree_,
    SIGNAL(itemSelectionChanged()), this, SLOT(selectionSlot()));
}

bool
CTkAppTreeView::
execConfig(const QString &name, const QVariant &var)
{
  auto value = tk_->variantToString(var);

  if      (name == "-columns") {
    std::vector<QString> strs;
    if (! tk_->splitList(value, strs))
      return tk_->throwError("Invalid columns \"" + value + "\"");

    qtree_->setColumnCount(strs.size() + 1);
  }
  else if (name == "-displaycolumns") {
    // TODO: reorder columns
    tk_->TODO(name);
  }
  else if (name == "-height") {
    long h;
    if (! CTkAppUtil::stringToInt(value, h))
      return tk_->throwError("bad integer \"" + value + "\"");
    qtree_->setHeight(h);
  }
  else if (name == "-selectmode") {
    if      (value == "single") {
      qtree_->setSelectionMode(QAbstractItemView::SingleSelection);
    }
    else if (value == "browse") {
      qtree_->setSelectionMode(QAbstractItemView::SingleSelection); // TODO
    }
    else if (value == "multiple") {
      qtree_->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    else if (value == "extended") {
      qtree_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }
    else
      return false;
  }
  else if (name == "-show") {
    if      (value == "tree") {
      tk_->TODO(name);
    }
    else if (value == "headings") {
      tk_->TODO(name);
    }
    else
      return false;
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppTreeView::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  //---

  auto stringToColumn = [&](const QString &arg, int &c) {
    if (arg == "#0") {
      c = 0;
      return true;
    }

    long l;
    if (! CTkAppUtil::stringToInt(arg, l))
      return false;
    c = int(l + 1);

    if (c < 0 || c >= qtree_->columnCount())
      return false;

    return true;
  };

  auto stringToRootIndex = [&](const QString &arg, Index &ind) {
    if (arg == "") {
      ind.setRoot(true);
      return true;
    }

    if (getIdIndex(arg, ind))
      return true;

    CQStrParse parse(arg);

    if (parse.isString("IND")) {
      parse.skipLastString();

      auto parent = Index::root();

      while (true) {
        long r;
        if (! parse.readInteger(&r))
          return false;

        if (! parse.isChar('.'))
          return false;

        parse.skipChar();

        long c;
        if (! parse.readInteger(&c))
          return false;

        ind = Index();

        ind.setRow(r);
        ind.setCol(c);

        if (! parent.isRoot())
          ind.setParent(parent);

        parent = ind;

        if (parse.eof())
          break;

        if (! parse.isChar('|'))
          return false;

        parse.skipChar();
      }
    }
    else {
      long l;
      if (! parse.readInteger(&l))
        return false;

      ind.setRow(l);
      ind.setCol(0);
    }

    if (! parse.eof())
      return false;

    return true;
  };

  auto stringToChildIndex = [&](const QString &arg, Index &parentInd,
                                Index &ind, bool quiet=false) {
    if (arg == "")
      return (quiet ? true : false);

    long l;
    if (! CTkAppUtil::stringToInt(arg, l))
      return (quiet ? true : false);

    ind.setRow(l);
    ind.setCol(0);

    if (! parentInd.isRoot())
      ind.setParent(parentInd);

    return true;
  };

  auto getIndexItem = [&](const Index &ind, bool create) {
    auto ind1 = ind;

    std::deque<Index> inds;

    while (ind1.hasParent()) {
      inds.push_front(ind1);

      ind1 = ind1.parent();
    }

    assert(ind1.row() >= 0);
    auto *item = qtree_->topLevelItem(ind1.row());

    if (! item) {
      if (! create)
        return static_cast<QTreeWidgetItem *>(nullptr);

      item = new QTreeWidgetItem(QStringList() << "");

      qtree_->addTopLevelItem(item);
    }

    for (auto &ind2 : inds) {
      assert(ind2.row() >= 0);
      auto *child = item->child(ind2.row());

      if (! child) {
        if (! create)
          return static_cast<QTreeWidgetItem *>(nullptr);

        child = new QTreeWidgetItem(QStringList() << "");

        item->addChild(child);
      }

      item = child;
    }

    return item;
  };

  auto indexToString = [&](const Index &ind) {
    QString str;

    if (getIndexId(ind, str))
      return str;

    str = "IND";

    auto ind1 = ind;

    std::deque<Index> inds;

    while (ind1.hasParent()) {
      inds.push_front(ind1);

      ind1 = ind1.parent();
    }

    str += QString::number(ind1.row()) + "." + QString::number(ind1.col());

    for (auto &ind2 : inds)
      str += "|" + QString::number(ind2.row()) + "." + QString::number(ind2.col());

    return str;
  };

  auto itemToIndex = [&](QTreeWidgetItem  *item) {
    std::deque<int> inds;

    auto *parent = item->parent();

    while (parent) {
      auto ind = parent->indexOfChild(item);

      item   = parent;
      parent = item->parent();

      inds.push_front(ind);
    }

    auto ind = qtree_->indexOfTopLevelItem(item);

    inds.push_front(ind);

    auto parentInd = Index::root();

    auto itemInd = parentInd;

    for (auto &ind : inds) {
      Index ind1;

      ind1.setRow(ind);
      ind1.setCol(0);

      if (! parentInd.isRoot())
        ind1.setParent(parentInd);

      itemInd   = ind1;
      parentInd = ind1;
    }

    return itemInd;
  };

  auto itemToString = [&](QTreeWidgetItem  *item) {
    return indexToString(itemToIndex(item));
  };

  //---

  static auto optionNames = std::vector<QString>({
    "bbox", "cellselection" , "children", "cget", "column", "configure", "delete", "detach",
    "drag", "drop", "exists", "focus", "heading", "identify", "index", "insert", "instate",
    "item", "move", "next", "parent", "prev", "see", "selection", "set", "state", "style",
    "tag", "xview", "yview"});

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  if      (option == "bbox") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs(getFullName() + " bbox itemid ?column");

    Index ind;
    if (! stringToRootIndex(tk_->variantToString(args[1]), ind))
      return false;

    if (numArgs == 3) {
      int c;
      if (! stringToColumn(tk_->variantToString(args[2]), c))
        return false;
      ind.setCol(c);
    }

    auto *item = getIndexItem(ind, /*create*/false);
    if (! item) return false;

    auto r = qtree_->visualItemRect(item);

    tk_->setIntegerArrayResult({{r.x(), r.y(), r.width(), r.height()}});
  }
  else if (option == "cellselection") {
    tk_->TODO(args);
  }
  else if (option == "children") {
   if (numArgs != 2 && numArgs != 3)
     return tk_->wrongNumArgs(getFullName() + " children item ?newchildren?");

    tk_->TODO(args);
  }
  else if (option == "column") {
    tk_->TODO(args);
  }
  else if (option == "delete") {
    tk_->TODO(args);
  }
  else if (option == "detach") {
    tk_->TODO(args);
  }
  else if (option == "drag") {
    tk_->TODO(args);
  }
  else if (option == "drop") {
    tk_->TODO(args);
  }
  else if (option == "exists") {
    tk_->TODO(args);
  }
  else if (option == "focus") {
    tk_->TODO(args);
  }
  else if (option == "heading") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " heading column -option value...");

    int c;
    if (! stringToColumn(tk_->variantToString(args[1]), c))
      return false;

    uint i = 2;

    for ( ; i < numArgs; ++i) {
      if (args[i] == "-text") {
        auto text = tk_->variantToString(args[++i]);

        auto *item = qtree_->headerItem();

        item->setText(c, text);
      }
      else
        return false;
    }
  }
  else if (option == "identify") {
    tk_->TODO(args);
  }
  else if (option == "index") {
    tk_->TODO(args);
  }
  else if (option == "insert") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getFullName() + " insert parent index ?-id id? -options...");

    Index parentInd;
    if (! stringToRootIndex(tk_->variantToString(args[1]), parentInd))
      return false;

    Index ind;
    if (! stringToChildIndex(tk_->variantToString(args[2]), parentInd, ind))
      return false;

    auto *item = getIndexItem(ind, /*create*/true);

    uint i = 3;

    for ( ; i < numArgs; ++i) {
      if      (args[i] == "-id") {
        auto id = tk_->variantToString(args[++i]);

        setIdIndex(id, ind);
      }
      else if (args[i] == "-text") {
        auto text = tk_->variantToString(args[++i]);

        item->setText(ind.col(), text);
      }
      else if (args[i] == "-values") {
        auto value = tk_->variantToString(args[++i]);

        std::vector<QString> strs;
        if (! tk_->splitList(value, strs))
          return tk_->throwError("Invalid values \"" + value + "\"");

        int col = 1;

        for (auto &str : strs)
          item->setText(col++, str);
      }
      else
        return false;
    }

    tk_->setStringResult(indexToString(ind));
  }
  else if (option == "instate") {
    tk_->TODO(args);
  }
  else if (option == "item") {
    tk_->TODO(args);
  }
  else if (option == "move") {
    tk_->TODO(args);
  }
  else if (option == "next") {
    tk_->TODO(args);
  }
  else if (option == "parent") {
    tk_->TODO(args);
  }
  else if (option == "prev") {
    tk_->TODO(args);
  }
  else if (option == "see") {
    tk_->TODO(args);
  }
  else if (option == "selection") {
    if (numArgs == 1) {
      auto items = qtree_->selectedItems();

      std::vector<QString> strs;

      for (auto *item : items) {
        strs.push_back(itemToString(item));
      }

      tk_->setStringArrayResult(strs);
    }
    else {
      auto arg = tk_->variantToString(args[1]);

      if      (arg == "add") {
        tk_->TODO(args);
      }
      else if (arg == "remove") {
        tk_->TODO(args);
      }
      else if (arg == "set") {
        tk_->TODO(args);
      }
      else if (arg == "toggle") {
        tk_->TODO(args);
      }
      else {
        return tk_->wrongNumArgs(getFullName() +
                 " selection ?add|remove|set|toggle items?");
      }
    }
  }
  else if (option == "set") {
    tk_->TODO(args);
  }
  else if (option == "state") {
    tk_->TODO(args);
  }
  else if (option == "tag") {
    tk_->TODO(args);
  }
  else if (option == "xview") {
    int step  = qtree_->horizontalScrollBar()->pageStep();
    int min   = qtree_->horizontalScrollBar()->minimum();
    int max   = qtree_->horizontalScrollBar()->maximum() - step;
    int value = qtree_->horizontalScrollBar()->value();

    if (numArgs < 2) {
      std::vector<double> reals;

      reals.push_back( value        /1000.0);
      reals.push_back((value + step)/1000.0);

      tk_->setRealArrayResult(reals);
    }
    else {
      auto opt = tk_->variantToString(args[1]);

      if      (opt == "moveto") {
        if (numArgs < 3)
          return tk_->wrongNumArgs(getFullName() + " xview moveto number");

        double x;

        if (! tk_->getOptionReal(opt, args[2], x))
          return false;

        qtree_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
      else if (opt == "scroll") {
        if (numArgs != 4)
          return tk_->wrongNumArgs(getFullName() + " xview scroll number pages|units");

        double x;
        if (! tk_->getOptionReal(opt, args[2], x))
          return false;

        if      (args[3] == "pages")
          value += x*1000*step;
        else if (args[3] == "units")
          value += x*1000;
        else
          return tk_->throwError(tk_->msg() + "bad argument \"" + args[3] +
                                 "\": must be pages or units");

        qtree_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
      else {
        double x;
        if (! CTkAppUtil::stringToReal(opt, x))
          return tk_->throwError("unknown option \"" + opt + "\": must be moveto or scroll");

        qtree_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
    }
  }
  else if (option == "yview") {
    int step  = qtree_->verticalScrollBar()->pageStep();
    int min   = qtree_->verticalScrollBar()->minimum();
    int max   = qtree_->verticalScrollBar()->maximum() - step;
    int value = qtree_->verticalScrollBar()->value();

    if (numArgs < 2) {
      std::vector<double> reals;

      reals.push_back( value        /1000.0);
      reals.push_back((value + step)/1000.0);

      tk_->setRealArrayResult(reals);
    }
    else {
      auto opt = tk_->variantToString(args[1]);

      if      (opt == "moveto") {
        if (numArgs != 3)
          return tk_->wrongNumArgs(getFullName() + " yview moveto fraction");

        double y;

        if (! tk_->getOptionReal(opt, args[2], y))
          return false;

        qtree_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
      else if (opt == "scroll") {
        if (numArgs != 4)
          return tk_->wrongNumArgs(getFullName() + " yview scroll number pages|units");

        double y;

        if (! tk_->getOptionReal(opt, args[2], y))
          return false;

        if      (args[3] == "pages")
          value += y*1000*step;
        else if (args[3] == "units")
          value += y*1000;
        else
          return tk_->throwError(tk_->msg() + "bad argument \"" + args[3] +
                                 "\": must be pages or units");

        qtree_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
      else {
        double y;
        if (! CTkAppUtil::stringToReal(opt, y))
          return tk_->throwError("unknown option \"" + opt + "\": must be moveto or scroll");

        qtree_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
    }
  }
  else
    return CTkAppWidget::execDefaultOp(option, args);

  return true;
}

void
CTkAppTreeView::
selectionSlot()
{
  CTkAppEventData eventData;

  eventData.type  = CTkAppEventType::Virtual;
  eventData.vtype = CTkAppVirtualEventType::TreeviewSelect;

  triggerVirtualEvents(eventData);
}

void
CTkAppTreeView::
setIdIndex(const QString &id, const Index &ind)
{
  ids_[id] = ind;
}

bool
CTkAppTreeView::
getIdIndex(const QString &id, Index &ind) const
{
  auto p = ids_.find(id);
  if (p == ids_.end()) return false;

  ind = (*p).second;

  return true;
}

bool
CTkAppTreeView::
getIndexId(const Index &ind, QString &id) const
{
  for (const auto &pi : ids_) {
    if (pi.second == ind) {
      id = pi.first;
      return true;
    }
  }

  return false;
}

//---

CTkAppTreeViewWidget::
CTkAppTreeViewWidget(CTkAppTreeView *treeview) :
 QTreeWidget(treeview->getParent() ? treeview->getParent()->getQWidget() : nullptr),
 treeview_(treeview)
{
  setObjectName("qtreeview");
}

void
CTkAppTreeViewWidget::
setHeight(const OptReal &h)
{
  height_ = h;
}

QSize
CTkAppTreeViewWidget::
sizeHint() const
{
  auto s = QTreeWidget::sizeHint();

  QFontMetrics fm(font());

  if (height_)
    s.setHeight((*height_)*fm.height());

  return s;
}

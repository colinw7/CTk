#include <CTkAppListBox.h>
#include <CTkAppWidgetCommand.h>

#include <CQUtil.h>

class CTkAppListBoxVarProc : public CTclTraceProc {
 public:
  CTkAppListBoxVarProc(CTkApp *tk, CTkAppListBox *listBox) :
   CTclTraceProc(tk), listBox_(listBox) {
  }

  void handleWrite(const char *) override {
    if (listBox_)
      listBox_->updateFromVar();
  }

 private:
  using ListBoxP = QPointer<CTkAppListBox>;

  ListBoxP listBox_;
};

class CTkAppListItem : public QListWidgetItem {
 public:
  CTkAppListItem(const QString &str) :
   QListWidgetItem(str) {
  }

  const QColor &background() const { return bg_; }
  void setBackground(const QColor &c) { bg_ = c; }

  const QColor &foreground() const { return fg_; }
  void setForeground(const QColor &c) { fg_ = c; }

  const QColor &selectBackground() const { return selectBg_; }
  void setSelectBackground(const QColor &c) { selectBg_ = c; }

  const QColor &selectForeground() const { return selectFg_; }
  void setSelectForeground(const QColor &c) { selectFg_ = c; }

 private:
  QColor bg_;
  QColor fg_;
  QColor selectBg_;
  QColor selectFg_;
};

//---

CTkAppListBox::
CTkAppListBox(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qlist_ = new CTkAppListBoxWidget(this);

  qlist_->verticalScrollBar  ()->setVisible(false);
  qlist_->horizontalScrollBar()->setVisible(false);

  setQWidget(qlist_);

  connectSlots(true);
}

void
CTkAppListBox::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qlist_,
    SIGNAL(itemSelectionChanged()), this, SLOT(selectionSlot()));
  CQUtil::connectDisconnect(b, qlist_->verticalScrollBar(),
    SIGNAL(valueChanged(int)), this, SLOT(vscrollSlot(int)));
  CQUtil::connectDisconnect(b, qlist_->horizontalScrollBar(),
    SIGNAL(valueChanged(int)), this, SLOT(hscrollSlot(int)));
}

bool
CTkAppListBox::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-activestyle") {
    static auto names = std::vector<QString>({"dotbox", "none", "underline"});
    QString value;
    if (! tk_->lookupName("activestyle", names, var, value))
      return false;

    setActiveStyle(value);

    widgetCommand_->setNameValue("-activestyle", value);
  }
  else if (name == "-exportselection") {
    bool b;
    if (! tk_->variantToBool(var, b))
      return tk_->invalidBool(var);

    setExportSelection(b);
  }
  else if (name == "-height") {
    CTkAppDistance h;
    if (! tk_->variantToDistance(var, h))
      return tk_->invalidDistance(var);

    qlist_->setHeight(CTkAppListBoxWidget::OptReal(h.rvalue));
  }
  else if (name == "-listvariable") {
    auto value = tk_->variantToString(var);

    setVarName(value);

    updateFromVar();

    varProc_ = new CTkAppListBoxVarProc(tk_, this);

    tk_->traceGlobalVar(varName(), varProc_);
  }
  else if (name == "-selectmode") {
    static auto names = std::vector<QString>({"single", "browse", "multiple", "extended"});
    QString value;
    if (! tk_->lookupName("selectmode", names, var, value, /*quiet*/true))
      value = tk_->variantToString(var);

    if      (value == "single") {
      qlist_->setSelectionMode(QAbstractItemView::SingleSelection);
    }
    else if (value == "browse") {
      qlist_->setSelectionMode(QAbstractItemView::SingleSelection); // TODO
    }
    else if (value == "multiple") {
      qlist_->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    else if (value == "extended") {
      qlist_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    }

    setSelectMode(value);
  }
  else if (name == "-state") {
    static auto names = std::vector<QString>({"disabled", "normal"});
    QString value;
    if (! tk_->lookupName("state", names, var, value, /*quiet*/true))
      value = tk_->variantToString(var);

    if (value == "readonly")
      qlist_->setReadOnly(true);
    else {
      if (! setWidgetStateFromString(value))
        return tk_->throwError(tk_->msg() + "bad state \"" + var + "\": must be "
                               "disabled or normal");
    }
  }
  else if (name == "-width") {
    CTkAppDistance w;
    if (! tk_->variantToDistance(var, w))
      return tk_->invalidDistance(var);

    qlist_->setWidth(CTkAppListBoxWidget::OptReal(w.rvalue));
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppListBox::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  //---

  enum class IndexPosition {
    NONE,
    END
  };

  struct IndexData {
    int           ind        { -1 };
    long          x          { -1 };
    long          y          { -1 };
    bool          isEnd      { false };
    bool          isPos      { false };
    IndexPosition pos        { IndexPosition::NONE };
    bool          rangeCheck { true };
  };

  auto varToIndexData = [&](const QVariant &var, IndexData &indexData) {
    auto str = tk_->variantToString(var);

    if (str == "") {
      indexData.ind = -1;
      return true;
    }

    indexData.ind   = -1;
    indexData.x     = -1;
    indexData.y     = -1;
    indexData.isEnd = false;
    indexData.isPos = false;

    if (str == "active") {
      indexData.ind = activeIndex();
      return true;
    }

    if (str == "anchor") {
      indexData.ind = selectionIndex();
      return true;
    }

    if (str == "end") {
      if (indexData.rangeCheck && indexData.pos != IndexPosition::END) {
        if (qlist_->count() == 0)
          return false;
      }

      indexData.ind   = qlist_->count() - 1;
      indexData.isEnd = true;

      return true;
    }

    if (str.length() > 1 && str[0] == '@') {
      auto strs = str.mid(1).split(",");
      if (strs.size() != 2) return false;

      long x, y;
      if (! CTkAppUtil::stringToInt(strs[0], x))
        return false;
      if (! CTkAppUtil::stringToInt(strs[1], y))
        return false;

      indexData.x = x;
      indexData.y = y;

      indexData.isPos = true;

      return true;
    }

    long i;
    if (! CTkAppUtil::stringToInt(str, i))
      return false;

    if (indexData.rangeCheck) {
      if (i < 0 || i >= qlist_->count()) {
        if (i != 0 || indexData.pos != IndexPosition::END)
          return false;
      }
    }

    indexData.ind = int(i);

    return true;
  };

  auto varToIndex = [&](const QVariant &var, int &ind, IndexPosition pos=IndexPosition::NONE,
                        bool rangeCheck=true) {
    IndexData indexData;
    indexData.pos        = pos;
    indexData.rangeCheck = rangeCheck;
    if (! varToIndexData(var, indexData))
      return false;
    ind = indexData.ind;
    return true;
  };

  auto indexDataToItem = [&](const IndexData &indexData) {
    return dynamic_cast<CTkAppListItem *>(qlist_->item(indexData.ind));
  };

#if 0
  auto varToItem = [&](const QVariant &var) {
    IndexData indexData;
    if (! varToIndexData(var, indexData))
      return static_cast<CTkAppListItem *>(nullptr);

    return indexDataToItem(indexData);
  };
#endif

  //---

  static auto optionNames = std::vector<QString>({
    "activate", "bbox", "cget", "configure", "curselection", "delete", "get",
    "index", "insert", "itemcget", "itemconfigure", "nearest", "scan",
    "see", "selection", "size", "xview", "yview"});

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  if      (option == "activate") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " activate index");

    int activeIndex;
    if (! varToIndex(args[1], activeIndex, IndexPosition::NONE, /*rangeCheck*/false))
      return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[1] + "\": must be "
                             "active, anchor, end, @x,y, or an index");

    setActiveIndex(activeIndex);
  }
  else if (option == "bbox") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " bbox index");

    IndexData indexData;
    indexData.rangeCheck = false;
    if (! varToIndexData(args[1], indexData))
      return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[1] + "\": must be "
                             "active, anchor, end, @x,y, or an index");

    auto *item = indexDataToItem(indexData);

    if (item) {
      auto r = qlist_->visualItemRect(item);

      tk_->setIntegerArrayResult({{r.x(), r.y(), r.width(), r.height()}});
    }
    else
      tk_->setStringResult("");
  }
  else if (option == "curselection") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getFullName() + " curselection");

    auto items = qlist_->selectedItems();

    std::vector<long> rows;

    for (auto *item : items)
      rows.push_back(qlist_->row(item));

    tk_->setIntegerArrayResult(rows);
  }
  else if (option == "delete") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs(getFullName() + " delete firstIndex ?lastIndex?");

    int startIndex;
    if (! varToIndex(args[1], startIndex, IndexPosition::NONE, /*rangeCheck*/false))
      return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[1] + "\": must be "
                             "active, anchor, end, @x,y, or an index");

    int endIndex = startIndex;
    if (numArgs == 3) {
      if (! varToIndex(args[2], endIndex, IndexPosition::NONE, /*rangeCheck*/false))
        return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[2] + "\": must be "
                               "active, anchor, end, @x,y, or an index");
    }

    for (int i = endIndex; i >= startIndex; --i) {
      if (i >= 0 && i < qlist_->count())
        delete qlist_->item(i);
    }
  }
  else if (option == "get") {
    if (numArgs != 2 && numArgs != 3)
      return tk_->wrongNumArgs(getFullName() + " get firstIndex ?lastIndex?");

    int startIndex;
    if (! varToIndex(args[1], startIndex, IndexPosition::NONE, /*rangeCheck*/false))
      return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[1] + "\": must be "
                             "active, anchor, end, @x,y, or an index");

    int endIndex = startIndex;
    if (numArgs == 3) {
      if (! varToIndex(args[2], endIndex, IndexPosition::NONE, /*rangeCheck*/false))
        return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[2] + "\": must be "
                               "active, anchor, end, @x,y, or an index");
    }
    else
      endIndex = startIndex;

    std::vector<QString> strs;

    for (int i = startIndex; i <= endIndex; ++i) {
      if (i >= 0 && i < qlist_->count())
        strs.push_back(qlist_->item(i)->text());
    }

    tk_->setStringArrayResult(strs);
  }
  else if (option == "index") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " index index");

    IndexData indexData;
    indexData.rangeCheck = false;
    if (! varToIndexData(args[1], indexData))
      return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[1] + "\": must be "
                             "active, anchor, end, @x,y, or an index");

    if (indexData.isEnd)
      ++indexData.ind;

    tk_->setIntegerResult(indexData.ind);
  }
  else if (option == "insert") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " insert index ?element ...?");

    IndexData indexData;
    indexData.pos        = IndexPosition::END;
    indexData.rangeCheck = false;
    if (! varToIndexData(args[1], indexData))
      return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[1] + "\": must be "
                             "active, anchor, end, @x,y, or an index");

    if (indexData.isEnd)
      indexData.ind = qlist_->count();

    uint i = 2;

    for ( ; i < numArgs; ++i) {
      auto str = tk_->variantToString(args[i]);

      auto *item = new CTkAppListItem(str);

      qlist_->insertItem(indexData.ind++, item);
    }
  }
  else if (option == "itemcget") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getFullName() + " itemcget index option");

    IndexData indexData;
    if (! varToIndexData(args[1], indexData))
      return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[1] + "\": must be "
                             "active, anchor, end, @x,y, or an index");

    auto *item = indexDataToItem(indexData);
    if (! item) return false;

    QString opt;
    auto str = tk_->variantToString(args[2]);
    if      (str == "-bg") opt = "-background";
    else if (str == "-fg") opt = "-foreground";
    else {
      static auto optNames = std::vector<QString>({
        "-background", "-foreground", "-selectbackground", "-selectforeground"});
      if (! tk_->lookupOptionName(optNames, args[2], opt))
        return false;
    }

    if      (opt == "-background") {
      tk_->setColorResult(item->background());
    }
    else if (opt == "-foreground") {
      tk_->setColorResult(item->foreground());
    }
    else if (opt == "-selectbackground") {
      tk_->setColorResult(item->selectBackground());
    }
    else if (opt == "-selectforeground") {
      tk_->setColorResult(item->selectForeground());
    }
  }
  else if (option == "itemconfigure") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() +
               " itemconfigure index ?option? ?value? ?option value ...?");

    IndexData indexData;
    if (! varToIndexData(args[1], indexData))
      return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[1] + "\": must be "
                             "active, anchor, end, @x,y, or an index");

    auto *item = indexDataToItem(indexData);
    if (! item) return false;

    if      (numArgs == 2) {
      std::vector<QVariant> res;

      auto addData = [&](const QString &name, const QString &type, const QString &ctype,
                         const QVariant &value) {
        QVariantList vars;

        vars.push_back(name);
        vars.push_back(type);
        vars.push_back(ctype);
        vars.push_back("");
        vars.push_back(value);

        res.push_back(vars);
      };

      auto addAlias = [&](const QString &alias, const QString &name) {
        QVariantList vars;

        vars.push_back(alias);
        vars.push_back(name);

        res.push_back(vars);
      };

      addData ("-background"      , "background"      , "Background", item->background());
      addAlias("-bg"              , "-background");
      addAlias("-fg"              , "-foreground");
      addData ("-foreground"      , "foreground"      , "Foreground", item->foreground());
      addData ("-selectbackground", "selectBackground", "Foreground", item->selectBackground());
      addData ("-selectforeground", "selectForeground", "Background", item->selectForeground());

      tk_->setVariantArrayResult(res);
    }
    else if (numArgs == 3) {
      QString name;
      auto str = tk_->variantToString(args[2]);
      if      (str == "-bg") name = "-background";
      else if (str == "-fg") name = "-foreground";
      else {
        static auto optNames = std::vector<QString>({
          "-background", "-foreground", "-selectbackground", "-selectforeground"});
        if (! tk_->lookupOptionName(optNames, args[2], name))
          return false;
      }

      std::vector<QVariant> res;

      auto addData = [&](const QString &name, const QString &type, const QString &ctype,
                         const QVariant &value) {
        res.push_back(name);
        res.push_back(type);
        res.push_back(ctype);
        res.push_back("");
        res.push_back(value);
      };

      if      (name == "-background")
        addData("-background"      , "background"      , "Background", item->background());
      else if (name == "-foreground")
        addData("-foreground"      , "foreground"      , "Foreground", item->foreground());
      else if (name == "-selectbackground")
        addData("-selectbackground", "selectBackground", "Foreground", item->selectBackground());
      else if (name == "-selectforeground")
        addData("-selectforeground", "selectForeground", "Background", item->selectForeground());

      tk_->setVariantArrayResult(res);
    }
    else {
      uint i = 2;

      while (i < numArgs) {
        QString name;
        auto str = tk_->variantToString(args[i]);
        if      (str == "-bg") name = "-background";
        else if (str == "-fg") name = "-foreground";
        else {
          static auto optNames = std::vector<QString>({
            "-background", "-foreground", "-selectbackground", "-selectforeground"});
          if (! tk_->lookupOptionName(optNames, args[i], name))
            return false;
        }
        ++i;

        if (i >= numArgs)
          return tk_->throwError(tk_->msg() + "Missing value for \"" + args[i] + "\"");

        if      (name == "-background") {
          QColor c;
          if (! tk_->variantToQColor(args[i], c))
            return tk_->invalidQColor(args[i]);
          ++i;

          item->setBackground(c);
        }
        else if (name == "-foreground") {
          QColor c;
          if (! tk_->variantToQColor(args[i], c))
            return tk_->invalidQColor(args[i]);
          ++i;

          item->setForeground(c);
        }
        else if (name == "-selectbackground") {
          QColor c;
          if (! tk_->variantToQColor(args[i], c))
            return tk_->invalidQColor(args[i]);
          ++i;

          item->setSelectBackground(c);
        }
        else if (name == "-selectforeground") {
          QColor c;
          if (! tk_->variantToQColor(args[i], c))
            return tk_->invalidQColor(args[i]);
          ++i;

          item->setSelectForeground(c);
        }
      }
    }
  }
  else if (option == "nearest") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " nearest y");

    long y;
    if (! tk_->variantToInt(args[1], y))
      return tk_->invalidInteger(args[1]);

    int d   = 0;
    int ind = -1;

    for (int i = 0; i < qlist_->count(); ++i) {
      auto *item = qlist_->item(i);

      auto r = qlist_->visualItemRect(item);

      int d1 = 0;

      if      (y <= r.top())
        d1 = r.top() - y;
      else if (y >= r.bottom())
        d1 = y - r.bottom();

      if (d1 < d) {
        ind = i;
        d   = d1;
      }
    }

    tk_->setIntegerResult(ind);
  }
  else if (option == "scan") {
    if (numArgs != 4)
      return tk_->wrongNumArgs(getFullName() + " scan mark|dragto x y");

    static auto optNames = std::vector<QString>({ "dragto", "mark" });

    QString opt;
    if (! tk_->lookupOptionName(optNames, args[1], opt))
      return false;

    long x, y;
    if (! tk_->variantToInt(args[2], x))
      return tk_->invalidInteger(args[2]);
    if (! tk_->variantToInt(args[3], y))
      return tk_->invalidInteger(args[3]);

    tk_->TODO(args);
  }
  else if (option == "see") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " see index");

    IndexData indexData;
    indexData.rangeCheck = false;
    if (! varToIndexData(args[1], indexData))
      return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[1] + "\": must be "
                             "active, anchor, end, @x,y, or an index");

    auto *item = indexDataToItem(indexData);

    if (item)
      qlist_->scrollToItem(item);
  }
  else if (option == "selection") {
    if (numArgs < 3 || numArgs > 4)
      return tk_->wrongNumArgs(getFullName() + " selection option index ?index?");

    static auto optNames = std::vector<QString>({ "anchor", "clear", "includes", "set" });

    QString opt;
    if (! tk_->lookupOptionName(optNames, args[1], opt))
      return false;

    if      (opt == "anchor") {
      if (numArgs > 4)
        return tk_->wrongNumArgs(getFullName() + " selection anchor index ?index?");

      int index1, index2;
      if (! varToIndex(args[2], index1, IndexPosition::NONE, /*rangeCheck*/false))
        return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[2] + "\": must be "
                               "active, anchor, end, @x,y, or an index");

      if (numArgs > 3) {
        if (! varToIndex(args[3], index2, IndexPosition::NONE, /*rangeCheck*/false))
          return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[3] + "\": must be "
                                 "active, anchor, end, @x,y, or an index");
      }
      else
        index2 = index1;

      setSelectionIndex(index1);
    }
    else if (opt == "clear") {
      if (numArgs > 4)
        return tk_->wrongNumArgs(getFullName() + " selection clear index ?index?");

      int startIndex;
      if (! varToIndex(args[2], startIndex, IndexPosition::NONE, /*rangeCheck*/false))
        return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[2] + "\": must be "
                               "active, anchor, end, @x,y, or an index");

      int endIndex = startIndex;
      if (numArgs == 4) {
        if (! varToIndex(args[3], endIndex, IndexPosition::NONE, /*rangeCheck*/false))
          return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[3] + "\": must be "
                                 "active, anchor, end, @x,y, or an index");
      }

      for (int i = startIndex; i <= endIndex; ++i) {
        if (i >= 0 && i < qlist_->count())
          qlist_->setCurrentItem(qlist_->item(i), QItemSelectionModel::Deselect);
      }
    }
    else if (opt == "includes") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getFullName() + " selection includes index");

      int ind;
      if (! varToIndex(args[2], ind, IndexPosition::NONE, /*rangeCheck*/false))
        return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[2] + "\": must be "
                               "active, anchor, end, @x,y, or an index");

      bool includes = false;

      if (ind >= 0 && ind < qlist_->count()) {
        auto *item = qlist_->item(ind);

        auto items = qlist_->selectedItems();

        for (auto *item1 : items)
          if (item1 == item)
            includes = true;
      }

      tk_->setBoolResult(includes);
    }
    else if (opt == "set") {
      if (numArgs > 4)
        return tk_->wrongNumArgs(getFullName() + " selection set index ?index?");

      int startIndex;
      if (! varToIndex(args[2], startIndex, IndexPosition::NONE, /*rangeCheck*/false))
        return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[2] + "\": must be "
                               "active, anchor, end, @x,y, or an index");

      int endIndex = startIndex;
      if (numArgs == 4) {
        if (! varToIndex(args[3], endIndex, IndexPosition::NONE, /*rangeCheck*/false))
          return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[3] + "\": must be "
                                 "active, anchor, end, @x,y, or an index");
      }

      for (int i = startIndex; i <= endIndex; ++i) {
        if (i >= 0 && i < qlist_->count())
          qlist_->setCurrentItem(qlist_->item(i), QItemSelectionModel::Select);
      }
    }
  }
  else if (option == "size") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getFullName() + " size");

    tk_->setIntegerResult(qlist_->count());
  }
  else if (option == "xview") {
    int step  = qlist_->horizontalScrollBar()->pageStep();
    int min   = qlist_->horizontalScrollBar()->minimum();
    int max   = qlist_->horizontalScrollBar()->maximum() - step;
    int value = qlist_->horizontalScrollBar()->value();

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

        qlist_->horizontalScrollBar()->setValue(min + (max - min)*x);
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

        qlist_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
      else {
        double x;
        if (! tk_->variantToReal(args[1], x))
          return tk_->invalidReal(args[1]);

        qlist_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
    }
  }
  else if (option == "yview") {
    int step  = qlist_->verticalScrollBar()->pageStep();
    int min   = qlist_->verticalScrollBar()->minimum();
    int max   = qlist_->verticalScrollBar()->maximum() - step;
    int value = qlist_->verticalScrollBar()->value();

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

        qlist_->verticalScrollBar()->setValue(min + (max - min)*y);
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

        qlist_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
      else {
        double y;
        if (! tk_->variantToReal(args[1], y))
          return tk_->invalidReal(args[1]);

        qlist_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
    }
  }
  else
    return CTkAppWidget::execDefaultOp(option, args);

  return true;
}

void
CTkAppListBox::
selectionSlot()
{
  CTkAppEventData eventData;

  eventData.type  = CTkAppEventType::Virtual;
  eventData.vtype = CTkAppVirtualEventType::ListboxSelect;

  triggerVirtualEvents(eventData);
}

void
CTkAppListBox::
vscrollSlot(int value)
{
  if (getYScrollCommand() != "") {
    int step = qlist_->verticalScrollBar()->pageStep();
    int min  = qlist_->verticalScrollBar()->minimum();
    int max  = qlist_->verticalScrollBar()->maximum() - step;

    double y1 = double(value        - min)/double(max - min);
    double y2 = double(value + step - min)/double(max - min);

    auto cmd = getYScrollCommand() + " " + QString::number(y1) + " " + QString::number(y2);

    tk_->eval(cmd);
  }
}

void
CTkAppListBox::
hscrollSlot(int value)
{
  if (getXScrollCommand() != "") {
    int step = qlist_->horizontalScrollBar()->pageStep();
    int min  = qlist_->horizontalScrollBar()->minimum();
    int max  = qlist_->horizontalScrollBar()->maximum();

    double x1 = double(value        - min)/double(max - min);
    double x2 = double(value + step - min)/double(max - min);

    auto cmd = getXScrollCommand() + " " + QString::number(x1) + " " + QString::number(x2);

    tk_->eval(cmd);
  }
}

void
CTkAppListBox::
updateFromVar()
{
  if (varName() != "" && tk_->hasGlobalVar(varName())) {
    std::vector<QString> strs;

    if (! tk_->getStringArrayGlobalVar(varName(), strs))
      return;

    while (qlist_->count() > int(strs.size())) {
      auto *item = qlist_->takeItem(qlist_->count() - 1);

      delete item;
    }

    while (qlist_->count() < int(strs.size()))
      qlist_->addItem(new CTkAppListItem(""));

    int i = 0;

    for (const auto &str : strs) {
      auto *item = dynamic_cast<CTkAppListItem *>(qlist_->item(i));

      item->setText(str);

      ++i;
    }
  }
}

//---

CTkAppListBoxWidget::
CTkAppListBoxWidget(CTkAppListBox *listbox) :
 QListWidget(listbox->getParent() ? listbox->getParent()->getQWidget() : nullptr),
 listbox_(listbox)
{
  setObjectName("qlistbox");
}

void
CTkAppListBoxWidget::
setWidth(const OptReal &w)
{
  width_ = w;
}

void
CTkAppListBoxWidget::
setHeight(const OptReal &h)
{
  height_ = h;
}

QSize
CTkAppListBoxWidget::
sizeHint() const
{
  auto s = QListWidget::sizeHint();

  QFontMetrics fm(font());

  if (width_ && *width_ > 0)
    s.setWidth((*width_)*fm.horizontalAdvance("0"));

  if (height_ && *height_ > 0)
    s.setHeight((*height_)*fm.height());

  return s;
}

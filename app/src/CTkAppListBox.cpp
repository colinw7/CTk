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

  auto varToIndex = [&](const QVariant &var, int &ind, IndexPosition pos=IndexPosition::NONE,
                        bool rangeCheck=true) {
    auto str = tk_->variantToString(var);

    ind = -1;

    if (str == "active") {
      ind = activeIndex();
      return true;
    }

    if (str == "anchor") {
      ind = selectionIndex();
      return true;
    }

    if (str == "end") {
      if (rangeCheck && pos != IndexPosition::END) {
        if (qlist_->count() == 0)
          return false;
      }

      ind = qlist_->count() - 1;

      return true;
    }

    if (str.length() > 1 && str[0] == '@') {
      return false; // @x, y
    }

    long i;
    if (! CTkAppUtil::stringToInt(str, i))
      return false;

    if (rangeCheck) {
      if (i < 0 || i >= qlist_->count()) {
        if (i != 0 || pos != IndexPosition::END)
          return false;
      }
    }

    ind = int(i);

    return true;
  };

  auto varToItem = [&](const QVariant &var) {
    int i;
    if (! varToIndex(var, i))
      return static_cast<QListWidgetItem *>(nullptr);

    return qlist_->item(i);
  };

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

    auto *item = varToItem(args[1]);
    if (! item) return true;

    auto r = qlist_->visualItemRect(item);

    tk_->setIntegerArrayResult({{r.x(), r.y(), r.width(), r.height()}});
  }
  else if (option == "curselection") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getFullName() + " curselection");

    auto items = qlist_->selectedItems();

    std::vector<int> rows;

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

    int index;
    if (! varToIndex(args[1], index, IndexPosition::NONE, /*rangeCheck*/false))
      return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[1] + "\": must be "
                             "active, anchor, end, @x,y, or an index");

    tk_->setIntegerResult(index);
  }
  else if (option == "insert") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " insert index ?element ...?");

    int ind;
    if (! varToIndex(args[1], ind, IndexPosition::END))
      return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[1] + "\": must be "
                             "active, anchor, end, @x,y, or an index");

    uint i = 2;

    for ( ; i < numArgs; ++i) {
      auto str = tk_->variantToString(args[i]);

      qlist_->insertItem(ind++, str);
    }
  }
  else if (option == "itemcget") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getFullName() + " itemcget index option");

    int ind;
    if (! varToIndex(args[1], ind))
      return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[1] + "\": must be "
                             "active, anchor, end, @x,y, or an index");

    auto opt = tk_->variantToString(args[2]);

    if      (opt == "-background") {
      QColor c;
      if (! tk_->variantToQColor(args[2], c))
        return tk_->invalidQColor(args[2]);

      tk_->TODO(option + " " + opt);
    }
    else if (opt == "-foreground") {
      QColor c;
      if (! tk_->variantToQColor(args[2], c))
        return tk_->invalidQColor(args[2]);

      tk_->TODO(option + " " + opt);
    }
    else if (opt == "-selectbackground") {
      QColor c;
      if (! tk_->variantToQColor(args[2], c))
        return tk_->invalidQColor(args[2]);

      tk_->TODO(option + " " + opt);
    }
    else if (opt == "-selectforeground") {
      QColor c;
      if (! tk_->variantToQColor(args[2], c))
        return tk_->invalidQColor(args[2]);

      tk_->TODO(option + " " + opt);
    }
    else
      return false;
  }
  else if (option == "itemconfigure") {
    if (numArgs <= 2)
      return tk_->wrongNumArgs(getFullName() + " itemcget index option");

    int ind;
    if (! varToIndex(args[1], ind))
      return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[1] + "\": must be "
                             "active, anchor, end, @x,y, or an index");

    if      (numArgs == 2) {
      tk_->TODO(args);
    }
    else if (numArgs == 3) {
      tk_->TODO(tk_->msg() + option + " " + args[2]);
    }
    else {
      uint i = 2;

      for ( ; i < numArgs; ++i) {
        auto name = tk_->variantToString(args[i]);

        if      (name == "-background") {
          QColor c;
          if (i >= numArgs - 1 || ! tk_->variantToQColor(args[++i], c))
            return tk_->invalidQColor(args[i]);

          tk_->TODO(args);
        }
        else if (name == "-foreground") {
          QColor c;
          if (i >= numArgs - 1 || ! tk_->variantToQColor(args[++i], c))
            return tk_->invalidQColor(args[i]);

          tk_->TODO(args);
        }
        else if (name == "-selectbackground") {
          QColor c;
          if (i >= numArgs - 1 || ! tk_->variantToQColor(args[++i], c))
            return tk_->invalidQColor(args[i]);

          tk_->TODO(args);
        }
        else if (name == "-selectforeground") {
          QColor c;
          if (i >= numArgs - 1 || ! tk_->variantToQColor(args[++i], c))
            return tk_->invalidQColor(args[i]);

          tk_->TODO(args);
        }
        else
          return false;
      }
    }
  }
  else if (option == "nearest") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " nearest y");

    long y;
    if (! tk_->variantToInt(args[1], y))
      return false;

    tk_->TODO(args);
  }
  else if (option == "scan") {
    if (numArgs != 4)
      return tk_->wrongNumArgs(getFullName() + " scan mark|dragto x y");

    tk_->TODO(args);
  }
  else if (option == "see") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " see index");

    auto *item = varToItem(args[1]);
    if (! item) return true;

    qlist_->scrollToItem(item);
  }
  else if (option == "selection") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getFullName() + " selection option index ?index?");

    auto option = tk_->variantToString(args[1]);

    if      (option == "anchor") {
      int index;
      if (! varToIndex(args[2], index, IndexPosition::NONE, /*rangeCheck*/false))
        return tk_->throwError(tk_->msg() + "bad listbox index \"" + args[2] + "\": must be "
                               "active, anchor, end, @x,y, or an index");

      setSelectionIndex(index);
    }
    else if (option == "clear") {
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
    else if (option == "includes") {
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
    else if (option == "set") {
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
    else
      return false;
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
        if (! CTkAppUtil::stringToReal(opt, x))
          return tk_->throwError("unknown option \"" + opt + "\": must be moveto or scroll");

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
        if (! CTkAppUtil::stringToReal(opt, y))
          return tk_->throwError("unknown option \"" + opt + "\": must be moveto or scroll");

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

     qlist_->clear();

     for (const auto &str : strs) {
       auto *item = new QListWidgetItem(str);

       qlist_->addItem(item);
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

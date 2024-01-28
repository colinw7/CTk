#include <CTkAppText.h>
#include <CTkAppTextWidget.h>
#include <CTkAppRoot.h>
#include <CTkAppCommands.h>
#include <CTkAppImage.h>
#include <CTkAppPackLayout.h>
#include <CTkAppGridLayout.h>
#include <CTkAppPlaceLayout.h>
#include <CTkAppX11.h>

#include <CQUtil.h>
#include <CQStrParse.h>

#include <deque>

CTkAppText::
CTkAppText(CTkApp *tk, CTkAppWidget *parent, const QString &name) :
 CTkAppWidget(tk, parent, name)
{
  qtext_ = new CTkAppTextWidget(this);

  qtext_->verticalScrollBar  ()->setVisible(false);
  qtext_->horizontalScrollBar()->setVisible(false);

  setQWidget(qtext_);

  connectSlots(true);
}

void
CTkAppText::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, qtext_->verticalScrollBar(),
    SIGNAL(valueChanged(int)), this, SLOT(vscrollSlot(int)));
  CQUtil::connectDisconnect(b, qtext_->horizontalScrollBar(),
    SIGNAL(valueChanged(int)), this, SLOT(hscrollSlot(int)));
}

bool
CTkAppText::
execConfig(const QString &name, const QVariant &var)
{
  if      (name == "-autoseparators") {
    tk_->TODO(name);
  }
  else if (name == "-blockcursor") {
    tk_->TODO(name);
  }
  else if (name == "-command") {
    auto value = tk_->variantToString(var);

    setCommand(value);
  }
  else if (name == "-endline") {
    tk_->TODO(name);
  }
  else if (name == "-exportselection") {
    bool b;
    if (! tk_->variantToBool(var, b))
      return tk_->invalidBool(var);

    setExportSelection(b);
  }
  else if (name == "-height") {
    long h;
    if (! tk_->variantToInt(var, h))
      return tk_->invalidInteger(var);

    qtext_->setHeight(h);
  }
  else if (name == "-inactiveselectbackground") {
    QColor c;
    if (! tk_->variantToQColor(var, c))
      return tk_->invalidQColor(var);

    tk_->TODO(name);
  }
  else if (name == "-insertunfocused") {
    tk_->TODO(name);
  }
  else if (name == "-maxundo") {
    tk_->TODO(name);
  }
  else if (name == "-spacing1") {
    tk_->TODO(name);
  }
  else if (name == "-spacing2") {
    tk_->TODO(name);
  }
  else if (name == "-spacing3") {
    tk_->TODO(name);
  }
  else if (name == "-startline") {
    auto value = tk_->variantToString(var);

    setText(value);
  }
  else if (name == "-state") {
    auto value = tk_->variantToString(var);

    if (! setWidgetStateFromString(value))
      return tk_->throwError(tk_->msg() + "bad state \"" + value + "\": must be "
                             "active, disabled, or normal");
  }
  else if (name == "-tabs") {
    auto value = tk_->variantToString(var);

    std::vector<QString> strs;
    if (! tk_->splitList(value, strs))
      return tk_->throwError("Invalid tabs \"" + value + "\"");

    tk_->TODO(name);
  }
  else if (name == "-tabstyle") {
    tk_->TODO(name);
  }
  else if (name == "-undo") {
    tk_->TODO(name);
  }
  else if (name == "-width") {
    long w;
    if (! tk_->variantToInt(var, w))
      return tk_->invalidInteger(var);

    qtext_->setWidth(w);
  }
  else if (name == "-wrap") {
    auto value = tk_->variantToString(var);

    if      (value == "char") {
      qtext_->setLineWrapMode(QTextEdit::WidgetWidth);
      qtext_->setWordWrapMode(QTextOption::WrapAnywhere);
    }
    else if (value == "none") {
      qtext_->setLineWrapMode(QTextEdit::NoWrap);
    }
    else if (value == "word") {
      qtext_->setLineWrapMode(QTextEdit::WidgetWidth);
      qtext_->setWordWrapMode(QTextOption::WordWrap);
    }
    else
      return false;
  }
  else
    return CTkAppWidget::execConfig(name, var);

  return true;
}

bool
CTkAppText::
execOp(const Args &args)
{
  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getFullName() + " option ?arg ...?");

  static auto optionNames = std::vector<QString>({
    "bbox", "cget", "compare", "configure", "count", "debug", "delete",
    "dlineinfo", "dump", "edit", "get", "image", "index", "insert",
    "mark", "peer", "pendingsync", "replace", "scan", "search",
    "see", "sync", "tag", "window", "xview", "yview"});

  QString option;
  if (! tk_->lookupOptionName(optionNames, args[0], option))
    return false;

  if      (option == "bbox") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " bbox index");

    tk_->TODO(args);
  }
  else if (option == "compare") {
    if (numArgs != 4)
      return tk_->wrongNumArgs(getFullName() + " compare index1 op index2");

    TextInd ind1;
    if (! stringToTextInd(args[1], ind1))
      return false;

    auto op = tk_->variantToString(args[2]);

    TextInd ind2;
    if (! stringToTextInd(args[3], ind2))
      return false;

    int cmp = TextInd::cmp(ind1, ind2);

    int res = 0;

    if (op == ">" ) res = (cmp >  0);
    if (op == ">=") res = (cmp >= 0);
    if (op == "<" ) res = (cmp <  0);
    if (op == "<=") res = (cmp <= 0);
    if (op == "==") res = (cmp == 0);
    if (op == "!=") res = (cmp != 0);

    tk_->setIntegerResult(res);
  }
  else if (option == "count") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getFullName() + " count ?-option value ...? index1 index2");

    tk_->TODO(args);
  }
  else if (option == "debug") {
    if (numArgs > 2)
      return tk_->wrongNumArgs(getFullName() + " debug boolean");

    tk_->TODO(args);
  }
  else if (option == "delete") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " delete index1 ?index2 ...?");

    std::vector<TextInd> inds;

    for (uint i = 1; i < numArgs; ++i) {
      TextInd ind;
      if (! stringToTextInd(args[i], ind))
        return false;

      inds.push_back(ind);
    }

    if (inds.size() & 1)
      inds.push_back(TextInd::end());

    TextIndRanges indRanges;

    uint i = 0;

    for ( ; i < inds.size(); i += 2)
      indRanges.push_back(TextIndRange(inds[i], inds[i + 1]));

    for (const auto &indRange : indRanges) {
      auto indRange1 = remapIndRange(indRange);

      qtext_->deleteText(indRange1.start.lineNum, indRange1.start.charNum,
                         indRange1.end  .lineNum, indRange1.end  .charNum);
    }
  }
  else if (option == "dlineinfo") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " dlineinfo index");

    tk_->TODO(args);
  }
  else if (option == "dump") {
    if (numArgs < 1)
      return tk_->wrongNumArgs(getFullName() +
        " dump ?-all -image -text -mark -tag -window? ?-command script? index ?index2?");

    tk_->TODO(args);
  }
  else if (option == "edit") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " edit option ?arg ...?");

    tk_->TODO(args);
  }
  else if (option == "get") {
    uint i = 1;

    if (args[i] == "-displaychars")
      ++i;

    if (args[i] == "--")
      ++i;

    if (i >= numArgs - 1)
      return tk_->wrongNumArgs(getFullName() +
               " get ?-displaychars? ?--? index1 ?index2 ...?");

    std::vector<QString> strs;

    for ( ; i < numArgs; ++i) {
      TextInd ind1;
      if (! stringToTextInd(args[i], ind1))
        return false;
      ind1 = remapInd(ind1);

      auto cursor = qtext_->textCursor();
      setCurrentInd(cursor, ind1);

      if (i < numArgs - 1) {
        ++i;

        TextInd ind2;
        if (! stringToTextInd(args[i], ind2))
          return false;
        ind2 = remapInd(ind2);

        setCurrentInd(cursor, ind2, QTextCursor::KeepAnchor);
      }
      else
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);

      strs.push_back(cursor.selectedText());
    }

    tk_->setStringArrayResult(strs);
  }
  else if (option == "image") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " image option ?arg ...?");

    tk_->TODO(args);
  }
  else if (option == "index") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " index index");

    CQStrParse parse(tk_->variantToString(args[1]));

    parse.skipSpace();

    auto parseInd = [&](TextInd &ind) {
      if      (parse.isString("end")) {
        parse.skipLastString();

        ind.lineNum = TextInd::END;
        ind.charNum = TextInd::END;
      }
      else if (parse.isString("insert")) {
        parse.skipLastString();

        auto cursor = qtext_->textCursor();
        getCurrentInd(cursor, ind);
      }
      else if (parse.isDigit()) {
        if (! parse.readInteger(&ind.lineNum))
          return false;
      }
      else if (parse.isChar('@')) {
        parse.skipChar();

        int x;
        if (! parse.readInteger(&x))
          return false;

        if (! parse.isChar(','))
          return false;
        parse.skipChar();

        int y;
        if (! parse.readInteger(&y))
          return false;

        auto cursor = qtext_->cursorForPosition(QPoint(x, y));
        getCurrentInd(cursor, ind);
        return true;
      }
      else
        return false;

      if (parse.isChar('.')) {
        parse.skipChar();

        if      (parse.isString("end")) {
          parse.skipLastString();
          ind.charNum = TextInd::END;
        }
        else if (parse.isDigit()) {
          if (! parse.readInteger(&ind.charNum))
            return false;
        }
        else
          return false;
      }
      return true;
    };

    TextInd ind1;
    if (! parseInd(ind1))
      return false;

    ind1 = remapInd(ind1);

    parse.skipSpace();

    if (parse.isChar('+') || parse.isChar('-')) {
      auto sign = (parse.isChar('+') ? 1 : -1);

      parse.skipChar();
      parse.skipSpace();

      if (parse.isDigit()) {
        long n;
        if (! parse.readInteger(&n))
          return false;
        n *= sign;

        parse.skipSpace();

        if      (parse.isString("chars") || parse.isString("c")) {
          parse.skipLastString();

          auto cursor = qtext_->textCursor();
          setCurrentInd(cursor, ind1);
          if (n > 0)
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, n);
          else
            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, -n);
          getCurrentInd(cursor, ind1);
        }
        else if (parse.isString("lines") || parse.isString("l")) {
          parse.skipLastString();

          auto cursor = qtext_->textCursor();
          setCurrentInd(cursor, ind1);
          if (n > 0)
            cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, n);
          else
            cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, -n);
          getCurrentInd(cursor, ind1);
        }
        else
          return false;
      }
      else
        return false;

      parse.skipSpace();

      if      (parse.isString("linestart")) {
        parse.skipLastString();

        auto cursor = qtext_->textCursor();
        setCurrentInd(cursor, ind1);
        cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
        getCurrentInd(cursor, ind1);
      }
      else if (parse.isString("lineend")) {
        parse.skipLastString();

        auto cursor = qtext_->textCursor();
        setCurrentInd(cursor, ind1);
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
        getCurrentInd(cursor, ind1);
      }
      else if (parse.isString("wordstart")) {
        parse.skipLastString();

        auto cursor = qtext_->textCursor();
        setCurrentInd(cursor, ind1);
        cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
        getCurrentInd(cursor, ind1);
      }
      else if (parse.isString("wordend")) {
        parse.skipLastString();

        auto cursor = qtext_->textCursor();
        setCurrentInd(cursor, ind1);
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::MoveAnchor);
        getCurrentInd(cursor, ind1);
      }
    }

    parse.skipSpace();
    if (! parse.eof())
      return false;

    tk_->setStringResult(ind1.toString());
  }
  else if (option == "insert") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getFullName() +
               " insert index chars ?tagList chars tagList ...?");

    TextInd ind;
    if (! stringToTextInd(args[1], ind))
      return false;
    ind = remapInd(ind);

    setCurrentInd(ind);

    qtext_->insertPlainText(tk_->variantToString(args[2]));
  }
  else if (option == "mark") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " mark option ?arg ...?");

    auto opt = tk_->variantToString(args[1]);

    if      (opt == "gravity") {
      if (numArgs == 3 || numArgs == 4) {
        auto mark = tk_->variantToString(args[2]);

        if (numArgs == 3) {
          QString gravity;
          if (! getMarkGravity(mark, gravity))
            return tk_->throwError("there is no mark named \"" + mark + "\"");

          tk_->setStringResult(gravity);
        }
        else {
          auto gravity = tk_->variantToString(args[3]);

          if (gravity == "left" || gravity == "right") {
            if (! setMarkGravity(mark, gravity))
              return tk_->throwError("there is no mark named \"" + mark + "\"");
          }
          else
            return tk_->throwError("ad mark gravity \"" + gravity + "\": must be left or right");
        }
     }
      else
        return tk_->wrongNumArgs(getFullName() + " mark gravity markName ?gravity?");
    }
    else if (opt == "names") {
      std::vector<QString> names;
      getMarkNames(names);

      tk_->setStringArrayResult(names);
    }
    else if (opt == "next") {
      tk_->TODO(args);
    }
    else if (opt == "previous") {
      tk_->TODO(args);
    }
    else if (opt == "set") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " mark set markName index");

      auto mark = tk_->variantToString(args[2]);

      TextInd ind;
      if (! stringToTextInd(args[3], ind))
        return false;
      ind = remapInd(ind);

      setMark(mark, ind);
    }
    else if (opt == "unset") {
      tk_->TODO(args);
    }
    else
      return false;
  }
  else if (option == "peer") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " peer option ?arg ...?");

    tk_->TODO(args);
  }
  else if (option == "pendingsync") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getFullName() + " pendingsync");

    tk_->TODO(args);
  }
  else if (option == "replace") {
    if (numArgs < 4)
      return tk_->wrongNumArgs(getFullName() +
        " replace index1 index2 chars ?tagList chars tagList ...?");

    tk_->TODO(args);
  }
  else if (option == "scan") {
    if (numArgs < 4)
      return tk_->wrongNumArgs(getFullName() + " scan mark x y\" or \"" +
                               getName() + " scan dragto x y ?gain?");

    tk_->TODO(args);
  }
  else if (option == "search") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getFullName() +
               " search ?switches? pattern index ?stopIndex?");

    bool    forwards     = true;
    bool    regexp       = false;
    bool    nolinestop   = false;
    bool    nocase       = false;
    bool    all          = false;
    bool    overlap      = false;
    bool    strictlimits = false;
    bool    elide        = false;
    QString countVar;

    uint i = 1;

    for ( ; i < numArgs; ++i) {
      auto arg = tk_->variantToString(args[i]);

      if      (arg == "-forwards" || arg == "-forward")
        forwards = true;
      else if (arg == "-backwards")
        forwards = false;
      else if (arg == "-exact")
        regexp = false;
      else if (arg == "-regexp")
        regexp = true;
      else if (arg == "-nolinestop")
        nolinestop = true;
      else if (arg == "-nocase")
        nocase = true;
      else if (arg == "-count") {
        ++i;

        if (i < numArgs)
          countVar = tk_->variantToString(args[i]);
      }
      else if (arg == "-all")
        all = true;
      else if (arg == "-overlap")
        overlap = true;
      else if (arg == "-strictlimits")
        strictlimits = true;
      else if (arg == "-elide")
        elide = true;
      else if (arg == "--") {
        ++i;
        break;
      }
      else
        return false;
    }

    QString pattern;

    if (i < numArgs)
      pattern = tk_->variantToString(args[i++]);
    else
      return tk_->wrongNumArgs(getFullName() +
               " search ?switches? pattern index ?stopIndex?");

    TextInd startInd, endInd;

    if (i < numArgs) {
      if (! stringToTextInd(args[i], startInd))
        return false;

      ++i;
    }
    else
      return tk_->wrongNumArgs(getFullName() +
               " search ?switches? pattern index ?stopIndex?");

    if (i < numArgs) {
      if (! stringToTextInd(args[i], endInd))
        return false;

      ++i;
    }

#if 0
    std::cerr << "search";
    if (! forwards) std::cerr << " -backwards";
    std::cerr << (regexp ? " -regexp" : " -exact");
    if (nolinestop) std::cerr << " -nolinestop";
    if (nocase) std::cerr << " -nocase";
    if (countVar != "") std::cerr << " -count " << countVar.toStdString();
    if (all) std::cerr << " -all";
    if (overlap) std::cerr << " -overlap";
    if (strictlimits) std::cerr << " -strictlimits";
    if (elide) std::cerr << " -elide";
    std::cerr << " " << pattern.toStdString();
    std::cerr << " " << startInd;
    std::cerr << " " << endInd;
    std::cerr << "\n";
#endif

    if (nolinestop  ) tk_->TODO("-nolinestop");
    if (overlap     ) tk_->TODO("-overlap");
    if (strictlimits) tk_->TODO("-strictlimits");
    if (elide       ) tk_->TODO("-elide");

    setCurrentInd(startInd);

    std::vector<QString> startInds;
    std::vector<long>    counts;

    while (true) {
      bool rc;

      QTextDocument::FindFlags findFlags;

      if (! forwards)
        findFlags |= QTextDocument::FindBackward;

      if (nocase)
        findFlags |= QTextDocument::FindCaseSensitively;

      if (regexp)
        rc = qtext_->find(QRegExp(pattern));
      else
        rc = qtext_->find(pattern);

      if (! rc)
        break;

      auto cursor = qtext_->textCursor();

      int pos1 = cursor.selectionStart();
      int pos2 = cursor.selectionEnd();

      counts.push_back(pos2 - pos1);

      cursor.setPosition(pos1);
      TextInd ind;
      getCurrentInd(cursor, ind);

      startInds.push_back(ind.toString());

      cursor.setPosition(pos2);

      if (! all)
        break;
    }

    if (countVar != "")
      tk_->setIntegerArrayLocalVar(countVar, counts);

    tk_->setStringArrayResult(startInds);
  }
  else if (option == "see") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getFullName() + " see index");

    TextInd ind;
    if (! stringToTextInd(args[1], ind))
      return false;

    TextInd currentInd;
    getCurrentInd(currentInd);
    setCurrentInd(ind);
    qtext_->ensureCursorVisible();
    setCurrentInd(currentInd);

    tk_->TODO(args);
  }
  else if (option == "sync") {
    uint i = 1;

    if (i < numArgs && args[i] == "-command")
      ++i;

    if (i != numArgs)
      return tk_->wrongNumArgs(getFullName() + " sync ?-command command?");

    tk_->TODO(args);
  }
  else if (option == "tag") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " tag option ?arg ...?");

    static auto tagOptionNames = std::vector<QString>({
      "add", "bind", "cget", "configure", "delete", "lower", "names",
      "nextrange", "prevrange", "raise", "ranges", "remove"});

    QString tagOption;
    if (! tk_->lookupOptionName(tagOptionNames, args[1], tagOption))
      return false;

    if      (tagOption == "add") {
      if (numArgs < 4)
        return tk_->wrongNumArgs(getFullName() +
                 " tag add tagName index1 ?index2 index1 index2 ...?");

      auto tag = tk_->variantToString(args[2]);

      std::vector<TextInd> inds;

      uint i = 3;

      for ( ; i < numArgs; ++i) {
        TextInd ind;
        if (! stringToTextInd(args[i], ind))
          return false;
        inds.push_back(ind);
      }

      if (inds.size() & 1)
        inds.push_back(TextInd::end());

      TextIndRanges indRanges;

      i = 0;

      for ( ; i < inds.size(); i += 2)
        indRanges.push_back(TextIndRange(inds[i], inds[i + 1]));

      addTagRanges(tag, indRanges);
    }
    else if (tagOption == "bind") {
      tk_->TODO(args);
    }
    else if (tagOption == "cget") {
      tk_->TODO(args);
    }
    else if (tagOption == "configure") {
      if (numArgs < 3)
        return tk_->wrongNumArgs(getFullName() +
          "tag configure tagName ?-option? ?value? ?-option value ...?");

      auto tag = tk_->variantToString(args[2]);

      auto &tagData = this->tagData(tag);

      if (numArgs == 3)
        tk_->TODO(args);
      else {
        uint i = 3;

        QVariant var1;
        QString  value1;

        auto nextI = [&]() {
          ++i; if (i >= numArgs) return false;
          var1 = args[i];
          value1 = tk_->variantToString(var1);
          return true;
        };

        for ( ; i < numArgs; ++i) {
          auto arg1 = tk_->variantToString(args[i]);

          if      (arg1 == "-background") {
            if (! nextI()) return false;
            QColor c;
            if (! tk_->variantToQColor(var1, c))
              return tk_->invalidQColor(var1);

            tagData.background = c;
          }
          else if (arg1 == "-foreground") {
            if (! nextI()) return false;
            QColor c;
            if (! tk_->variantToQColor(var1, c))
              return tk_->invalidQColor(var1);

            tagData.foreground = c;
          }
          else if (arg1 == "-font") {
            if (! nextI()) return false;

            if (value1 == "")
              return tk_->throwError("font \"" + value1 + "\" doesn't exist");

            tagData.font = tk_->getQFont(value1);
          }
          else if (arg1 == "-borderwidth") {
            if (! nextI()) return false;
            CTkAppDistance w;
            if (! tk_->variantToDistance(var1, w))
              return tk_->invalidDistance(var1);

            tagData.borderWidth = w.rvalue;
          }
          else if (arg1 == "-lmargin1") {
            if (! nextI()) return false;
            tk_->TODO(args);
          }
          else if (arg1 == "-offset") {
            if (! nextI()) return false;
            tk_->TODO(args);
          }
          else if (arg1 == "-relief") {
            if (! nextI()) return false;

            Relief relief { Relief::NONE };
            if (! CTkAppUtil::variantToRelief(tk_, var1, relief))
              return tk_->throwError(tk_->msg() + "bad relief \"" + var1 + "\": must be "
                                     "flat, groove, raised, ridge, solid, or sunken");

            tk_->TODO(args);
          }
          else if (arg1 == "-underline") {
            if (! nextI()) return false;
            bool b;
            if (! tk_->variantToBool(var1, b))
              return tk_->invalidBool(var1);

            tagData.underline = b;
          }
          else {
            return tk_->throwError("Invalid config option \"" + arg1 + "\"");
          }
        }

        applyTag(tagData);
      }
    }
    else if (tagOption == "delete") {
      if (numArgs < 2)
        return tk_->wrongNumArgs(getFullName() + " tag delete tagName ?tagName ...?");

      uint i = 2;

      for ( ; i < numArgs; ++i) {
        auto tag = tk_->variantToString(args[i]);

        deleteTag(tag);
      }
    }
    else if (tagOption == "lower") {
      if (numArgs != 3 && numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " tag lower tagName ?aboveThis?");

      auto tag = tk_->variantToString(args[2]);

      QString aboveTag;

      if (numArgs == 4)
        aboveTag = tk_->variantToString(args[3]);

      lowerTag(tag, aboveTag);
    }
    else if (tagOption == "names") {
      std::vector<QString> names;
      getTagNames(names);

      tk_->setStringArrayResult(names);
    }
    else if (tagOption == "nextrange") {
      tk_->TODO(args);
    }
    else if (tagOption == "prevrange") {
      tk_->TODO(args);
    }
    else if (tagOption == "raise") {
      if (numArgs != 3 && numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " tag raise tagName ?aboveThis?");

      auto tag = tk_->variantToString(args[2]);

      QString aboveTag;

      if (numArgs == 4)
        aboveTag = tk_->variantToString(args[3]);

      raiseTag(tag, aboveTag);
    }
    else if (tagOption == "ranges") {
      if (numArgs != 3)
        return tk_->wrongNumArgs(getFullName() + " tag ranges tagName");

      auto tag = tk_->variantToString(args[2]);

      TextIndRanges indRanges;

      getTagRanges(tag, indRanges);

      std::vector<QString> strs;

      for (const auto &indRange : indRanges) {
        auto indRange1 = remapIndRange(indRange);

        strs.push_back(indRange1.toString());
      }

      tk_->setStringArrayResult(strs);
    }
    else if (tagOption == "remove") {
      if (numArgs < 4)
        return tk_->wrongNumArgs(getFullName() +
          "tag remove tagName index1 ?index2 index1 index2 ...?");

      auto tag = tk_->variantToString(args[2]);

      tagRemove(tag); // remove tag from chars between index pairs
    }
  }
  else if (option == "window") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getFullName() + " window option ?arg ...?");

    auto opt = tk_->variantToString(args[1]);

    if      (opt == "cget") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getFullName() + " window cget index option");

      tk_->TODO(args);
    }
    else if (opt == "configure" || opt == "config") {
      if (numArgs < 3)
        return tk_->wrongNumArgs(getFullName() + " window configure index ?-option value ...?");

      tk_->TODO(args);
    }
    else if (opt == "create") {
      if (numArgs < 3)
        return tk_->wrongNumArgs(getFullName() + " window create index ?-option value ...?");

      TextInd ind;
      if (! stringToTextInd(args[2], ind))
        return false;

      uint i = 3;

      for ( ; i < numArgs; ++i) {
        if      (args[i] == "-align") {
          tk_->TODO(args);
          ++i;
        }
        else if (args[i] == "-create") {
          tk_->TODO(args);
          ++i;
        }
        else if (args[i] == "-padx") {
          tk_->TODO(args);
          ++i;
        }
        else if (args[i] == "-pady") {
          tk_->TODO(args);
          ++i;
        }
        else if (args[i] == "-stretch") {
          tk_->TODO(args);
          ++i;
        }
        else if (args[i] == "-window") {
          if (i + 1 >= numArgs)
            return false;

          auto window = tk_->variantToString(args[++i]);

          auto *w = tk_->lookupWidgetByName(window);
          if (! w) return false;

          qtext_->setCurrentInd(ind.lineNum, ind.charNum);

          qtext_->insertWidget(w->getQWidget());
        }
        else
          return false;
      }
    }
    else if (opt == "names") {
      if (numArgs != 2)
        return tk_->wrongNumArgs(getFullName() + " window names");

      tk_->TODO(args);
    }
    else
      return false;
  }
  else if (option == "xview") {
    int step  = qtext_->horizontalScrollBar()->pageStep();
    int min   = qtext_->horizontalScrollBar()->minimum();
    int max   = qtext_->horizontalScrollBar()->maximum() - step;
    int value = qtext_->horizontalScrollBar()->value();

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

        qtext_->horizontalScrollBar()->setValue(min + (max - min)*x);
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

        qtext_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
      else {
        double x;
        if (! CTkAppUtil::stringToReal(opt, x))
          return tk_->throwError("unknown option \"" + opt + "\": must be moveto or scroll");

        qtext_->horizontalScrollBar()->setValue(min + (max - min)*x);
      }
    }
  }
  else if (option == "yview") {
    int step  = qtext_->verticalScrollBar()->pageStep();
    int min   = qtext_->verticalScrollBar()->minimum();
    int max   = qtext_->verticalScrollBar()->maximum() - step;
    int value = qtext_->verticalScrollBar()->value();

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

        qtext_->verticalScrollBar()->setValue(min + (max - min)*y);
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

        qtext_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
      else {
        double y;
        if (! CTkAppUtil::stringToReal(opt, y))
          return tk_->throwError("unknown option \"" + opt + "\": must be moveto or scroll");

        qtext_->verticalScrollBar()->setValue(min + (max - min)*y);
      }
    }
  }
  else
    return CTkAppWidget::execDefaultOp(option, args);

  return true;
}

void
CTkAppText::
setCurrentInd(const TextInd &ind, const QTextCursor::MoveMode &moveMode) const
{
  qtext_->setCurrentInd(ind.lineNum, ind.charNum, moveMode);
}

void
CTkAppText::
setCurrentInd(QTextCursor &cursor, const TextInd &ind,
              const QTextCursor::MoveMode &moveMode) const
{
  qtext_->setCurrentInd(cursor, ind.lineNum, ind.charNum, moveMode);
}

void
CTkAppText::
getCurrentInd(TextInd &ind) const
{
  auto cursor = qtext_->textCursor();
  getCurrentInd(cursor, ind);
}

void
CTkAppText::
getCurrentInd(QTextCursor &cursor, TextInd &ind) const
{
  ind.lineNum = cursor.blockNumber() + 1;
  int pos = cursor.position();
  cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
  ind.charNum = pos - cursor.position();
}

bool
CTkAppText::
stringToTextInd(const QVariant &var, TextInd &ind) const
{
  auto str = tk_->variantToString(var);

  auto errorMsg = [&]() {
    std::cerr << "Invalid TextInd '" << str.toStdString() << "'\n";
    return false;
  };

  ind = TextInd::end();

  CQStrParse parse(str);

  parse.skipSpace();

  if      (parse.isString("end")) {
    parse.skipLastString();
  }
  else if (parse.isAlpha()) {
    QString mark;

    while (! parse.eof() && parse.isAlnum())
      mark += parse.getChar();

    if (! getMark(mark, ind))
      return errorMsg();
  }
  else if (parse.isDigit()) {
    int lineNum = ind.lineNum;
    int charNum = ind.charNum;

    if (! parse.readInteger(&lineNum))
      return errorMsg();

    if (parse.isChar('.')) {
      parse.skipChar();

      if (parse.isString("end")) {
        parse.skipLastString();
      }
      else {
        if (! parse.readInteger(&charNum))
          return errorMsg();
      }
    }

    ind = TextInd(lineNum, charNum);
  }

  parse.skipSpace();

  if (parse.isChar('+') || parse.isChar('-')) {
    auto sign = (parse.isChar('+') ? 1 : -1);

    parse.skipChar();
    parse.skipSpace();

    if (parse.isDigit()) {
      long n;
      if (! parse.readInteger(&n))
        return errorMsg();
      n *= sign;

      parse.skipSpace();

      if      (parse.isString("chars") || parse.isString("c")) {
        parse.skipLastString();

        auto cursor = qtext_->textCursor();
        setCurrentInd(cursor, ind);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, n);
        getCurrentInd(cursor, ind);
      }
      else if (parse.isString("lines") || parse.isString("l")) {
        parse.skipLastString();

        auto cursor = qtext_->textCursor();
        setCurrentInd(cursor, ind);
        cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, n);
        getCurrentInd(cursor, ind);
      }
      else
        return errorMsg();
    }
    else
      return errorMsg();
  }

  parse.skipSpace();

  if      (parse.isString("linestart")) {
    parse.skipLastString();

    auto cursor = qtext_->textCursor();
    setCurrentInd(cursor, ind);
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
    getCurrentInd(cursor, ind);
  }
  else if (parse.isString("lineend")) {
    parse.skipLastString();

    auto cursor = qtext_->textCursor();
    setCurrentInd(cursor, ind);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
    getCurrentInd(cursor, ind);
  }
  else if (parse.isString("wordstart")) {
    parse.skipLastString();

    auto cursor = qtext_->textCursor();
    setCurrentInd(cursor, ind);
    cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
    getCurrentInd(cursor, ind);
  }
  else if (parse.isString("wordend")) {
    parse.skipLastString();

    auto cursor = qtext_->textCursor();
    setCurrentInd(cursor, ind);
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::MoveAnchor);
    getCurrentInd(cursor, ind);
  }

  parse.skipSpace();

  if (! parse.eof())
    return errorMsg();

  return true;
}

void
CTkAppText::
setMark(const QString &mark, const TextInd &ind)
{
  marks_[mark] = MarkData(ind);
}

bool
CTkAppText::
getMark(const QString &mark, TextInd &ind) const
{
  auto pm = marks_.find(mark);
  if (pm == marks_.end()) return false;

  ind = (*pm).second.textInd;

  return true;
}

void
CTkAppText::
getMarkNames(std::vector<QString> &names) const
{
  for (const auto &pm : marks_)
    names.push_back(pm.first);
}

bool
CTkAppText::
getMarkGravity(const QString &name, QString &gravity) const
{
  auto pm = marks_.find(name);
  if (pm == marks_.end()) return false;

  gravity = (*pm).second.gravity;

  return true;
}

bool
CTkAppText::
setMarkGravity(const QString &name, const QString &gravity)
{
  auto pm = marks_.find(name);
  if (pm == marks_.end()) return false;

  (*pm).second.gravity = gravity;

  return true;
}

CTkAppText::TextIndRange
CTkAppText::
remapIndRange(const TextIndRange &ind) const
{
  TextIndRange ind1;
  ind1.start = remapInd(ind.start);
  ind1.end   = remapInd(ind.end);
  return ind1;
}

CTkAppText::TextInd
CTkAppText::
remapInd(const TextInd &ind) const
{
  TextInd ind1 = ind;

  auto cursor = qtext_->textCursor();

  if (ind.lineNum == CTkAppText::TextInd::END) {
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    ind1.lineNum = cursor.blockNumber() + 1;
    int pos1 = cursor.position();
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
    int pos2 = cursor.position();
    ind1.charNum = pos1 - pos2;
  }

  if (ind.charNum == CTkAppText::TextInd::END) {
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, ind1.lineNum - 1);
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
    int pos1 = cursor.position();
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
    int pos2 = cursor.position();
    ind1.charNum = pos2 - pos1;
  }

  return ind1;
}

void
CTkAppText::
setText(const QString &text)
{
  CTkAppWidget::setText(text);

  qtext_->setText(text);
}

void
CTkAppText::
vscrollSlot(int value)
{
  if (getYScrollCommand() != "") {
    int step = qtext_->verticalScrollBar()->pageStep();

    double y1 = double(value       )/1000.0;
    double y2 = double(value + step)/1000.0;

    auto cmd = getYScrollCommand() + " " + QString::number(y1) + " " + QString::number(y2);

    tk_->eval(cmd);
  }
}

void
CTkAppText::
hscrollSlot(int value)
{
  if (getXScrollCommand() != "") {
    int step = qtext_->horizontalScrollBar()->pageStep();

    double x1 = double(value       )/1000.0;
    double x2 = double(value + step)/1000.0;

    auto cmd = getXScrollCommand() + " " + QString::number(x1) + " " + QString::number(x2);

    tk_->eval(cmd);
  }
}

void
CTkAppText::
raiseTag(const QString & /*tag*/, const QString & /*aboveTag*/)
{
  // TODO
}

void
CTkAppText::
lowerTag(const QString & /*tag*/, const QString & /*aboveTag*/)
{
  // TODO
}

void
CTkAppText::
applyTag(const TagData &tagData)
{
  for (const auto &indRange : tagData.indRanges) {
    setCurrentInd(indRange.start);
    setCurrentInd(indRange.end, QTextCursor::KeepAnchor);

    QTextCharFormat fmt;

    fmt.setForeground   (tagData.foreground);
    fmt.setBackground   (tagData.background);
    fmt.setFont         (tagData.font);
//  fmt.setBorderWidth  (tagData.borderWidth);
    fmt.setFontUnderline(tagData.underline);

    auto cursor = qtext_->textCursor();
    cursor.setCharFormat(fmt);
  }
}

void
CTkAppText::
deleteTag(const QString &tag)
{
  auto pt = tags_.find(tag);

  if (pt != tags_.end())
    tags_.erase(pt);
}

void
CTkAppText::
addTagRanges(const QString &tag, const std::vector<TextIndRange> &indRanges)
{
  auto &tagData = this->tagData(tag);

  for (const auto &indRange : indRanges)
    tagData.indRanges.push_back(indRange);

  applyTag(tagData);
}

void
CTkAppText::
getTagRanges(const QString &tag, TextIndRanges &indRanges) const
{
  auto &tagData = const_cast<CTkAppText *>(this)->tagData(tag);

  indRanges = tagData.indRanges;
}

void
CTkAppText::
tagRemove(const QString & /*tag*/)
{
  // TODO
}

void
CTkAppText::
getTagNames(std::vector<QString> &names) const
{
  for (const auto &pt : tags_)
    names.push_back(pt.first);
}

CTkAppText::TagData &
CTkAppText::
tagData(const QString &name)
{
  auto pt = tags_.find(name);

  if (pt == tags_.end()) {
    TagData tagData;

    tagData.background = qtext_->palette().window().color();
    tagData.foreground = qtext_->palette().windowText().color();
    tagData.font       = qtext_->font();

    pt = tags_.insert(pt, Tags::value_type(name, tagData));
  }

  return (*pt).second;
}

//---

CTkAppTextWidget::
CTkAppTextWidget(CTkAppText *text) :
 CQTextWidget(text->getParent() ? text->getParent()->getQWidget() : nullptr), text_(text)
{
  setObjectName("qtext");

  setFocusPolicy(Qt::StrongFocus);
}

QSize
CTkAppTextWidget::
sizeHint() const
{
  auto s = CQTextWidget::sizeHint();

  QFontMetrics fm(font());

  if (width_ > 0)
    s.setWidth(width_*fm.horizontalAdvance("0"));

  if (height_ > 0)
    s.setHeight(height_*fm.height());

  return s;
}

#include <CTkAppImageCommand.h>
#include <CTkAppImage.h>
#include <CTkApp.h>

CTkAppImageCommand::
CTkAppImageCommand(CTkApp *app, const QString &name, const QString &type) :
 CTkAppCommand(app, name), type_(type)
{
}

CTkAppImageCommand::
~CTkAppImageCommand()
{
}

bool
CTkAppImageCommand::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs < 1)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  auto image = tk_->getImage(getName());
  assert(image);

  static auto photoNames  =
    std::vector<QString>({"blank", "cget", "configure", "copy", "data", "get", "put",
                          "read", "redither", "transparency", "write"});
  static auto bitmapNames = std::vector<QString>({"cget", "configure"});

  QString option;
  if (type_ == "photo") {
    if (! tk_->lookupOptionName(photoNames, args[0], option))
      return false;
  }
  else {
    if (! tk_->lookupOptionName(bitmapNames, args[0], option))
      return false;
  }

  if      (option == "blank") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getName() + " blank");

    image->clear();
  }
  else if (option == "cget") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " cget option");

    auto opt = tk_->variantToString(args[1]);

    if (type_ == "photo") {
      if      (opt == "-data") {
        tk_->setStringResult(image->getEncodedDataStr());
      }
      else if (opt == "-format") {
        tk_->setStringResult(image->format());
      }
      else if (opt == "-file") {
        tk_->setStringResult(image->filename());
      }
      else if (opt == "-gamma") {
        tk_->setRealResult(image->gamma());
      }
      else if (opt == "-height") {
        tk_->setIntegerResult(image->height());
      }
      else if (opt == "-palette") {
        int r, g, b;
        image->getColorBits(r, g, b);
        tk_->setIntegerArrayResult({r, g, b});
      }
      else if (opt == "-width") {
        tk_->setIntegerResult(image->width());
      }
      else
        return false;
    }
    else {
      if      (opt == "-background") {
        tk_->setStringResult(image->background().name());
      }
      else if (opt == "-data") {
        tk_->setStringResult(image->getBitmapStr());
      }
      else if (opt == "-file") {
        tk_->setStringResult(image->filename());
      }
      else if (opt == "-foreground") {
        tk_->setStringResult(image->foreground().name());
      }
      else if (opt == "-maskdata") {
        tk_->setStringResult(image->getMaskBitmapStr());
      }
      else if (opt == "-maskfile") {
        tk_->setStringResult(image->maskFilename());
      }
      else
        return false;
    }
  }
  else if (option == "configure") {
    if      (numArgs == 1) {
      QString res;

      auto addData = [&](const QString &name, const QString &def, const QString &value) {
        if (res != "")
          res += " ";

        res += "{" + name + " {} {} {" + def + "} {" + value + "}}";
      };

      if (type_ == "photo") {
        int r, g, b;
        image->getColorBits(r, g, b);

        addData("-data"   , "" , image->getEncodedDataStr());
        addData("-format" , "" , image->format());
        addData("-file"   , "" , image->filename());
        addData("-gamma"  , "1", QString::number(image->gamma()));
        addData("-height" , "0", QString::number(image->height()));
        addData("-palette", "" , QString("%1 %2 %3").arg(r).arg(g).arg(b));
        addData("-width " , "0", QString::number(image->width()));
      }
      else {
        addData("-background", "", image->background().name());
        addData("-data"      , "", image->getBitmapStr());
        addData("-file"      , "", image->filename());
        addData("-foreground", "", image->foreground().name());
        addData("-maskdata"  , "", image->getMaskBitmapStr());
        addData("-maskfile"  , "", image->maskFilename());
      }

      tk_->setStringResult(res);
    }
    else if (numArgs == 2) {
      auto name = tk_->variantToString(args[1]);

      QString res;

      if (type_ == "photo") {
        int r, g, b;
        image->getColorBits(r, g, b);

        if (name == "-data"   ) res = image->getEncodedDataStr();
        if (name == "-format" ) res = image->format();
        if (name == "-file"   ) res = image->filename();
        if (name == "-gamma"  ) res = QString::number(image->gamma());
        if (name == "-height" ) res = QString::number(image->height());
        if (name == "-palette") res = QString("%1 %2 %3").arg(r).arg(g).arg(b);
        if (name == "-width " ) res = QString::number(image->width());
      }
      else {
        if (name == "-background") res = image->background().name();
        if (name == "-data"      ) res = image->getBitmapStr();
        if (name == "-file"      ) res = image->filename();
        if (name == "-foreground") res = image->foreground().name();
        if (name == "-maskdata"  ) res = image->getMaskBitmapStr();
        if (name == "-maskfile"  ) res = image->maskFilename();
      }

      tk_->setStringResult(res);
    }
    else {
      uint i = 1;

      for ( ; i < numArgs; ++i) {
        auto name = tk_->variantToString(args[i++]);

        if (i >= numArgs)
          return tk_->throwError("value for \"" + name + "\" missing");

        auto value = tk_->variantToString(args[i]);

        if (type_ == "photo") {
          if      (name == "-data") {
            auto data = value.toStdString();
            image->loadEncodedData("", "", data);
          }
          else if (name == "-format") {
            image->setFormat(value);
          }
          else if (name == "-file") {
            image->setFilename(value);
          }
          else if (name == "-gamma") {
            double r;
            if (! tk_->variantToReal(args[i], r))
              return tk_->throwError(tk_->msg() + "expected real but got \"" + args[i] + "\"");
            image->setGamma(r);
          }
          else if (name == "-height") {
            long h;
            if (! tk_->variantToInt(args[i], h))
              return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i] + "\"");
            image->setHeight(h);
          }
          else if (name == "-palette") {
            std::vector<QString> strs;
            if (! tk_->splitList(value, strs))
              return false;
            if (strs.size() != 1 && strs.size() != 3)
              return false;
            long r, g, b;
            if (strs.size() == 1) {
              if (! CTkAppUtil::stringToInt(strs[0], g))
                return false;
              r = g;
              b = g;
            }
            else {
              if (! CTkAppUtil::stringToInt(strs[0], r) ||
                  ! CTkAppUtil::stringToInt(strs[1], g) ||
                  ! CTkAppUtil::stringToInt(strs[2], b))
                return false;
            }
            image->setColorBits(r, g, b);
          }
          else if (name == "-width") {
            long w;
            if (! tk_->variantToInt(args[i], w))
              return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i] + "\"");
            image->setWidth(w);
          }
          else
            return false;
        }
        else {
          if      (name == "-background") {
            QColor c;
            if (! CTkAppUtil::variantToQColor(tk_, args[i], c))
              return false;
            image->setBackground(c);
          }
          else if (name == "-data") {
            auto data = value.toStdString();
            if (! image->loadXBM("bitmap", data))
              return false;
          }
          else if (name == "-file") {
            image->setFilename(value);
          }
          else if (name == "-foreground") {
            QColor c;
            if (! CTkAppUtil::variantToQColor(tk_, args[i], c))
              return false;
            image->setForeground(c);
          }
          else if (name == "-maskdata") {
            auto data = value.toStdString();
            if (! image->loadMaskXBM("mask", data))
              return false;
          }
          else if (name == "-maskfile") {
            image->setMaskFilename(value);
          }
        }
      }
    }
  }
  else if (option == "copy") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getName() +
        " copy source-image ?-compositingrule rule? "
        "?-from x1 y1 x2 y2? ?-to x1 y1 x2 y2? ?-zoom x y? ?-subsample x y?");

    tk_->TODO(args);
  }
  else if (option == "data") {
    uint i = 1;

    while (i < numArgs) {
      auto name = tk_->variantToString(args[i++]);

      if      (name == "-background") {
        auto value = tk_->variantToString(args[i++]);

        tk_->TODO(args);
      }
      else if (name == "-format") {
        auto value = tk_->variantToString(args[i++]);

        tk_->TODO(args);
      }
      else if (name == "-from") {
        long x1, y1, x2, y2;
        if (! tk_->variantToInt(args[i++], x1))
          return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i - 1] + "\"");
        if (! tk_->variantToInt(args[i++], y1))
          return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i - 1] + "\"");
        if (! tk_->variantToInt(args[i++], x2))
          return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i - 1] + "\"");
        if (! tk_->variantToInt(args[i++], y2))
          return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i - 1] + "\"");
      }
      else if (name == "-grayscale") {
        tk_->TODO(args);
      }
      else if (name == "-metadata") {
        auto value = tk_->variantToString(args[i++]);

        tk_->TODO(args);
      }
      else
        return tk_->throwError("invalid option \"" + name + "\"");
    }

    tk_->setStringResult(image->dataString());
  }
  else if (option == "get") {
    if (numArgs != 3)
      return tk_->wrongNumArgs(getName() + " get x y");

    long x;
    if (! tk_->variantToInt(args[1], x))
      return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[1] + "\"");

    long y;
    if (! tk_->variantToInt(args[2], y))
      return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[2] + "\"");

    QColor c;
    if (! image->getPixel(x, y, c))
      return tk_->throwError("invalid position");

    tk_->setIntegerArrayResult({c.red(), c.green(), c.blue()});
  }
  else if (option == "put") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " put data ?-option value ...?");

    auto data = tk_->variantToString(args[1]);

    QColor c;
    if (image->isColor()) {
      (void) CTkAppUtil::variantToQColor(tk_, args[1], c);
    }
    else {
      bool b;
      tk_->variantToBool(data, b);
      c = (b ? Qt::white : Qt::black);
    }

    uint i = 2;

    for ( ; i < numArgs; ++i) {
      if      (args[i] == "-format") {
        ++i;
        tk_->TODO(args);
      }
      else if (args[i] == "-to") {
        ++i;

        long x1;
        if (i >= numArgs || ! tk_->variantToInt(args[i], x1))
          return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i] + "\"");

        ++i;

        long y1;
        if (i >= numArgs || ! tk_->variantToInt(args[i], y1))
          return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i] + "\"");

        if (i < numArgs) {
          ++i;

          long x2;
          if (i >= numArgs || ! tk_->variantToInt(args[i], x2))
            return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i] + "\"");

          ++i;

          long y2;
          if (i >= numArgs || ! tk_->variantToInt(args[i], y2))
            return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i] + "\"");

          image->setPixels(x1, y1, x2, y2, c);
        }
        else
          image->setPixel(x1, y1, c);
      }
    }
  }
  else if (option == "read") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " read fileName ?-option value ...?");

    auto filename = tk_->variantToString(args[1]);

    uint i = 2;

    for ( ; i < numArgs; ++i) {
      if      (args[i] == "-format") {
        ++i;
        tk_->TODO(args);
      }
      else if (args[i] == "-from") {
        long x1;
        if (i >= numArgs || ! tk_->variantToInt(args[i], x1))
          return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i] + "\"");

        ++i;

        long y1;
        if (i >= numArgs || ! tk_->variantToInt(args[i], y1))
          return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i] + "\"");

        ++i;

        long x2;
        if (i >= numArgs || ! tk_->variantToInt(args[i], x2))
          return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i] + "\"");

        ++i;

        long y2;
        if (i >= numArgs || ! tk_->variantToInt(args[i], y2))
          return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i] + "\"");
      }
      else if (args[i] == "-to") {
        long x1;
        if (i >= numArgs || ! tk_->variantToInt(args[i], x1))
          return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i] + "\"");

        ++i;

        long y1;
        if (i >= numArgs || ! tk_->variantToInt(args[i], y1))
          return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i] + "\"");

        ++i;
      }
      else if (args[i] == "-shrink") {
        tk_->TODO(args);
      }
    }

    if (! image->loadFile(filename))
      return false;
  }
  else if (option == "redither") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getName() + " redither");

    tk_->TODO(args);
  }
  else if (option == "transparency") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " transparency option ?arg ...?");

    if      (args[1] == "get") {
      if (numArgs != 4)
        return tk_->wrongNumArgs(getName() + " transparency get x y");

      long x;
      if (! tk_->variantToInt(args[1], x))
        return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[1] + "\"");

      long y;
      if (! tk_->variantToInt(args[2], y))
        return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[2] + "\"");

      QColor c;
      if (! image->getPixel(x, y, c))
        return tk_->throwError("invalid position");

      tk_->setBoolResult(c.alpha() < 255);
    }
    else if (args[1] == "set") {
      if (numArgs != 5)
        return tk_->wrongNumArgs(getName() + " transparency set x y boolean");

      long x;
      if (! tk_->variantToInt(args[1], x))
        return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[1] + "\"");

      long y;
      if (! tk_->variantToInt(args[2], y))
        return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[2] + "\"");

      bool b;
      if (! tk_->variantToBool(args[3], b))
        return tk_->throwError(tk_->msg() + "expected boolean value but got \"" + args[3] + "\"");

      QColor c;
      if (! image->getPixel(x, y, c))
        return tk_->throwError("invalid position");

      c.setAlpha(b ? 0 : 255);

      image->setPixel(x, y, c);
    }
    else
      return false;
  }
  else if (option == "write") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " write fileName ?-option value ...?");

    auto filename = tk_->variantToString(args[1]);

    uint i = 2;

    for ( ; i < numArgs; ++i) {
      if      (args[i] == "-background") {
        ++i;
        tk_->TODO(args);
      }
      else if (args[i] == "-format") {
        ++i;
        tk_->TODO(args);
      }
      else if (args[i] == "-from") {
        long x1;
        if (i >= numArgs || ! tk_->variantToInt(args[i], x1))
          return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i] + "\"");

        ++i;

        long y1;
        if (i >= numArgs || ! tk_->variantToInt(args[i], y1))
          return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i] + "\"");

        ++i;

        long x2;
        if (i >= numArgs || ! tk_->variantToInt(args[i], x2))
          return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i] + "\"");

        ++i;

        long y2;
        if (i >= numArgs || ! tk_->variantToInt(args[i], y2))
          return tk_->throwError(tk_->msg() + "expected integer but got \"" + args[i] + "\"");
      }
      else if (args[i] == "-grayscale") {
        tk_->TODO(args);
      }
    }

    tk_->TODO(args);
  }

  return true;
}

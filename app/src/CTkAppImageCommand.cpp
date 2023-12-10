#include <CTkAppImageCommand.h>
#include <CTkAppImage.h>
#include <CTkApp.h>

#include <QBuffer>

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

  uint i = 0;

  QString option;
  if (type_ == "photo") {
    if (! tk_->lookupOptionName(photoNames, args[i], option))
      return false;
  }
  else {
    if (! tk_->lookupOptionName(bitmapNames, args[i], option))
      return false;
  }

  //---

  auto encodePalette = [&](int r, int g, int b) {
    return QString("%1/%2/%3").arg(r).arg(g).arg(b);
  };

  //---

  ++i;

  if      (option == "blank") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getName() + " blank");

    image->clear();
  }
  else if (option == "cget") {
    if (numArgs != 2)
      return tk_->wrongNumArgs(getName() + " cget option");

    auto opt = tk_->variantToString(args[i]);

    if (type_ == "photo") {
      int r, g, b;
      image->getColorBits(r, g, b);

      if      (opt == "-data"   ) { tk_->setStringResult(image->getEncodedDataStr()); }
      else if (opt == "-format" ) { tk_->setStringResult(image->format()); }
      else if (opt == "-file"   ) { tk_->setStringResult(image->filename()); }
      else if (opt == "-gamma"  ) { tk_->setRealResult(image->gamma()); }
      else if (opt == "-height" ) { tk_->setIntegerResult(image->height()); }
      else if (opt == "-palette") { tk_->setStringResult(encodePalette(r, g, b)); }
      else if (opt == "-width"  ) { tk_->setIntegerResult(image->width()); }
      else return tk_->throwError("unknown option \"" + opt + "\"");
    }
    else {
      if      (opt == "-background") { tk_->setStringResult(image->background().name()); }
      else if (opt == "-data"      ) { tk_->setStringResult(image->getBitmapStr()); }
      else if (opt == "-file"      ) { tk_->setStringResult(image->filename()); }
      else if (opt == "-foreground") { tk_->setStringResult(image->foreground().name()); }
      else if (opt == "-maskdata"  ) { tk_->setStringResult(image->getMaskBitmapStr()); }
      else if (opt == "-maskfile"  ) { tk_->setStringResult(image->maskFilename()); }
      else return tk_->throwError("unknown option \"" + opt + "\"");
    }
  }
  else if (option == "configure") {
    if      (numArgs == 1) {
      std::vector<QVariant> res;

      auto addData = [&](const QString &name, const QString &def, const QString &value) {
        QVariantList vars;

        vars.push_back(name);
        vars.push_back("");
        vars.push_back("");
        vars.push_back(def);
        vars.push_back(value);

        res.push_back(vars);
      };

      if (type_ == "photo") {
        int r, g, b;
        image->getColorBits(r, g, b);

        addData("-data"   , "" , image->getEncodedDataStr());
        addData("-format" , "" , image->format());
        addData("-file"   , "" , image->filename());
        addData("-gamma"  , "1", QString::number(image->gamma()));
        addData("-height" , "0", QString::number(image->height()));
        addData("-palette", "" , encodePalette(r, g, b));
        addData("-width " , "0", QString::number(image->width()));
      }
      else {
        addData("-background", ""       , image->background().name());
        addData("-data"      , ""       , image->getBitmapStr());
        addData("-file"      , ""       , image->filename());
        addData("-foreground", "#000000", image->foreground().name());
        addData("-maskdata"  , ""       , image->getMaskBitmapStr());
        addData("-maskfile"  , ""       , image->maskFilename());
      }

      tk_->setVariantArrayResult(res);
    }
    else if (numArgs == 2) {
      QVariantList res;

      auto addData = [&](const QString &name, const QString &def, const QString &value) {
        res.push_back(name);
        res.push_back("");
        res.push_back("");
        res.push_back(def);
        res.push_back(value);
      };

      auto name = tk_->variantToString(args[1]);

      if (type_ == "photo") {
        int r, g, b;
        image->getColorBits(r, g, b);

        if      (name == "-data"   ) addData(name, "", image->getEncodedDataStr());
        else if (name == "-format" ) addData(name, "", image->format());
        else if (name == "-file"   ) addData(name, "", image->filename());
        else if (name == "-gamma"  ) addData(name, "", QString::number(image->gamma()));
        else if (name == "-height" ) addData(name, "", QString::number(image->height()));
        else if (name == "-palette") addData(name, "", encodePalette(r, g, b));
        else if (name == "-width " ) addData(name, "", QString::number(image->width()));
        else return tk_->throwError("unknown option \"" + name + "\"");
      }
      else {
        if      (name == "-background") addData(name, ""       , image->background().name());
        else if (name == "-data"      ) addData(name, ""       , image->getBitmapStr());
        else if (name == "-file"      ) addData(name, ""       , image->filename());
        else if (name == "-foreground") addData(name, "#000000", image->foreground().name());
        else if (name == "-maskdata"  ) addData(name, ""       , image->getMaskBitmapStr());
        else if (name == "-maskfile"  ) addData(name, ""       , image->maskFilename());
        else return tk_->throwError("unknown option \"" + name + "\"");
      }

      tk_->setResult(res);
    }
    else {
      for ( ; i < numArgs; ++i) {
        auto name = tk_->variantToString(args[i++]);

        if (i >= numArgs)
          return tk_->throwError("value for \"" + name + "\" missing");

        if (! processOption(tk_, image, type_, name, args[i]))
           return false;
      }
    }
  }
  else if (option == "copy") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() +
        " copy source-image ?-compositingrule rule? "
        "?-from x1 y1 x2 y2? ?-to x1 y1 x2 y2? ?-zoom x y? ?-subsample x y?");

    auto srcName = tk_->variantToString(args[i++]);

    auto srcImage = tk_->getImage(srcName);
    if (! srcImage)
      return tk_->throwError("image \"" + srcName + "\" doesn't exist or is not a photo image");

    auto fromRect = QRect(0, 0, srcImage->width(), srcImage->height());
    auto toRect   = QRect(0, 0, srcImage->width(), srcImage->height());

    QString rule { "overlay" };
    long    zoomX { -1 }, zoomY { -1 };
    long    sampleX { -1 }, sampleY { -1 };
    bool    shrink { false };

    while (i < numArgs) {
      static auto optionNames = std::vector<QString>({
        "-compositingrule", "-from", "-shrink", "-subsample", "-to", "-zoom" });

      QString option;
      if (! tk_->lookupOptionName(optionNames, args[i], option, /*quiet*/true))
        return tk_->throwError(tk_->msg() + "unrecognized option \"" + args[i] + "\": must be "
                 "-compositingrule, -from, -shrink, -subsample, -to, or -zoom");
      ++i;

      if      (option == "-compositingrule") {
        if (i >= numArgs)
          return tk_->throwError("the \"-compositingrule\" option requires a value");

        static auto ruleNames = std::vector<QString>({ "overlay", "set" });

        if (! tk_->lookupOptionName(ruleNames, args[i], rule, /*quiet*/true))
          return tk_->throwError(tk_->msg() + "bad compositing rule \"" + args[i] + "\": must be "
                   "overlay or set");
        ++i;
      }
      else if (option == "-from") {
        if (i >= numArgs)
          return tk_->throwError("missing value for \"" + option + "\"");

        long x1, y1, x2, y2;

        if (! tk_->variantToInt(args[i], x1))
          return tk_->invalidInteger(args[i]);
        ++i;

        if (i >= numArgs)
          return tk_->throwError("missing value for \"" + option + "\"");

        if (! tk_->variantToInt(args[i], y1))
          return tk_->invalidInteger(args[i]);
        ++i;

        if (i < numArgs && args[i].toString()[0] != "-") {
          if (! tk_->variantToInt(args[i], x2))
            return tk_->invalidInteger(args[i - 1]);
          ++i;

          if (i >= numArgs)
            return tk_->throwError("missing value for \"" + option + "\"");

          if (! tk_->variantToInt(args[i], y2))
            return tk_->invalidInteger(args[i - 1]);
          ++i;

          if (x1 > x2) std::swap(x1, x2);
          if (y1 > y2) std::swap(y1, y2);

          fromRect = QRect(x1, y1, x2 - x1, y2 - y1);
        }
        else
          fromRect = QRect(x1, y1, srcImage->width() - x1, srcImage->height() - y1);
      }
      else if (option == "-shrink") {
        shrink = true;
      }
      else if (option == "-to") {
        if (i >= numArgs)
          return tk_->throwError("missing value for \"" + option + "\"");

        long x1, y1, x2, y2;

        if (! tk_->variantToInt(args[i], x1))
          return tk_->invalidInteger(args[i]);
        ++i;

        if (i >= numArgs)
          return tk_->throwError("missing value for \"" + option + "\"");

        if (! tk_->variantToInt(args[i], y1))
          return tk_->invalidInteger(args[i]);
        ++i;

        if (i < numArgs && args[i].toString()[0] != "-") {
          if (! tk_->variantToInt(args[i], x2))
            return tk_->invalidInteger(args[i - 1]);
          ++i;

          if (i >= numArgs)
            return tk_->throwError("missing value for \"" + option + "\"");

          if (! tk_->variantToInt(args[i], y2))
            return tk_->invalidInteger(args[i - 1]);
          ++i;

          if (x1 > x2) std::swap(x1, x2);
          if (y1 > y2) std::swap(y1, y2);

          toRect = QRect(x1, y1, x2 - x1, y2 - y1);
        }
        else
          toRect = QRect(x1, y1, fromRect.width(), fromRect.height());
      }
      else if (option == "-zoom") {
        if (i >= numArgs)
          return tk_->throwError("missing value for \"" + option + "\"");

        if (! tk_->variantToInt(args[i], zoomX))
          return tk_->invalidInteger(args[i]);
        ++i;

        if (i < numArgs && args[i].toString()[0] != "-") {
          if (! tk_->variantToInt(args[i], zoomY))
            return tk_->invalidInteger(args[i]);
          ++i;
        }
        else
          zoomY = zoomX;
      }
      else if (option == "-subsample") {
        if (i >= numArgs)
          return tk_->throwError("missing value for \"" + option + "\"");

        if (! tk_->variantToInt(args[i], sampleX))
          return tk_->invalidInteger(args[i]);
        ++i;

        if (i < numArgs && args[i].toString()[0] != "-") {
          if (! tk_->variantToInt(args[i], sampleY))
            return tk_->invalidInteger(args[i]);
          ++i;
        }
        else
          sampleY = sampleX;
      }
    }

    if (shrink)
      tk_->TODO(args);

    image->resize(toRect.right() + 1, toRect.bottom() + 1);

    int y1 = toRect.left();

    for (int y = fromRect.top(); y <= fromRect.bottom(); ++y) {
      int x1 = toRect.top();

      for (int x = fromRect.left(); x <= fromRect.right(); ++x) {
        QColor c;
        (void) srcImage->getPixel(x, y, c);

        image->setPixel(x1, y1, c);

        ++x1;
      }

      ++y1;
    }
  }
  else if (option == "data") {
    QRect   fromRect;
    QString format, bg, metaData;
    bool    grayScale { false };

    while (i < numArgs) {
      static auto optionNames = std::vector<QString>({
        "-background", "-format", "-from", "-grayscale", "-metadata" });

      QString option;
      if (! tk_->lookupOptionName(optionNames, args[i], option, /*quiet*/true))
        return tk_->throwError(tk_->msg() + "unrecognized option \"" + args[i] + "\": must be "
                 "-background, -format, -from, -grayscale, or -metadata");
      ++i;

      if (i >= numArgs)
        return tk_->throwError("missing value");

      if      (option == "-background") {
        bg = tk_->variantToString(args[i++]);
      }
      else if (option == "-format") {
        auto str = tk_->variantToString(args[i++]);
        if (! lookupFormatName(tk_, str, format))
          return tk_->throwError("image file format \"" + str + "\" is not supported");
      }
      else if (option == "-from") {
        long x1, y1, x2, y2;

        if (! tk_->variantToInt(args[i], x1))
          return tk_->invalidInteger(args[i]);
        ++i;

        if (i >= numArgs)
          return tk_->throwError("missing value");

        if (! tk_->variantToInt(args[i], y1))
          return tk_->invalidInteger(args[i]);
        ++i;

        if (i >= numArgs)
          return tk_->throwError("missing value");

        if (! tk_->variantToInt(args[i], x2))
          return tk_->invalidInteger(args[i - 1]);
        ++i;

        if (i >= numArgs)
          return tk_->throwError("missing value");

        if (! tk_->variantToInt(args[i], y2))
          return tk_->invalidInteger(args[i - 1]);
        ++i;

        if (x1 > x2) std::swap(x1, x2);
        if (y1 > y2) std::swap(y1, y2);

        fromRect = QRect(x1, y1, x2 - x1, y2 - y1);
      }
      else if (option == "-grayscale") {
        if (! tk_->variantToBool(args[i++], grayScale))
          return tk_->invalidBool(args[i - 1]);
      }
      else if (option == "-metadata") {
        metaData = tk_->variantToString(args[i++]);

        tk_->TODO(args);
      }
    }

    QImage qimage1;

    if (! fromRect.isNull())
      qimage1 = image->getQImage().copy(fromRect);
    else
      qimage1 = image->getQImage();

    static auto formatNames = std::vector<QString>({
      "png", "jpg", "jpeg", "bmp", "xpm", "xbm", "gif", "ppm"});

    QString format1;
    if (tk_->lookupOptionName(formatNames, format, format1, /*quiet*/true)) {
      QString qformat1;
      if (! CTkAppImage::mapToQFormat(format1, qformat1))
        return false;

      QByteArray byteArray;
      QBuffer buffer(&byteArray);

      buffer.open(QIODevice::WriteOnly);
      auto rc = qimage1.save(&buffer, qformat1.toLatin1().constData());
      buffer.close();
      if (! rc) return false;

      tk_->setBytesResult(byteArray);
    }
    else
      tk_->setStringResult(CTkAppImage::dataString(tk_, qimage1));
  }
  else if (option == "get") {
    if (numArgs < 3)
      return tk_->wrongNumArgs(getName() + " get x y ?-withalpha?");

    long x;
    if (! tk_->variantToInt(args[i], x))
      return tk_->invalidInteger(args[i]);
    ++i;

    long y;
    if (! tk_->variantToInt(args[i], y))
      return tk_->invalidInteger(args[i]);
    ++i;

    bool retAlpha = false;

    if (i < numArgs) {
      if (args[i] == "-withalpha") {
        retAlpha = true;
        ++i;
      }
    }

    if (i != numArgs)
      return tk_->wrongNumArgs(getName() + " get x y");

    QColor c;
    if (! image->getPixel(x, y, c))
      return tk_->throwError("invalid position");

    if (retAlpha)
      tk_->setIntegerArrayResult({c.red(), c.green(), c.blue(), c.alpha()});
    else
      tk_->setIntegerArrayResult({c.red(), c.green(), c.blue()});
  }
  else if (option == "put") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " put data ?-option value ...?");

    //---

    using RowColors  = std::vector<QColor>;
    using ColorArray = std::vector<RowColors>;

    ColorArray colorArray;

    if (image->isColor()) {
      auto data = tk_->variantToString(args[i++]);

      std::vector<QString> strs;
      if (tk_->splitList(data, strs)) {
        for (const auto &str : strs) {
          RowColors rowColors;

          std::vector<QString> strs1;
          if (tk_->splitList(str, strs1)) {
            for (const auto &str1 : strs1) {
              QColor c1;
              if (! tk_->variantToQColor(str1, c1))
                return tk_->throwError("Invalid color \"" + str1 + "\"");

              rowColors.push_back(c1);
            }
          }
          else {
            QColor c;
            if (! tk_->variantToQColor(str, c))
              return tk_->throwError("Invalid color \"" + str + "\"");

            rowColors.push_back(c);
          }

          colorArray.push_back(rowColors);
        }
      }
      else {
        QColor c;
        if (! tk_->variantToQColor(data, c))
          return tk_->throwError("Invalid color \"" + data + "\"");

        RowColors rowColors;

        rowColors.push_back(c);

        colorArray.push_back(rowColors);
      }
    }
    else {
      auto data = tk_->variantToString(args[i++]);

      bool b;
      tk_->variantToBool(data, b);

      auto c = QColor(b ? Qt::white : Qt::black);

      RowColors rowColors;

      rowColors.push_back(c);

      colorArray.push_back(rowColors);
    }

    uint nr = colorArray.size();
    uint nc = 0;

    for (const auto &rowColors : colorArray) {
      nc = std::max(nc, uint(rowColors.size()));
    }

    if (nc == 0 || nr == 0)
      return tk_->throwError("zero data size");

    //---

    auto    toRect = QRect(0, 0, nc, nr);
    QString format;
    QString metaData;

    while (i < numArgs) {
      static auto optionNames = std::vector<QString>({
        "-format", "-metadata", "-to" });

      QString option;
      if (! tk_->lookupOptionName(optionNames, args[i], option, /*quiet*/true))
        return tk_->throwError(tk_->msg() + "unrecognized option \"" + args[i] + "\": must be "
                 "-format, -metadata, or -to");
      ++i;

      if (i >= numArgs)
        return tk_->wrongNumArgs(getName() + " put data ?-option value ...?");

      if      (option == "-format") {
        auto str = tk_->variantToString(args[i++]);
        if (! lookupFormatName(tk_, str, format))
          return tk_->throwError("image file format \"" + str + "\" is not supported");
      }
      else if (option == "-metadata") {
        metaData = tk_->variantToString(args[i++]);
      }
      else if (option == "-to") {
        long x1;
        if (! tk_->variantToInt(args[i], x1))
          return tk_->invalidInteger(args[i]);
        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs(getName() + " put data ?-option value ...?");

        long y1;
        if (! tk_->variantToInt(args[i], y1))
          return tk_->invalidInteger(args[i]);
        ++i;

        if (i < numArgs) {
          long x2;
          if (! tk_->variantToInt(args[i], x2))
            return tk_->invalidInteger(args[i]);
          ++i;

          if (i >= numArgs)
            return tk_->wrongNumArgs(getName() + " put data ?-option value ...?");

          long y2;
          if (! tk_->variantToInt(args[i], y2))
            return tk_->invalidInteger(args[i]);
          ++i;

          toRect = QRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
        }
        else {
          toRect = QRect(x1, y1, nc, nr);
        }
      }
    }

    int iwidth  = std::max(image->width (), toRect.right( ) + int(nc));
    int iheight = std::max(image->height(), toRect.bottom() + int(nr));

    if (iwidth != image->width() || iheight != image->height())
      image->resize(iwidth, iheight);

    for (int y = toRect.top(); y <= toRect.bottom(); ++y) {
      for (int x = toRect.left(); x <= toRect.right(); ++x) {
        auto c = colorArray[y % nr][x % nc];

        image->setPixel(x, y, c);
      }
    }
  }
  else if (option == "read") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " read fileName ?-option value ...?");

    auto    filename = tk_->variantToString(args[i++]);
    QString format;
    bool    shrink { false };

    for ( ; i < numArgs; ++i) {
      if      (args[i] == "-format") {
        auto str = tk_->variantToString(args[++i]);
        if (! lookupFormatName(tk_, str, format))
          return tk_->throwError("image file format \"" + str + "\" is not supported");
      }
      else if (args[i] == "-from") {
        long x1;
        if (i >= numArgs || ! tk_->variantToInt(args[i], x1))
          return tk_->invalidInteger(args[i]);

        ++i;

        long y1;
        if (i >= numArgs || ! tk_->variantToInt(args[i], y1))
          return tk_->invalidInteger(args[i]);

        ++i;

        long x2;
        if (i >= numArgs || ! tk_->variantToInt(args[i], x2))
          return tk_->invalidInteger(args[i]);

        ++i;

        long y2;
        if (i >= numArgs || ! tk_->variantToInt(args[i], y2))
          return tk_->invalidInteger(args[i]);
      }
      else if (args[i] == "-to") {
        long x1;
        if (i >= numArgs || ! tk_->variantToInt(args[i], x1))
          return tk_->invalidInteger(args[i]);

        ++i;

        long y1;
        if (i >= numArgs || ! tk_->variantToInt(args[i], y1))
          return tk_->invalidInteger(args[i]);

        ++i;
      }
      else if (args[i] == "-shrink") {
        shrink = true;
      }
    }

    if (shrink)
      tk_->TODO(args);

    if (! image->loadFile(filename))
      return tk_->throwError("couldn't open \"" + filename + "\": no such file or directory");
  }
  else if (option == "redither") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getName() + " redither");

    tk_->TODO(args);
  }
  else if (option == "transparency") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " transparency option ?arg ...?");

    static auto optNames = std::vector<QString>({"get", "set"});

    QString optName;
    if (! tk_->lookupOptionName(optNames, args[i], optName))
      return false;
    ++i;

    if      (optName == "get") {
      if (numArgs < 4)
        return tk_->wrongNumArgs(getName() + " transparency get x y ?-option?");

      long x;
      if (! tk_->variantToInt(args[i], x))
        return tk_->invalidInteger(args[i]);
      ++i;

      long y;
      if (! tk_->variantToInt(args[i], y))
        return tk_->invalidInteger(args[i]);
      ++i;

      bool retAlpha = false;

      if (i < numArgs) {
        if (args[i] == "-alpha") {
          retAlpha = true;
          ++i;
        }
      }

      if (i != numArgs)
        return tk_->wrongNumArgs(getName() + " transparency get x y ?-option?");

      QColor c;
      if (! image->getPixel(x, y, c))
        return tk_->throwError("invalid position");

      if (retAlpha)
        tk_->setIntegerResult(c.alpha());
      else
        tk_->setBoolResult(c.alpha() < 255);
    }
    else if (optName == "set") {
      if (numArgs < 5)
        return tk_->wrongNumArgs(getName() + " transparency set x y newVal ?-option?");

      long x;
      if (! tk_->variantToInt(args[i], x))
        return tk_->invalidInteger(args[i]);
      ++i;

      long y;
      if (! tk_->variantToInt(args[i], y))
        return tk_->invalidInteger(args[i]);
      ++i;

      auto newVal = args[i++];

      bool setAlpha = false;

      if (i < numArgs) {
        if (args[i] == "-alpha") {
          setAlpha = true;
          ++i;
        }
      }

      if (i != numArgs)
        return tk_->wrongNumArgs(getName() + " transparency set x y newVal ?-option?");

      long ialpha = 255;

      if (! setAlpha) {
        bool b;
        if (! tk_->variantToBool(newVal, b))
          return tk_->invalidBool(newVal);

        ialpha = (b ? 0 : 255);
      }
      else {
        if (! tk_->variantToInt(newVal, ialpha))
          return tk_->invalidInteger(newVal);

        if (ialpha < 0 || ialpha > 255)
          return tk_->wrongNumArgs("invalid alpha value");
      }

      QColor c;
      if (! image->getPixel(x, y, c))
        return tk_->throwError("invalid position");

      c.setAlpha(ialpha);

      image->setPixel(x, y, c);
    }
  }
  else if (option == "write") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " write fileName ?-option value ...?");

    auto filename = tk_->variantToString(args[i++]);

    QColor  bg;
    QString format;
    QRect   fromRect;
    bool    grayscale { false };

    for ( ; i < numArgs; ++i) {
      if      (args[i] == "-background") {
        if (! tk_->variantToQColor(args[++i], bg))
          return tk_->invalidQColor(args[i]);
      }
      else if (args[i] == "-format") {
        auto str = tk_->variantToString(args[++i]);
        if (! lookupFormatName(tk_, str, format))
          return tk_->throwError("image file format \"" + str + "\" is not supported");
      }
      else if (args[i] == "-from") {
        long x1;
        if (i >= numArgs || ! tk_->variantToInt(args[i], x1))
          return tk_->invalidInteger(args[i]);
        ++i;

        long y1;
        if (i >= numArgs || ! tk_->variantToInt(args[i], y1))
          return tk_->invalidInteger(args[i]);
        ++i;

        long x2;
        if (i >= numArgs || ! tk_->variantToInt(args[i], x2))
          return tk_->invalidInteger(args[i]);
        ++i;

        long y2;
        if (i >= numArgs || ! tk_->variantToInt(args[i], y2))
          return tk_->invalidInteger(args[i]);
        ++i;

        if (x1 > x2) std::swap(x1, x2);
        if (y1 > y2) std::swap(y1, y2);

        fromRect = QRect(x1, y1, x2 - x1, y2 - y1);
      }
      else if (args[i] == "-grayscale") {
        grayscale = true;
      }
    }

    if (grayscale)
      tk_->TODO(args);

    QImage qimage1;

    if (! fromRect.isNull())
      qimage1 = image->getQImage().copy(fromRect);
    else
      qimage1 = image->getQImage();

    if (! CTkAppImage::writeImage(filename, qimage1, format))
      return tk_->throwError("Failed to write image");
  }

  return true;
}

bool
CTkAppImageCommand::
processOption(CTkApp *tk, CTkAppImageRef &image, const QString &type,
              const QString &name, const QVariant &var)
{
  auto value = tk->variantToString(var);

  if (type == "photo") {
    if      (name == "-data") {
      if (! image->loadVarData(var, ""))
        return false;
    }
    else if (name == "-format") {
      auto str = tk->variantToString(value);
      QString format;
      if (! lookupFormatName(tk, str, format))
        return tk->throwError("image file format \"" + str + "\" is not supported");

      image->setFormat(format);
    }
    else if (name == "-file") {
      if (! image->loadFile(value))
        return tk->throwError("couldn't open \"" + value + "\": " "no such file or directory");
    }
    else if (name == "-gamma") {
      double r;
      if (! tk->variantToReal(var, r))
        return tk->invalidReal(var);
      image->setGamma(r);
    }
    else if (name == "-height") {
      long h;
      if (! tk->variantToInt(var, h))
        return tk->invalidInteger(var);

      image->setHeight(h);
    }
    else if (name == "-palette") {
      if (tk->variantIsValid(var)) {
        std::vector<QString> strs;
        if (! tk->splitList(value, strs))
          return tk->throwError("invalid palette \"" + value + "\"");

        if (strs.size() != 1 && strs.size() != 3)
          return tk->throwError("invalid palette \"" + value + "\"");

        long r, g, b;
        if (strs.size() == 1) {
          if (! tk->variantToInt(strs[0], g)) {
            auto strs1 = strs[0].split("/");

            if (strs1.size() == 3) {
              strs.clear();

              for (const auto &str1 : strs1)
                strs.push_back(str1);
            }
            else
              return tk->invalidInteger(strs[0]);
          }

          r = g;
          b = g;
        }

        if (strs.size() == 3) {
          if (! tk->variantToInt(strs[0], r))
            return tk->invalidInteger(strs[0]);
          if (! tk->variantToInt(strs[1], g))
            return tk->invalidInteger(strs[1]);
          if (! tk->variantToInt(strs[2], b))
            return tk->invalidInteger(strs[1]);
        }
        else
          return tk->invalidInteger(strs[0]);

        image->setColorBits(r, g, b);
      }
      else
        image->resetColorBits();
    }
    else if (name == "-width") {
      long w;
      if (! tk->variantToInt(var, w))
        return tk->invalidInteger(var);

      image->setWidth(w);
    }
    else
      return false;
  }
  else {
    if      (name == "-background") {
      QColor c;
      if (! tk->variantToQColor(var, c))
        return tk->invalidQColor(var);

      image->setBackground(c);
    }
    else if (name == "-data") {
      auto data = value.toStdString();

      if (! image->loadXBM("bitmap", data))
        return tk->throwError("failed to load bitmap file");
    }
    else if (name == "-file") {
      if (! image->loadFile(value))
        return tk->throwError("couldn't open \"" + value + "\": " "no such file or directory");
    }
    else if (name == "-foreground") {
      QColor c;
      if (! tk->variantToQColor(var, c))
        return tk->invalidQColor(var);

      image->setForeground(c);
    }
    else if (name == "-maskdata") {
      auto data = value.toStdString();
      if (! image->loadMaskXBM("mask", data))
        return tk->throwError("failed to load bitmap file");
    }
    else if (name == "-maskfile") {
      if (! image->loadMaskFile(value))
        return tk->throwError("couldn't open \"" + value + "\": " "no such file or directory");
    }
  }

  return true;
}

bool
CTkAppImageCommand::
lookupFormatName(CTkApp *tk, const QString &format, QString &format1)
{
  static auto formatNames = std::vector<QString>({
    "png", "jpg", "jpeg", "bmp", "xpm", "xbm", "gif", "ppm"});

  return tk->lookupOptionName(formatNames, format, format1, /*quiet*/true);
}

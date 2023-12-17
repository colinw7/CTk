#include <CTkAppImageCommand.h>
#include <CTkAppImage.h>
#include <CTkApp.h>

#include <QBuffer>
#include <QFileInfo>

#include <optional>

CTkAppImageCommand::
CTkAppImageCommand(CTkApp *tk, const QString &name, const QString &type) :
 CTkAppCommand(tk, name), type_(type)
{
  //std::cerr << "Create Image Command " << name_.toStdString() << "\n";

  tk_->addCommandTrace(name.toLatin1().constData(), this);
}

CTkAppImageCommand::
~CTkAppImageCommand()
{
  //std::cerr << "Destroy Image Command " << name_.toStdString() << "\n";

  tk_->removeCommandTrace(name_.toLatin1().constData(), this);
}

bool
CTkAppImageCommand::
run(const Args &args)
{
  using FormatData = CTkAppImage::FormatData;

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

  auto isOpt = [&](uint i) {
    return (i < numArgs && args[i].toString()[0] == "-");
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

    if (type_ == "photo") {
      static auto optionNames = std::vector<QString>({
        "-data", "-format", "-file", "-gamma", "-height", "-metadata", "-palette", "-width" });

      QString opt;
      if (! tk_->lookupOptionName(optionNames, args[i], opt, /*quiet*/true))
        return tk_->throwError(tk_->msg() + "unrecognized option \"" + args[i] + "\": must be " +
                               tk_->concatOptionNames(optionNames));

      int r, g, b;
      image->getColorBits(r, g, b);

      if      (opt == "-data"    ) { tk_->setStringResult(image->getEncodedDataStr()); }
      else if (opt == "-format"  ) { tk_->setStringResult(image->format()); }
      else if (opt == "-file"    ) { tk_->setStringResult(image->filename()); }
      else if (opt == "-gamma"   ) { tk_->setRealResult(image->gamma()); }
      else if (opt == "-height"  ) { tk_->setIntegerResult(image->height()); }
      else if (opt == "-metadata") { tk_->setStringResult(image->metaData()); }
      else if (opt == "-palette" ) { tk_->setStringResult(encodePalette(r, g, b)); }
      else if (opt == "-width"   ) { tk_->setIntegerResult(image->width()); }
    }
    else {
      static auto optionNames = std::vector<QString>({
        "-background", "-data", "-file", "-foreground", "-maskdata", "-maskfile" });

      QString opt;
      if (! tk_->lookupOptionName(optionNames, args[i], opt, /*quiet*/true))
        return tk_->throwError(tk_->msg() + "unrecognized option \"" + args[i] + "\": must be " +
                               tk_->concatOptionNames(optionNames));

      if      (opt == "-background") { tk_->setStringResult(image->background().name()); }
      else if (opt == "-data"      ) { tk_->setStringResult(image->getBitmapStr()); }
      else if (opt == "-file"      ) { tk_->setStringResult(image->filename()); }
      else if (opt == "-foreground") { tk_->setStringResult(image->foreground().name()); }
      else if (opt == "-maskdata"  ) { tk_->setStringResult(image->getMaskBitmapStr()); }
      else if (opt == "-maskfile"  ) { tk_->setStringResult(image->maskFilename()); }
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

        addData("-data"    , "" , image->getEncodedDataStr());
        addData("-format"  , "" , image->format());
        addData("-file"    , "" , image->filename());
        addData("-gamma"   , "1", QString::number(image->gamma()));
        addData("-height"  , "0", QString::number(image->height()));
        addData("-metadata", "" , image->metaData());
        addData("-palette" , "" , encodePalette(r, g, b));
        addData("-width "  , "0", QString::number(image->width()));
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

      if (type_ == "photo") {
        static auto optionNames = std::vector<QString>({
          "-data", "-format", "-file", "-gamma", "-height", "-metadata", "-palette", "width" });

        QString opt;
        if (! tk_->lookupOptionName(optionNames, args[i], opt, /*quiet*/true))
          return tk_->unknownOption(args[i]);

        int r, g, b;
        image->getColorBits(r, g, b);

        if      (opt == "-data"    ) addData(opt, "", image->getEncodedDataStr());
        else if (opt == "-format"  ) addData(opt, "", image->format());
        else if (opt == "-file"    ) addData(opt, "", image->filename());
        else if (opt == "-gamma"   ) addData(opt, "", QString::number(image->gamma()));
        else if (opt == "-height"  ) addData(opt, "", QString::number(image->height()));
        else if (opt == "-metadata") addData(opt, "", image->metaData());
        else if (opt == "-palette" ) addData(opt, "", encodePalette(r, g, b));
        else if (opt == "-width "  ) addData(opt, "", QString::number(image->width()));
      }
      else {
        static auto optionNames = std::vector<QString>({
          "-background", "-data", "-file", "-foreground", "-maskdata", "-maskfile" });

        QString opt;
        if (! tk_->lookupOptionName(optionNames, args[i], opt, /*quiet*/true))
          return tk_->unknownOption(args[i]);

        if      (opt == "-background") addData(opt, ""       , image->background().name());
        else if (opt == "-data"      ) addData(opt, ""       , image->getBitmapStr());
        else if (opt == "-file"      ) addData(opt, ""       , image->filename());
        else if (opt == "-foreground") addData(opt, "#000000", image->foreground().name());
        else if (opt == "-maskdata"  ) addData(opt, ""       , image->getMaskBitmapStr());
        else if (opt == "-maskfile"  ) addData(opt, ""       , image->maskFilename());
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
    if (! srcImage || srcImage->isColor() != image->isColor())
      return tk_->throwError("image \"" + srcName + "\" doesn't exist or is not a photo image");

    QRect fromRect;

    if (srcImage->isWidthSet() && srcImage->isHeightSet())
      fromRect = QRect(0, 0, srcImage->width(), srcImage->height());

    QRect toRect;

    if      (image->isWidthSet() && image->isHeightSet())
      toRect = QRect(0, 0, image->width(), image->height());
    else if (srcImage->isWidthSet() && srcImage->isHeightSet())
      toRect = QRect(0, 0, srcImage->width(), srcImage->height());

    QString rule { "overlay" };
    long    zoomX { -1 }, zoomY { -1 };
    long    sampleX { -1 }, sampleY { -1 };
    bool    shrink { false };
    bool    toRectSet { false }, fromRectSet { false };

    while (i < numArgs) {
      static auto optionNames = std::vector<QString>({
        "-compositingrule", "-from", "-shrink", "-subsample", "-to", "-zoom" });

      QString option;
      if (! tk_->lookupOptionName(optionNames, args[i], option, /*quiet*/true))
        return tk_->throwError(tk_->msg() + "unrecognized option \"" + args[i] + "\": must be " +
                               tk_->concatOptionNames(optionNames));
      ++i;

      if      (option == "-compositingrule") {
        if (i >= numArgs)
          return tk_->throwError("the \"-compositingrule\" option requires a value");

        static auto ruleNames = std::vector<QString>({ "overlay", "set" });

        if (! tk_->lookupOptionName(ruleNames, args[i], rule, /*quiet*/true))
          return tk_->throwError(tk_->msg() + "bad compositing rule \"" + args[i] + "\": must be " +
                                 tk_->concatOptionNames(ruleNames));
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

        if (i < numArgs && ! isOpt(i)) {
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
        else {
          if (srcImage->isWidthSet() && srcImage->isHeightSet())
            fromRect = QRect(x1, y1, srcImage->width() - x1, srcImage->height() - y1);
          else
            fromRect = QRect(x1, y1, 256, 256);
        }

        fromRectSet = true;
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

        if (i < numArgs && ! isOpt(i)) {
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

        toRectSet = true;
      }
      else if (option == "-zoom") {
        if (i >= numArgs)
          return tk_->throwError("missing value for \"" + option + "\"");

        if (! tk_->variantToInt(args[i], zoomX))
          return tk_->invalidInteger(args[i]);
        if (zoomX <= 0)
          return tk_->throwError(tk_->msg() + "invalid zoom value \"" + args[i] + "\"");
        ++i;

        if (i < numArgs && ! isOpt(i)) {
          if (! tk_->variantToInt(args[i], zoomY))
            return tk_->invalidInteger(args[i]);
          if (zoomY <= 0)
            return tk_->throwError(tk_->msg() + "invalid zoom value \"" + args[i] + "\"");
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

        if (i < numArgs && ! isOpt(i)) {
          if (! tk_->variantToInt(args[i], sampleY))
            return tk_->invalidInteger(args[i]);
          ++i;
        }
        else
          sampleY = sampleX;
      }
    }

    if (sampleX <= 0) sampleX = 1;
    if (sampleY <= 0) sampleY = 1;

    if      (shrink) {
      int w = 0, h = 0;

      if (! toRectSet) {
        if      (image->isWidthSet())
          w = image->width();
        else if (fromRectSet)
          w = fromRect.width();
        else if (srcImage->isWidthSet())
          w = srcImage->width();

        if      (image->isHeightSet())
          h = image->height();
        else if (fromRectSet)
          h = fromRect.height();
        else if (srcImage->isHeightSet())
          h = srcImage->height();

        toRect = QRect(0, 0, w/sampleX, h/sampleY);
      }
    }
    else if (zoomX > 0) {
      if (! toRectSet)
        toRect = QRect(0, 0, zoomX*fromRect.width(), zoomY*fromRect.height());
    }

    if (zoomX <= 0) zoomX = 1;
    if (zoomY <= 0) zoomY = 1;

    auto srcQImage = srcImage->getQImage().copy();

    image->resize(toRect.right() + 1, toRect.bottom() + 1);

    int y1 = fromRect.top();

    for (int y = toRect.top(); y <= toRect.bottom(); y += zoomY) {
      int x1 = fromRect.left();

      for (int x = toRect.left(); x <= toRect.right(); x += zoomX) {
        QColor c;
        if (x1 < srcQImage.width() && y1 < srcQImage.height())
          c = srcQImage.pixelColor(x1, y1);

        for (int iy1 = 0; iy1 < zoomY; ++iy1) {
          for (int ix1 = 0; ix1 < zoomX; ++ix1) {

            if (c.alpha() == 0 && rule == "overlay") // skip transparent
              continue;

            image->setPixel(x + ix1, y + iy1, c);
          }
        }

        x1 += sampleX;

        if (x1 > fromRect.right())
          x1 = fromRect.left();
      }

      y1 += sampleY;

      if (y1 > fromRect.bottom())
        y1 = fromRect.top();
    }
  }
  else if (option == "data") {
    QRect      fromRect;
    QPoint     fromPoint;
    FormatData formatData;
    QString    bg, metaData;
    bool       grayscale { false };
    bool       fromRectSet { false }, fromPointSet { false };

    while (i < numArgs) {
      static auto optionNames = std::vector<QString>({
        "-background", "-format", "-from", "-grayscale", "-metadata" });

      QString option;
      if (! tk_->lookupOptionName(optionNames, args[i], option, /*quiet*/true))
        return tk_->throwError(tk_->msg() + "unrecognized option \"" + args[i] + "\": must be " +
                               tk_->concatOptionNames(optionNames));
      ++i;

      if (i >= numArgs)
        return tk_->throwError("missing value");

      if      (option == "-background") {
        bg = tk_->variantToString(args[i++]);
      }
      else if (option == "-format") {
        auto str = tk_->variantToString(args[i]);
        if (! CTkAppImage::lookupFormatName(tk_, str, formatData))
          return tk_->throwError(tk_->msg() + "image string format \"" +
                                 args[i] + "\" is unknown");
        ++i;
      }
      else if (option == "-from") {
        long x1, y1, x2, y2;

        if (! tk_->variantToInt(args[i], x1))
          return tk_->invalidInteger(args[i]);
        if (x1 < 0)
          return tk_->throwError("value(s) for the -from option must be non-negative");
        ++i;

        if (i >= numArgs)
          return tk_->throwError("missing value");

        if (! tk_->variantToInt(args[i], y1))
          return tk_->invalidInteger(args[i]);
        if (y1 < 0)
          return tk_->throwError("value(s) for the -from option must be non-negative");
        ++i;

        if (i < numArgs && ! isOpt(i)) {
          if (! tk_->variantToInt(args[i], x2))
            return tk_->invalidInteger(args[i - 1]);
          if (x2 < 0)
            return tk_->throwError("value(s) for the -from option must be non-negative");
          ++i;

          if (i >= numArgs)
            return tk_->throwError("missing value");

          if (! tk_->variantToInt(args[i], y2))
            return tk_->invalidInteger(args[i - 1]);
          if (y2 < 0)
            return tk_->throwError("value(s) for the -from option must be non-negative");
          ++i;

          if (x1 > x2) std::swap(x1, x2);
          if (y1 > y2) std::swap(y1, y2);

          fromRect    = QRect(x1, y1, x2 - x1, y2 - y1);
          fromRectSet = true;
        }
        else {
          fromPoint    = QPoint(x1, y1);
          fromPointSet = true;
        }
      }
      else if (option == "-grayscale") {
        grayscale = true;
      }
      else if (option == "-metadata") {
        metaData = tk_->variantToString(args[i++]);
      }
    }

    if (grayscale)
      tk_->TODO(args);

    QImage qimage1;

    if      (fromRectSet) {
      if (fromRect.right() >= image->width() || fromRect.bottom() >= image->height())
        return tk_->throwError("coordinates for -from option extend outside image");

      qimage1 = image->getQImage().copy(fromRect);
    }
    else if (fromPointSet) {
      int x = fromPoint.x();
      int y = fromPoint.y();

      if (x >= image->width() || y >= image->height())
        return tk_->throwError("coordinates for -from option extend outside image");

      int w = image->getQImage().width ();
      int h = image->getQImage().height();

      qimage1 = image->getQImage().copy(x, y, w - x, h - y);
    }
    else
      qimage1 = image->getQImage();

    static auto formatNames = std::vector<QString>({
      "png", "jpg", "jpeg", "bmp", "xpm", "xbm", "gif", "ppm"});

    QString format1;
    if (tk_->lookupOptionName(formatNames, formatData.format, format1, /*quiet*/true)) {
      QString qformat1;
      if (! CTkAppImage::mapToQFormat(format1, qformat1))
        return false;

      QByteArray byteArray;
      QBuffer buffer(&byteArray);

      buffer.open(QIODevice::WriteOnly);
      auto rc = qimage1.save(&buffer, qformat1.toLatin1().constData());
      buffer.close();
      if (! rc)
        return tk_->throwError("failed to save image");

      tk_->setBytesResult(byteArray);
    }
    else {
      tk_->setStringResult(CTkAppImage::dataString(tk_, qimage1, formatData.colorformat));
    }
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
      auto arg = tk_->variantToString(args[i]);
      if (arg[0] != "-")
        return tk_->wrongNumArgs(getName() + " get x y ?-withalpha?");

      static auto retNames = std::vector<QString>({"-withalpha"});

      QString retName;
      if (! tk_->lookupOptionName(retNames, args[i], retName))
        return tk_->throwError(tk_->msg() + "unrecognized option \"" + args[i] + "\": must be " +
                               tk_->concatOptionNames(retNames));
      ++i;

      if (retName == "-withalpha")
        retAlpha = true;
    }

    if (i != numArgs)
      return tk_->wrongNumArgs(getName() + " get x y ?-withalpha?");

    QColor c;
    if (! image->getPixel(x, y, c))
      return tk_->throwError(getName() + " get: coordinates out of range");

    if (retAlpha)
      tk_->setIntegerArrayResult({c.red(), c.green(), c.blue(), c.alpha()});
    else
      tk_->setIntegerArrayResult({c.red(), c.green(), c.blue()});
  }
  else if (option == "put") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " put data ?-option value ...?");

    //---

    auto dataVar = args[i++];

    //---

    using OptString = std::optional<QString>;

    QRect      toRect;
    QPoint     toPoint;
    OptString  metaData;
    FormatData formatData;
    bool       toRectSet { false }, toPointSet { false };

    while (i < numArgs) {
      auto arg = tk_->variantToString(args[i]);
      if (arg[0] != "-")
        return tk_->wrongNumArgs(getName() + " put data ?-option value ...?");

      static auto optionNames = std::vector<QString>({
        "-format", "-metadata", "-to" });

      QString option;
      if (! tk_->lookupOptionName(optionNames, args[i], option, /*quiet*/true))
        return tk_->throwError(tk_->msg() + "unrecognized option \"" + args[i] + "\": must be " +
                               tk_->concatOptionNames(optionNames));
      ++i;

      if (i >= numArgs)
        return tk_->wrongNumArgs(getName() + " put data ?-option value ...?");

      if      (option == "-format") {
        auto str = tk_->variantToString(args[i]);
        if (! CTkAppImage::lookupFormatName(tk_, str, formatData))
          return tk_->throwError(tk_->msg() + "image format \"" +
                                 args[i] + "\" is not supported");
        ++i;
      }
      else if (option == "-metadata") {
        metaData = tk_->variantToString(args[i++]);
      }
      else if (option == "-to") {
        long x1, y1, x2, y2;

        if (! tk_->variantToInt(args[i], x1))
          return tk_->invalidInteger(args[i]);
        ++i;

        if (i >= numArgs)
          return tk_->wrongNumArgs(getName() + " put data ?-option value ...?");

        if (! tk_->variantToInt(args[i], y1))
          return tk_->invalidInteger(args[i]);
        ++i;

        if (i < numArgs && ! isOpt(i)) {
          if (! tk_->variantToInt(args[i], x2))
            return tk_->invalidInteger(args[i]);
          ++i;

          if (i >= numArgs)
            return tk_->wrongNumArgs(getName() + " put data ?-option value ...?");

          if (! tk_->variantToInt(args[i], y2))
            return tk_->invalidInteger(args[i]);
          ++i;

          toRect    = QRect(x1, y1, x2 - x1, y2 - y1);
          toRectSet = true;
        }
        else {
          toPoint    = QPoint(x1, y1);
          toPointSet = true;
        }
      }
    }

    //---

    if (formatData.format == "" && dataVar.type() != QVariant::ByteArray) {
      auto data = tk_->variantToString(dataVar);

      using RowColors  = std::vector<QColor>;
      using ColorArray = std::vector<RowColors>;

      ColorArray colorArray;

      if (image->isColor()) {
        std::vector<QString> strs;
        if (tk_->splitList(data, strs)) {
          for (const auto &str : strs) {
            RowColors rowColors;

            std::vector<QString> strs1;
            if (tk_->splitList(str, strs1)) {
              for (const auto &str1 : strs1) {
                QColor c1;
                if (! tk_->variantToQColor(str1, c1))
                  return tk_->invalidQColor1(str1);

                rowColors.push_back(c1);
              }
            }
            else {
              QColor c;
              if (! tk_->variantToQColor(str, c))
                return tk_->invalidQColor1(str);

              rowColors.push_back(c);
            }

            colorArray.push_back(rowColors);
          }
        }
        else {
          QColor c;
          if (! tk_->variantToQColor(data, c))
            return tk_->invalidQColor1(data);

          RowColors rowColors;

          rowColors.push_back(c);

          colorArray.push_back(rowColors);
        }
      }
      else {
        bool b;
        tk_->variantToBool(data, b);

        auto c = QColor(b ? Qt::white : Qt::black);

        RowColors rowColors;

        rowColors.push_back(c);

        colorArray.push_back(rowColors);
      }

      uint nr = colorArray.size();

      uint nc    = 0;
      bool ncSet = false;

      int ir = 0;

      for (const auto &rowColors : colorArray) {
        if (! ncSet) {
          nc    = uint(rowColors.size());
          ncSet = true;
        }
        else {
          if (uint(rowColors.size()) != nc)
            return tk_->throwError("invalid row # " + QString::number(ir) + ": "
                                   "all rows must have the same number of elements");
        }

        ++ir;
      }

      if (nc == 0 || nr == 0)
        return true;

      //---

      if (! toRectSet) {
        if (! toPointSet)
          toPoint = QPoint(0, 0);

        toRect = QRect(toPoint.x(), toPoint.y(), nc, nr);
      }

      int iwidth  = std::max(image->width (), toRect.right () + 1);
      int iheight = std::max(image->height(), toRect.bottom() + 1);

      if (iwidth != image->width() || iheight != image->height())
        image->resize(iwidth, iheight);

      for (int y = toRect.top(); y <= toRect.bottom(); ++y) {
        for (int x = toRect.left(); x <= toRect.right(); ++x) {
          auto c = colorArray[y % nr][x % nc];

          image->setPixel(x, y, c);
        }
      }
    }
    else {
      CTkAppImage image1(tk_, "");

      if (! image1.loadVarData(dataVar, formatData.format))
        return tk_->throwError("couldn't recognize image data");

      if (! toRectSet) {
        if (! toPointSet)
          toPoint = QPoint(0, 0);

        toRect = QRect(toPoint.x(), toPoint.y(), int(image1.width()), int(image1.height()));
      }

      int iwidth  = std::max(image->width (), toRect.left() + int(image1.width ()));
      int iheight = std::max(image->height(), toRect.top () + int(image1.height()));

      if (iwidth != image->width() || iheight != image->height())
        image->resize(iwidth, iheight);

      for (int y = toRect.top(); y <= toRect.bottom(); ++y) {
        for (int x = toRect.left(); x <= toRect.right(); ++x) {
          QColor c;

          if (image1.getPixel(x, y, c))
            image->setPixel(x, y, c);
        }
      }
    }

    if (metaData)
      image->setMetaData(*metaData);
  }
  else if (option == "read") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " read fileName ?-option value ...?");

    auto filename = tk_->variantToString(args[i++]);

    FormatData formatData;
    bool       shrink { false };
    QRect      fromRect;
    QPoint     fromPoint, toPoint;
    bool       fromRectSet { false }, fromPointSet { false }, toPointSet { false };

    while (i < numArgs) {
      static auto optionNames = std::vector<QString>({
        "-format", "-from", "-metadata", "-shrink", "-to" });

      QString opt;
      if (! tk_->lookupOptionName(optionNames, args[i], opt, /*quiet*/true))
        return tk_->throwError(tk_->msg() + "unrecognized option \"" + args[i] + "\": must be " +
                               tk_->concatOptionNames(optionNames));
      ++i;

      if      (opt == "-format") {
        if (i >= numArgs)
          return tk_->throwError("missing value for \"" + option + "\"");

        auto str = tk_->variantToString(args[i]);
        if (! CTkAppImage::lookupFormatName(tk_, str, formatData))
          return tk_->throwError(tk_->msg() + "image file format \"" +
                                 args[i] + "\" is not supported");
        ++i;
      }
      else if (opt == "-from") {
        if (i >= numArgs)
          return tk_->throwError("missing value for \"" + option + "\"");

        long x1, y1, x2, y2;

        if (! tk_->variantToInt(args[i], x1))
          return tk_->invalidInteger(args[i]);
        if (x1 < 0)
          return tk_->throwError("value(s) for the -from option must be non-negative");
        ++i;

        if (i >= numArgs)
          return tk_->throwError("missing value for \"" + option + "\"");

        if (! tk_->variantToInt(args[i], y1))
          return tk_->invalidInteger(args[i]);
        if (y1 < 0)
          return tk_->throwError("value(s) for the -from option must be non-negative");
        ++i;

        if (i < numArgs && ! isOpt(i)) {
          if (! tk_->variantToInt(args[i], x2))
            return tk_->invalidInteger(args[i]);
          if (x2 < 0)
            return tk_->throwError("value(s) for the -from option must be non-negative");
          ++i;

          if (i >= numArgs)
            return tk_->throwError("missing value for \"" + option + "\"");

          if (! tk_->variantToInt(args[i], y2))
            return tk_->invalidInteger(args[i]);
          if (y2 < 0)
            return tk_->throwError("value(s) for the -from option must be non-negative");
          ++i;

          if (x1 > x2) std::swap(x1, x2);
          if (y1 > y2) std::swap(y1, y2);

          fromRect    = QRect(x1, y1, x2 - x1, y2 - y1);
          fromRectSet = true;
        }
        else {
          fromPoint    = QPoint(x1, y1);
          fromPointSet = true;
        }
      }
      else if (opt == "-to") {
        if (i >= numArgs)
          return tk_->throwError("missing value for \"" + option + "\"");

        long x1, y1;

        if (! tk_->variantToInt(args[i], x1))
          return tk_->invalidInteger(args[i]);
        ++i;

        if (i >= numArgs)
          return tk_->throwError("missing value for \"" + option + "\"");

        if (! tk_->variantToInt(args[i], y1))
          return tk_->invalidInteger(args[i]);
        ++i;

        toPoint    = QPoint(x1, y1);
        toPointSet = true;
      }
      else if (opt == "-shrink") {
        shrink = true;
      }
    }

    QImage qimage;
    if (! image->loadImageFile(filename, qimage))
      return false;

    if      (fromRectSet) {
      qimage = qimage.copy(fromRect);
    }
    else if (fromPointSet) {
      qimage = qimage.copy(toPoint.x(), toPoint.y(),
        qimage.width() - toPoint.x(), qimage.height() - toPoint.y());
    }

    if (! toPointSet)
      toPoint = QPoint(0, 0);

    if (shrink)
      image->resize(toPoint.x() + qimage.width(), toPoint.y() + qimage.height());

    image->copy(toPoint.x(), toPoint.y(), qimage);
  }
  else if (option == "redither") {
    if (numArgs != 1)
      return tk_->wrongNumArgs(getName() + " redither");

    // NO OP
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
        auto arg = tk_->variantToString(args[i]);
        if (arg[0] != "-")
          return tk_->wrongNumArgs(getName() + " transparency get x y ?-option?");

        static auto retNames = std::vector<QString>({"-alpha"});

        QString retName;
        if (! tk_->lookupOptionName(retNames, args[i], retName))
          return tk_->throwError(tk_->msg() + "unrecognized option \"" + args[i] + "\": must be " +
                                 tk_->concatOptionNames(retNames));
        ++i;

        if (retName == "-alpha")
          retAlpha = true;
      }

      if (i != numArgs)
        return tk_->wrongNumArgs(getName() + " transparency get x y ?-option?");

      QColor c;
      if (! image->getPixel(x, y, c))
        return tk_->throwError(getName() + " transparency get: coordinates out of range");

      if (retAlpha)
        tk_->setIntegerResult(c.alpha());
      else
        tk_->setBoolResult(c.alpha() == 0);
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
        auto arg = tk_->variantToString(args[i]);
        if (arg[0] != "-")
          return tk_->wrongNumArgs(getName() + " transparency set x y newVal ?-option?");

        static auto retNames = std::vector<QString>({"-alpha"});

        QString retName;
        if (! tk_->lookupOptionName(retNames, args[i], retName))
          return tk_->throwError(tk_->msg() + "unrecognized option \"" + args[i] + "\": must be " +
                                 tk_->concatOptionNames(retNames));
        ++i;

        if (retName == "-alpha")
          setAlpha = true;
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
          return tk_->throwError(tk_->msg() + "invalid alpha value \"" + newVal +
                                 "\": must be integer between 0 and 255");
      }

      QColor c;
      if (! image->getPixel(x, y, c))
        return tk_->throwError(getName() + " transparency set: coordinates out of range");

      c.setAlpha(ialpha);

      image->setPixel(x, y, c);
    }
  }
  else if (option == "write") {
    if (numArgs < 2)
      return tk_->wrongNumArgs(getName() + " write fileName ?-option value ...?");

    using OptString = std::optional<QString>;

    QColor     bg;
    OptString  filename;
    QRect      fromRect;
    FormatData formatData;
    bool       formatSet { false };
    bool       grayscale { false };

    while (i < numArgs) {
      if (isOpt(i)) {
        static auto optNames =
          std::vector<QString>({"-background", "-format", "-from", "-grayscale"});

        QString optName;
        if (! tk_->lookupOptionName(optNames, args[i], optName))
          return tk_->throwError(tk_->msg() + "unrecognized option \"" + args[i] + "\": must be " +
                                 tk_->concatOptionNames(optNames));
        ++i;

        if      (optName == "-background") {
          if (i >= numArgs)
            return tk_->throwError("missing value for \"" + option + "\"");

          if (! tk_->variantToQColor(args[i], bg))
            return tk_->invalidQColor1(args[i]);
          ++i;
        }
        else if (optName == "-format") {
          if (i >= numArgs)
            return tk_->throwError("missing value for \"" + option + "\"");

          auto str = tk_->variantToString(args[i]);
          if (! CTkAppImage::lookupFormatName(tk_, str, formatData))
            return tk_->throwError(tk_->msg() + "image file format \"" +
                                   args[i] + "\" is unknown");
          ++i;

          formatSet = true;
        }
        else if (optName == "-from") {
          if (i >= numArgs)
            return tk_->throwError("missing value for \"" + option + "\"");

          long x1, y1, x2, y2;

          if (! tk_->variantToInt(args[i], x1))
            return tk_->invalidInteger(args[i]);
          if (x1 < 0)
            return tk_->throwError("value(s) for the -from option must be non-negative");
          ++i;

          if (i >= numArgs)
            return tk_->throwError("missing value for \"" + option + "\"");

          if (! tk_->variantToInt(args[i], y1))
            return tk_->invalidInteger(args[i]);
          if (y1 < 0)
            return tk_->throwError("value(s) for the -from option must be non-negative");
          ++i;

          if (i < numArgs && ! isOpt(i)) {
            if (! tk_->variantToInt(args[i], x2))
              return tk_->invalidInteger(args[i]);
            if (x2 < 0)
              return tk_->throwError("value(s) for the -from option must be non-negative");
            ++i;

            if (i >= numArgs)
              return tk_->throwError("missing value for \"" + option + "\"");

            if (! tk_->variantToInt(args[i], y2))
              return tk_->invalidInteger(args[i]);
            if (y2 < 0)
              return tk_->throwError("value(s) for the -from option must be non-negative");
             ++i;
          }
          else {
            x2 = image->width () - x1;
            y2 = image->height() - y1;
          }

          if (x1 > x2) std::swap(x1, x2);
          if (y1 > y2) std::swap(y1, y2);

          fromRect = QRect(x1, y1, x2 - x1, y2 - y1);
        }
        else if (optName == "-grayscale") {
          grayscale = true;
        }
      }
      else {
        filename = tk_->variantToString(args[i++]);
      }
    }

    if (! filename)
      return tk_->wrongNumArgs(getName() + " write fileName ?-option value ...?");

    if (grayscale)
      tk_->TODO(args);

    QImage qimage1;

    if (! fromRect.isNull())
      qimage1 = image->getQImage().copy(fromRect);
    else
      qimage1 = image->getQImage();

    if (! formatSet) {
      QFileInfo fi(*filename);

      formatData.format = fi.suffix();

      if (formatData.format == "default")
        formatData.format = "";
    }

    if (formatData.format == "")
      return tk_->throwError("image file format \"default\" has no file writing capability");

    if (! CTkAppImage::writeImage(*filename, qimage1, formatData.format))
      return tk_->throwError("Failed to write image");
  }

  return true;
}

bool
CTkAppImageCommand::
processOption(CTkApp *tk, CTkAppImageRef &image, const QString &type,
              const QString &name, const QVariant &var)
{
  using FormatData = CTkAppImage::FormatData;

  auto value = tk->variantToString(var);

  if (type == "photo") {
    if      (name == "-data") {
      if (! image->loadVarData(var, ""))
        return false;
    }
    else if (name == "-format") {
      FormatData formatData;

      auto str = tk->variantToString(value);
      if (! CTkAppImage::lookupFormatName(tk, str, formatData))
        return tk->throwError(tk->msg() + "image format \"" + value + "\" is not supported");

      image->setFormat(formatData.format);
    }
    else if (name == "-file") {
      if (! image->loadFile(value))
        return false;
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
    else if (name == "-metadata") {
      auto str = tk->variantToString(value);

      image->setMetaData(str);
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
        return false;
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
        return false;
    }
  }

  return true;
}

void
CTkAppImageCommand::
handleRename(const char *oldName, const char *newName)
{
  auto oldName1 = QString(oldName);

  while (oldName1.left(2) == "::")
    oldName1 = oldName1.mid(2);

  auto newName1 = QString(newName);

  while (newName1.left(2) == "::")
    newName1 = newName1.mid(2);

  tk_->renameImage(oldName1, newName1);

  name_ = newName1;
}

void
CTkAppImageCommand::
handleDelete(const char *name)
{
  auto name1 = QString(name);

  while (name1.left(2) == "::")
    name1 = name1.mid(2);

  auto imageRef = tk_->getImage(name1);

  tk_->deleteImage(imageRef);

  name_ = "";
}

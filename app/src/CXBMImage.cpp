#include <CXBMImage.h>
#include <CGenImage.h>

#include <cstring>
#include <iostream>

namespace {

bool skipSpace(const char *str, uint *pos) {
  if (str[*pos] == '\0' || ! ::isspace(str[*pos]))
    return false;

  while (str[*pos] != '\0' && ::isspace(str[*pos]))
    ++(*pos);

  return true;
}

bool skipNonSpace(const char *str, uint *pos) {
  if (str[*pos] == '\0' || ::isspace(str[*pos]))
    return false;

  while (str[*pos] != '\0' && ! ::isspace(str[*pos]))
    ++(*pos);

  return true;
}

bool readInteger(const char *str, uint *pos, long &l) {
  auto pos1 = *pos;

  if (str[pos1] != '\0' && (str[pos1] == '+' || str[pos1] == '-'))
    ++pos1;

  if (str[pos1] == '\0' || ! ::isdigit(str[pos1]))
    return false;

  ++pos1;

  while (str[pos1] != '\0' && ::isdigit(str[pos1]))
    ++pos1;

  l = std::stol(std::string(&str[*pos], pos1 - *pos));

  *pos = pos1;

  return true;
}

bool toInteger(const std::string &str, long &l) {
  const char *c_str = str.c_str();

  int i = 0;

  while (c_str[i] != 0 && ::isspace(c_str[i]))
    ++i;

  if (c_str[i] == '\0')
    return false;

  const char *p;

  errno = 0;

  l = strtol(&c_str[i], const_cast<char **>(&p), 10);

  if (errno == ERANGE)
    return false;

  while (*p != 0 && ::isspace(*p))
    ++p;

  if (*p != '\0')
    return false;

  return true;
}

std::string toHexString(uint integer, uint width, bool upper=false) {
  static char format[16];
  static char buffer[64];

  if (upper)
    ::sprintf(format, "%%0%dX", width);
  else
    ::sprintf(format, "%%0%dx", width);

  ::sprintf(buffer, format, integer);

  return std::string(buffer);
}

void errorMsg(const std::string &msg) {
  std::cerr << msg << "\n";
}

}

//---

static char hex_chars[] = "0123456789abcdefABCDEF";

CXBMImage::
CXBMImage()
{
}

bool
CXBMImage::
check(std::istream &file) const
{
  static char buffer[257];

  file.seekg(0, std::ios_base::beg);
  file.seekg(0, std::ios_base::end);

  auto len = file.tellg();

  file.seekg(0, std::ios_base::beg);

  uint len1 = std::min(uint(len), uint(sizeof(buffer) - 1));

  memset(buffer, 0, sizeof(buffer));

  if (! file.read(buffer, len1 + 1))
    return false;

  uint pos = 0;

  skipSpace(buffer, &pos);

  auto isString = [&](const std::string &str) {
    auto n = str.size();
    return (pos <= len1 - n && strncmp(&buffer[pos], str.c_str(), n) == 0);
  };

  auto skipString = [&](const std::string &str) {
    auto n = str.size();
    if (pos > len1 - n || strncmp(&buffer[pos], str.c_str(), n) != 0)
      return false;
    pos += uint(n);
    return true;
  };

  if (! skipString("#define"))
    return false;

  pos += 7;

  skipSpace   (buffer, &pos);
  skipNonSpace(buffer, &pos);
  skipSpace   (buffer, &pos);

  long w;
  if (! readInteger(buffer, &pos, w))
    return false;

  skipSpace(buffer, &pos);

  if (! skipString("#define"))
    return false;

  pos += 7;

  skipSpace   (buffer, &pos);
  skipNonSpace(buffer, &pos);
  skipSpace   (buffer, &pos);

  long h;
  if (! readInteger(buffer, &pos, h))
    return false;

  skipSpace(buffer, &pos);

  if (isString("static")) {
    pos += 6;

    skipSpace(buffer, &pos);
  }

  if (! skipString("char"))
    return false;

  return true;
}

bool
CXBMImage::
read(std::istream &file, CGenImage *image)
{
  image->setColormap(true);

  uint *data;
  int   x_hot;
  int   y_hot;
  uint  width;
  uint  height;

  bool flag = readBitmap(file, &width, &height, &data, &x_hot, &y_hot);

  if (! flag)
    return false;

  //------

  image->setType(CGenImage::Type::XBM);

  image->setDataSize(int(width), int(height));

  image->setColorIndexData(data);

  //------

  image->addColor(CGenImage::RGBA(1, 1, 1));
  image->addColor(CGenImage::RGBA(0, 0, 0));

  //------

  return true;
}

bool
CXBMImage::
read(const unsigned char *data, CGenImage *image, int width, int height)
{
  uint *data1 = expandBitmapData(data, width, height);

  //------

  image->setType(CGenImage::Type::XBM);

  image->setDataSize(width, height);

  image->setColorIndexData(data1);

  //------

  image->addColor(CGenImage::RGBA(1, 1, 1));
  image->addColor(CGenImage::RGBA(0, 0, 0));

  //------

  return true;
}

bool
CXBMImage::
readHeader(std::istream &file, CGenImage *image)
{
  int  x_hot;
  int  y_hot;
  uint width;
  uint height;

  bool flag = readBitmap(file, &width, &height, nullptr, &x_hot, &y_hot);

  if (! flag)
    return false;

  //------

  image->setType(CGenImage::Type::XBM);

  image->setSize(int(width), int(height));

  //------

  return true;
}

bool
CXBMImage::
readBitmap(std::istream &file, uint *width, uint *height, uint **data, int *x_hot, int *y_hot)
{
  try {
    *width  = 0;
    *height = 0;
    *x_hot  = 0;
    *y_hot  = 0;

    if (data != nullptr)
      *data = nullptr;

    //------

    file.seekg(0, std::ios_base::beg);

    auto pos1 = file.tellg();

    file.seekg(0, std::ios_base::end);

    auto pos2 = file.tellg();

    auto text_len = pos2 - pos1;

    file.seekg(pos1, std::ios_base::beg);

    //------

    std::vector<char> text;

    text.resize(size_t(text_len + 1));

    file.read(&text[0], text_len);

    text[text_len] = '\0';

    //------

    uint i = 0;

    //------

    char *ptext = &text[0];

    skipSpaceAndComment(ptext, &i);

    if (strncmp(&ptext[i], "#define", 7) != 0)
      return false;

    i += 7;

    skipSpaceAndComment(ptext, &i);

    skipNonSpace(ptext, &i);

    skipSpaceAndComment(ptext, &i);

    uint j = i;

    skipNonSpace(ptext, &i);

    std::string str = std::string(&ptext[j], i - j);

    long lw;
    if (! toInteger(str, lw))
      return false;

    *width = uint(lw);

    //------

    skipSpaceAndComment(ptext, &i);

    if (strncmp(&ptext[i], "#define", 7) != 0)
      return false;

    i += 7;

    skipSpaceAndComment(ptext, &i);

    skipNonSpace(ptext, &i);

    skipSpaceAndComment(ptext, &i);

    j = i;

    skipNonSpace(ptext, &i);

    str = std::string(&ptext[j], i - j);

    long lh;
    if (! toInteger(str, lh))
      return false;

    *height = uint(lh);

    //------

    if (data != nullptr) {
      int width1 = ((*width) + 7)/8;

      auto num_bytes = (*width)*(*height);

      *data = new uint [num_bytes + 7];

      char hex_string[3];

      hex_string[2] = '\0';

      for (uint jj = 0; jj < *height; ++jj) {
        auto l = jj*(*width);

        for (int k = 0; k < width1; ++k) {
          while (ptext[i] != '\0' &&
                 (ptext[i] != '0' || ptext[i + 1] != 'x' ||
                  strchr(hex_chars, ptext[i + 2]) == nullptr ||
                  strchr(hex_chars, ptext[i + 3]) == nullptr))
            ++i;

          if (ptext[i] == '\0') {
            errorMsg("Invalid Hex Number in Data");
            return false;
          }

          hex_string[0] = ptext[i + 2];
          hex_string[1] = ptext[i + 3];

          uint hex_value;

          int no = sscanf(hex_string, "%x", &hex_value);

          if (no != 1) {
            errorMsg("Invalid Hex Number in Data");
            return false;
          }

          i += 4;

          (*data)[l++] = (hex_value & 0x01) != 0;
          (*data)[l++] = (hex_value & 0x02) != 0;
          (*data)[l++] = (hex_value & 0x04) != 0;
          (*data)[l++] = (hex_value & 0x08) != 0;
          (*data)[l++] = (hex_value & 0x10) != 0;
          (*data)[l++] = (hex_value & 0x20) != 0;
          (*data)[l++] = (hex_value & 0x40) != 0;
          (*data)[l++] = (hex_value & 0x80) != 0;
        }
      }
    }

    //------

    return true;
  }
  catch (...) {
    if (data != nullptr)
      delete [] *data;

    return false;
  }
}

uint *
CXBMImage::
expandBitmapData(const unsigned char *data, int width, int height)
{
  int width1 = (width + 7)/8;

  int num_bytes = width*height;

  uint *data1 = new uint [size_t(num_bytes + 7)];

  int i = 0;

  for (int j = 0; j < height; ++j) {
    int l = j*width;

    for (int k = 0; k < width1; ++k, ++i) {
      data1[l++] = (data[i] & 0x01) >> 0;
      data1[l++] = (data[i] & 0x02) >> 1;
      data1[l++] = (data[i] & 0x04) >> 2;
      data1[l++] = (data[i] & 0x08) >> 3;
      data1[l++] = (data[i] & 0x10) >> 4;
      data1[l++] = (data[i] & 0x20) >> 5;
      data1[l++] = (data[i] & 0x40) >> 6;
      data1[l++] = (data[i] & 0x80) >> 7;
    }
  }

  return data1;
}

void
CXBMImage::
skipSpaceAndComment(char *data, uint *i)
{
  skipSpace(data, i);

  while (data[*i] != '\0' && (data[*i] == '/' && data[*i + 1] == '*')) {
    *i += 2;

    while (data[*i] != '\0' && ! (data[*i] == '*' && data[*i + 1] == '/'))
      (*i)++;

    if (data[*i] == '\0')
      return;

    *i += 2;

    skipSpace(data, i);
  }
}

bool
CXBMImage::
write(std::ostream &os, CGenImage *image, const std::string &base)
{
  os << "#define ";
  os << base.c_str();
  os << "_width  ";
  os << std::to_string(image->getWidth());
  os << "\n";

  os << "#define ";
  os << base.c_str();
  os << "_height ";
  os << std::to_string(image->getHeight());
  os << "\n";

  os << "\n";

  os << "static const unsigned char\n";
  os << base.c_str();
  os << "_bits[] = {\n";

  //------

  uint byte    = 0;
  int  bit_no  = 0;
  int  byte_no = 0;

  auto colormap = image->hasColormap();

  for (uint i = 0, k = 0; i < image->getHeight(); ++i) {
    for (uint j = 0; j < image->getWidth(); ++j, ++k) {
      uint pixel = 0;

      if (! colormap) {
        pixel = image->getPixel(i, j);

        CGenImage::RGBA rgba;

        CGenImage::RGBA::decodeARGB(pixel, rgba);

        auto gray = rgba.getGray();

        pixel = (gray > 0.5 ? 0 : 1);
      }
      else {
        auto ind = image->getColorIndex(i, j);

        pixel = (ind == 0 ? 0 : 1);
      }

      byte |= (pixel << bit_no);

      ++bit_no;

      if (bit_no == 8) {
        if (byte_no == 0)
          os << "  0x";
        else
          os << " 0x";

        os << toHexString(byte & 0xFF, 2);

        os << ",";

        ++byte_no;

        if (byte_no >= 8) {
          os << "\n";

          byte_no = 0;
        }

        byte   = 0;
        bit_no = 0;
      }
    }

    if (bit_no > 0) {
      if (byte_no == 0)
        os << "  0x";
      else
        os << " 0x";

      os << toHexString(byte & 0xFF, 2);

      os << ",";

      ++byte_no;

      if (byte_no >= 8) {
        os << "\n";

        byte_no = 0;
      }

      byte   = 0;
      bit_no = 0;
    }
  }

  if (byte_no > 0)
    os << "\n";

  //------

  os << "};\n";

  //------

  return true;
}

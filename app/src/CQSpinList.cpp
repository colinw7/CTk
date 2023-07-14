#include <CQSpinList.h>
#include <CQStrUtil.h>

#include <iostream>

CQSpinList::
CQSpinList(QWidget *parent) :
 QSpinBox(parent)
{
}

void
CQSpinList::
setStrings(const QStringList &s)
{
  mode_ = Mode::STRINGS;

  strings_ = s;

  setRange(0, strings_.length() - 1);
}

QString
CQSpinList::
textFromValue(int value) const
{
  if (mode_ == Mode::STRINGS) {
    //std::cerr << "textFromValue: " << value << "\n";

    if (value < 0 || value >= strings_.length())
      return "";

    return strings_[value];
  }
  else {
    return QSpinBox::textFromValue(value);
  }
}

int
CQSpinList::
valueFromText(const QString &text) const
{
  if (mode_ == Mode::STRINGS) {
    //std::cerr << "valueFromText: " << text.toStdString() << "\n";

    for (int i = 0; i < strings_.length(); ++i)
      if (strings_[i] == text)
        return i;

    return -1;
  }
  else {
    return QSpinBox::valueFromText(text);
  }
}

QValidator::State
CQSpinList::
validate(QString &text, int &pos) const
{
  if (mode_ == Mode::STRINGS) {
    //std::cerr << "validate: " << text.toStdString() << " " << pos << "\n";

    auto strs = CQStrUtil::matchStrs(text, strings());

    //for (const auto &str : strs) {
    //  std::cerr << "match: " << str.toStdString() << "\n";
    //}

    if (strs.length() == 0)
      return QValidator::Invalid;

    if (strs.length() == 1 && strs[0] == text)
      return QValidator::Acceptable;

    bool exact;

    text = CQStrUtil::longestMatch(strs, exact);
    pos = text.length();

    return QValidator::Intermediate;
  }
  else {
    return QSpinBox::validate(text, pos);
  }
}

void
CQSpinList::
fixup(QString &input) const
{
  if (mode_ == Mode::STRINGS) {
    //std::cerr << "fixup: " << input.toStdString() << "\n";

  #if 0
    auto strs = CQStrUtil::matchStrs(input, strings());

    if (strs.length() == 1 && strs[0] != input)
      input = strs[0];
    else {
      bool exact;
      input = CQStrUtil::longestMatch(strs, exact);
    }
#endif
  }
  else {
    QSpinBox::fixup(input);
  }
}

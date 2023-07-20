#ifndef CTkAppEventData_H
#define CTkAppEventData_H

#include <string>

enum class CTkAppEventType {
  None,
  Button,
  Key,
  Enter,
  Leave,
  Help,
  MenuSelect
};

enum class CTkAppEventMode {
  None,
  Press,
  Motion,
  Release
};

enum class CTkAppEventModifier {
  None    = 0,
  Control = (1<<0),
  Shift   = (1<<1),
  Alt     = (1<<2),
  Lock    = (1<<3),
  Command = (1<<4),
  Meta    = (1<<5)
};

struct CTkAppEventData {
  std::string     pattern;
  CTkAppEventType type      { CTkAppEventType::None };
  CTkAppEventMode mode      { CTkAppEventMode::None };
  int             button    { 0 };
  int             clicks    { 1 };
  std::string     key;
  uint            modifiers { 0 };
  std::string     command;

  friend bool operator==(const CTkAppEventData &lhs, const CTkAppEventData &rhs) {
    if (lhs.type != rhs.type) return false;

    if      (lhs.type == CTkAppEventType::Button) {
      if (lhs.mode      != rhs.mode     ) return false;
      if (lhs.button    != rhs.button   ) return false;
      if (lhs.modifiers != rhs.modifiers) return false;
    }
    else if (lhs.type == CTkAppEventType::Key) {
      if (lhs.mode      != rhs.mode     ) return false;
      if (lhs.key       != rhs.key      ) return false;
      if (lhs.modifiers != rhs.modifiers) return false;
    }

    return true;
  }
};

#endif

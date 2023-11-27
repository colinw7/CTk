#ifndef CTkAppEventData_H
#define CTkAppEventData_H

#include <QString>

enum class CTkAppEventType {
  None,
  Button,
  Key,
  Enter,
  Leave,
  Destroy,
  Expose,
  Virtual,
};

enum class CTkAppVirtualEventType {
  None,

  AltUnderlined,
  Invoke,
  Modified,
  Selection,
  ThemeChanged,
  TkWorldChanged,
  TraverseIn,
  TraverseOut,
  UndoStack,
  WidgetViewSync,

  ListboxSelect,
  MenuSelect,
  TreeviewSelect,

  Clear,
  Copy,
  Cut,
  LineEnd,
  LineStart,
  NextChar,
  NextLine,
  NextPara,
  NextWord,
  Paste,
  PasteSelection,
  PrevChar,
  PrevLine,
  PrevPara,
  PrevWindow,
  PrevWord,
  Redo,
  SelectAll,
  SelectLineEnd,
  SelectLineStart,
  SelectNextChar,
  SelectNextLine,
  SelectNextPara,
  SelectNextWord,
  SelectNone,
  SelectPrevChar,
  SelectPrevLine,
  SelectPrevPara,
  SelectPrevWord,
  ToggleSelection,
  Undo,

  Help
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

struct CTkAppVirtualEventData {
  CTkAppVirtualEventType type { CTkAppVirtualEventType::None };

  bool operator<(const CTkAppVirtualEventData &rhs) const {
    return (type < rhs.type);
  }

  bool operator==(const CTkAppVirtualEventData &rhs) const {
    return (type == rhs.type);
  }
};

struct CTkAppEventData {
  QString                pattern;
  CTkAppEventType        type        { CTkAppEventType::None };
  CTkAppVirtualEventType vtype       { CTkAppVirtualEventType::None };
  CTkAppEventMode        mode        { CTkAppEventMode::None };
  bool                   anyButton   { false };
  int                    button      { 0 };
  int                    clicks      { 1 };
  bool                   anyKey      { false };
  QString                key;
  bool                   anyModifier { false };
  uint                   modifiers   { 0 };
  QString                command;

  friend bool operator==(const CTkAppEventData &lhs, const CTkAppEventData &rhs) {
    if (lhs.type != rhs.type) return false;

    if      (lhs.type == CTkAppEventType::Button) {
      if (lhs.mode != rhs.mode) return false;

      if (! lhs.anyButton && ! rhs.anyButton) {
        if (lhs.button != rhs.button) return false;
      }

      if (! lhs.anyModifier && ! rhs.anyModifier) {
        if (lhs.modifiers != rhs.modifiers) return false;
      }
    }
    else if (lhs.type == CTkAppEventType::Key) {
      if (lhs.mode != rhs.mode) return false;

      if (! lhs.anyKey && ! rhs.anyKey) {
        if (lhs.key != rhs.key) return false;
      }

      if (! lhs.anyModifier && ! rhs.anyModifier) {
        if (lhs.modifiers != rhs.modifiers) return false;
      }
    }

    return true;
  }

  friend bool operator!=(const CTkAppEventData &lhs, const CTkAppEventData &rhs) {
    return ! operator==(lhs, rhs);
  }
};

#endif

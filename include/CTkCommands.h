#ifndef CTkCommands_H
#define CTkCommands_H

#include <CTkOptionValue.h>
#include <CTkOptData.h>

#include <CTcl.h>

class CTk;
class CTkWidget;
class CTkRootWidget;
class CTkOpt;

//---

class CTkCommand : public CTclCommand {
 public:
  CTkCommand(CTk *tk, const std::string &name);

  virtual ~CTkCommand() { }

  CTk *getTk() const { return tk_; }

  CTkRootWidget *root() const;

 protected:
  CTk* tk_ { nullptr };
};

class CTkBellCommand : public CTkCommand {
 public:
  CTkBellCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkBindCommand : public CTkCommand {
 public:
  CTkBindCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkButtonCommand : public CTkCommand {
 public:
  CTkButtonCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkCanvasCommand : public CTkCommand {
 public:
  CTkCanvasCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkCheckButtonCommand : public CTkCommand {
 public:
  CTkCheckButtonCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkDestroyCommand : public CTkCommand {
 public:
  CTkDestroyCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkEntryCommand : public CTkCommand {
 public:
  CTkEntryCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkFocusCommand : public CTkCommand {
 public:
  CTkFocusCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkFrameCommand : public CTkCommand {
 public:
  CTkFrameCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkGridCommand : public CTkCommand {
 public:
  CTkGridCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkImageCommand : public CTkCommand {
 public:
  CTkImageCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkLabelCommand : public CTkCommand {
 public:
  CTkLabelCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkLabelFrameCommand : public CTkCommand {
 public:
  CTkLabelFrameCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkListBoxCommand : public CTkCommand {
 public:
  CTkListBoxCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkLowerCommand : public CTkCommand {
 public:
  CTkLowerCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkPackCommand : public CTkCommand {
 public:
  CTkPackCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkPanedWindowCommand : public CTkCommand {
 public:
  CTkPanedWindowCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkPlaceCommand : public CTkCommand {
 public:
  CTkPlaceCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkRadioButtonCommand : public CTkCommand {
 public:
  CTkRadioButtonCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkRaiseCommand : public CTkCommand {
 public:
  CTkRaiseCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkScaleCommand : public CTkCommand {
 public:
  CTkScaleCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkScrollBarCommand : public CTkCommand {
 public:
  CTkScrollBarCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkSpinBoxCommand : public CTkCommand {
 public:
  CTkSpinBoxCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkTextCommand : public CTkCommand {
 public:
  CTkTextCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkTopLevelCommand : public CTkCommand {
 public:
  CTkTopLevelCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkWmCommand : public CTkCommand {
 public:
  CTkWmCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

class CTkWInfoCommand : public CTkCommand {
 public:
  CTkWInfoCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;
};

//-----

class CTkRootWidgetCommand : public CTkCommand {
 public:
  CTkRootWidgetCommand(CTk *tk);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;

 private:
  CTkOptData opts_;
};

//-----

class CTkWidgetCommand : public CTkCommand {
 public:
  CTkWidgetCommand(CTkCommand *command, const std::string &name,
                   CTkWidget *w, const CTkOpt *opts=nullptr);

  CTclValueRef exec(const std::vector<CTclValueRef> &args) override;

  bool processArgs(const std::vector<CTclValueRef> &args);

  bool getOptValue(const std::string &name, std::string &value);
  bool setOptValue(const std::string &name, const std::string &value);

 private:
  CTkCommand* command_ { nullptr };
  CTkWidget*  w_       { nullptr };
  CTkOptData  opts_;
};

//---

namespace CTkCommands {

void addCommands(CTk *tk);

}

#endif

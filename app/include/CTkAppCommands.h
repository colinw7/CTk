#ifndef CTkAppCommands_H
#define CTkAppCommands_H

#include <CTclAppCommand.h>
#include <CTkAppOptData.h>

class CTkAppRoot;
class CTkAppWidget;

namespace CTkAppCommands {

void addCommands(CTkApp *tk);

}

//---

class CTkAppCommand : public CTclAppCommand {
 public:
  using Args = std::vector<std::string>;

 public:
  explicit CTkAppCommand(CTkApp *tk, const std::string &name);

  CTkApp *getTk() const { return tk_; }

 protected:
  bool proc(int argc, const char **argv) override;

  virtual bool run(const Args &args) = 0;

  CTkAppRoot *root() const;

 protected:
  CTkApp *tk_ { nullptr };

  mutable std::string arg0_;
};

//---

class CTkAppWidgetCommand : public CTkAppCommand {
 public:
  explicit CTkAppWidgetCommand(CTkAppCommand *command, const std::string &name,
                               CTkAppWidget *w, const CTkAppOpt *opts = nullptr);
  explicit CTkAppWidgetCommand(CTkApp *app, const std::string &name,
                               CTkAppWidget *w, const CTkAppOpt *opts = nullptr);

 ~CTkAppWidgetCommand();

  bool run(const Args &args) override;

  const CTkAppOptData &getOpts() const { return opts_; }

 public:
  using Args = std::vector<std::string>;

  bool processArgs(const Args &args);

  bool getOptValue(const std::string &name, std::string &value);
  bool setOptValue(const std::string &name, const std::string &value);

 private:
  CTkAppCommand* command_ { nullptr };
  CTkAppWidget*  w_       { nullptr };
  CTkAppOptData  opts_;
};

#endif

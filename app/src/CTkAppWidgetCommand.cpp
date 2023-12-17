#include <CTkAppWidgetCommand.h>
#include <CTkAppWidget.h>

CTkAppWidgetCommand::
CTkAppWidgetCommand(CTkAppCommand *command, const QString &name, CTkAppWidget *w,
                    const CTkAppOpt *opts) :
 CTkAppOptsCommand(command->getTk(), name), command_(command), w_(w)
{
  opts_.init(opts);

  w->setWidgetCommand(this);
}

CTkAppWidgetCommand::
CTkAppWidgetCommand(CTkApp *app, const QString &name, CTkAppWidget *w, const CTkAppOpt *opts) :
 CTkAppOptsCommand(app, name), w_(w)
{
  opts_.init(opts);

  w->setWidgetCommand(this);
}

CTkAppWidgetCommand::
~CTkAppWidgetCommand()
{
}

bool
CTkAppWidgetCommand::
run(const Args &args)
{
  tk_->debugCmd(name_, args);

  uint numArgs = args.size();

  if (numArgs == 0)
    return tk_->wrongNumArgs(getName() + " option ?arg ...?");

  auto arg = tk_->variantToString(args[0]);

  // set config name/value
  if      (arg == "configure" || arg == "config") {
    return execConfigure(args);
  }
  // get config name/value
  else if (arg == "cget") {
    return execCGet(args);
  }
  // apply operation
  else {
    if (! w_->execOp(args))
      return false;
  }

  return true;
}

bool
CTkAppWidgetCommand::
processArgs(const Args &args, bool inCreate)
{
  setInCreate(inCreate);

  w_->setInitNotify(true);

  auto numArgs = args.size();

  bool rc = true;

  for (uint i = 0; i < numArgs; ++i) {
    auto name = tk_->variantToString(args[i]);

    if (name.size() > 0 && name[0] == '-') {
      ++i;

      if (i < numArgs) {
        auto value = args[i];

        if (! setOptValue(name, value))
          rc = false;
      }
      else {
        getTk()->throwError("value for \"" + name + "\" missing");
        rc = false;
      }
    }
  }

  w_->setInitNotify(false);

  setInCreate(false);

  return rc;
}

bool
CTkAppWidgetCommand::
setOptValue(const QString &name, const QVariant &value)
{
  const CTkAppOpt *opt;

  if (! opts_.setOptValue(name, value, &opt))
    return getTk()->unknownOption(name);

  if (! w_->execConfig(opt->name, value))
    return false;

  return true;
}

bool
CTkAppWidgetCommand::
setNameValue(const QString &name, const QVariant &value)
{
  const CTkAppOpt *opt;
  return opts_.setOptValue(name, value, &opt);
}

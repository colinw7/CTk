#include <CTkApp.h>

#include <CQApp.h>
#include <CQStyle.h>

#include <CArgs.h>
#include <CReadLine.h>

static std::string opts = "\
-debug:f \
-init:s \
-namespace:s \
";

class ReadLine : public CReadLine {
 public:
  ReadLine(CTkApp *app) :
   CReadLine(), app_(app) {
    setAutoHistory(true);

    enableTimeoutHook();
  }

  void timeout() override {
    if (! qApp->activeModalWidget())
      app_->processEvents();
  }

  void loop() {
    for (;;) {
      setPrompt("> ");

      auto line1 = readLine();

//    bool join;

//    while (! app_->isCompleteLine(line, join)) {
//      setPrompt("+> ");

//      auto line1 = readLine();

//      if (! join)
//        line += "\n" + line1;
//      else
//        line += line1;
//    }

      if (app_->eval(QString::fromStdString(line1))) {
        QString res;
        if (app_->getStringResult(res))
          std::cerr << res.toStdString() << "\n";
      }

      addHistory(line1);
    }
  }

 private:
  CTkApp *app_ { nullptr };
};

//---

static bool debug = false;

CTkApp          *tk;
static ReadLine *readline;

int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

  //QApplication::setStyle(new QWindowsStyle);

  CArgs cargs(opts);

  cargs.parse(&argc, argv);

  if (cargs.getBooleanArg("-debug"))
    debug = true;

  QString context;

  if (cargs.isStringArg("-namespace"))
    context = QString::fromStdString(cargs.getStringArg("-namespace"));

  QString initFile;

  if (cargs.isStringArg("-init"))
    initFile = QString::fromStdString(cargs.getStringArg("-init"));

  std::deque<QString>      filenames;
  std::vector<std::string> args;
  bool                     processing = true;

  for (int i = 1; i < argc; ++i) {
    auto arg = std::string(argv[i]);

    if      (arg == "--") {
      processing = false;
    }
    else if (! processing) {
      args.push_back(arg);
    }
    else if (arg[0] == '-') {
      std::cerr << "Invalid arg '" << arg << "'\n";
    }
    else {
      if (filenames.empty()) {
        filenames.push_back(QString::fromStdString(arg));

        processing = false;
      }
      else
        args.push_back(arg);
    }
  }

  if (initFile != "")
    filenames.push_front(initFile);

  int    argc1 = int(args.size()) + 1;
  char **argv1 = new char * [argc1 + 1];

  int i = 0;

  argv1[i++] = argv[0];

  for (const auto &arg : args)
    argv1[i++] = const_cast<char *>(arg.c_str());

  argv1[i] = nullptr;

  tk = new CTkApp(argc1, const_cast<const char **>(argv1), context);

  tk->setAppFont(CQStyleMgrInst->font());

  for (const auto &filename : filenames)
    tk->evalFile(filename);

  tk->show();

  readline = new ReadLine(tk);

  readline->loop();

  delete [] argv1;
  delete tk;

  return 0;
}

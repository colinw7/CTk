#include <CTkApp.h>

#include <CQApp.h>

#include <CArgs.h>
#include <CReadLine.h>

static std::string opts = "\
-debug:f \
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

      if (app_->eval(line1))
        std::cerr << app_->getStringResult() << "\n";

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

  tk = new CTkApp(argc, argv);

  std::vector<std::string> filenames;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto arg = std::string(argv[i]);

      std::cerr << "Invalid arg'" << arg << "'\n";
    }
    else
      filenames.push_back(argv[i]);
  }

  for (const auto &filename : filenames)
    tk->evalFile(filename);

  tk->show();

  readline = new ReadLine(tk);

  readline->loop();

  delete tk;

  return 0;
}

#include <CTk.h>
#include <CQApp.h>
#include <CArgs.h>
#include <CReadLine.h>
//#include <QMotifStyle>
//#include <QWindowsStyle>

static std::string opts = "\
-debug:f \
";

class CQReadLine : public CReadLine {
 public:
  CQReadLine() :
   CReadLine() {
    enableTimeoutHook();
  }

  void timeout() override;
};

static bool debug = false;

CTcl              *tcl;
CTk               *tk;
static CQReadLine *readline;

bool
readLine(std::string &line)
{
  readline->setPrompt("% ");

  auto line1 = readline->readLine();

  if (readline->eof()) return false;

  while (! tcl->isCompleteLine(line1)) {
    readline->setPrompt("");

    auto line2 = readline->readLine();

    if (readline->eof()) return false;

    line1 += line2;
  }

  line = line1;

  return true;
}

int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

  //QApplication::setStyle(new QWindowsStyle);

  CArgs cargs(opts);

  cargs.parse(&argc, argv);

  if (cargs.getBooleanArg("-debug"))
    debug = true;

  tcl = new CTcl(argc, argv);

  tcl->setDebug(debug);

  tk = new CTk(tcl);

  if (argc > 1) {
    tcl->parseFile(argv[1]);

    tk->exec();
  }
  else {
    tk->exec();

    readline = new CQReadLine;

    std::string line;

    while (readLine(line)) {
      auto value = tcl->parseLine(line);

      if (value.isValid()) {
        value->print(std::cout);

        std::cout << std::endl;
      }
    }
  }

  while (true)
    tk->processEvents();

  delete tk;

  return 0;
}

void
CQReadLine::
timeout()
{
  tk->processEvents();
}

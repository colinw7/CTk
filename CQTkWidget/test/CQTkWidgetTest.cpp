#include <CQTkWidgetTest.h>
#include <CQTkWidget.h>

#include <QApplication>
#include <QTimer>
#include <QVBoxLayout>
#include <QPushButton>
#include <QThread>
#include <QAbstractEventDispatcher>

#include <tcl.h>

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  auto *test = new CQTkWidgetTest;

  test->init();

  test->show();

  return app.exec();
}

//---

int s_eventEventProc(Tcl_Event * /*evPtr*/, int flags) {
  if (! (flags & TCL_WINDOW_EVENTS))
    return 0;

  QCoreApplication::processEvents();

  return 1;
}

void s_eventSetupProc(ClientData /*clientData*/, int flags) {
//auto *app = static_cast<CTkApp *>(clientData);
  Tcl_Time block_time = {0, 0};
  if (! (flags & TCL_WINDOW_EVENTS))
    return;

  block_time.usec = 10000;
  Tcl_SetMaxBlockTime(&block_time);
}

void s_eventCheckProc(ClientData /*clientData*/, int flags) {
//auto *app = static_cast<CTkApp *>(clientData);
  if (! (flags & TCL_WINDOW_EVENTS))
    return;

  if (! QThread::currentThread()->eventDispatcher()->hasPendingEvents())
    return;

  auto *event = reinterpret_cast<Tcl_Event *>(Tcl_Alloc(sizeof(Tcl_Event)));
  event->proc = s_eventEventProc;
  Tcl_QueueEvent(event, TCL_QUEUE_TAIL);
}

//---

CQTkWidgetTest::
CQTkWidgetTest()
{
  auto *layout = new QVBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  auto *widget = new CQTkWidget(this, "test");

  widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout->addWidget(widget);

  auto *button = new QPushButton("Button");

  layout->addWidget(button);
}

void
CQTkWidgetTest::
init()
{
  Tcl_CreateEventSource(s_eventSetupProc, s_eventCheckProc, this);

  //---

  auto *timer = new QTimer(this);

  connect(timer, SIGNAL(timeout()), this, SLOT(timerSlot()));

  timer->start(100);
}

void
CQTkWidgetTest::
timerSlot()
{
  while (Tcl_DoOneEvent(TCL_ALL_EVENTS) >= 0) {
  }

  //Tcl_ServiceAll();
}

#include "timelinewindow.h"
#include "ui_timelinewindow.h"

TimelineWindow::TimelineWindow(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::TimelineWindow)
{
    ui->setupUi(this);
}

TimelineWindow::~TimelineWindow()
{
    delete ui;
}

#ifndef TIMELINEWINDOW_H
#define TIMELINEWINDOW_H

#include <QDockWidget>

namespace Ui {
class TimelineWindow;
}

class TimelineWindow : public QDockWidget
{
    Q_OBJECT

public:
    explicit TimelineWindow(QWidget *parent = 0);
    ~TimelineWindow();

private:
    Ui::TimelineWindow *ui;
};

#endif // TIMELINEWINDOW_H

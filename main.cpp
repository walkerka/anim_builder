#include "mainwindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QSizePolicy>
#include "rasterimageeditor.h"
#include "timeline.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    w.move(0, 0);

    return a.exec();
}

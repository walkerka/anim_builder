#ifndef BRUSHPROPERTYWINDOW_H
#define BRUSHPROPERTYWINDOW_H

#include <QDockWidget>
#include <QPainter>

namespace Ui {
class BrushPropertyWindow;
}

class BrushPropertyWindow : public QDockWidget
{
    Q_OBJECT

public:
    explicit BrushPropertyWindow(QWidget *parent = 0);
    ~BrushPropertyWindow();
public slots:
    void OnBrushSizeChanged(double);
    void OnSmoothChanged(int);
    void OnModeChanged(QString mode);

signals:
    void brushSizeChanged(float);
    void smoothChanged(int);
    void modeChanged(QPainter::CompositionMode);
private:
    Ui::BrushPropertyWindow *ui;
};

#endif // BRUSHPROPERTYWINDOW_H

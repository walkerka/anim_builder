#ifndef COLORTOOL_H
#define COLORTOOL_H
#include "canvastool.h"
#include <QColor>

class RasterImageEditor;
class MainWindow;

class ColorTool : public CanvasTool
{
public:
    explicit ColorTool(RasterImageEditor* editor, MainWindow* window);

    // CanvasTool interface
public:
    void OnDragBegin(int x, int y, float pressure);
    void OnDrag(int x, int y, float pressure);
    void OnDragEnd(int x, int y, float pressure);
    void OnPaint(QPainter &painter);
    void PickColor(int x, int y);

private:
    RasterImageEditor* mEditor;
    MainWindow* mWindow;
};

#endif // QCOLORPICKER_H

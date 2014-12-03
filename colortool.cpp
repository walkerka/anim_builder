#include "colortool.h"
#include "rasterimageeditor.h"
#include "mainwindow.h"

ColorTool::ColorTool(RasterImageEditor* editor, MainWindow* window)
    :mEditor(editor)
    ,mWindow(window)
{
}


void ColorTool::OnDragBegin(int x, int y, float)
{
    mEditor->grabMouse();
    PickColor(x, y);
}

void ColorTool::OnDrag(int x, int y, float)
{
    PickColor(x, y);
}

void ColorTool::OnDragEnd(int x, int y, float)
{
    PickColor(x, y);
    mEditor->releaseMouse();
}

void ColorTool::OnPaint(QPainter&)
{
}

void ColorTool::PickColor(int x, int y)
{
    if (x < 0 || x >= mEditor->width() || y < 0 || y >= mEditor->height())
    {
        return;
    }

    QImage img = mEditor->grab(QRect(x, y, 1, 1)).toImage();
    QColor color(img.pixel(0, 0));
    mWindow->OnColorChange(color);
}

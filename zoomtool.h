#ifndef ZOOMTOOL_H
#define ZOOMTOOL_H
#include "canvastool.h"
#include <QWidget>

class RasterImageEditor;

class ZoomTool : public CanvasTool
{
public:
    ZoomTool(RasterImageEditor* editor);

    // CanvasTool interface
public:
    void OnDragBegin(int x, int y, float pressure);
    void OnDrag(int x, int y, float pressure);
    void OnDragEnd(int x, int y, float pressure);
    void OnPaint(QPainter &painter);

private:
    RasterImageEditor* mEditor;
    QPoint mLastPosition;
    QPoint mAnchor;
    float mLastScale;
};

#endif // ZOOMTOOL_H

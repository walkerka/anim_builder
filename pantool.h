#ifndef PANTOOL_H
#define PANTOOL_H
#include "canvastool.h"

class RasterImageEditor;

class PanTool : public CanvasTool
{
public:
    PanTool(RasterImageEditor* editor);

    // CanvasTool interface
public:
    void OnDragBegin(int x, int y, float pressure);
    void OnDrag(int x, int y, float pressure);
    void OnDragEnd(int x, int y, float pressure);
    void OnPaint(QPainter &painter);

private:
    RasterImageEditor* mEditor;
    QPoint mLastPosition;
    QPoint mOldTranslate;
};

#endif // PANTOOL_H

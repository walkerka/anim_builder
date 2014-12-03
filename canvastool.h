#ifndef CANVASTOOL_H
#define CANVASTOOL_H
#include "StrokePoint.h"
#include <QPainter>

class CanvasTool
{
public:
    virtual ~CanvasTool() {}
    virtual void OnDragBegin(int x, int y, float pressure) = 0;
    virtual void OnDrag(int x, int y, float pressure) = 0;
    virtual void OnDragEnd(int x, int y, float pressure) = 0;
    virtual void OnPaint(QPainter& painter) = 0;
};

#endif

#ifndef SPLINETOOL_H
#define SPLINETOOL_H
#include "canvastool.h"
#include <QImage>
#include <QPainter>
#include <vector>
#include <QUndoStack>

class RasterImageEditor;

class SplineTool : public CanvasTool
{
public:
    SplineTool(RasterImageEditor* editor, QUndoStack* undoStack);

    void OnDragBegin(int x, int y, float pressure);
    void OnDrag(int x, int y, float pressure);
    void OnDragEnd(int x, int y, float pressure);
    void OnPaint(QPainter &painter);
    void SetUndoStack(QUndoStack* stack) { mUndoStack = stack; }
    void SetBrushSize(float value);
    void SetColor(const QColor& color);
    void SetMode(QPainter::CompositionMode mode);
    float GetBrushSize() { return mBrushSize; }
    QPainter::CompositionMode GetMode() { return mBrushMode; }

private:
    void DrawLastStroke(const StrokePoint& point);
    void BuildDrawSegment(const StrokePoint& p0, const StrokePoint& p1, QPainterPath& path);

signals:

public slots:


private:
    RasterImageEditor* mEditor;
    QUndoStack* mUndoStack;
    float mBrushSize;
    QColor mColor;
    QPainter::CompositionMode mBrushMode;
    std::vector<StrokePoint> mPoints;
    StrokePoint mCurrentPoint;
    QPainterPath mTempPath;
};

#endif // SPLINETOOL_H

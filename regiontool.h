#ifndef REGIONTOOL_H
#define REGIONTOOL_H
#include "canvastool.h"
#include <QImage>
#include <QPainter>
#include <vector>
#include <QUndoStack>

class RasterImageEditor;

class RegionTool : public CanvasTool
{
public:
    RegionTool(RasterImageEditor* editor, QUndoStack* undoStack);

    void OnDragBegin(int x, int y, float pressure);
    void OnDrag(int x, int y, float pressure);
    void OnDragEnd(int x, int y, float pressure);
    void OnPaint(QPainter &painter);
    void SetUndoStack(QUndoStack* stack) { mUndoStack = stack; }
    void SetBrushSize(float value);
    void SetColor(const QColor& color);
    void SetSmooth(int value);
    void SetMode(QPainter::CompositionMode mode);
    float GetBrushSize() { return mBrushSize; }
    int GetSmooth() { return mSmooth; }
    QPainter::CompositionMode GetMode() { return mBrushMode; }

private:
    void DrawLastStroke();
    void BuildDrawSegment(const StrokePoint& p0, const StrokePoint& p1, QPainterPath& path);

signals:

public slots:


private:
    RasterImageEditor* mEditor;
    QUndoStack* mUndoStack;
    float mBrushSize;
    QColor mColor;
    int mSmooth;
    QPainter::CompositionMode mBrushMode;
    std::vector<StrokePoint> mPoints;
    QPainterPath mTempPath;
};

#endif // REGIONTOOL_H

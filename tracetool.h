#ifndef TRACETOOL_H
#define TRACETOOL_H
#include "canvastool.h"
#include <QImage>
#include <QPainter>
#include <vector>
#include <QUndoStack>
#include <QTime>

class RasterImageEditor;
class MainWindow;

class TraceTool : public CanvasTool
{
public:
    TraceTool(RasterImageEditor* editor, QUndoStack* undoStack);
    ~TraceTool();

    void OnDragBegin(int x, int y, float pressure);
    void OnDrag(int x, int y, float pressure);
    void OnDragEnd(int x, int y, float pressure);
    void OnPaint(QPainter &painter);
    void SetUndoStack(QUndoStack* stack) { mUndoStack = stack; }
    void SetSmooth(int value);
    int GetSmooth() { return mSmooth; }
    

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
    std::vector<int> mPointMs;
    QPainterPath mTempPath;
    QTime mTime;
    int mLastMs;
    int mOffset;
};

#endif // TRACETOOL_H

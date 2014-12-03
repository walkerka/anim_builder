#ifndef FILLTOOL_H
#define FILLTOOL_H
#include "canvastool.h"
#include <QImage>
#include <QPainter>
#include <vector>
#include <QUndoStack>

class RasterImageEditor;

class FillTool : public CanvasTool
{
public:
    FillTool(RasterImageEditor* editor, QUndoStack* undoStack);

    void OnDragBegin(int x, int y, float pressure);
    void OnDrag(int x, int y, float pressure);
    void OnDragEnd(int x, int y, float pressure);
    void OnPaint(QPainter &painter);
    void SetUndoStack(QUndoStack* stack) { mUndoStack = stack; }
    void SetColor(const QColor& color);
    void SetSmooth(int value);
    void SetMode(QPainter::CompositionMode mode);
    int GetSmooth() { return mSmooth; }
    QPainter::CompositionMode GetMode() { return mBrushMode; }

signals:

public slots:


private:
    RasterImageEditor* mEditor;
    QUndoStack* mUndoStack;
    QColor mColor;
    int mSmooth;
    int mExpand;
    QPainter::CompositionMode mBrushMode;
};

#endif // FILLTOOL_H

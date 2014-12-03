#ifndef ROTATETOOL_H
#define ROTATETOOL_H
#include "canvastool.h"
#include <QWidget>

class RasterImageEditor;

class RotateTool : public CanvasTool
{
public:
    RotateTool(RasterImageEditor* editor);

    // CanvasTool interface
public:
    void OnDragBegin(int x, int y, float pressure);
    void OnDrag(int x, int y, float pressure);
    void OnDragEnd(int x, int y, float pressure);
    void OnPaint(QPainter &painter);

private:
    RasterImageEditor* mEditor;
    float mMiniDistance;
    float mTolerance;
    QPoint mRotateStartPointScreen;
    QPoint mPivotLocal;
    float mOldAngle;
    float mRefAngle;
    bool mShowCursor;
    QPoint mPivotScreenOld;
    bool mIsDrag;
    bool mLock;
};

#endif // ROTATETOOL_H

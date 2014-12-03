#include "rotatetool.h"
#include "rasterimageeditor.h"
#include <qmath.h>
#include "openglrenderer.h"

static float GetAngle(const QPoint& pivot, const QPoint& pt)
{
    Vector2 dir(pt.x() - pivot.x(), pt.y() - pivot.y());
    if (dir.LengthSq() == 0)
    {
        return 0;
    }
    dir.Normalise();
    float angle = 180.0f * acosf(dir.Dot(Vector2(1.0f, 0))) / 3.1415926f;
    if (dir.y < 0)
    {
        angle = - angle;
    }
    return angle;
}

RotateTool::RotateTool(RasterImageEditor* editor)
    :mEditor(editor)
    ,mMiniDistance(30.0f)
    ,mTolerance(10.0f)
    ,mShowCursor(false)
    ,mIsDrag(false)
    ,mLock(true)
{
}

void RotateTool::OnDragBegin(int x, int y, float)
{
    mPivotScreenOld = QPoint(x, y);
    StrokePoint sp = mEditor->ScreenToLocal(x, y, 0);
    mPivotLocal = QPoint(sp.x, sp.y);
    mShowCursor = true;
    mIsDrag = false;
    mLock = true;
}

void RotateTool::OnDrag(int x, int y, float)
{
    QPoint pos(x, y);

    if (mIsDrag)
    {
        QPoint posScreen(x, y);
        float deltaAngle = GetAngle(mPivotScreenOld, posScreen) - mRefAngle;
        if (mLock)
        {
            if (deltaAngle > mTolerance || deltaAngle < -mTolerance)
            {
                mRefAngle = GetAngle(mPivotScreenOld, posScreen);
                mLock = false;
            }
        }
        else
        {
            mEditor->SetRotate(mOldAngle + deltaAngle);
            QPoint pivotScreenNew = mEditor->LocalToScreen(mPivotLocal.x(), mPivotLocal.y());
            mEditor->ModTranslate(-pivotScreenNew.x() + mPivotScreenOld.x(), -pivotScreenNew.y() + mPivotScreenOld.y());
        }
        mEditor->update();
    }
    else
    {
        QPoint v = pos - mPivotScreenOld;
        float length = sqrtf((float)v.x() * v.x() + v.y() * v.y());

        if (length >= mMiniDistance)
        {
            mIsDrag = true;
            mOldAngle = mEditor->GetRotate();

            mRotateStartPointScreen = pos;
            mRefAngle = GetAngle(mPivotScreenOld, mRotateStartPointScreen);
        }
        mEditor->update();
    }
}

void RotateTool::OnDragEnd(int, int, float)
{
    mShowCursor = false;
    mEditor->update();
    mIsDrag = false;
}

void RotateTool::OnPaint(QPainter &painter)
{
    if (mShowCursor)
    {
        painter.save();
        painter.resetTransform();
        QPoint pivot = mPivotScreenOld;
        int size = 10;
        painter.drawLine(pivot.x() - size, pivot.y(), pivot.x() + size, pivot.y());
        painter.drawLine(pivot.x(), pivot.y() - size, pivot.x(), pivot.y() + size);
        painter.drawEllipse(pivot, (int)mMiniDistance, (int)mMiniDistance);
        painter.restore();

    }
}

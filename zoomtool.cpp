#include "zoomtool.h"
#include "rasterimageeditor.h"

ZoomTool::ZoomTool(RasterImageEditor* editor)
    :mEditor(editor)
{
}


void ZoomTool::OnDragBegin(int x, int y, float)
{
    mLastPosition = QPoint(x, y);
    StrokePoint sp = mEditor->ScreenToLocal(x, y, 1.0f);
    mAnchor.setX(sp.x);
    mAnchor.setY(sp.y);
    mLastScale = mEditor->GetScale();
}

void ZoomTool::OnDrag(int x, int y, float)
{
    float s = mLastScale * (1.0f + (x - mLastPosition.x()) * 0.01f);
    if (s < 0.01f)
    {
        s = 0.01f;
    }
    mEditor->SetScale(s);
    QPoint sp = mEditor->LocalToScreen(mAnchor.x(), mAnchor.y());
    mEditor->ModTranslate(-sp.x() + mLastPosition.x(), -sp.y() + mLastPosition.y());
    mEditor->update();
}

void ZoomTool::OnDragEnd(int x, int y, float pressure)
{
}

void ZoomTool::OnPaint(QPainter &painter)
{
}

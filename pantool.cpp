#include "pantool.h"
#include "rasterimageeditor.h"

PanTool::PanTool(RasterImageEditor* editor)
    :mEditor(editor)
{
}


void PanTool::OnDragBegin(int x, int y, float)
{
    mLastPosition = QPoint(x, y);
}

void PanTool::OnDrag(int x, int y, float)
{
    mEditor->ModTranslate(x - mLastPosition.x(),
                          y - mLastPosition.y());

    mLastPosition.setX(x);
    mLastPosition.setY(y);
    mEditor->update();
}

void PanTool::OnDragEnd(int x, int y, float pressure)
{
}

void PanTool::OnPaint(QPainter &painter)
{
}

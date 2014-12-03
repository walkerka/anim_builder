#include "splinetool.h"
#include "rasterimageeditor.h"
#include "openglrenderer.h"
#include "command.h"

SplineTool::SplineTool(RasterImageEditor* editor, QUndoStack* undoStack)
    :mEditor(editor)
    ,mUndoStack(undoStack)
    ,mBrushSize(1.0f)
    ,mBrushMode(QPainter::CompositionMode_SourceOver)
{
}

void SplineTool::OnDragBegin(int x, int y, float pressure)
{
    if (!mEditor->GetImage())
    {
        return;
    }

    DrawLastStroke(mEditor->ScreenToLocal(x, y, 1.0f));
}

void SplineTool::OnDrag(int x, int y, float pressure)
{
    if (!mEditor->GetImage())
    {
        return;
    }

    DrawLastStroke(mEditor->ScreenToLocal(x, y, 1.0f));
}

void SplineTool::OnDragEnd(int x, int y, float pressure)
{
    if (!mEditor->GetImage())
    {
        return;
    }

    DrawLastStroke(mEditor->ScreenToLocal(x, y, 1.0f));
    if (mPoints.size() < 2)
    {
        mCurrentPoint.pressure = 1.0f;
        mPoints.push_back(mCurrentPoint);
    }
    else
    {
        std::vector<StrokePoint>::iterator it = mPoints.begin() + (mPoints.size() - 1);
        mCurrentPoint.pressure = 1.0f;
        mPoints.insert(it, mCurrentPoint);

        if (mPoints.size() >= 3)
        {
            QImage* oldImage = new QImage(mEditor->GetImage()->width(),mEditor->GetImage()->height(), QImage::Format_RGBA8888);
            QImage* newImage = new QImage(mEditor->GetImage()->width(),mEditor->GetImage()->height(), QImage::Format_RGBA8888);

            QPainter hp(oldImage);
            hp.setCompositionMode(QPainter::CompositionMode_Source);
            hp.drawImage(0, 0, *mEditor->GetImage());

            QPainter np(newImage);
            np.setCompositionMode(QPainter::CompositionMode_Source);
            np.drawImage(0, 0, *mEditor->GetImage());
            np.setCompositionMode(mBrushMode);
            np.setRenderHint(QPainter::Antialiasing, true);
            QBrush brush(mColor);
            np.fillPath(mTempPath, brush);
            mUndoStack->push(new DrawCommand(mEditor, newImage, oldImage));

            mPoints.clear();
            QPainterPath path;
            mTempPath.swap(path);
            mEditor->update();
        }
    }
}

void SplineTool::OnPaint(QPainter &p)
{
    if (!mEditor->GetImage() || mPoints.size() == 0)
    {
        return;
    }
    p.setOpacity(1.0f);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setCompositionMode(mBrushMode);
    p.fillPath(mTempPath, QBrush(mColor));
}

void SplineTool::BuildDrawSegment(const StrokePoint& p0, const StrokePoint& p1, QPainterPath& path)
{
    QPointF pts[4];
    float r0 = p0.pressure * mBrushSize * 0.5f;
    float r1 = p1.pressure * mBrushSize * 0.5f;
    Vector2 dir(p1.x - p0.x, p1.y - p0.y);
    Vector2 perp0 = dir.GetPerpendicular().GetNormalized() * r0;
    Vector2 perp1 = dir.GetPerpendicular().GetNormalized() * r1;
    Vector2 v0(p0.x, p0.y);
    Vector2 v1(p1.x, p1.y);

    pts[0].setX(v0.x - perp0.x);
    pts[0].setY(v0.y - perp0.y);
    pts[1].setX(v1.x - perp1.x);
    pts[1].setY(v1.y - perp1.y);
    pts[2].setX(v1.x + perp1.x);
    pts[2].setY(v1.y + perp1.y);
    pts[3].setX(v0.x + perp0.x);
    pts[3].setY(v0.y + perp0.y);


    QPolygonF polygon;
    polygon.append(pts[0]);
    polygon.append(pts[1]);
    polygon.append(pts[2]);
    polygon.append(pts[3]);
    path.addPolygon(polygon);
    if (r0 > 3.0f)
    {
        path.addEllipse(QPointF(v0.x, v0.y), r0, r0);
    }

}

void SplineTool::DrawLastStroke(const StrokePoint& point)
{
    mCurrentPoint = point;
    std::vector<StrokePoint> points = mPoints;
    if (mPoints.size() < 2)
    {
        points.push_back(point);
    }
    else
    {
        std::vector<StrokePoint>::iterator it = points.begin() + (points.size() - 1);
        points.insert(it, point);
    }

    int n = (int)points.size();
    if (n >= 2)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);

        std::vector<StrokePoint> samples;

        for(int i = 1; i < n; ++i)
        {
            StrokePoint& p1 = points[i - 1];
            StrokePoint& p2 = points[i];
            StrokePoint& p0 = i - 2 >= 0 ? points[i - 2] : p1;
            StrokePoint& p3 = i + 1 < n ? points[i + 1] : p2;

            StrokePoint::CatmulRomSpline(p0, p1, p2, p3, samples);
            if (i < n - 1)
            {
                samples.pop_back();
            }
        }
        for(int j = 1; j < (int)samples.size(); ++j)
        {
            BuildDrawSegment(samples[j - 1], samples[j], path);
        }

        mTempPath.swap(path);
        mEditor->update();
    }
}

void SplineTool::SetBrushSize(float value)
{
    if (value <= 0)
    {
        return;
    }
    mBrushSize = value;
}

void SplineTool::SetColor(const QColor& color)
{
    if (color == mColor)
    {
        return;
    }
    mColor = color;
}

void SplineTool::SetMode(QPainter::CompositionMode mode)
{
    mBrushMode = mode;
}

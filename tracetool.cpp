#include "tracetool.h"
#include "rasterimageeditor.h"
#include "timeline.h"
#include "openglrenderer.h"
#include "command.h"
#include "tracelayer.h"

TraceTool::TraceTool(RasterImageEditor* editor, QUndoStack* undoStack)
    :mEditor(editor)
    ,mUndoStack(undoStack)
    ,mBrushSize(1.0f)
    ,mSmooth(0)
    ,mBrushMode(QPainter::CompositionMode_SourceOver)
    ,mOffset(0)
{
}

TraceTool::~TraceTool()
{
}

void TraceTool::OnDragBegin(int x, int y, float pressure)
{
    mPoints.clear();
    mPointMs.clear();
    QPainterPath p;
    mTempPath.swap(p);

    mTime.restart();
    mLastMs = 0;
    mOffset = mEditor->GetTimeline()->GetFrameIndex();

    mPoints.push_back(mEditor->ScreenToLocal(x, y, pressure));
    mPointMs.push_back(0);
}

void TraceTool::OnDrag(int x, int y, float pressure)
{   
    int t = mTime.elapsed();
    if (t + mLastMs > 1000 / mEditor->GetTimeline()->GetFps())
    {
        mLastMs = t;
        mEditor->GetTimeline()->SetFrameIndex(mEditor->GetTimeline()->GetFrameIndex() + 1);
    }
    mPoints.push_back(mEditor->ScreenToLocal(x, y, pressure));
    mPointMs.push_back(t);
    DrawLastStroke();
}

void TraceTool::OnDragEnd(int x, int y, float pressure)
{
    int t = mTime.elapsed();
    int interval = 1000 / mEditor->GetTimeline()->GetFps();
    if (t + mLastMs > interval)
    {
        mLastMs = t;
        mEditor->GetTimeline()->SetFrameIndex(mEditor->GetTimeline()->GetFrameIndex());
    }
    mPoints.push_back(mEditor->ScreenToLocal(x, y, pressure));
    mPointMs.push_back(t);
    DrawLastStroke();

    Timeline* timeline = mEditor->GetTimeline();
    Layer* layer = timeline->GetLayerAt(timeline->GetLayerIndex());
    if (layer->GetType() == LayerTypeTrace)
    {
        TraceLayer* tl = (TraceLayer*)layer;

        int last = mOffset + mPointMs.back() / interval;
        for (int i = mOffset; i <= last; ++i )
        {
            tl->RemoveFrame(i);
        }
                
        for (size_t i = 0; i < mPoints.size(); ++i)
        {
            int index = mPointMs[i] / interval;
            tl->SetFrame(mOffset + index, (int)mPoints[i].x, (int)mPoints[i].y);
        }
    }

    mPoints.clear();
    mPointMs.clear();
    QPainterPath path;
    mTempPath.swap(path);
    mEditor->update();
}

void TraceTool::OnPaint(QPainter &p)
{
    if (mPoints.size() == 0)
    {
        return;
    }
    p.setPen(QPen(mColor));
    p.setOpacity(1.0f);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    mTempPath.setFillRule(Qt::WindingFill);
    p.fillPath(mTempPath, QBrush(mColor));
    //for (size_t i = 0; i < mPoints.size(); ++i)
    //{
    //    p.drawEllipse(QPointF(mPoints[i].x, mPoints[i].y), 5, 5);
    //}
}

void TraceTool::BuildDrawSegment(const StrokePoint& p0, const StrokePoint& p1, QPainterPath& path)
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

void TraceTool::DrawLastStroke()
{
    //int n = (int)mPoints.size();
    //if (n >= 2)
    //{
    //    QPainterPath path;
    //    path.setFillRule(Qt::WindingFill);

    //    std::vector<StrokePoint> samples;

    //    for(int i = 1; i < n; ++i)
    //    {
    //        StrokePoint& p1 = mPoints[i - 1];
    //        StrokePoint& p2 = mPoints[i];
    //        StrokePoint& p0 = i - 2 >= 0 ? mPoints[i - 2] : p1;
    //        StrokePoint& p3 = i + 1 < n ? mPoints[i + 1] : p2;

    //        StrokePoint::CatmulRomSpline(p0, p1, p2, p3, samples);
    //        if (i < n - 1)
    //        {
    //            samples.pop_back();
    //        }
    //    }
    //    StrokePoint::Smooth(samples, mSmooth);
    //    for(int j = 1; j < (int)samples.size(); ++j)
    //    {
    //        BuildDrawSegment(samples[j - 1], samples[j], path);
    //    }

    //    mTempPath.swap(path);
    //    mEditor->update();
    //}
    mTempPath.addEllipse(QPointF(mPoints.back().x, mPoints.back().y), 5.0f, 5.0f);
    mEditor->update();
}

void TraceTool::SetSmooth(int value)
{
    if (value < 0)
    {
        value = 0;
    }
    else if (value > 100)
    {
        value = 100;
    }
    mSmooth = value;
}

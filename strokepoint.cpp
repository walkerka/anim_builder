#include "StrokePoint.h"
#include <list>
#include "openglrenderer.h"

struct SplineSamplingContext
{
    float x0;
    float x1;
    float x2;
    float x3;

    float y0;
    float y1;
    float y2;
    float y3;

    float z0;
    float z1;
    float z2;
    float z3;

    float tolerance;

    std::list<StrokePoint> points;
};

float StrokePoint::DistanceToSq(StrokePoint& p0, StrokePoint& p1, StrokePoint& p)
{

    Vector2 vl(p1.x - p0.x, p1.y - p0.y);
    Vector2 vp(p.x - p0.x, p1.y - p0.y);

    //dot2 = vl2 * vp2 * cos2
    //d2 = vp2 - vp2.cos2
    //   = vp2 - dot / vl2
    float d2 = vp.LengthSq();
    float vl2 = vl.LengthSq();
    if (vl2 > 0)
    {
        float dot2 = vl.Dot(vp);
        dot2 *= dot2;
        d2 -= dot2 / vl2;
    }

    return d2;
}

static void Subdivide(SplineSamplingContext& ctx, std::list<StrokePoint>::iterator endIt, float beginT, float endT)
{
    if (endT - beginT < 0.001f)
    {
        return;
    }
    float t = (beginT + endT) * 0.5f;
    float t2 = t * t;
    float t3 = t2 * t;
    StrokePoint pt;
    pt.x = ctx.x0 + ctx.x1 * t + ctx.x2 * t2 + ctx.x3 * t3;
    pt.y = ctx.y0 + ctx.y1 * t + ctx.y2 * t2 + ctx.y3 * t3;
    pt.pressure = ctx.z0 + ctx.z1 * t + ctx.z2 * t2 + ctx.z3 * t3;

    std::list<StrokePoint>::iterator beginIt = endIt;
    beginIt--;

    //printf("Subdivide t=(%f,%f) p0=(%f,%f) p1=(%f,%f) pt=(%f,%f)\n", beginT, endT, beginIt->x, beginIt->y, endIt->x, endIt->y, pt.x, pt.y);

//    float tolerance = cosf(1.0f * 3.1415926f / 180.0f);
    Vector2 v12(endIt->x - beginIt->x, endIt->y - beginIt->y);
//    Vector2 v1(pt.x - beginIt->x, pt.y - beginIt->y);
//    Vector2 v2(endIt->x - pt.x, endIt->y - pt.y);
    //v1.Normalise();
    //v2.Normalise();

    //if (StrokePoint::DistanceToSq(*(beginIt), *(endIt), pt) > ctx.tolerance * ctx.tolerance)
    if (v12.Length() > 5.0f)
    {
        std::list<StrokePoint>::iterator it = ctx.points.insert(endIt, pt);
        Subdivide(ctx, it, beginT, t);
        Subdivide(ctx, endIt, t, endT);
    }
}

void StrokePoint::CatmulRomSpline(const StrokePoint& P0, const StrokePoint& P1, const StrokePoint& P2, const StrokePoint& P3, std::vector<StrokePoint>& result)
{
    SplineSamplingContext ctx;
    ctx.x0 = P1.x;
    ctx.x1 = (-P0.x + P2.x) * 0.5f;
    ctx.x2 = P0.x - 2.5f * P1.x + 2.0f * P2.x - 0.5f * P3.x;
    ctx.x3 = -0.5f * P0.x + 1.5f * P1.x- 1.5f * P2.x + 0.5f * P3.x;

    ctx.y0 = P1.y;
    ctx.y1 = (-P0.y + P2.y) * 0.5f;
    ctx.y2 = P0.y - 2.5f * P1.y + 2.0f * P2.y - 0.5f * P3.y;
    ctx.y3 = -0.5f * P0.y + 1.5f * P1.y- 1.5f * P2.y + 0.5f * P3.y;

    ctx.z0 = P1.pressure;
    ctx.z1 = (-P0.pressure + P2.pressure) * 0.5f;
    ctx.z2 = P0.pressure - 2.5f * P1.pressure + 2.0f * P2.pressure - 0.5f * P3.pressure;
    ctx.z3 = -0.5f * P0.pressure + 1.5f * P1.pressure- 1.5f * P2.pressure + 0.5f * P3.pressure;

    ctx.points.push_back(P1);
    ctx.points.push_back(P2);
    ctx.tolerance = 1.0f;

    std::list<StrokePoint>::iterator it = ctx.points.begin();
    it++;
    Subdivide(ctx, it, 0.0f, 1.0f);
    //printf("subdivide points = %d\n", ctx.points.size());

    for (std::list<StrokePoint>::iterator i = ctx.points.begin(); i != ctx.points.end(); ++i)
    {
        if (result.size() == 0 || result.back() != *i)
        {
            result.push_back(*i);
        }
    }
}

void
StrokePoint::Smooth(std::vector<StrokePoint>& points, int radius)
{
    int n = (int)points.size();
    if (radius < 1 || n < 3)
    {
        return;
    }

    std::vector<Vector2> result;

    float d = 1.0f / (radius * 2 + 1);

    for (int i = 1; i < n - 1; ++i)
    {
        int lower = i - radius;
        int upper = i + radius;

        Vector2 total;
        for (int j = lower; j <= upper; ++j)
        {
            int idx = j;
            if(idx < 0)
            {
                idx = 0;
            }
            else if(idx >= n)
            {
                idx = n - 1;
            }
            total.x += points[idx].x;
            total.y += points[idx].y;
        }

        total *= d;
        result.push_back(total);
    }

    for (int i = 1; i < n - 1; ++i)
    {
        points[i].x = result[i - 1].x;
        points[i].y = result[i - 1].y;
    }
}



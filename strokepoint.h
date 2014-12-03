#ifndef STROKEPOINT_H
#define STROKEPOINT_H
#include <vector>

struct StrokePoint
{
    // in pixel
    float x;
    // in pixel
    float y;
    // range is [0, 1.0f]
    float pressure;
public:
    bool operator == (const StrokePoint& v) const
    {
        return v.x == x && v.y == y;
    }
    bool operator != (const StrokePoint& v) const
    {
        return v.x != x || v.y != y;
    }

    static float DistanceToSq(StrokePoint& p0, StrokePoint& p1, StrokePoint& p);
    static void CatmulRomSpline(const StrokePoint& P0, const StrokePoint& P1, const StrokePoint& P2, const StrokePoint& P3, std::vector<StrokePoint>& result);
    static void Smooth(std::vector<StrokePoint>& points, int radius);
};

#endif // STROKEPOINT_H

#include "filltool.h"
#include "rasterimageeditor.h"
#include "command.h"
#include "timeline.h"
#include "animationfile.h"


typedef struct {		/* window: a discrete 2-D rectangle */
    int x0, y0;			/* xmin and ymin */
    int x1, y1;			/* xmax and ymax (inclusive) */
} Window;

typedef unsigned int Pixel;		/* 1-channel frame buffer assumed */

static Pixel* pixelBuffer = NULL;
static unsigned char* maskBuffer = NULL;
static int pixelBufferWidth = 0;
static int pixelBufferHeight = 0;

inline Pixel pixelread(int x, int y)
{
    return pixelBuffer[y * pixelBufferWidth + x];
}

inline bool pixelEquals(int x, int y, Pixel p0, int threshold)
{
#define R(v) ((v>>24) & 0xff)
#define G(v) ((v>>16) & 0xff)
#define B(v) ((v>>8) & 0xff)
#define A(v) (v)

    Pixel p1 = pixelBuffer[y * pixelBufferWidth + x];

    int t2 = threshold * threshold;
    int da = A(p0) - A(p1);
    if (da * da > t2)
    {
        return false;
    }

    int dr = R(p0) - R(p1);
    if (dr * dr > t2)
    {
        return false;
    }

    int dg = G(p0) - G(p1);
    if (dg * dg > t2)
    {
        return false;
    }

    int db = B(p0) - B(p1);
    if (db * db > t2)
    {
        return false;
    }

    if (maskBuffer[y * pixelBufferWidth + x])
    {
        return false;
    }

    return true;

#undef R
#undef G
#undef B
#undef A
}

void pixelwrite(int x, int y)
{
    maskBuffer[y * pixelBufferWidth + x] = 0xFF;
}

typedef struct {short y, xl, xr, dy;} Segment;
/*
* Filled horizontal segment of scanline y for xl<=x<=xr.
* Parent segment was on line y-dy.  dy=1 or -1
*/

#define MAX 10000		/* max depth of stack */

#define PUSH(Y, XL, XR, DY)	/* push new segment on stack */ \
    if (sp<stack+MAX && Y+(DY)>=win->y0 && Y+(DY)<=win->y1) \
{sp->y = Y; sp->xl = XL; sp->xr = XR; sp->dy = DY; sp++;}

#define POP(Y, XL, XR, DY)	/* pop segment off stack */ \
{sp--; Y = sp->y+(DY = sp->dy); XL = sp->xl; XR = sp->xr;}

/*
* fill: set the pixel at (x,y) and all of its 4-connected neighbors
* with the same pixel value to the new pixel value nv.
* A 4-connected neighbor is a pixel above, below, left, or right of a pixel.
*/

void fill(int x, int y,	/* seed point */
            Window *win,	/* screen window */
            int threshold
)
{
    int l, x1, x2, dy;
    Pixel ov;	/* old pixel value */
    Segment stack[MAX], *sp = stack;	/* stack of filled segments */

    ov = pixelread(x, y);		/* read pv at seed point */
    if (x<win->x0 || x>win->x1 || y<win->y0 || y>win->y1)
    {
        return;
    }
    PUSH(y, x, x, 1);			/* needed in some cases */
    PUSH(y+1, x, x, -1);		/* seed segment (popped 1st) */

    while (sp>stack)
    {
        /* pop segment off stack and fill a neighboring scan line */
        POP(y, x1, x2, dy);
        /*
        * segment of scan line y-dy for x1<=x<=x2 was previously filled,
        * now explore adjacent pixels in scan line y
        */
        for (x=x1; x>=win->x0 && pixelEquals(x, y, ov, threshold); x--)
        {
            pixelwrite(x, y);
        }

        if (x>=x1)
        {
            goto skip;
        }
        l = x+1;
        if (l<x1)
        {
            PUSH(y, l, x1-1, -dy);		/* leak on left? */
        }

        x = x1+1;
        do
        {
            for (; x<=win->x1 && pixelEquals(x, y, ov, threshold); x++)
            {
                pixelwrite(x, y);
            }

            PUSH(y, l, x-1, dy);
            if (x>x2+1)
            {
                PUSH(y, x2+1, x-1, -dy);	/* leak on right? */
            }

skip:	    for (x++; x<=x2 && !pixelEquals(x, y, ov, threshold); x++)
            {
            }
            l = x;
        } while (x<=x2);
    }
}

void fill(unsigned int* pixels, int width, int height, int x, int y, int threshold, unsigned char* mask)
{
    Window win;
    win.x0 = 0;
    win.y0 = 0;
    win.x1 = width - 1;
    win.y1 = height - 1;
    pixelBuffer = pixels;
    maskBuffer = mask;
    pixelBufferWidth = width;
    pixelBufferHeight = height;
    fill(x, y, &win, threshold);
}

FillTool::FillTool(RasterImageEditor* editor, QUndoStack* undoStack)
    :mEditor(editor)
    ,mUndoStack(undoStack)
    ,mColor(0,0,0,0xFF)
    ,mSmooth(0)
    ,mExpand(1)
    ,mBrushMode(QPainter::CompositionMode_SourceOver)
{
}

void FillTool::OnDragBegin(int x, int y, float pressure)
{
    if (!mEditor->GetImage())
    {
        return;
    }
}

void FillTool::OnDrag(int x, int y, float pressure)
{
    if (!mEditor->GetImage())
    {
        return;
    }
}

void FillTool::OnDragEnd(int x, int y, float pressure)
{
    if (!mEditor->GetImage())
    {
        return;
    }

    QImage* img = mEditor->GetTimeline()->GetCompositeImage();
    if (!img)
    {
        return;
    }

    StrokePoint sp = mEditor->ScreenToLocal(x, y, pressure);

    int w = img->width();
    int h = img->height();

    QImage* hi = new QImage(w, h, QImage::Format_RGBA8888);
    QPainter hp(hi);
    hp.setCompositionMode(QPainter::CompositionMode_Source);
    hp.drawImage(0, 0, *mEditor->GetImage());

    unsigned char* mask = new unsigned char[w * h];
    memset(mask, 0, w * h);
    fill((unsigned int*) img->bits(), w, h, (int)sp.x, (int)sp.y, 0, mask);

    QImage maskImg(w, h, QImage::Format_RGBA8888);
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            bool hit = false;
            for (int dy = -mExpand; dy <= mExpand; ++dy)
            {
                int py = y + dy;
                if (py < 0 || py >= h)
                {
                    continue;
                }
                for (int dx = -mExpand; dx <= mExpand; ++dx)
                {
                    int px = x + dx;
                    if (px < 0 || px >= w)
                    {
                        continue;
                    }

                    if (mask[py * w + px])
                    {
                        hit = true;
                        goto done;
                    }
                }
            }
            done:
            if (hit)
            {
                maskImg.setPixel(x, y, QColor(mColor.red(), mColor.green(), mColor.blue(), mColor.alpha()).rgba());
            }
        }
    }

//    int* depthMask = new int[w * h];
//    for (int y = 0; y < h; ++y)
//    {
//        for (int x = 0; x < w; ++x)
//        {
//            depthMask[y * w + x] = mask[y * w + x] ? 1 : 0;
//        }
//    }

//    bool hasChange = false;
//    do
//    {
//        hasChange = false;

//        for (int y = 0; y < h; ++y)
//        {
//            for (int x = 0; x < w; ++x)
//            {
//                if (depthMask[y * w + x] == 0)
//                {
//                    bool isNearby = false;
//                    int maxAlpha = 0;
//                    int minAlpha = 0xFF;
//                    int maxDepth = 0;
//                    // if depth[nearby] > 0, find maxAlpha(srImage)
//                    for (int by = y - 1; by <= y + 1; ++by)
//                    {
//                        for (int bx = x - 1; bx <= x + 1; ++bx)
//                        {
//                            if (bx == x && by == y)
//                            {
//                                continue;
//                            }
//                            if (bx < 0 || bx >= w || by < 0 || by >= h)
//                            {
//                                continue;
//                            }
//                            int depth = depthMask[by * w + bx];
//                            if (depth > 0)
//                            {
//                                isNearby = true;
//                                int alpha = img->pixel(bx, by) >> 24;
//                                if (alpha > maxAlpha)
//                                {
//                                    maxAlpha = alpha;
//                                }
//                                if (alpha < minAlpha)
//                                {
//                                    minAlpha = alpha;
//                                }
//                                if (depth > maxDepth)
//                                {
//                                    maxDepth = depth;
//                                }
//                            }
//                        }
//                    }

//                    if (!isNearby)
//                    {
//                        continue;
//                    }

//                    // if alpha > maxAlpha, mark as positive & mask = alpha
//                    int alpha = img->pixel(x, y) >> 24;
//                    if (alpha >= minAlpha)
//                    {
//                        depthMask[y * w + x] = maxDepth + 1;
//                        mask[y * w + x] = alpha;
//                    }
//                    else
//                    {
//                        depthMask[y * w + x] = -1;
//                    }


//                    hasChange = true;
//                }
//            }
//        }
//    } while (hasChange);

//    for (int y = 0; y < h; ++y)
//    {
//        for (int x = 0; x < w; ++x)
//        {
//            int a = depthMask[y * w + x] > 0 ? 0xFF : 0;
//            int mFillHole = 1;
//            if (a == 0)
//            {
//                int total = 0;
//                int hit = 0;
//                for (int by = y - mFillHole; by <= y + mFillHole; ++by)
//                {
//                    for (int bx = x - mFillHole; bx <= x + mFillHole; ++bx)
//                    {
//                        if (bx == x && by == y)
//                        {
//                            continue;
//                        }
//                        if (bx < 0 || bx >= w || by < 0 || by >= h)
//                        {
//                            continue;
//                        }
//                        if (depthMask[by * w + bx] > 0)
//                        {
//                            ++hit;
//                        }
//                        ++total;
//                    }
//                }
//                if (hit * 2 > total)
//                {
//                    a = 0xFF;
//                }
//            }

//            //int a = mask[y * w + x];

//            maskImg.setPixel(x, y, QColor(mColor.red(), mColor.green(), mColor.blue(), mColor.alpha() * a / 255).rgba());
//        }
//    }
//    QPainter maskPainter(&maskImg);
//    maskPainter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
//    maskPainter.drawImage(0, 0, *img);

//    QImage debugImg(w, h, QImage::Format_RGBA8888);
//    for (int y = 0; y < h; ++y)
//    {
//        for (int x = 0; x < w; ++x)
//        {
//            int v = depthMask[y * w + x];
//            if ( v > 0)
//            {
//                debugImg.setPixel(x, y, 0xFFFF0000);
//            }
//            else if (v < 0)
//            {
//                debugImg.setPixel(x, y, 0xFF00FF00);
//            }
//            else
//            {
//                debugImg.setPixel(x, y, 0xFFFFFFFF);
//            }
//        }
//    }
//    debugImg.save("d:/debug.png");
//    delete[] depthMask;

    QImage* newImage = new QImage(w, h, QImage::Format_RGBA8888);
    QPainter painter(newImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(0, 0, *mEditor->GetImage());
    painter.setCompositionMode(mBrushMode);
    painter.drawImage(0, 0, maskImg);
    mUndoStack->push(new DrawCommand(mEditor, newImage, hi));
    mEditor->update();
}

void FillTool::OnPaint(QPainter &p)
{

}

void FillTool::SetColor(const QColor& color)
{
    if (color == mColor)
    {
        return;
    }
    mColor = color;
}

void FillTool::SetSmooth(int value)
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

void FillTool::SetMode(QPainter::CompositionMode mode)
{
    mBrushMode = mode;
}

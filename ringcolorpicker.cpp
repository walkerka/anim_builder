#include "ringcolorpicker.h"
#include <QWidget>
#include <QPainter>
#include <cmath>
#include "openglrenderer.h"

RingColorPicker::RingColorPicker(QWidget *parent) :
    QWidget(parent),
    mColor(QColor(0, 0, 0)),
    mRingWidth(20),
    mRectSize(0),
    mRingSize(192),
    mDragRing(false),
    mDragRect(false)
{
    mColor.setHsvF(1.0f, 1.0f, 0.0f);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    mRectSize = (int)((mRingSize - mRingWidth * 2) / 1.414f) - 10;
    mRectImage = new QImage(mRectSize, mRectSize, QImage::Format_RGBA8888);

    int padding = (mRingSize - mRectSize) / 2;
    mRect.setRect(padding, padding, mRectSize, mRectSize);

    mRingPath.setFillRule(Qt::OddEvenFill);
    mRingPath.addEllipse(0, 0, mRingSize, mRingSize);
    mRingPath.addEllipse(mRingWidth, mRingWidth, mRingSize - mRingWidth * 2, mRingSize - mRingWidth * 2);

    UpdateRectImage();
}


QSize RingColorPicker::sizeHint() const
{
    return QSize(mRingSize, mRingSize);
}

QSize RingColorPicker::minimumSizeHint() const
{
    return QSize(mRingSize, mRingSize);
}

void RingColorPicker::mousePressEvent(QMouseEvent *e)
{
    this->grabMouse();
    Vector2 pt((float)e->x(), (float)e->y());
    Vector2 center(mRingSize / 2.0f, mRingSize / 2.0f);
    float dist = center.DistanceTo(pt);
    if (dist >= (mRingSize / 2.0f - mRingWidth) &&
        dist <= mRingSize / 2.0f)
    {
        mDragRing = true;
    }
    else if (mRect.contains(e->pos()))
    {
        mDragRect = true;
    }

    if (mDragRing)
    {
        UpdateHue(e->pos());
    }
    else if (mDragRect)
    {
        UpdateSatValue(e->pos());
    }
}

void RingColorPicker::mouseReleaseEvent(QMouseEvent *e)
{
    this->releaseMouse();
    mDragRing = false;
    mDragRect = false;
}

void RingColorPicker::mouseMoveEvent(QMouseEvent *e)
{
    if (mDragRing)
    {
        UpdateHue(e->pos());
    }
    else if (mDragRect)
    {
        UpdateSatValue(e->pos());
    }
}

void RingColorPicker::UpdateHue(const QPoint& pos)
{
    QPoint center(mRingSize / 2, mRingSize / 2);
    if (pos == center)
    {
        return;
    }

    Vector2 dir(pos.x() - center.x(), pos.y() - center.y());
    dir.Normalise();

    float hue = acos(dir.Dot(Vector2(1, 0))) / (3.1415926f * 2.0f);
    if (dir.y > 0)
    {
        hue = 1.0f - hue;
    }
    if (hue != mColor.hsvHueF())
    {
        float s = mColor.hsvSaturationF();
        float v = mColor.valueF();
        if (s == -1)
        {
            s = 0;
        }
        if (v == -1)
        {
            v = 0;
        }
        mColor.setHsvF(hue, s, v);
        UpdateRectImage();
        emit valueChanged(mColor);
    }
}

void RingColorPicker::UpdateSatValue(const QPoint& pos)
{
    float s = (pos.x() - mRect.x()) / (float)mRect.width();
    float v = 1.0f - (pos.y() - mRect.y()) / (float)mRect.height();
    if (s < 0)
    {
        s = 0;
    }
    else if (s > 1.0f)
    {
        s = 1.0f;
    }
    if (v < 0)
    {
        v = 0;
    }
    else if (v > 1.0f)
    {
        v = 1.0f;
    }
    if (s != mColor.hsvSaturationF() || v != mColor.valueF())
    {
        float h = mColor.hsvHueF();
        mColor.setHsvF(h, s, v);
        update();
        emit valueChanged(mColor);
    }

}

void RingColorPicker::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.drawImage(mRect, *mRectImage);


    p.setRenderHint(QPainter::Antialiasing, true);
    QConicalGradient g(mRingSize / 2, mRingSize / 2, 0);
    for (int i = 0; i <= 6; ++i)
    {
        float angle = i * 60.0f / 360.0f;
        g.setColorAt(angle, QColor::fromHsvF(angle, 1.0f, 1.0f));
    }
    p.setBrush(g);
    p.drawPath(mRingPath);

    QPointF center(mRingSize / 2.0f, mRingSize / 2.0f);
    float r = mRingSize / 2.0f;
    float hue = mColor.hueF() * 3.1415926f * 2.0f;
    float innerR = r - mRingWidth;
    QPointF dir(cosf(-hue), sinf(-hue));

    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QColor(0, 0, 0)));
    p.drawLine(dir * r + center, dir * innerR + center);

    p.drawEllipse(QPoint(mRect.x() + (int)(mColor.hsvSaturationF() * mRect.width()),
                         mRect.y() + (int)((1.0f - mColor.valueF()) * mRect.height())),
                  3, 3);

    p.setPen(QPen(QColor(255, 255, 255)));
    p.drawEllipse(QPoint(mRect.x() + (int)(mColor.hsvSaturationF() * mRect.width()),
                         mRect.y() + (int)((1.0f - mColor.valueF()) * mRect.height())),
                  3, 3);
}

void RingColorPicker::SetColor(const QColor& color)
{
    if (color != mColor)
    {
        mColor = color;
        UpdateRectImage();
        emit valueChanged(mColor);
    }
}

void RingColorPicker::UpdateRectImage()
{
    float hue = mColor.hsvHueF();
    if (hue < 0.0f)
    {
        hue = 0.0f;
    }

    int w = mRectImage->width();
    int h = mRectImage->height();
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            mRectImage->setPixel(x, y,
            QColor::fromHsvF(hue,
                             x / (float)(w - 1),
                             (h - 1 - y) / (float)(h - 1)
                             ).rgba());
        }
    }
    update();
}

void RingColorPicker::resizeEvent(QResizeEvent *)
{
}

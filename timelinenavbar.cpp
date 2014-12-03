#include "timelinenavbar.h"
#include "timeline.h"
#include <QPainter>
#include <QMouseEvent>

TimelineNavBar::TimelineNavBar(Timeline* timeline, QWidget *parent) :
    QWidget(parent)
{
    mTimeline = timeline;
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
}


QSize TimelineNavBar::sizeHint() const
{
    return QSize(mTimeline->GetMaxFrames() * mTimeline->GetCellSize().width(), 12);
}

QSize TimelineNavBar::minimumSizeHint() const
{
    return QSize(mTimeline->GetMaxFrames() * mTimeline->GetCellSize().width(), 12);
}

void TimelineNavBar::mousePressEvent(QMouseEvent *e)
{
    int idx = (e->pos().x() + mTimeline->GetOffset()) / mTimeline->GetCellSize().width();
    mTimeline->SetFrameIndex(idx);
    update();
}

void TimelineNavBar::mouseMoveEvent(QMouseEvent *e)
{
    int idx = (e->pos().x() + mTimeline->GetOffset()) / mTimeline->GetCellSize().width();
    mTimeline->SetFrameIndex(idx);
    update();
}

void TimelineNavBar::keyPressEvent(QKeyEvent *)
{
}

void TimelineNavBar::keyReleaseEvent(QKeyEvent *)
{
}

void TimelineNavBar::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setBackground(Qt::NoBrush);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QColor(128, 128, 128)));

    int h = height();
    int w = mTimeline->GetCellSize().width();
    int n = width() / w + 1;
    int mOffset = mTimeline->GetOffset();

    QFont font;
    font.setFamily("Arial");
    font.setPixelSize(10);
    p.setFont(font);
    p.setPen(QPen(QColor(0,0,0)));

    int minIdx = mOffset / w;
    int maxIdx = (mOffset + width()) / w;

    for (int i = minIdx; i <= maxIdx; ++i)
    {
        QString s;
        int top = h / 2;

        if(i % 24 == 0)
        {
            top = 0;
        }
        else if (i %  12 == 0)
        {
            top = h / 4;
        }

        p.drawLine(i * w - mOffset, top, i * w - mOffset, h);

        if (i % 6 == 0)
        {
            s.sprintf("%d", i);
            p.drawText(i * w + 2 - mOffset, h-3, s);
        }
    }
    p.drawLine(0, h - 1, width(), h - 1);
    QPolygon polygon;
    int idx = mTimeline->GetFrameIndex();
    polygon.append(QPoint(w * idx + 1 - mOffset, 0));
    polygon.append(QPoint(w * idx + 1 - mOffset, h / 2));
    polygon.append(QPoint(w * (idx + idx + 1) / 2 - mOffset, h));
    polygon.append(QPoint(w * (idx + 1) - 1 - mOffset, h / 2));
    polygon.append(QPoint(w * (idx + 1) - 1 - mOffset, 0));
    QPainterPath path;
    path.addPolygon(polygon);
    p.fillPath(path, QBrush(QColor(128,128,128)));
    p.drawPolygon(polygon);

    QString s;
    s.sprintf("%d", idx);
    p.setPen(QColor(255, 0, 0));
    p.drawText(idx * w + w - mOffset, h-5, s);
}

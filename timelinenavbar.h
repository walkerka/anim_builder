#ifndef TIMELINENAVBAR_H
#define TIMELINENAVBAR_H

#include <QWidget>

class Timeline;

class TimelineNavBar : public QWidget
{
    Q_OBJECT
public:
    explicit TimelineNavBar(Timeline* timeline, QWidget *parent = 0);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

signals:

public slots:


    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void paintEvent(QPaintEvent *);

private:
    int mCellWidth;
    Timeline* mTimeline;
};

#endif // TIMELINENAVBAR_H

#ifndef RINGCOLORPICKER_H
#define RINGCOLORPICKER_H

#include <QtWidgets>
#include <QPainter>
#include <QPainterPath>

class RingColorPicker : public QWidget
{
    Q_OBJECT
public:
    explicit RingColorPicker(QWidget *parent = 0);
    void SetColor(const QColor& color);
    const QColor& GetColor() const { return mColor; }
    void UpdateRectImage();

    void UpdateSatValue(const QPoint& pos);
    void UpdateHue(const QPoint& pos);
signals:
    void valueChanged( const QColor& );

public slots:


    // QWidget interface
public:
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

private:
    QColor mColor;

    QRect mRect;
    int mRingWidth;
    int mRectSize;
    int mRingSize;
    QImage* mRectImage;
    QPainterPath mRingPath;
    bool mDragRing;
    bool mDragRect;
};

#endif // RINGCOLORPICKER_H

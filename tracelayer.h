#ifndef TRACELAYER_H
#define TRACELAYER_H

#include "layer.h"
#include <QWidget>
#include <QUndoStack>
#include "cachedimage.h"

class Timeline;
class TraceLayer;
class TraceLayerModel;

class TracePropertyWindow : public QWidget
{
    Q_OBJECT
public:
    TracePropertyWindow(Timeline* timeline, TraceLayer* layer);
    ~TracePropertyWindow();

    public slots:
        void SetOpacity(int value);
        void Enable(bool value);
        void EnableOnion(bool value);

protected:
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

private:
    Timeline* mTimeline;
    TraceLayer* mLayer;
};

class TraceLayer : public Layer
{
    Q_OBJECT
public:
    explicit TraceLayer(Timeline* timeline, TraceLayerModel* layerModel, QWidget *parent = 0);
    ~TraceLayer();

    void SetFrame(int index, int x, int y);
    void RemoveFrame(int index);
    LayerType GetType() { return LayerTypeTrace; }
    void SetSelected(bool selected);
    void OnFrameChanged(int frameIndex);
    QImage* GetImage(int frameIndex);
    bool IsOnionEnabled();
    void EnableOnion(bool enable);
    unsigned char GetOpacity();
    bool IsEnabled();
    void Enable(bool enable);
    QWidget* GetPropertyWindow() { return mPropertyWindow; }
    int GetMaxFrames() { return mMaxFrames; }
    void UpdateMaxFrames();
    void Render(QPainter& painter);

private:
    int GetFrameIndex(int x, int y);

signals:

    public slots:
        void SetOpacity(unsigned char value);

public:
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    void SetUndoStack(QUndoStack* stack) { mUndoStack = stack; }

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
private:
    enum EditorState
    {
        EditorStateMove,
        EditorStateErase
    };

private:
    TraceLayerModel* mLayerModel;
    Timeline* mTimeline;
    QUndoStack* mUndoStack;
    bool mSelected;
    unsigned char mOpacity;
    bool mEnabled;
    bool mOnionEnabled;
    EditorState mState;
    TracePropertyWindow* mPropertyWindow;
    int mMaxFrames;
};

#endif // TRACELAYER_H

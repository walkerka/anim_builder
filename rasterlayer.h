#ifndef RASTERLAYER_H
#define RASTERLAYER_H

#include "layer.h"
#include <QWidget>
#include <QUndoStack>
#include "cachedimage.h"

class Timeline;
class RasterLayer;
class RasterLayerModel;
class RasterFrameModel;

struct RasterLayerFrame
{
    CachedImage* image;
    int exposure;
};

class RasterPropertyWindow : public QWidget
{
    Q_OBJECT
public:
    RasterPropertyWindow(Timeline* timeline, RasterLayer* layer);
    ~RasterPropertyWindow();

public slots:
    void SetOpacity(int value);
    void Enable(bool value);
    void EnableOnion(bool value);

protected:
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

private:
    Timeline* mTimeline;
    RasterLayer* mLayer;
};

class RasterLayer : public Layer
{
    Q_OBJECT
public:
    explicit RasterLayer(Timeline* timeline, RasterLayerModel* layerModel, QWidget *parent = 0);
    ~RasterLayer();

    void AddFrame(int insertIndex);
    void RemoveFrame(int index);
    void ModExposure(int index, int delta);
    LayerType GetType() { return LayerTypeRaster; }
    RasterFrameModel* GetFrameAt(int index);
    int GetPrevImageIndex(int index);
    int GetNextImageIndex(int index);
    void SetSelected(bool selected);
    void OnFrameChanged(int frameIndex);
    QImage* GetImage(int frameIndex);
    int GetWidth() const { return mWidth; }
    int GetHeight() const { return mHeight; }
    bool IsOnionEnabled();
    void EnableOnion(bool enable);
    unsigned char GetOpacity();
    bool IsEnabled();
    void Enable(bool enable);
    QWidget* GetPropertyWindow() { return mPropertyWindow; }
    int GetMaxFrames() { return mMaxFrames; }
    void UpdateMaxFrames();

private:
    std::vector<RasterFrameModel*>::iterator GetFrameIterator(int index);
    int GetImageIndexFromFrameIndex(int frameIndex);
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
        EditorStateScale
    };

private:
    RasterLayerModel* mLayerModel;
    Timeline* mTimeline;
    int mWidth;
    int mHeight;
    QUndoStack* mUndoStack;
    bool mSelected;
    unsigned char mOpacity;
    bool mEnabled;
    bool mOnionEnabled;
    EditorState mState;
    RasterFrameModel* mEditFrame;
    int mEditFrameIndex;
    int mEditFrameExposure;
    RasterPropertyWindow* mPropertyWindow;
    int mMaxFrames;
};

#endif // RASTERLAYER_H

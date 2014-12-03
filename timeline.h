#ifndef TIMELINE_H
#define TIMELINE_H

#include <QtWidgets>
#include <vector>
#include <QUndoStack>
#include "layer.h"

class TimelineNavBar;
class RasterImageEditor;
class RasterLayer;
class SceneModel;
class RasterLayerModel;

class Timeline : public QWidget
{
    Q_OBJECT
public:
    explicit Timeline(QWidget *parent = 0);
    virtual ~Timeline();

    void SetScene(SceneModel* scene);
    SceneModel* GetScene() { return mScene; }
    int GetMaxFrames() const { return mMaxFrames; }
    int GetFrameIndex() const { return mFrameIndex; }
    int GetLayerIndex() const { return mLayerIndex; }
    void MoveLayer(int modIndex);
    const QSize& GetCellSize() const { return mCellSize; }
    int GetFps();

    void SetMaxFrames(int value) { mMaxFrames = value; }
    void UpdateMaxFrames();
    Layer* GetLayerAt(int idx);

    void SetUndoStack(QUndoStack* stack) { mUndoStack = stack; }
    void SetEditor(RasterImageEditor* editor) { mEditor = editor; }
    RasterImageEditor* GetEditor() { return mEditor; }

    void UpdateCanvas();
    void SetLayer(Layer* l);
    void Render(QPainter& painter);

    void UpdateLayersUi();
    void ExportFrames(const QString& path);

    QImage* GetCompositeImage();
    QImage* GetCompositeImage(int index);
    int GetOffset() const { return mOffset; }

signals:

public slots:
    void AddRasterLayer(int index = -1);
    void AddTraceLayer(int index = -1);
    void AddSoundLayer(int index = -1);
    void RemoveLayer(int index = -1);
    void SetFrameIndex(int value);
    void ModFrameIndex(int delta);
    void SetLayerIndex(int index);
    void ModLayerIndex(int delta);
    void OnTimeScroll(int value);

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void paintEvent(QPaintEvent *);

private:
    RasterImageEditor* mEditor;
    SceneModel* mScene;
    int mFrameIndex;
    int mLayerIndex;
    int mMaxFrames;
    QSize mCellSize;
    TimelineNavBar* mNavBar;
    std::vector<Layer*> mLayers;
    RasterLayer* mFrameLayer;
    QUndoStack* mUndoStack;
    QWidget* mPropertyPanel;
    QWidget* mTimeLinePanel;
    QScrollBar* mTimeScroll;
    int mOffset;
    QImage* mCompositeImage;
};

#endif // TIMELINE_H

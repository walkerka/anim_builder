#ifndef RASTERIMAGEEDITOR_H
#define RASTERIMAGEEDITOR_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <vector>
#include <QGLWidget>
#include <QTime>
#include "StrokePoint.h"
#include <QUndoStack>
#include "canvastool.h"

class PanTool;
class ZoomTool;
class RotateTool;
class ColorTool;
class RegionTool;
class Timeline;
class GLTexture;
class GLRenderTarget;
class GLRenderer;
class GLShape;

class RasterImageEditor : public QGLWidget
{
    Q_OBJECT
public:
    explicit RasterImageEditor(QWidget *parent = 0);
    virtual ~RasterImageEditor();

    QImage* GetImage() { return mImage; }
    void Load(QImage* image);
    void SetUndoStack(QUndoStack* stack) { mUndoStack = stack; }
    void SetTool(CanvasTool* tool);
    QPoint GetTranslate() const { return mTranslate; }
    void SetTranslate(int x, int y) { mTranslate.setX(x); mTranslate.setY(y); }
    void ModTranslate(int x, int y);
    float GetScale() const { return mScale; }
    void SetScale(float value) { mScale = value; }
    void ModScale(float value) { mScale *= (1.0f + value); }
    float GetRotate() const { return mRotate; }
    void SetRotate(float value) { mRotate = value; }
    void ModRotate(float value) { mRotate += value; }
    StrokePoint ScreenToLocal(int x, int y, float pressure);
    QPoint LocalToScreen(int x, int y);
    Timeline* GetTimeline() { return mTimeline; }
    void SetTimeline(Timeline* t) { mTimeline = t; }
    bool IsOnionEnabled() const { return mShowOnionSkin; }

    void SetPanTool(PanTool* t) { mPanTool = t; }
    void SetZoomTool(ZoomTool* t) { mZoomTool = t; }
    void SetRotateTool(RotateTool* t) { mRotateTool = t; }
    void SetColorTool(ColorTool* t) { mColorTool = t; }
    void SetEraseTool(RegionTool* t) { mEraseTool = t; }


private:
    enum EditorState
    {
        EditorStateTool,
        EditorStatePan,
        EditorStateZoom,
        EditorStateRotate,
        EditorStateColor,
        EditorStateErase
    };

signals:

public slots:
    void Clear();
    void ToggleOnionSkin();
    void Identity();

protected:
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
    void tabletEvent(QTabletEvent *);
    void initializeGL();
    void resizeGL(int w, int h);


private:
    CanvasTool* mTool;
    QImage* mImage;
    float mTempPressure;
    QUndoStack* mUndoStack;
    bool mPanKeyDown;
    bool mZoomKeyDown;
    bool mRotateKeyDown;
    bool mColorKeyDown;
    bool mEraseKeyDown;
    EditorState mEditorState;
    QPoint mTranslate;
    float mScale;
    float mRotate;
    bool mShowOnionSkin;
    PanTool* mPanTool;
    ZoomTool* mZoomTool;
    RotateTool* mRotateTool;
    ColorTool* mColorTool;
    RegionTool* mEraseTool;
    Timeline* mTimeline;

    QTime mFpsClock;
    int mFps;
    int mFrameCounter;
    int mCurrentFrame;
    std::map<int, GLTexture*> mFrames;
    std::vector<GLRenderTarget*> mTargets;
    GLRenderer* mRenderer;
    GLShape* mShape;

};

#endif // RASTERIMAGEEDITOR_H

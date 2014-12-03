#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include "openglwindow.h"
#include <QtWidgets>
#include <QTime>
#include <vector>
#include <QGLWidget>

class GLTexture;
class GLRenderTarget;
class GLRenderer;
class GLShape;

class RenderWindow : public QGLWidget
{
public:
    RenderWindow(QWidget *parent = 0);
    ~RenderWindow();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void LoadImage(int index, QImage* image);
    void SetCurrentFrame(int index);

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void tabletEvent(QTabletEvent *);
    void resizeEvent(QResizeEvent *);

private:
    QTime mFpsClock;
    int mFps;
    int mFrameCounter;
    int mCurrentFrame;
    std::map<int, GLTexture*> mFrames;
    std::vector<GLRenderTarget*> mTargets;
    GLRenderer* mRenderer;
    GLShape* mShape;
};

#endif // RENDERWINDOW_H

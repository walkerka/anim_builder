#include "renderwindow.h"
#include "openglrenderer.h"


RenderWindow::RenderWindow(QWidget *parent)
    :QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
    ,mFps(0)
    ,mFrameCounter(0)
    ,mCurrentFrame(0)
    ,mRenderer(0)
{
}

RenderWindow::~RenderWindow()
{
    for (size_t i = 0; i < mTargets.size(); ++i)
    {
        delete mTargets[i];
    }

    delete mRenderer;
}

void RenderWindow::mousePressEvent(QMouseEvent *)
{
}

void RenderWindow::mouseReleaseEvent(QMouseEvent *)
{
}

void RenderWindow::mouseMoveEvent(QMouseEvent *)
{
}

void RenderWindow::tabletEvent(QTabletEvent *)
{
}

void RenderWindow::resizeEvent(QResizeEvent *)
{
}

void RenderWindow::LoadImage(int index, QImage* image)
{
    if (!image)
    {
        return;
    }

    GLTexture* texture = mRenderer->CreateTexture(image->width(), image->height(), (int*)image->bits(), true);
    mFrames[index] = texture;
}

void RenderWindow::SetCurrentFrame(int index)
{
    mCurrentFrame = index;
}


void RenderWindow::initializeGL()
{
    mRenderer = new GLRenderer();
    mRenderer->SetScreenSize(width(), height());

    for (size_t i = 0; i < 4; ++i)
    {
        mTargets.push_back(mRenderer->CreateTarget(width(), height(), 0, false));
    }

    mFpsClock.start();

    mShape = new GLShape();
    mShape->SetColor(0, 0, 1, 0.5f);
    mShape->AddCircle(100, 100, 50);
    mShape->AddCircle(30, 100, 10);
    mShape->AddCircle(40, 100, 20);
    mShape->AddLineV(10, 10, 10, 80, 30, 20);
    mShape->AddRect(0, 0, 1280, 720);
    mShape->AddLine(0, 0, 30, 900);
    mShape->Build();
}

void RenderWindow::resizeGL(int w, int h)
{
    if (mRenderer)
    {
        mRenderer->SetScreenSize(w, h);
    }
}

void RenderWindow::paintGL()
{
    makeCurrent();

    glViewport(0, 0, width(), height());
    //glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    std::map<int, GLTexture*>::iterator it = mFrames.find(mCurrentFrame);
    if (it != mFrames.end())
    {
        GLTexture* tex = it->second;
        Matrix4 mvp = Matrix4::BuildOrtho(0, tex->GetWidth(), 0, tex->GetHeight(), -1000, 1000);

        mRenderer->Clear(mTargets[1], 0, 0, 0, 0);
        mRenderer->Clear(mTargets[0], 0, 0, 0, 0);

        glDisable(GL_BLEND);
        mRenderer->Blur(mTargets[1], mTargets[0], &mvp.m00, tex, 100);



        mRenderer->BlendSource(mTargets[0], tex, &mvp.m00, 1, 1, 1, 1);
        mRenderer->BlendMultipy(mTargets[0], tex, mTargets[1]->GetTexture(), &mvp.m00, 1, 1, 1, 1.0f);

        glEnable(GL_BLEND);

        mRenderer->BlendSource(NULL, mTargets[0]->GetTexture(), &mvp.m00, 1, 1, 1, 1);


        //mRenderer->Mask(NULL, tex, &mvp.m00, NULL, mTargets[1]->GetTexture(), 1);
        mRenderer->DrawShape(mShape, &mvp.m00);
    }

    mRenderer->UseProgram(0);

    ++mFrameCounter;
    QString fpsStr;
    if (mFpsClock.elapsed() >= 1000)
    {
        mFps = mFrameCounter;
        mFrameCounter = 0;
        mFpsClock.restart();

        fpsStr.sprintf("FPS: %d", mFps);
        this->setWindowTitle(fpsStr);
    }

}

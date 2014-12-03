#include "rasterimageeditor.h"
#include <QMouseEvent>
#include <list>
#include "command.h"
#include "pantool.h"
#include "zoomtool.h"
#include "rotatetool.h"
#include "colortool.h"
#include "regiontool.h"
#include "timeline.h"
#include "openglrenderer.h"


RasterImageEditor::RasterImageEditor(QWidget *parent)
    :QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
    ,mFps(0)
    ,mFrameCounter(0)
    ,mCurrentFrame(0)
    ,mRenderer(0)
{
    setMinimumSize(64, 64);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(false);

    mImage = NULL;
    mTempPressure = 1.0f;

    mZoomKeyDown = false;
    mPanKeyDown = false;
    mRotateKeyDown = false;
    mColorKeyDown = false;
    mEraseKeyDown = false;
    mEditorState = EditorStateTool;

    setFocusPolicy(Qt::ClickFocus);
    mShowOnionSkin = false;
    mTool = NULL;

    mScale = 1.0f;
    mRotate = 0.0f;
}

RasterImageEditor::~RasterImageEditor()
{
}

void RasterImageEditor::mousePressEvent(QMouseEvent *e)
{
    if (mPanKeyDown)
    {
        mEditorState = EditorStatePan;
    }
    else if (mZoomKeyDown)
    {
        mEditorState = EditorStateZoom;
    }
    else if (mRotateKeyDown)
    {
        mEditorState = EditorStateRotate;
    }
    else if (mColorKeyDown)
    {
        mEditorState = EditorStateColor;
    }
    else if (mEraseKeyDown)
    {
        mEditorState = EditorStateErase;
    }
    else
    {
        mEditorState = EditorStateTool;
    }

    switch (mEditorState)
    {
    case EditorStatePan:
        mPanTool->OnDragBegin(e->x(), e->y(), mTempPressure);
        break;
    case EditorStateZoom:
        mZoomTool->OnDragBegin(e->x(), e->y(), mTempPressure);
        break;
    case EditorStateRotate:
        mRotateTool->OnDragBegin(e->x(), e->y(), mTempPressure);
        break;
    case EditorStateColor:
        mColorTool->OnDragBegin(e->x(), e->y(), mTempPressure);
        break;
    case EditorStateErase:
        mEraseTool->OnDragBegin(e->x(), e->y(), mTempPressure);
        break;
    default:
        if (mTool)
        {
            mTool->OnDragBegin(e->x(), e->y(), mTempPressure);
        }
        break;
    }
}

void RasterImageEditor::mouseReleaseEvent(QMouseEvent *e)
{
    switch (mEditorState)
    {
    case EditorStatePan:
        mPanTool->OnDragEnd(e->x(), e->y(), mTempPressure);
        break;
    case EditorStateZoom:
        mZoomTool->OnDragEnd(e->x(), e->y(), mTempPressure);
        break;
    case EditorStateRotate:
        mRotateTool->OnDragEnd(e->x(), e->y(), mTempPressure);
        break;
    case EditorStateColor:
        mColorTool->OnDragEnd(e->x(), e->y(), mTempPressure);
        break;
    case EditorStateErase:
        mEraseTool->OnDragEnd(e->x(), e->y(), mTempPressure);
        break;
    default:
        if (mTool)
        {
            mTool->OnDragEnd(e->x(), e->y(), mTempPressure);
        }
        break;
    }
    mEditorState = EditorStateTool;
    mTempPressure = 1.0f;
}

void RasterImageEditor::mouseMoveEvent(QMouseEvent *e)
{
    switch (mEditorState)
    {
    case EditorStatePan:
        mPanTool->OnDrag(e->x(), e->y(), mTempPressure);
        break;
    case EditorStateZoom:
        mZoomTool->OnDrag(e->x(), e->y(), mTempPressure);
        break;
    case EditorStateRotate:
        mRotateTool->OnDrag(e->x(), e->y(), mTempPressure);
        break;
    case EditorStateColor:
        mColorTool->OnDrag(e->x(), e->y(), mTempPressure);
        break;
    case EditorStateErase:
        mEraseTool->OnDrag(e->x(), e->y(), mTempPressure);
        break;
    default:
        if (mTool)
        {
            mTool->OnDrag(e->x(), e->y(), mTempPressure);
        }
        break;
    }
}

void RasterImageEditor::keyPressEvent(QKeyEvent *e)
{
    mZoomKeyDown = false;
    mPanKeyDown = false;
    mRotateKeyDown = false;
    mColorKeyDown = false;
    mEraseKeyDown = false;

    if (e->key() == Qt::Key_Control)
    {
        mZoomKeyDown = true;
    }
    else if (e->key() == Qt::Key_Shift)
    {
        mRotateKeyDown = true;
    }
    else if (e->key() == Qt::Key_Space)
    {
        mPanKeyDown = true;
    }
    else if (e->key() == Qt::Key_Alt)
    {
        mColorKeyDown = true;
    }
    else if (e->key() == Qt::Key_E)
    {
        mEraseKeyDown = true;
    }
}

void RasterImageEditor::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Control)
    {
        mZoomKeyDown = false;
    }
    else if (e->key() == Qt::Key_Shift)
    {
        mRotateKeyDown = false;
    }
    else if (e->key() == Qt::Key_Space)
    {
        mPanKeyDown = false;
    }
    else if (e->key() == Qt::Key_Alt)
    {
        mColorKeyDown = false;
    }
    else if (e->key() == Qt::Key_E)
    {
        mEraseKeyDown = false;
    }
}

void RasterImageEditor::paintEvent(QPaintEvent *)
{
    makeCurrent();

    QPainter p(this);

    glClearColor(0.5, 0.5, 0.5, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    p.translate(mTranslate);
    p.scale(mScale, mScale);
    p.rotate(mRotate);

    if (mImage)
    {
        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.fillRect(0, 0, mImage->width(), mImage->height(), QColor(0xFF, 0xFF, 0xFF, 0xFF));
    }

    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.setRenderHint(QPainter::Antialiasing, true);
    if (mScale != 1.0f || mRotate != 0.0f)
    {
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    }

    mTimeline->Render(p);

    switch (mEditorState)
    {
    case EditorStatePan:
        mPanTool->OnPaint(p);
        break;
    case EditorStateZoom:
        mZoomTool->OnPaint(p);
        break;
    case EditorStateRotate:
        mRotateTool->OnPaint(p);
        break;
    case EditorStateColor:
        mColorTool->OnPaint(p);
        break;
    case EditorStateErase:
        mEraseTool->OnPaint(p);
        break;
    default:
        if (mTool)
        {
            mTool->OnPaint(p);
        }
        break;
    }


    ++mFrameCounter;
    QString fpsStr;
    if (mFpsClock.elapsed() >= 1000)
    {
        mFps = mFrameCounter;
        mFrameCounter = 0;
        mFpsClock.restart();
    }
    fpsStr.sprintf("FPS: %d", mFps);

//    p.end();
}

void RasterImageEditor::tabletEvent(QTabletEvent *e)
{
    QWidget::tabletEvent(e);
    mTempPressure = e->pressure();
//    QPoint offset = this->pos();
//    offset = parentWidget()->mapToGlobal(offset);
//    StrokePoint pt;
//    pt.x = e->hiResGlobalX() - offset.x();
//    pt.y = e->hiResGlobalY() - offset.y();
//    pt.pressure = e->pressure();

//    printf("tablet x=%f, y=%f, p=%f\n", pt.x, pt.y, pt.pressure);fflush(stdout);

//    if (mPoints.size() > 0)
//    {
//        if (mPoints.back().x != pt.x || mPoints.back().y != pt.y)
//        {
//            mPoints.push_back(pt);
//            DrawLastStroke();
//        }
//    }
//    else
//    {
//        mPoints.push_back(pt);
//        DrawLastStroke();
//    }
}

void RasterImageEditor::SetTool(CanvasTool* tool)
{
    mTool = tool;
}

StrokePoint RasterImageEditor::ScreenToLocal(int x, int y, float pressure)
{
    QTransform p;
    p.translate(mTranslate.x(), mTranslate.y());
    p.scale(mScale, mScale);
    p.rotate(mRotate);
    p = p.inverted();

    StrokePoint sp;
    QPointF position = p.map(QPointF(x, y));
    sp.x = position.x();
    sp.y = position.y();
    sp.pressure = pressure;
    return sp;
}

QPoint RasterImageEditor::LocalToScreen(int x, int y)
{
    QTransform p;
    p.translate(mTranslate.x(), mTranslate.y());
    p.scale(mScale, mScale);
    p.rotate(mRotate);

    QPointF position = p.map(QPointF(x, y));
    return QPoint((int)position.x(), (int)position.y());
}

void RasterImageEditor::Clear()
{
    QImage* oldImage = new QImage(mImage->width(),mImage->height(), QImage::Format_RGBA8888);
    QImage* newImage = new QImage(mImage->width(),mImage->height(), QImage::Format_RGBA8888);

    QPainter hp(oldImage);
    hp.setCompositionMode(QPainter::CompositionMode_Source);
    hp.drawImage(0, 0, *mImage);
    mUndoStack->push(new DrawCommand(this, newImage, oldImage));

}

void RasterImageEditor::Load(QImage* image)
{
    mImage = image;
    update();
}

void RasterImageEditor::ToggleOnionSkin()
{
    mShowOnionSkin = !mShowOnionSkin;
    update();
}

void RasterImageEditor::ModTranslate(int x, int y)
{
    mTranslate += QPoint(x, y);
}

void RasterImageEditor::Identity()
{
    mTranslate.setX(0);
    mTranslate.setY(0);
    mScale = 1.0f;
    mRotate = 0.0f;
    update();
}


void RasterImageEditor::initializeGL()
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

void RasterImageEditor::resizeGL(int w, int h)
{
    if (mRenderer)
    {
        mRenderer->SetScreenSize(w, h);
    }
}

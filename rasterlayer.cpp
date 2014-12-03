#include "rasterlayer.h"
#include <QPainter>
#include <QWidget>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QSlider>
#include <QCheckBox>
#include "timeline.h"
#include "command.h"
#include "animationfile.h"

RasterPropertyWindow::RasterPropertyWindow(Timeline* timeline, RasterLayer* layer)
    :mTimeline(timeline)
    ,mLayer(layer)
{
    setLayout(new QHBoxLayout);
    layout()->setMargin(0);
    layout()->setSpacing(0);

    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setMaximum(255);
    slider->setValue(255);
    layout()->addWidget(slider);
    connect(slider, SIGNAL(valueChanged(int)),
            this, SLOT(SetOpacity(int)));

    QCheckBox* enableBox = new QCheckBox;
    enableBox->setIcon(QIcon(":/icons/visible.png"));
    enableBox->setChecked(true);
    connect(enableBox, SIGNAL(toggled(bool)),
            this, SLOT(Enable(bool)));
    layout()->addWidget(enableBox);

    QCheckBox* onionBox = new QCheckBox;
    onionBox->setIcon(QIcon(":/icons/onion.png"));
    onionBox->setChecked(false);
    connect(onionBox, SIGNAL(toggled(bool)),
            this, SLOT(EnableOnion(bool)));
    layout()->addWidget(onionBox);

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}

RasterPropertyWindow::~RasterPropertyWindow()
{

}

QSize RasterPropertyWindow::sizeHint() const
{
    return QSize(200, mTimeline->GetCellSize().height());
}

QSize RasterPropertyWindow::minimumSizeHint() const
{
    return QSize(200, mTimeline->GetCellSize().height());
}

void RasterPropertyWindow::SetOpacity(int value)
{
    mLayer->SetOpacity((unsigned char) value);
}

void RasterPropertyWindow::Enable(bool value)
{
    mLayer->Enable(value);
}

void RasterPropertyWindow::EnableOnion(bool value)
{
    mLayer->EnableOnion(value);
}

RasterLayer::RasterLayer(Timeline* timeline, RasterLayerModel* layerModel, QWidget *parent) :
    Layer(parent),
    mLayerModel(layerModel),
    mTimeline(timeline),
    mWidth(layerModel->GetWidth()),
    mHeight(layerModel->GetHeight()),
    mSelected(false),
    mOpacity(0xFF),
    mEnabled(true),
    mOnionEnabled(false),
    mState(EditorStateMove),
    mEditFrame(NULL),
    mEditFrameIndex(0),
    mEditFrameExposure(1),
    mMaxFrames(0)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    mPropertyWindow = new RasterPropertyWindow(timeline, this);
    UpdateMaxFrames();
    mTimeline->UpdateCanvas();
}

RasterLayer::~RasterLayer()
{
}

QSize RasterLayer::sizeHint() const
{
    return QSize(mTimeline->GetCellSize().width() * mMaxFrames, mTimeline->GetCellSize().height());
}

QSize RasterLayer::minimumSizeHint() const
{
    return QSize(mTimeline->GetCellSize().width() * mMaxFrames, mTimeline->GetCellSize().height());
}

void RasterLayer::mousePressEvent(QMouseEvent *ev)
{
    QPoint e(ev->x() + mTimeline->GetOffset(), ev->y());
    mTimeline->SetLayer(this);
    int frameIndex = GetFrameIndex(e.x(), e.y());

    mState = EditorStateMove;
    this->grabMouse();

    int n = (int)mLayerModel->GetFrames().size();
    if (frameIndex >= 0)
    {
        int idx = 0;
        int i = 0;
        for (i = 0; i < n; ++i)
        {
            RasterFrameModel* frame = mLayerModel->GetFrames()[i];
            int exposure = frame->GetExposure();
            if (idx + exposure - 1 >= frameIndex)
            {
                break;
            }
            idx += exposure;
        }

        if (i < n)
        {
            RasterFrameModel* frame = mLayerModel->GetFrames()[i];
            int xMax = (idx + frame->GetExposure()) * mTimeline->GetCellSize().width();
            int xMin = xMax - mTimeline->GetCellSize().width() / 2;
            if (e.x() >= xMin && e.x() <= xMax)
            {
                mState = EditorStateScale;
                mEditFrame = GetFrameAt(frameIndex);
                mEditFrameIndex = frameIndex;
                mEditFrameExposure = mEditFrame->GetExposure();
            }
        }
    }
    mTimeline->SetFrameIndex(frameIndex);
}

void RasterLayer::mouseReleaseEvent(QMouseEvent *e)
{
    this->releaseMouse();
}

void RasterLayer::mouseMoveEvent(QMouseEvent *ev)
{
    QPoint e(ev->x() + mTimeline->GetOffset(), ev->y());
    int frameIndex = GetFrameIndex(e.x(), e.y());
    if (mState == EditorStateScale)
    {
        if (mEditFrame)
        {
            int exposure = mEditFrameExposure + (frameIndex - mEditFrameIndex);
            if (exposure < 1)
            {
                exposure = 1;
            }
            mEditFrame->SetExposure(exposure);
            update();
            UpdateMaxFrames();
        }
    }
    else
    {
        mTimeline->SetFrameIndex(frameIndex);
    }
}

void RasterLayer::mouseDoubleClickEvent(QMouseEvent *e)
{
    int frameIndex = GetFrameIndex(e->x() + mTimeline->GetOffset(), e->y());
    AddFrame(frameIndex);
}

void RasterLayer::paintEvent(QPaintEvent *)
{
    QSize cellSize = mTimeline->GetCellSize();
    int offset = mTimeline->GetOffset();

    QPainter p(this);
    p.setPen(Qt::NoPen);
    if (mSelected)
    {
        p.setBrush(QBrush(QColor(250, 250, 250)));
    }
    else
    {
        p.setBrush(QBrush(QColor(200, 200, 200)));
    }

    p.drawRect(0, 0, width(), cellSize.height());

    int r = cellSize.width() - 4;
    int frameCount = (int)mLayerModel->GetFrames().size();
    int x = 0;
    int w = cellSize.width();
    int idxMin = mLayerModel->GetImageIndexFromFrameIndex(offset / w);
    int idxMax = mLayerModel->GetImageIndexFromFrameIndex((offset + width() + w) / w);
    int n = (int)mLayerModel->GetFrames().size();

    p.setPen(QPen(QColor(0, 0, 0)));
    for(int i = 0; i < n; ++i)
    {
        RasterFrameModel* frame = mLayerModel->GetFrames()[i];
        int exposure = frame->GetExposure();
        if (i >= idxMin && i <= idxMax)
        {
            p.drawRect(x - offset, 0, cellSize.width() * exposure, cellSize.height() - 1);
            p.drawEllipse(x - offset + 1, cellSize.height() / 2 - r / 2, r, r);
        }
        
        x += exposure * w;
    }
}

void RasterLayer::AddFrame(int frameIndex)
{
    mLayerModel->AddFrame(frameIndex);
    update();
    mTimeline->UpdateCanvas();
    UpdateMaxFrames();
}

void RasterLayer::RemoveFrame(int index)
{
    mLayerModel->RemoveFrame(index);
    update();
    mTimeline->UpdateCanvas();
    UpdateMaxFrames();
}

void RasterLayer::ModExposure(int index, int delta)
{
    mLayerModel->ModExposure(index, delta);
    update();
    UpdateMaxFrames();
}

RasterFrameModel* RasterLayer::GetFrameAt(int index)
{
    return mLayerModel->GetFrameAt(index);
}

std::vector<RasterFrameModel*>::iterator RasterLayer::GetFrameIterator(int index)
{
    int n = (int)mLayerModel->GetFrames().size();
    if (n == 0 || index < 0)
    {
        return mLayerModel->GetFrames().begin();
    }

    int idx = 0;
    for (int i = 0; i < n; ++i)
    {
        int exposure = mLayerModel->GetFrames()[i]->GetExposure();
        if (idx + exposure - 1 >= index)
        {
            std::vector<RasterFrameModel*>::iterator where = mLayerModel->GetFrames().begin();
            where += i;
            return where;
        }
        idx += exposure;
    }

    return mLayerModel->GetFrames().end();
}

int RasterLayer::GetImageIndexFromFrameIndex(int index)
{
    int n = (int)mLayerModel->GetFrames().size();
    if (n == 0 || index < 0)
    {
        return 0;
    }

    int idx = 0;
    for (int i = 0; i < n; ++i)
    {
        int exposure = mLayerModel->GetFrames()[i]->GetExposure();
        if (idx + exposure - 1 >= index)
        {
            return i;
        }
        idx += exposure;
    }

    return n - 1;
}

int RasterLayer::GetPrevImageIndex(int index)
{
    int n = (int)mLayerModel->GetFrames().size();
    if (n == 0)
    {
        return 0;
    }
    int imgIndex = GetImageIndexFromFrameIndex(index) - 1;
    if (imgIndex < 0)
    {
        imgIndex = n - 1;
    }

    int idx = 0;
    for (int i = 0; i < imgIndex; ++i)
    {
        idx += mLayerModel->GetFrames()[i]->GetExposure();
    }
    return idx;
}

int RasterLayer::GetNextImageIndex(int index)
{
    int n = (int)mLayerModel->GetFrames().size();
    if (n == 0)
    {
        return 0;
    }
    int imgIndex = GetImageIndexFromFrameIndex(index) + 1;
    if (imgIndex >= n)
    {
        imgIndex = 0;
    }

    int idx = 0;
    for (int i = 0; i < imgIndex; ++i)
    {
        idx += mLayerModel->GetFrames()[i]->GetExposure();
    }
    return idx;
}

void RasterLayer::SetSelected(bool selected)
{
    mSelected = selected;
    update();
}


void RasterLayer::OnFrameChanged(int frameIndex)
{

}

QImage* RasterLayer::GetImage(int frameIndex)
{
    return mLayerModel->GetImage(frameIndex);
}


bool RasterLayer::IsOnionEnabled()
{
    return mLayerModel->IsOnionEnabled();
}

void RasterLayer::EnableOnion(bool enable)
{
    if (mLayerModel->IsOnionEnabled() != enable)
    {
        mLayerModel->EnableOnion(enable);
        mTimeline->UpdateCanvas();
    }
}

unsigned char RasterLayer::GetOpacity()
{
    return mLayerModel->GetOpacity();
}

void RasterLayer::SetOpacity(unsigned char value)
{
    if (value != mLayerModel->GetOpacity())
    {
        mLayerModel->SetOpacity(value);
        mTimeline->UpdateCanvas();
    }
}

bool RasterLayer::IsEnabled()
{
    return mLayerModel->IsEnabled();
}

void RasterLayer::Enable(bool enable)
{
    if (mLayerModel->IsEnabled() != enable)
    {
        mLayerModel->Enable(enable);
        mTimeline->UpdateCanvas();
    }
}

int RasterLayer::GetFrameIndex(int x, int y)
{
    return x / mTimeline->GetCellSize().width();
}

void RasterLayer::UpdateMaxFrames()
{
    mMaxFrames = mLayerModel->GetMaxFrames();
    mTimeline->UpdateMaxFrames();
}

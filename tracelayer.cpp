#include "tracelayer.h"
#include <QPainter>
#include <QWidget>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QSlider>
#include <QCheckBox>
#include "timeline.h"
#include "command.h"
#include "animationfile.h"

TracePropertyWindow::TracePropertyWindow(Timeline* timeline, TraceLayer* layer)
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

TracePropertyWindow::~TracePropertyWindow()
{

}

QSize TracePropertyWindow::sizeHint() const
{
    return QSize(200, mTimeline->GetCellSize().height());
}

QSize TracePropertyWindow::minimumSizeHint() const
{
    return QSize(200, mTimeline->GetCellSize().height());
}

void TracePropertyWindow::SetOpacity(int value)
{
    mLayer->SetOpacity((unsigned char) value);
}

void TracePropertyWindow::Enable(bool value)
{
    mLayer->Enable(value);
}

void TracePropertyWindow::EnableOnion(bool value)
{
    mLayer->EnableOnion(value);
}

TraceLayer::TraceLayer(Timeline* timeline, TraceLayerModel* layerModel, QWidget *parent) :
    Layer(parent),
    mLayerModel(layerModel),
    mTimeline(timeline),
    mSelected(false),
    mOpacity(0xFF),
    mEnabled(true),
    mOnionEnabled(false),
    mState(EditorStateMove),
    mMaxFrames(0)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    mPropertyWindow = new TracePropertyWindow(timeline, this);
    UpdateMaxFrames();
    mTimeline->UpdateCanvas();
}

TraceLayer::~TraceLayer()
{
}

QSize TraceLayer::sizeHint() const
{
    return QSize(mTimeline->GetCellSize().width() * mMaxFrames, mTimeline->GetCellSize().height());
}

QSize TraceLayer::minimumSizeHint() const
{
    return QSize(mTimeline->GetCellSize().width() * mMaxFrames, mTimeline->GetCellSize().height());
}

void TraceLayer::mousePressEvent(QMouseEvent *ev)
{
    QPoint e(ev->x() + mTimeline->GetOffset(), ev->y());
    mTimeline->SetLayer(this);
    int frameIndex = GetFrameIndex(e.x(), e.y());

    mState = EditorStateMove;
    this->grabMouse();

    if (ev->button() == Qt::RightButton)
    {
        mState = EditorStateErase;
    }

    if (mState == EditorStateMove)
    {
        mTimeline->SetFrameIndex(frameIndex);
    }
    else if (mState == EditorStateErase)
    {
        mLayerModel->RemoveFrame(frameIndex);
        update();
    }
}

void TraceLayer::mouseReleaseEvent(QMouseEvent *e)
{
    this->releaseMouse();
}

void TraceLayer::mouseMoveEvent(QMouseEvent *ev)
{
    QPoint e(ev->x() + mTimeline->GetOffset(), ev->y());
    int frameIndex = GetFrameIndex(e.x(), e.y());
    if (mState == EditorStateMove)
    {
        mTimeline->SetFrameIndex(frameIndex);
    }
    else if (mState == EditorStateErase)
    {
        mLayerModel->RemoveFrame(frameIndex);
        update();
    }
}

void TraceLayer::mouseDoubleClickEvent(QMouseEvent *e)
{
    //int frameIndex = GetFrameIndex(e->x() + mTimeline->GetOffset(), e->y());
    //AddFrame(frameIndex);
}

void TraceLayer::paintEvent(QPaintEvent *)
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
    int w = cellSize.width();
    int idxMin = offset / w;
    int idxMax = (offset + width() + w) / w;

    p.setPen(QPen(QColor(0, 0, 0)));
    for(int i = idxMin; i <= idxMax; ++i)
    {
        int x = i * w;
        QPoint* pt = mLayerModel->GetFrameAt(i);
        p.drawRect(x - offset, 0, cellSize.width(), cellSize.height() - 1);
        if (pt)
        {
            p.drawEllipse(x - offset + 1, cellSize.height() / 2 - r / 2, r, r);
        }
    }
}

void TraceLayer::Render(QPainter& p)
{
    int range = 5;
    int index = mTimeline->GetFrameIndex();
    int idxMin = index - range;
    int idxMax = index;

    
    for(int i = idxMin; i <= idxMax; ++i)
    {
        QPoint* pt = mLayerModel->GetFrameAt(i);
        if (pt)
        {
            int d = i - index;
            d = d * d * 5;
            
            if (d == 0)
            {
                p.setBrush(QBrush(QColor(255, 0, 0)));
                p.drawEllipse(*pt, 6, 6);
            }
            else
            {
                p.setBrush(QBrush(QColor(d, d, d)));
            }
            
            p.drawEllipse(*pt, 4, 4);
            
        }
    }
}

void TraceLayer::SetFrame(int index, int x, int y)
{
    mLayerModel->SetFrame(index, x, y);
    update();
    mTimeline->UpdateCanvas();
    UpdateMaxFrames();
}

void TraceLayer::RemoveFrame(int index)
{
    mLayerModel->RemoveFrame(index);
}

void TraceLayer::SetSelected(bool selected)
{
    mSelected = selected;
    update();
}

void TraceLayer::OnFrameChanged(int frameIndex)
{

}

QImage* TraceLayer::GetImage(int frameIndex)
{
    return mLayerModel->GetImage(frameIndex);
}


bool TraceLayer::IsOnionEnabled()
{
    return mLayerModel->IsOnionEnabled();
}

void TraceLayer::EnableOnion(bool enable)
{
    if (mLayerModel->IsOnionEnabled() != enable)
    {
        mLayerModel->EnableOnion(enable);
        mTimeline->UpdateCanvas();
    }
}

unsigned char TraceLayer::GetOpacity()
{
    return mLayerModel->GetOpacity();
}

void TraceLayer::SetOpacity(unsigned char value)
{
    if (value != mLayerModel->GetOpacity())
    {
        mLayerModel->SetOpacity(value);
        mTimeline->UpdateCanvas();
    }
}

bool TraceLayer::IsEnabled()
{
    return mLayerModel->IsEnabled();
}

void TraceLayer::Enable(bool enable)
{
    if (mLayerModel->IsEnabled() != enable)
    {
        mLayerModel->Enable(enable);
        mTimeline->UpdateCanvas();
    }
}

int TraceLayer::GetFrameIndex(int x, int y)
{
    return x / mTimeline->GetCellSize().width();
}

void TraceLayer::UpdateMaxFrames()
{
    mMaxFrames = mLayerModel->GetMaxFrames();
    mTimeline->UpdateMaxFrames();
}

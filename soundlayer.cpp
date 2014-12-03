#include "soundlayer.h"
#include "timeline.h"
#include <QPainter>
#include "wavfile.h"

SoundLayer::SoundLayer(Timeline* timeline, QWidget *parent) :
    Layer(parent),
    mTimeline(timeline),
    mFile(NULL),
    mWaveForm(NULL),
    mOffset(0),
    mSelected(false)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
}

SoundLayer::~SoundLayer()
{
    delete mFile;
    delete mWaveForm;
}

QSize SoundLayer::sizeHint() const
{
    return QSize(mTimeline->GetCellSize().width(), mTimeline->GetCellSize().height());
}

QSize SoundLayer::minimumSizeHint() const
{
    return QSize(mTimeline->GetCellSize().width(), mTimeline->GetCellSize().height());
}

void SoundLayer::mousePressEvent(QMouseEvent *)
{
}

void SoundLayer::mouseReleaseEvent(QMouseEvent *)
{
}

void SoundLayer::mouseMoveEvent(QMouseEvent *)
{
}

void SoundLayer::paintEvent(QPaintEvent *)
{
    QSize cellSize = mTimeline->GetCellSize();
    QPainter p(this);
    p.setPen(QPen(QColor(0, 0, 0)));
    p.setBrush(QBrush(QColor(200, 200, 200)));
    p.drawRect(0, 0, width(), cellSize.height());

    if (mWaveForm)
    {
        p.drawImage(mTimeline->GetCellSize().width() * mOffset, 0, *mWaveForm);
    }

    if (mSelected)
    {
        p.setPen(QPen(QColor(255, 255, 255)));
        p.setBrush(QBrush(Qt::NoBrush));
        p.drawRect(0, 0, width(), cellSize.height());
    }
}

void SoundLayer::Load(const QString& path)
{
    WavFile* file = new WavFile(path);
    if (file->IsReady())
    {
        int n = file->GetSampleCount();
        int samplesPerFrame = file->GetSampleRate() / mTimeline->GetFps();
        int frameNum = file->GetSampleCount() / samplesPerFrame;
        int h = mTimeline->GetCellSize().height();
        int w = mTimeline->GetCellSize().width();
        int iw = frameNum * w;

        if (mWaveForm)
        {
            delete mWaveForm;
        }

        mWaveForm = new QImage(iw, h, QImage::Format_Mono);
        QPainter p(mWaveForm);
        p.setBackground(Qt::NoBrush);
        p.eraseRect(0, 0, mWaveForm->width(), mWaveForm->height());
        for (int i = 0; i < iw; ++i)
        {
            int low = n * i / iw;
            int high = n * (i + 1) / iw;

            short vMin = file->GetSample(low);;
            short vMax = vMin;

            for (int j = low + 1; j < high; ++j)
            {
                short value = file->GetSample(j);
                if (value > vMax)
                {
                    vMax = value;
                }
                else if(value < vMin)
                {
                    vMin = value;
                }
            }

            p.drawLine(i, h / 2 + vMin * h / 2 / 0xFFFF, i, h / 2 + vMax * h / 2 / 0xFFFF);
        }

        if (mFile)
        {
            delete mFile;
            mFile = file;
        }
        mOffset = mTimeline->GetFrameIndex();
    }
}

void SoundLayer::SetSelected(bool selected)
{
    mSelected = selected;
}


void SoundLayer::OnFrameChanged(int frameIndex)
{
}


bool SoundLayer::IsOnionEnabled()
{
    return false;
}

void SoundLayer::EnableOnion(bool enable)
{
}

unsigned char SoundLayer::GetOpacity()
{
    return 0;
}

void SoundLayer::SetOpacity(unsigned char value)
{
}

bool SoundLayer::IsEnabled()
{
    return true;
}

void SoundLayer::Enable(bool enable)
{
}


QWidget *SoundLayer::GetPropertyWindow()
{
    return NULL;
}

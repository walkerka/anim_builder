#include "timeline.h"
#include <QPainter>
#include <QWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <QScrollBar>
#include "rasterlayer.h"
#include "tracelayer.h"
#include "soundlayer.h"
#include "timelinenavbar.h"
#include "rasterimageeditor.h"
#include <QScrollArea>
#include <QtWidgets>
#include "animationfile.h"

Timeline::Timeline(QWidget *parent) :
    QWidget(parent),
    mScene(NULL),
    mEditor(NULL),
    mFrameIndex(0),
    mLayerIndex(-1),
    mMaxFrames(0),
    mCellSize(8, 16),
    mOffset(0),
    mCompositeImage(NULL)
{
//    QVBoxLayout* l = new QVBoxLayout;
//    l->setSpacing(2);
//    l->setMargin(1);
//    setLayout(l);

//    QSplitter* splitter = new QSplitter(Qt::Horizontal);
//    l->addWidget(splitter);

//    QWidget* left = new QWidget();
//    QWidget* right = new QWidget();
//    left->setLayout(new QVBoxLayout);
//    right->setLayout(new QVBoxLayout);
//    left->layout()->setSpacing(0);
//    right->layout()->setSpacing(0);
//    splitter->addWidget(left);
//    splitter->addWidget(right);
//    splitter->setSizes( QList<int>() << 100 << 600 );

//    QLabel* dummy = new QLabel("Layers");
//    dummy->resize(left->width(), mCellSize.height());
//    left->layout()->addWidget(dummy);
//    mNavBar = new TimelineNavBar(this);
//    right->layout()->addWidget(mNavBar);

//    QScrollArea* propertyScroolPanel = new QScrollArea();
//    QScrollArea* timeLineScroolPanel = new QScrollArea();

//    left->layout()->addWidget(propertyScroolPanel);
//    right->layout()->addWidget(timeLineScroolPanel);

//    mPropertyPanel = new QWidget();
//    mTimeLinePanel = new QWidget();
//    mPropertyPanel->setLayout(new QVBoxLayout);
//    mTimeLinePanel->setLayout(new QVBoxLayout);
//    mPropertyPanel->layout()->setSpacing(2);
//    mTimeLinePanel->layout()->setSpacing(2);
//    mPropertyPanel->layout()->setContentsMargins(0, 0, 0, 1);
//    mTimeLinePanel->layout()->setContentsMargins(0, 0, 0, 1);

//    propertyScroolPanel->setWidget(mPropertyPanel);
//    propertyScroolPanel->setWidgetResizable(true);
//    timeLineScroolPanel->setWidget(mTimeLinePanel);
//    timeLineScroolPanel->setWidgetResizable(true);

    QHBoxLayout* l = new QHBoxLayout;
    l->setSpacing(0);
    l->setMargin(0);
    setLayout(l);

    QWidget* left = new QWidget();
    QWidget* right = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout();
    QVBoxLayout* rightLayout = new QVBoxLayout();
    left->setLayout(leftLayout);
    right->setLayout(rightLayout);
    leftLayout->setSpacing(0);
    leftLayout->setMargin(0);
    rightLayout->setSpacing(0);
    rightLayout->setMargin(0);
    QScrollBar* vScroll = new QScrollBar(Qt::Vertical);

    l->addWidget(left);
    l->addWidget(vScroll);
    l->addWidget(right, 1);

    QLabel* dummy = new QLabel("Layers");
    dummy->resize(left->width(), mCellSize.height());
    leftLayout->addWidget(dummy);
    mNavBar = new TimelineNavBar(this);
    rightLayout->addWidget(mNavBar);

    QWidget* propertyScroolPanel = new QWidget();
    QWidget* timeLineScroolPanel = new QWidget();
    mTimeScroll = new QScrollBar(Qt::Horizontal);

    leftLayout->addWidget(propertyScroolPanel, 1);
    rightLayout->addWidget(timeLineScroolPanel, 1);
    rightLayout->addWidget(mTimeScroll);

    mPropertyPanel = propertyScroolPanel;
    mTimeLinePanel = timeLineScroolPanel;
    mPropertyPanel->setLayout(new QVBoxLayout);
    mTimeLinePanel->setLayout(new QVBoxLayout);
    mPropertyPanel->layout()->setSpacing(2);
    mTimeLinePanel->layout()->setSpacing(2);
    mPropertyPanel->layout()->setContentsMargins(0, 0, 0, 1);
    mTimeLinePanel->layout()->setContentsMargins(0, 0, 0, 1);

    connect(mTimeScroll, SIGNAL(valueChanged(int)), this, SLOT(OnTimeScroll(int)));
}

Timeline::~Timeline()
{
    delete mCompositeImage;
}

void Timeline::mousePressEvent(QMouseEvent *)
{
}

void Timeline::mouseReleaseEvent(QMouseEvent *)
{
}

void Timeline::mouseMoveEvent(QMouseEvent *)
{
}

void Timeline::wheelEvent(QWheelEvent *)
{
}

void Timeline::paintEvent(QPaintEvent *)
{
    //QPainter p(this);
    //p.setRenderHint(QPainter::Antialiasing, true);
    //p.setBrush(QBrush(QColor(0xFF, 0, 0)));
    //p.drawRect(0, 0, this->width(), this->height());
}

void Timeline::SetScene(SceneModel* scene)
{
    if (mScene == scene)
    {
        return;
    }

    // unload all layers
    QVBoxLayout* l = (QVBoxLayout*)mTimeLinePanel->layout();
    while(QLayoutItem* item = l->itemAt(0))
    {
        l->removeItem(item);
    }

    l = (QVBoxLayout*)mPropertyPanel->layout();
    while(QLayoutItem* item = l->itemAt(0))
    {
        l->removeItem(item);
    }

    for (size_t i = 0; i < mLayers.size(); ++i)
    {
        Layer* layer = mLayers[i];
        delete layer->GetPropertyWindow();
        delete layer;
    }
    mLayers.clear();

    mScene = scene;
    if (mScene)
    {
        for (size_t i = 0; i < mScene->GetLayers().size(); ++i)
        {
            LayerModel* layerModel = mScene->GetLayers()[i];
            if (layerModel)
            {
                switch(layerModel->GetType())
                {
                    case LayerModel::LayerTypeRaster:
                        {
                            RasterLayerModel* rlm = (RasterLayerModel*)layerModel;
                            RasterLayer* layer = new RasterLayer(this, rlm);
                            mLayers.push_back(layer);
                        }
                        break;
                    case LayerModel::LayerTypeTrace:
                        {
                            TraceLayerModel* lm = (TraceLayerModel*)layerModel;
                            TraceLayer* layer = new TraceLayer(this, lm);
                            mLayers.push_back(layer);
                        }
                        break;
                }
            }
        }
        delete mCompositeImage;
        mCompositeImage = new QImage(scene->GetWidth(), scene->GetHeight(), QImage::Format_RGBA8888);
    }

    UpdateLayersUi();
    SetLayerIndex((int)mLayers.size() - 1);
    UpdateMaxFrames();
}

void Timeline::UpdateLayersUi()
{
    {
        QVBoxLayout* l = (QVBoxLayout*)mTimeLinePanel->layout();

        while(QLayoutItem* item = l->itemAt(0))
        {
            l->removeItem(item);
        }

        for (size_t i = 0; i < mLayers.size(); ++i)
        {
            l->addWidget(mLayers[mLayers.size() - 1 - i], 1);
        }
        l->addStretch();
    }

    {
        QVBoxLayout* l = (QVBoxLayout*)mPropertyPanel->layout();

        while(QLayoutItem* item = l->itemAt(0))
        {
            l->removeItem(item);
        }

        for (size_t i = 0; i < mLayers.size(); ++i)
        {
            l->addWidget(mLayers[mLayers.size() - 1 - i]->GetPropertyWindow());
        }
        l->addStretch();
    }

    update();
}

void Timeline::AddRasterLayer(int index)
{
    if (!mScene)
    {
        return;
    }

    if (index < 0 || index > mLayers.size())
    {
        index = mLayers.size();
    }

    int width = mScene->GetWidth();
    int height = mScene->GetHeight();
    QString name;
    name.sprintf("layer%d", mLayers.size());
    RasterLayerModel* layerModel = mScene->AddRasterLayer(index, name, width, height);
    if (layerModel)
    {
        RasterLayer* layer = new RasterLayer(this, layerModel);
        layer->SetUndoStack(mUndoStack);

        std::vector<Layer*>::iterator it = mLayers.begin();
        it += index;
        mLayers.insert(it, layer);
        UpdateLayersUi();
    }
}

void Timeline::AddTraceLayer(int index)
{
    if (!mScene)
    {
        return;
    }

    if (index < 0 || index > mLayers.size())
    {
        index = mLayers.size();
    }

    int width = mScene->GetWidth();
    int height = mScene->GetHeight();
    QString name;
    name.sprintf("layer%d", mLayers.size());
    TraceLayerModel* layerModel = mScene->AddTraceLayer(index, name);
    if (layerModel)
    {
        TraceLayer* layer = new TraceLayer(this, layerModel);
        layer->SetUndoStack(mUndoStack);

        std::vector<Layer*>::iterator it = mLayers.begin();
        it += index;
        mLayers.insert(it, layer);
        UpdateLayersUi();
    }
}

void Timeline::AddSoundLayer(int index)
{
//    if (index < 0 || index > mLayers.size())
//    {
//        index = mLayers.size();
//    }

//    SoundLayer* layer = new SoundLayer(this);
//    QVBoxLayout* l = (QVBoxLayout*)mTimeLinePanel->layout();
//    l->addWidget(layer);
//    l->setAlignment(layer, Qt::AlignTop);
//    mLayers.push_back(layer);
}

void Timeline::RemoveLayer(int index)
{
    if (index == -1)
    {
        index = mLayerIndex;
    }
    if (index < 0 || index >= (int)mLayers.size())
    {
        return;
    }
    std::vector<Layer*>::iterator it = mLayers.begin();
    it += index;
    Layer* l = *it;
    mLayers.erase(it);
    delete l;
    mScene->RemoveLayer(index);
    UpdateLayersUi();
}

void Timeline::MoveLayer(int modIndex)
{
    if (!mScene || modIndex == 0)
    {
        return;
    }

    int n = (int)mLayers.size();
    if (n < 2 || mLayerIndex < 0 || mLayerIndex >= n)
    {
        return;
    }

    int newIndex = mLayerIndex + modIndex;
    Layer* layer = mLayers[mLayerIndex];

    if (modIndex < 0)
    {
        if (newIndex < 0)
        {
            newIndex = 0;
        }

        for (int i = mLayerIndex; i > newIndex; --i)
        {
            mLayers[i] = mLayers[i - 1];
        }
    }
    else
    {
        if (newIndex >= n)
        {
            newIndex = n - 1;
        }

        for (int i = mLayerIndex; i < newIndex; ++i)
        {
            mLayers[i] = mLayers[i + 1];
        }
    }

    mScene->MoveLayer(mLayerIndex, newIndex);

    mLayers[newIndex] = layer;
    mLayerIndex = newIndex;

    UpdateLayersUi();
    mEditor->update();
}

int Timeline::GetFps()
{
    if (!mScene)
    {
        return 24;
    }
    return mScene->GetFps();
}

void Timeline::SetFrameIndex(int value)
{
    if (value != mFrameIndex)
    {
        mFrameIndex = value;
        UpdateCanvas();
    }
}

void Timeline::ModFrameIndex(int delta)
{
    int v = mFrameIndex + delta;
    if (v < 0)
    {
        v = 0;
    }
    SetFrameIndex(v);
}

void Timeline::SetLayerIndex(int index)
{
    if (!mScene || mLayers.size() == 0)
    {
        return;
    }

    if (index < 0)
    {
        index = 0;
    }
    else if (index >= (int)mLayers.size())
    {
        index = (int)mLayers.size() - 1;
    }

    if (index != mLayerIndex)
    {
        mLayerIndex = index;
        for (int i = 0; i < mLayers.size(); ++i)
        {
            mLayers[i]->SetSelected(i == index);
        }
        UpdateCanvas();
    }
}

void Timeline::SetLayer(Layer* l)
{
    for (size_t i = 0; i < mLayers.size(); ++i)
    {
        if (mLayers[i] == l)
        {
            SetLayerIndex(i);
            break;
        }
    }
}

void Timeline::ModLayerIndex(int delta)
{
    int v = mLayerIndex + delta;
    if (v < 0)
    {
        v = 0;
    }
    else if (v >= (int)mLayers.size())
    {
        v = (int)mLayers.size() - 1;
    }

    SetLayerIndex(v);
}

Layer* Timeline::GetLayerAt(int idx)
{
    if (idx < 0 || idx >= (int)mLayers.size())
    {
        return NULL;
    }
    return mLayers[idx];
}

void Timeline::ExportFrames(const QString& path)
{
    mScene->Export(path);
}

void Timeline::UpdateCanvas()
{
    std::vector<QImage*> layerImages;
    std::vector<QImage*> onions;
    QImage* editImage = NULL;

    for (size_t i = 0; i < mLayers.size(); ++i)
    {
        if (!mLayers[i]->IsEnabled())
        {
            continue;
        }
        QImage* img = mLayers[i]->GetImage(mFrameIndex);
        if (img)
        {
            if (mLayerIndex == i)
            {
                if (mLayers[i]->GetType() == LayerTypeRaster)
                {
                    RasterLayer* l = (RasterLayer*)mLayers[i];
                    QImage* prev = l->GetImage(l->GetPrevImageIndex(mFrameIndex));
                    if(prev && prev != img)
                    {
                        onions.push_back(prev);
                    }
                    QImage* next = l->GetImage(l->GetNextImageIndex(mFrameIndex));
                    if(next && next != img)
                    {
                        onions.push_back(next);
                    }
                }

                editImage = img;
            }
            layerImages.push_back(img);
        }
    }
    mEditor->Load(editImage);
    update();
}

void Timeline::Render(QPainter& painter)
{
    std::vector<QImage*> onions;

    for (size_t i = 0; i < mLayers.size(); ++i)
    {
        Layer* layer = mLayers[i];
        if (!layer->IsEnabled())
        {
            continue;
        }
        if (layer->GetType() == LayerTypeRaster)
        {
            QImage* img = mLayers[i]->GetImage(mFrameIndex);
            if (img)
            {
                if (mEditor->IsOnionEnabled() && mLayerIndex == (int)i)
                {
                    if (layer->GetType() == LayerTypeRaster && layer->IsOnionEnabled())
                    {
                        RasterLayer* l = (RasterLayer*)mLayers[i];
                        QImage* prev = l->GetImage(l->GetPrevImageIndex(mFrameIndex));
                        if(prev && prev != img)
                        {
                            onions.push_back(prev);
                        }
                        QImage* next = l->GetImage(l->GetNextImageIndex(mFrameIndex));
                        if(next && next != img)
                        {
                            onions.push_back(next);
                        }
                    }
                }

                painter.setOpacity(layer->GetOpacity() / 255.0f);
                painter.drawImage(0, 0, *img);
            }
        }
        else if (layer->GetType() == LayerTypeTrace)
        {
            TraceLayer* tl = (TraceLayer*)layer;
            tl->Render(painter);
        }
    }

    painter.setOpacity(0.25f);
    for (size_t i = 0; i < onions.size(); ++i)
    {
        painter.drawImage(0, 0, *onions[i]);
    }
}

void Timeline::UpdateMaxFrames()
{
    int maxFrames = 0;
    for (size_t i = 0; i < mLayers.size(); ++i)
    {
        Layer* layer = mLayers[i];
        int f = layer->GetMaxFrames();
        if (f > maxFrames)
        {
            maxFrames = f;
        }
    }
    mMaxFrames = maxFrames;
    mTimeScroll->setMaximum(maxFrames * mCellSize.width());
}

QImage* Timeline::GetCompositeImage()
{
    return GetCompositeImage(mFrameIndex);
}

QImage* Timeline::GetCompositeImage(int index)
{
    if (!mScene || !mCompositeImage)
    {
        return NULL;
    }

    mScene->GetCompositeImage(index, mCompositeImage);

    return mCompositeImage;
}

void Timeline::OnTimeScroll(int value)
{
    mOffset = value;
    update();
}

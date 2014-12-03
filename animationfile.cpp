#include "animationfile.h"
#include <QtWidgets>
#include <QtXml/QtXml>

//**************************************LayerModel**************************************
LayerModel::LayerModel(const QString& absPath, const QString& path, const QString& name, LayerType type)
    :mAbsPath(absPath)
    ,mPath(path)
    ,mName(name)
    ,mType(type)
{
}

LayerModel::~LayerModel()
{

}

//**************************************RasterFrameModel**************************************
RasterFrameModel::RasterFrameModel(RasterLayerModel* layer, const QString& absImagePath, const QString& imagePath, int exposure)
    :mLayer(layer)
    ,mAbsImagePath(absImagePath)
    ,mImagePath(imagePath)
    ,mExposure(exposure)
    ,mImage(NULL)
{
    QFile f(absImagePath);
    if (f.exists())
    {
        mImage = new QImage(absImagePath);
    }
    else
    {
        mImage = new QImage(layer->GetWidth(), layer->GetHeight(), QImage::Format_RGBA8888);
        QPainter p(mImage);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.fillRect(0, 0, mImage->width(), mImage->height(), QBrush(QColor(0xFF, 0xFF, 0xFF, 0)));
        mImage->save(mAbsImagePath);
    }
}

RasterFrameModel::~RasterFrameModel()
{
    delete mImage;
}

QImage* RasterFrameModel::GetImage()
{
    return mImage;
}

void RasterFrameModel::Save()
{
    if (!mImage)
    {
        return;
    }
    mImage->save(mAbsImagePath);
}

//**************************************RasterLayerModel**************************************
RasterLayerModel::RasterLayerModel(const QString& absPath, const QString& path, int width, int height)
    :LayerModel(absPath, path, "", LayerTypeRaster)
    ,mWidth(width)
    ,mHeight(height)
    ,mNextImageId(1)
    ,mOpacity(0xFF)
    ,mEnabled(true)
    ,mOnionEnabled(false)
{

}

RasterLayerModel::~RasterLayerModel()
{
    for (size_t i = 0; i < mFrames.size(); ++i)
    {
        delete mFrames[i];
    }
}

RasterLayerModel* RasterLayerModel::New(const QString& absPath, const QString& path, int width, int height)
{
    QDir dir(absPath);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    QFile file(absPath + "/layer.xml");
    if (!file.open(QIODevice::WriteOnly))
    {
        return NULL;
    }

    QDomDocument doc("");
    QDomElement root = doc.createElement("layer");
    root.setAttribute("version", "1.0");
    root.setAttribute("type", (int)LayerTypeRaster);
    root.setAttribute("width", width);
    root.setAttribute("height", height);
    root.setAttribute("height", height);
    doc.appendChild(root);

    RasterLayerModel* layer = new RasterLayerModel(absPath, path, width, height);
    if (!layer)
    {
        return NULL;
    }

    char imgPath[300];
    sprintf(imgPath, "%d.png", layer->mNextImageId++);
    RasterFrameModel* frame = new RasterFrameModel(layer, absPath + "\\" + imgPath, imgPath, 1);
    layer->mFrames.push_back(frame);

    QDomElement fd = doc.createElement("frame");
    fd.setAttribute("version", "1.0");
    fd.setAttribute("exposure", frame->GetExposure());
    fd.setAttribute("imagePath", imgPath);
    root.appendChild(fd);
    root.setAttribute("nextImageId", layer->mNextImageId);

    QTextStream stream(&file);
    doc.save(stream, 4);
    file.close();

    return layer;
}

RasterLayerModel* RasterLayerModel::Open(const QString& absPath, const QString& path)
{
    QDir dir(absPath);
    if (!dir.exists())
    {
        return NULL;
    }

    QFile file(absPath + "/layer.xml");
    if (!file.open(QIODevice::ReadOnly))
    {
        return NULL;
    }

    QDomDocument doc("");
    if (!doc.setContent(&file)) {
        file.close();
        return NULL;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    if (docElem.isNull())
    {
        return NULL;
    }
    int width = docElem.attribute("width", "1").toInt();
    int height = docElem.attribute("height", "1").toInt();

    RasterLayerModel* layer = new RasterLayerModel(absPath, path, width, height);
    if (!layer)
    {
        return NULL;
    }
    layer->mNextImageId = docElem.attribute("nextImageId").toInt();

    const QDomNodeList& nodes = docElem.elementsByTagName("frame");
    for (int i = 0; i < nodes.size(); ++i)
    {
        const QDomElement& n = nodes.at(i).toElement();
        if (n.isNull())
        {
            continue;
        }
        QString imagePath = n.attribute("imagePath");
        int exposure = n.attribute("exposure").toInt();
        RasterFrameModel* frame = new RasterFrameModel(layer, absPath + "/" + imagePath, imagePath, exposure);
        layer->mFrames.push_back(frame);
    }

    return layer;
}

void RasterLayerModel::Save()
{
    QDir dir(mAbsPath);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    QFile file(mAbsPath + "/layer.xml");
    if (!file.open(QIODevice::WriteOnly))
    {
        return;
    }

    QDomDocument doc("");
    QDomElement root = doc.createElement("layer");
    root.setAttribute("version", "1.0");
    root.setAttribute("type", (int)LayerTypeRaster);
    root.setAttribute("width", mWidth);
    root.setAttribute("height", mHeight);
    root.setAttribute("nextImageId", mNextImageId);
    doc.appendChild(root);

    for (size_t i = 0; i < mFrames.size(); ++i)
    {
        RasterFrameModel* frame = mFrames[i];
        frame->Save();

        QDomElement elem = doc.createElement("frame");
        elem.setAttribute("version", "1.0");
        elem.setAttribute("exposure", frame->GetExposure());
        elem.setAttribute("imagePath", frame->GetImagePath());
        root.appendChild(elem);
    }

    QTextStream stream(&file);
    doc.save(stream, 4);
    file.close();
}

void RasterLayerModel::AddFrame(int frameIndex)
{
    QString path;
    path.sprintf("%d.png", mNextImageId++);
    RasterFrameModel* frame = new RasterFrameModel(this, mAbsPath + "/" + path, path);

    int n = (int)mFrames.size();
    int idx = 0;
    int i = 0;
    for (i = 0; i < n; ++i)
    {
        int exposure = mFrames[i]->GetExposure();
        if (idx + exposure - 1 >= frameIndex)
        {
            break;
        }
        idx += exposure;
    }

    if (i == n)
    {
        if (n == 0 && frameIndex > 0)
        {
            // Add a blank image before
            QString path;
            path.sprintf("%d.png", mNextImageId++);
            RasterFrameModel* frame2 = new RasterFrameModel(this, mAbsPath + "/" + path, path);
            mFrames.push_back(frame2);
        }

        if (frameIndex > idx)
        {
            mFrames.back()->SetExposure(frameIndex - idx + 1);
        }
        mFrames.push_back(frame);
    }
    else
    {
        int exp = mFrames[i]->GetExposure();
        if (exp > 1 && frameIndex > idx)
        {
            // inside frame
            // split frame and insert after
            std::vector<RasterFrameModel*>::iterator where = mFrames.begin();
            where += i + 1;
            int oldExp = frameIndex - idx;
            int newExp = exp - oldExp;
            mFrames[i]->SetExposure(oldExp);
            frame->SetExposure(newExp);
            mFrames.insert(where, frame);
        }
        else
        {
            // at the beginning of frame
            // insert new frame before
            std::vector<RasterFrameModel*>::iterator where = mFrames.begin();
            where += i;
            mFrames.insert(where, frame);
        }
    }
}

void RasterLayerModel::RemoveFrame(int index)
{
    if (mFrames.size() == 0)
    {
        return;
    }
    int imgIndex = GetImageIndexFromFrameIndex(index);
    std::vector<RasterFrameModel*>::iterator where = mFrames.begin();
    where += imgIndex;
    RasterFrameModel* frame = *where;
    mFrames.erase(where);
    delete frame;
}

void RasterLayerModel::ModExposure(int index, int delta)
{
    if (mFrames.size() == 0)
    {
        return;
    }
    int imgIndex = GetImageIndexFromFrameIndex(index);
    RasterFrameModel* frame = mFrames[imgIndex];
    int exposure = frame->GetExposure() + delta;
    if (exposure < 1)
    {
        exposure = 1;
    }
    frame->SetExposure(exposure);
}

RasterFrameModel* RasterLayerModel::GetFrameAt(int index)
{
    if (mFrames.size() == 0)
    {
        return NULL;
    }
    int imgIndex = GetImageIndexFromFrameIndex(index);
    return mFrames[imgIndex];
}

int RasterLayerModel::GetImageIndexFromFrameIndex(int index)
{
    int n = (int)mFrames.size();
    if (n == 0 || index < 0)
    {
        return 0;
    }

    int idx = 0;
    for (int i = 0; i < n; ++i)
    {
        int exposure = mFrames[i]->GetExposure();
        if (idx + exposure - 1 >= index)
        {
            return i;
        }
        idx += exposure;
    }

    return n - 1;
}

int RasterLayerModel::GetPrevImageIndex(int index)
{
    int n = (int)mFrames.size();
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
        idx += mFrames[i]->GetExposure();
    }
    return idx;
}

int RasterLayerModel::GetNextImageIndex(int index)
{
    int n = (int)mFrames.size();
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
        idx += mFrames[i]->GetExposure();
    }
    return idx;
}

QImage* RasterLayerModel::GetImage(int frameIndex)
{
    if (mFrames.size() == 0)
    {
        return NULL;
    }
    int idx = GetImageIndexFromFrameIndex(frameIndex);
    return mFrames[idx]->GetImage();
}


bool RasterLayerModel::IsOnionEnabled()
{
    return mOnionEnabled;
}

void RasterLayerModel::EnableOnion(bool enable)
{
    mOnionEnabled = enable;
}

unsigned char RasterLayerModel::GetOpacity()
{
    return mOpacity;
}

void RasterLayerModel::SetOpacity(unsigned char value)
{
    mOpacity = value;
}

bool RasterLayerModel::IsEnabled()
{
    return mEnabled;
}

void RasterLayerModel::Enable(bool enable)
{
    mEnabled = enable;
}

int RasterLayerModel::GetMaxFrames()
{
    int maxFrames = 0;
    for (size_t i = 0; i < mFrames.size(); ++i)
    {
        maxFrames += mFrames[i]->GetExposure();
    }
    return maxFrames;
}


//**************************************TraceLayerModel**************************************
TraceLayerModel::TraceLayerModel(const QString& absPath, const QString& path)
    :LayerModel(absPath, path, "", LayerTypeTrace)
    ,mOpacity(0xFF)
    ,mEnabled(true)
    ,mOnionEnabled(false)
    ,mMaxFrames(0)
{

}

TraceLayerModel::~TraceLayerModel()
{
}

TraceLayerModel* TraceLayerModel::New(const QString& absPath, const QString& path)
{
    QDir dir(absPath);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    QFile file(absPath + "/layer.xml");
    if (!file.open(QIODevice::WriteOnly))
    {
        return NULL;
    }

    QDomDocument doc("");
    QDomElement root = doc.createElement("layer");
    root.setAttribute("version", "1.0");
    root.setAttribute("type", (int)LayerTypeTrace);
    doc.appendChild(root);

    TraceLayerModel* layer = new TraceLayerModel(absPath, path);
    if (!layer)
    {
        return NULL;
    }

    QTextStream stream(&file);
    doc.save(stream, 4);
    file.close();

    return layer;
}

TraceLayerModel* TraceLayerModel::Open(const QString& absPath, const QString& path)
{
    QDir dir(absPath);
    if (!dir.exists())
    {
        return NULL;
    }

    QFile file(absPath + "/layer.xml");
    if (!file.open(QIODevice::ReadOnly))
    {
        return NULL;
    }

    QDomDocument doc("");
    if (!doc.setContent(&file)) {
        file.close();
        return NULL;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    if (docElem.isNull())
    {
        return NULL;
    }

    TraceLayerModel* layer = new TraceLayerModel(absPath, path);
    if (!layer)
    {
        return NULL;
    }

    const QDomNodeList& nodes = docElem.elementsByTagName("frame");
    for (int i = 0; i < nodes.size(); ++i)
    {
        const QDomElement& n = nodes.at(i).toElement();
        if (n.isNull())
        {
            continue;
        }
        
        int index = n.attribute("index").toInt();
        int x = n.attribute("x").toInt();
        int y = n.attribute("y").toInt();
        layer->mFrames[index] = QPoint(x, y);
    }

    return layer;
}

void TraceLayerModel::Save()
{
    QDir dir(mAbsPath);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    QFile file(mAbsPath + "/layer.xml");
    if (!file.open(QIODevice::WriteOnly))
    {
        return;
    }

    QDomDocument doc("");
    QDomElement root = doc.createElement("layer");
    root.setAttribute("version", "1.0");
    root.setAttribute("type", (int)LayerTypeTrace);
    doc.appendChild(root);

    for (std::map<int, QPoint>::iterator it = mFrames.begin(); it != mFrames.end(); ++it)
    {
        QDomElement elem = doc.createElement("frame");
        elem.setAttribute("index", it->first);
        elem.setAttribute("x", it->second.x());
        elem.setAttribute("y", it->second.y());
        root.appendChild(elem);
    }

    QTextStream stream(&file);
    doc.save(stream, 4);
    file.close();
}

void TraceLayerModel::SetFrame(int index, int x, int y)
{
    mFrames[index] = QPoint(x, y);
    if (mMaxFrames < index)
    {
        mMaxFrames = index;
    }
}

void TraceLayerModel::RemoveFrame(int index)
{
    FrameList::iterator it = mFrames.find(index);
    if (it != mFrames.end())
    {
        mFrames.erase(it);
    }
}

QPoint* TraceLayerModel::GetFrameAt(int index)
{
    std::map<int, QPoint>::iterator it = mFrames.find(index);
    if (it == mFrames.end())
    {
        return NULL;
    }
    return &it->second;
}

QImage* TraceLayerModel::GetImage(int frameIndex)
{
    return NULL;
}


bool TraceLayerModel::IsOnionEnabled()
{
    return mOnionEnabled;
}

void TraceLayerModel::EnableOnion(bool enable)
{
    mOnionEnabled = enable;
}

unsigned char TraceLayerModel::GetOpacity()
{
    return mOpacity;
}

void TraceLayerModel::SetOpacity(unsigned char value)
{
    mOpacity = value;
}

bool TraceLayerModel::IsEnabled()
{
    return mEnabled;
}

void TraceLayerModel::Enable(bool enable)
{
    mEnabled = enable;
}

//**************************************SceneModel**************************************
SceneModel::SceneModel(const QString& absPath, const QString& path, int width, int height, int fps)
    :mAbsPath(absPath)
    ,mPath(path)
    ,mWidth(width)
    ,mHeight(height)
    ,mFps(fps)
{

}

SceneModel::~SceneModel()
{
    for (size_t i = 0; i < mLayers.size(); ++i)
    {
        delete mLayers[i];
    }

    for (size_t i = 0; i < mCompositeImages.size(); ++i)
    {
        delete mCompositeImages[i];
    }
}

SceneModel* SceneModel::New(const QString& absPath, const QString& path, int width, int height, int fps)
{
    QDir dir(absPath);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    QFile file(absPath + "/scene.xml");
    if (!file.open(QIODevice::WriteOnly))
    {
        return NULL;
    }

    QDomDocument doc("");
    QDomElement root = doc.createElement("scene");
    root.setAttribute("version", "1.0");
    root.setAttribute("width", width);
    root.setAttribute("height", height);
    root.setAttribute("fps", fps);
    doc.appendChild(root);

    RasterLayerModel* layer = RasterLayerModel::New(absPath + "/default", "default", width, height);
    if (!layer)
    {
        return NULL;
    }

    QDomElement elem = doc.createElement("layer");
    elem.setAttribute("version", "1.0");
    elem.setAttribute("type", layer->GetType());
    elem.setAttribute("path", layer->GetPath());
    root.appendChild(elem);

    SceneModel* scene = new SceneModel(absPath, path, width, height, fps);
    scene->mLayers.push_back(layer);

    QTextStream stream(&file);
    doc.save(stream, 4);
    file.close();

    return scene;
}

SceneModel* SceneModel::Open(const QString& absPath, const QString& path)
{
    QDir dir(absPath);
    if (!dir.exists())
    {
        return NULL;
    }

    QFile projectInfo(absPath + "/scene.xml");
    if (!projectInfo.open(QIODevice::ReadOnly))
    {
        return NULL;
    }

    QDomDocument doc("");
    if (!doc.setContent(&projectInfo)) {
        projectInfo.close();
        return NULL;
    }
    projectInfo.close();

    QDomElement docElem = doc.documentElement();
    if (docElem.isNull())
    {
        return NULL;
    }
    int width = docElem.attribute("width", "1").toInt();
    int height = docElem.attribute("height", "1").toInt();
    int fps = docElem.attribute("fps", "24").toInt();

    SceneModel* result = new SceneModel(absPath, path, width, height, fps);

    const QDomNodeList& sceneNodes = docElem.elementsByTagName("layer");
    for (int i = 0; i < sceneNodes.size(); ++i)
    {
        const QDomElement& n = sceneNodes.at(i).toElement();
        if (n.isNull())
        {
            continue;
        }
        QString scenePath = n.attribute("path");
        LayerModel::LayerType type = (LayerModel::LayerType)n.attribute("type").toInt();
        switch (type)
        {
        case LayerModel::LayerTypeRaster:
            {
                RasterLayerModel* layer = RasterLayerModel::Open(absPath + "/" + scenePath, scenePath);
                if (layer)
                {
                    result->mLayers.push_back(layer);
                }
            }
            break;
        case LayerModel::LayerTypeTrace:
            {
                TraceLayerModel* layer = TraceLayerModel::Open(absPath + "/" + scenePath, scenePath);
                if (layer)
                {
                    result->mLayers.push_back(layer);
                }
            }
            break;
        default:
            break;
        }
    }

    return result;
}

void SceneModel::Save()
{
    QDir dir(mAbsPath);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    QFile file(mAbsPath + "/scene.xml");
    file.open(QIODevice::WriteOnly);

    QDomDocument doc("");
    QDomElement root = doc.createElement("scene");
    root.setAttribute("version", "1.0");
    root.setAttribute("width", mWidth);
    root.setAttribute("height", mHeight);
    root.setAttribute("fps", mFps);
    doc.appendChild(root);

    for (size_t i = 0; i < mLayers.size(); ++i)
    {
        LayerModel* layer = mLayers[i];
        layer->Save();

        QDomElement elem = doc.createElement("layer");
        elem.setAttribute("version", "1.0");
        elem.setAttribute("type", layer->GetType());
        elem.setAttribute("path", layer->GetPath());
        root.appendChild(elem);
    }

    QTextStream stream(&file);
    doc.save(stream, 4);
    file.close();
}

RasterLayerModel* SceneModel::AddRasterLayer(int index, const QString& name, int width, int height)
{
    if (mLayers.size() == 0 || index < 0 || index > (int)mLayers.size())
    {
        return NULL;
    }

    QString absPath = mAbsPath + "/" + name;
    RasterLayerModel* l = RasterLayerModel::New(absPath, name, width, height);
    if (l)
    {
        std::vector<LayerModel*>::iterator where = mLayers.begin();
        where += index;
        mLayers.insert(where, l);
    }
    return l;
}

TraceLayerModel* SceneModel::AddTraceLayer(int index, const QString& name)
{
    if (mLayers.size() == 0 || index < 0 || index > (int)mLayers.size())
    {
        return NULL;
    }

    QString absPath = mAbsPath + "/" + name;
    TraceLayerModel* l = TraceLayerModel::New(absPath, name);
    if (l)
    {
        std::vector<LayerModel*>::iterator where = mLayers.begin();
        where += index;
        mLayers.insert(where, l);
    }
    return l;
}

void SceneModel::RemoveLayer(int index)
{
    if (mLayers.size() == 0 || index < 0 || index >= (int)mLayers.size())
    {
        return;
    }

    LayerModel* l = mLayers[index];
    std::vector<LayerModel*>::iterator it = mLayers.begin();
    it += index;
    mLayers.erase(it);
    delete l;
}

void SceneModel::Export(const QString& filePath)
{
    QString path = filePath;
    if (path.isEmpty())
    {
        return;
    }

    int dotPos = path.lastIndexOf(".");
    QString ext = ".png";
    if (dotPos != -1)
    {
        ext = path.right(path.length() - dotPos);
        path = path.left(dotPos);
    }

    int maxFrames = GetMaxFrames();
    if (maxFrames == 0)
    {
        return;
    }

    QImage out(mWidth, mHeight, QImage::Format_RGBA8888);
    QPainter pt(&out);

    for (int f = 0; f < maxFrames; ++f)
    {
        pt.setCompositionMode(QPainter::CompositionMode_Source);
        pt.fillRect(0, 0, mWidth, mHeight, QBrush(QColor(0, 0, 0, 0)));

        for (size_t i = 0; i < mLayers.size(); ++i)
        {
            LayerModel* layer = mLayers[i];
            if (!layer->IsEnabled() || layer->GetOpacity() == 0)
            {
                continue;
            }
            QImage* img = layer->GetImage(f);
            if (!img)
            {
                continue;
            }
            pt.setCompositionMode(QPainter::CompositionMode_SourceOver);
            pt.setOpacity(layer->GetOpacity() / 255.0f);
            pt.drawImage(0, 0, *img);
        }
        QString idxStr;
        idxStr.sprintf("%06d", f);
        out.save(path + idxStr + ext);
    }
}

int SceneModel::GetMaxFrames()
{
    int maxFrames = 0;
    for (size_t i = 0; i < mLayers.size(); ++i)
    {
        LayerModel* layer = mLayers[i];
        if (!layer->IsEnabled() || layer->GetOpacity() == 0)
        {
            continue;
        }
        int fs = layer->GetMaxFrames();
        if (fs > maxFrames)
        {
            maxFrames = fs;
        }
    }

    return maxFrames;
}

void SceneModel::MoveLayer(int oldIndex, int newIndex)
{
    int n = (int)mLayers.size();
    if (n < 2 || oldIndex < 0 || oldIndex >= n)
    {
        return;
    }

    LayerModel* layer = mLayers[oldIndex];

    if (newIndex - oldIndex < 0)
    {
        if (newIndex < 0)
        {
            newIndex = 0;
        }

        for (int i = oldIndex; i > newIndex; --i)
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

        for (int i = oldIndex; i < newIndex; ++i)
        {
            mLayers[i] = mLayers[i + 1];
        }
    }

    mLayers[newIndex] = layer;
}

void SceneModel::GetCompositeImage(int frameIndex, QImage* result)
{
    if (frameIndex < 0 || frameIndex > GetMaxFrames() || !result)
    {
        return;
    }

    QPainter p(result);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.setBrush(QBrush(QColor(0,0,0,0)));
    p.fillRect(0, 0, mWidth, mHeight, QColor(0,0,0,0));
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    for (size_t i = 0; i < mLayers.size(); ++i)
    {
        LayerModel* layer = mLayers[i];
        if (!layer->IsEnabled() || layer->GetOpacity() == 0)
        {
            continue;
        }

        QImage* img = layer->GetImage(frameIndex);
        if (img)
        {
            p.setOpacity(layer->GetOpacity() / 255.0f);
            p.drawImage(0, 0, *img);
        }
    }
}

//**************************************AnimationProject**************************************
AnimationProject::AnimationProject(const QString& path, int width, int height, int fps)
    :mPath(path)
    ,mWidth(width)
    ,mHeight(height)
    ,mFps(fps)
{

}

AnimationProject::~AnimationProject()
{
    for (size_t i = 0; i < mScenes.size(); ++i)
    {
        delete mScenes[i];
    }
}

AnimationProject* AnimationProject::New(const QString& path, int width, int height, int fps)
{
    QDir dir(path);
    if (!dir.exists())
    {
        dir.mkpath("");
    }

    QFile projectInfo(path + "/project.xml");
    projectInfo.open(QIODevice::WriteOnly);

    QDomDocument doc("");
    QDomElement root = doc.createElement("project");
    root.setAttribute("version", "1.0");
    root.setAttribute("width", width);
    root.setAttribute("height", height);
    root.setAttribute("fps", fps);
    doc.appendChild(root);

    QString absPath = dir.absolutePath() + "/" + "default";
    SceneModel* scene = SceneModel::New(absPath, "default", width, height, fps);
    if (!scene)
    {
        return NULL;
    }

    QDomElement elem = doc.createElement("scene");
    elem.setAttribute("path", "default");
    root.appendChild(elem);


    AnimationProject* result = new AnimationProject(dir.absolutePath(), width, height, fps);
    result->mScenes.push_back(scene);

    QTextStream stream(&projectInfo);
    doc.save(stream, 4);
    projectInfo.close();

    return result;
}

AnimationProject* AnimationProject::Open(const QString& path)
{
    QDir dir(path);
    if (!dir.exists())
    {
        return NULL;
    }

    QFile projectInfo(path + "/project.xml");
    if (!projectInfo.open(QIODevice::ReadOnly))
    {
        return NULL;
    }

    QDomDocument doc("project");
    if (!doc.setContent(&projectInfo)) {
        projectInfo.close();
        return NULL;
    }
    projectInfo.close();

    QDomElement docElem = doc.documentElement();
    if (docElem.isNull())
    {
        return NULL;
    }
    int width = docElem.attribute("width", "1").toInt();
    int height = docElem.attribute("height", "1").toInt();
    int fps = docElem.attribute("fps", "24").toInt();

    QString absPath = dir.absolutePath();
    AnimationProject* result = new AnimationProject(absPath, width, height, fps);

    const QDomNodeList& sceneNodes = docElem.elementsByTagName("scene");
    for (int i = 0; i < sceneNodes.size(); ++i)
    {
        const QDomElement& n = sceneNodes.at(i).toElement();
        if (n.isNull())
        {
            continue;
        }
        QString scenePath = n.attribute("path");
        SceneModel* scene = SceneModel::Open(absPath + "/" + scenePath, scenePath);
        if (scene)
        {
            result->mScenes.push_back(scene);
        }
    }

    return result;
}

void AnimationProject::Save()
{
    QDir dir(mPath);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    QFile projectInfo(mPath + "/project.xml");
    projectInfo.open(QIODevice::WriteOnly);

    QDomDocument doc("");
    QDomElement root = doc.createElement("project");
    root.setAttribute("version", "1.0");
    root.setAttribute("width", mWidth);
    root.setAttribute("height", mHeight);
    root.setAttribute("fps", mFps);
    doc.appendChild(root);

    for (size_t i = 0; i < mScenes.size(); ++i)
    {
        SceneModel* scene = mScenes[i];
        scene->Save();

        QDomElement docScene = doc.createElement("scene");
        docScene.setAttribute("path", scene->GetPath());
        root.appendChild(docScene);
    }

    QTextStream stream(&projectInfo);
    doc.save(stream, 4);
    projectInfo.close();
}

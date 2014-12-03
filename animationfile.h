#ifndef ANIMATIONFILE_H
#define ANIMATIONFILE_H

#include <QObject>
#include <vector>
#include <map>
#include <QImage>

class SceneModel;
class AnimationProject;
class RasterLayerModel;

class LayerModel
{
public:
    enum LayerType
    {
        LayerTypeRaster = 1,
        LayerTypeTransform,
        LayerTypeTrace,
        LayerTypeScene,
        LayerTypeBlend,
        LayerTypeBlur,
        LayerTypeSound,
    };

public:
    LayerModel(const QString& absPath, const QString& path, const QString& name, LayerType type);
    virtual ~LayerModel();

    virtual void Save() = 0;
    virtual QImage* GetImage(int frameIndex) = 0;
    virtual bool IsEnabled() = 0;
    virtual unsigned char GetOpacity() = 0;
    virtual int GetMaxFrames() = 0;

    void SetName(const QString& value) { mName = value; }
    const QString& GetName() const { return mName; }
    LayerType GetType() const { return mType; }
    const QString& GetAbsolutePath() const { return mAbsPath; }
    const QString& GetPath() const { return mPath; }

protected:
    QString mAbsPath;
    QString mPath;
    QString mName;
    LayerType mType;
};


class RasterFrameModel
{
public:
    RasterFrameModel(RasterLayerModel* layer, const QString& absImagePath, const QString& imagePath, int exposure = 1);
    ~RasterFrameModel();

    void SetExposure(int value) { mExposure = value; }
    int GetExposure() const { return mExposure; }
    const QString& GetImagePath() const { return mImagePath; }
    QImage* GetImage();
    void Save();

private:
    RasterLayerModel* mLayer;
    QString mAbsImagePath;
    QString mImagePath;
    int mExposure;
    QImage* mImage;
};

class RasterLayerModel:
        public LayerModel
{
public:
    RasterLayerModel(const QString& absPath, const QString& path, int width, int height);
    ~RasterLayerModel();

    static RasterLayerModel* New(const QString& absPath, const QString& path, int width, int height);
    static RasterLayerModel* Open(const QString& absPath, const QString& path);

    void Save();
    std::vector<RasterFrameModel*>& GetFrames() { return mFrames; }
    int GetWidth() const { return mWidth; }
    int GetHeight() const { return mHeight; }
    int GetImageIndexFromFrameIndex(int frameIndex);
    int GetMaxFrames();

    void AddFrame(int insertIndex);
    void RemoveFrame(int index);
    void ModExposure(int index, int delta);
    RasterFrameModel* GetFrameAt(int index);
    int GetPrevImageIndex(int index);
    int GetNextImageIndex(int index);
    QImage* GetImage(int frameIndex);
    bool IsOnionEnabled();
    void EnableOnion(bool enable);
    unsigned char GetOpacity();
    void SetOpacity(unsigned char value);
    bool IsEnabled();
    void Enable(bool enable);

private:
    int mWidth;
    int mHeight;
    int mNextImageId;
    unsigned char mOpacity;
    bool mEnabled;
    bool mOnionEnabled;
    std::vector<RasterFrameModel*> mFrames;
};

class TraceLayerModel:
    public LayerModel
{
public:
    TraceLayerModel(const QString& absPath, const QString& path);
    ~TraceLayerModel();

    static TraceLayerModel* New(const QString& absPath, const QString& path);
    static TraceLayerModel* Open(const QString& absPath, const QString& path);

    void Save();
    int GetMaxFrames() { return mMaxFrames;}
    void SetFrame(int index, int x, int y);
    void RemoveFrame(int index);
    QPoint* GetFrameAt(int index);
    
    QImage* GetImage(int frameIndex);
    bool IsOnionEnabled();
    void EnableOnion(bool enable);
    unsigned char GetOpacity();
    void SetOpacity(unsigned char value);
    bool IsEnabled();
    void Enable(bool enable);

private:
    unsigned char mOpacity;
    bool mEnabled;
    bool mOnionEnabled;
    int mMaxFrames;
    typedef std::map<int, QPoint> FrameList;
    std::map<int, QPoint> mFrames;
};


class SoundLayerModel
{
private:
    int mFrameOffset;
    int mSoundId;
};


class SceneModel
{
public:
    static SceneModel* New(const QString& absPath, const QString& path, int width, int height, int fps);
    static SceneModel* Open(const QString& absPath, const QString& path);

    SceneModel(const QString& absPath, const QString& path, int width, int height, int fps);
    ~SceneModel();
    int GetWidth() const { return mWidth; }
    int GetHeight() const { return mHeight; }
    int GetFps() const { return mFps; }
    void SetFps(int fps) { mFps = fps; }
    const QString& GetAbsolutePath() const { return mAbsPath; }
    const QString& GetPath() const { return mPath; }

    std::vector<LayerModel*>& GetLayers() { return mLayers; }
    RasterLayerModel* AddRasterLayer(int index, const QString& name, int width, int height);
    TraceLayerModel* AddTraceLayer(int index, const QString& name);
    void RemoveLayer(int index);
    void Save();
    void Export(const QString& path);
    int GetMaxFrames();
    void MoveLayer(int oldIndex, int newIndex);
    void GetCompositeImage(int frameIndex, QImage* result);

private:
    QString mAbsPath;
    QString mPath;
    // Scene output width
    int mWidth;
    // Scene output height
    int mHeight;
    // Scene output fps
    int mFps;
    // Layers
    std::vector<LayerModel*> mLayers;
    // Cached final composite images
    std::vector<QString> mFrames;
    std::vector<QImage*> mCompositeImages;
    // Cached final sound layer
    QString mSound;
};

class AnimationProject
{
public:
    ~AnimationProject();
    static AnimationProject* New(const QString& path, int width, int height, int fps);
    static AnimationProject* Open(const QString& path);
    void Save();

    std::vector<SceneModel*>& GetScenes() { return mScenes; }
    int GetWidth() const { return mWidth; }
    int GetHeight() const { return mHeight; }
    int GetFps() const { return mFps; }

signals:

public slots:

private:
    explicit AnimationProject(const QString& path, int width, int height, int fps);

private:
    QString mPath;
    int mWidth;
    int mHeight;
    int mFps;
    std::vector<SceneModel*> mScenes;
};

#endif // ANIMATIONFILE_H

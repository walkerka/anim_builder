#ifndef SOUNDLAYER_H
#define SOUNDLAYER_H
#include "layer.h"
#include <QWidget>

class Timeline;
class WavFile;

class SoundLayer : public Layer
{
    Q_OBJECT
public:
    explicit SoundLayer(Timeline* timeline, QWidget *parent = 0);
    virtual ~SoundLayer();
    LayerType GetType() { return LayerTypeSound; }
    void SetSelected(bool selected);
    QImage* GetImage(int frameIndex) { return NULL; }
    int GetMaxFrames() { return 0; }

signals:

public slots:
    void Load(const QString& path);

    // QWidget interface
public:
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

private:
    Timeline* mTimeline;
    WavFile* mFile;
    QImage* mWaveForm;
    int mOffset;
    bool mSelected;

    // Layer interface
public:
    void OnFrameChanged(int frameIndex);

    // Layer interface
public:
    bool IsOnionEnabled();
    void EnableOnion(bool enable);
    unsigned char GetOpacity();
    void SetOpacity(unsigned char value);
    bool IsEnabled();
    void Enable(bool enable);

    // Layer interface
public:
    QWidget *GetPropertyWindow();
};

#endif // SOUNDLAYER_H

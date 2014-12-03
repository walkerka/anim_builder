#ifndef LAYER_H
#define LAYER_H

#include <QWidget>

enum LayerType
{
    LayerTypeRaster,
    LayerTypeTrace,
    LayerTypeSound,
};

class Layer : public QWidget
{
    Q_OBJECT
public:
    explicit Layer(QWidget *parent = 0);
    virtual ~Layer() {}
    virtual LayerType GetType() = 0;
    virtual void SetSelected(bool selected) = 0;
    virtual void OnFrameChanged(int frameIndex) = 0;
    virtual QImage* GetImage(int frameIndex) = 0;
    virtual bool IsOnionEnabled() = 0;
    virtual void EnableOnion(bool enable) = 0;
    virtual unsigned char GetOpacity() = 0;
    virtual void SetOpacity(unsigned char value) = 0;
    virtual bool IsEnabled() = 0;
    virtual void Enable(bool enable) = 0;
    virtual QWidget* GetPropertyWindow() = 0;
    virtual int GetMaxFrames() = 0;

signals:

public slots:

};

#endif // LAYER_H

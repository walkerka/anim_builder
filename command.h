#ifndef COMMAND_H
#define COMMAND_H
#include <QUndoCommand>
#include <QWidget>
#include <QPainter>
#include <openglrenderer.h>

class RasterImageEditor;
class RasterLayer;
class CachedImage;

class DrawCommand: public QUndoCommand
{
public:
    DrawCommand(RasterImageEditor* editor, QImage* newImage, QImage* oldImage);
    ~DrawCommand();
    void undo();
    void redo();

private:
    RasterImageEditor* mEditor;
    QImage* mNewImage;
    QImage* mOldImage;

};

//class AddFrameCommand: public QUndoCommand
//{
//public:
//    AddFrameCommand(RasterImageEditor* editor, RasterLayer* rasterLayer, const QString& path, int frameIndex);
//    ~AddFrameCommand();
//    void undo();
//    void redo();

//private:
//    RasterImageEditor* mEditor;
//    RasterLayer* mRasterLayer;
//    CachedImage* mNewImage;
//    int mFrameIndex;
//    bool mExecuted;
//};

//class RemoveFrameCommand: public QUndoCommand
//{
//public:
//    RemoveFrameCommand(RasterImageEditor* editor, RasterLayer* rasterLayer, CachedImage* image, int frameIndex);
//    ~RemoveFrameCommand();
//    void undo();
//    void redo();

//private:
//    RasterImageEditor* mEditor;
//    RasterLayer* mRasterLayer;
//    CachedImage* mImage;
//    int mFrameIndex;
//    bool mExecuted;
//};

class ModifyExposureCommand: public QUndoCommand
{
public:
    ModifyExposureCommand(RasterImageEditor* editor, RasterLayer* rasterLayer, int frameIndex, int delta);
    ~ModifyExposureCommand();
    void undo();
    void redo();

private:
    RasterImageEditor* mEditor;
    RasterLayer* mRasterLayer;
    int mFrameIndex;
    int mDelta;
};

#endif // COMMAND_H

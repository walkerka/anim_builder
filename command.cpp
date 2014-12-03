#include "command.h"
#include "rasterimageeditor.h"
#include "rasterlayer.h"
#include "cachedimage.h"

DrawCommand::DrawCommand(RasterImageEditor* editor, QImage* newImage, QImage* oldImage)
    :QUndoCommand("fill")
    ,mEditor(editor)
    ,mNewImage(newImage)
    ,mOldImage(oldImage)
{
}

DrawCommand::~DrawCommand()
{
    delete mOldImage;
    delete mNewImage;
}

void DrawCommand::undo()
{
    QPainter p(mEditor->GetImage());
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.drawImage(0, 0, *mOldImage);
    mEditor->update();
}

void DrawCommand::redo()
{
    QPainter p(mEditor->GetImage());
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.drawImage(0, 0 , *mNewImage);
    mEditor->update();
}


//AddFrameCommand::AddFrameCommand(RasterImageEditor* editor, RasterLayer* rasterLayer, const QString& path, int frameIndex)
//    :QUndoCommand("add frame")
//    ,mEditor(editor)
//    ,mRasterLayer(rasterLayer)
//    ,mNewImage(NULL)
//    ,mFrameIndex(frameIndex)
//    ,mExecuted(false)
//{
//    mNewImage = new CachedImage(path, rasterLayer->GetWidth(), rasterLayer->GetHeight());
//}

//AddFrameCommand::~AddFrameCommand()
//{
//    if (!mExecuted)
//    {
//        delete mNewImage;
//    }
//}


//void AddFrameCommand::undo()
//{
//    mRasterLayer->RemoveFrame(mFrameIndex);
//    mRasterLayer->update();
//    mExecuted = false;
//}

//void AddFrameCommand::redo()
//{
//    mRasterLayer->AddFrame(mFrameIndex, mNewImage);
//    mRasterLayer->update();
//    mExecuted = true;
//}

//RemoveFrameCommand::RemoveFrameCommand(RasterImageEditor* editor, RasterLayer* rasterLayer, CachedImage* image, int frameIndex)
//    :QUndoCommand("add frame")
//    ,mEditor(editor)
//    ,mRasterLayer(rasterLayer)
//    ,mImage(image)
//    ,mFrameIndex(frameIndex)
//    ,mExecuted(false)
//{
//}

//RemoveFrameCommand::~RemoveFrameCommand()
//{
//    if (mExecuted)
//    {
//        delete mImage;
//    }
//}


//void RemoveFrameCommand::undo()
//{
//    mRasterLayer->AddFrame(mFrameIndex, mImage);
//    mRasterLayer->update();
//    mExecuted = false;
//}

//void RemoveFrameCommand::redo()
//{
//    mRasterLayer->RemoveFrame(mFrameIndex);
//    mRasterLayer->update();
//    mExecuted = true;
//}


ModifyExposureCommand::ModifyExposureCommand(RasterImageEditor* editor, RasterLayer* rasterLayer, int frameIndex, int delta)
    :QUndoCommand("modify exposure")
    ,mEditor(editor)
    ,mRasterLayer(rasterLayer)
    ,mFrameIndex(frameIndex)
    ,mDelta(delta)
{
}

ModifyExposureCommand::~ModifyExposureCommand()
{
}


void ModifyExposureCommand::undo()
{
    mRasterLayer->ModExposure(mFrameIndex, -mDelta);
    mRasterLayer->update();
}

void ModifyExposureCommand::redo()
{
    mRasterLayer->ModExposure(mFrameIndex, mDelta);
    mRasterLayer->update();
}

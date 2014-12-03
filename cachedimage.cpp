#include "cachedimage.h"

CachedImage::CachedImage(const QString& path, int width, int height)
    :mImage(NULL)
    ,mPath(path)
{
    mImage = new QImage(width, height, QImage::Format_RGBA8888);
}

CachedImage::~CachedImage()
{
    delete mImage;
}

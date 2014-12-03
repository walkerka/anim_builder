#ifndef CACHEDIMAGE_H
#define CACHEDIMAGE_H
#include <QImage>

class CachedImage
{
public:
    CachedImage(const QString& path, int width, int height);
    ~CachedImage();

    QImage* GetImage() { return mImage; }
    const QString& GetPath() const { return mPath; }

private:
    QImage* mImage;
    QString mPath;
};

#endif // CACHEDIMAGE_H

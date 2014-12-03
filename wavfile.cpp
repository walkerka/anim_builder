#include "wavfile.h"
#include <QFile>

WavFile::WavFile(const QString& path)
    :mData(NULL)
{
    memset(&mHeader, 0, sizeof(mHeader));
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
    {
        return;
    }

    f.read((char*)&mHeader.riffId, sizeof(mHeader.riffId));
    if (strncmp(mHeader.riffId, "RIFF", 4) != 0)
    {
        return;
    }

    f.read((char*)&mHeader.chunkSize, sizeof(mHeader.chunkSize));

    f.read((char*)&mHeader.waveId, sizeof(mHeader.waveId));
    if (strncmp(mHeader.waveId, "WAVE", 4) != 0)
    {
        return;
    }

    f.read((char*)&mHeader.fmtId, sizeof(mHeader.fmtId));
    if (strncmp(mHeader.fmtId, "fmt ", 4) != 0)
    {
        return;
    }

    f.read((char*)&mHeader.fmtChunkSize, sizeof(mHeader.fmtChunkSize));
    if (mHeader.fmtChunkSize != 16 && mHeader.fmtChunkSize != 18)
    {
        return;
    }

    f.read((char*)&mHeader.audioFormat, sizeof(mHeader.audioFormat));
    if (mHeader.audioFormat != 1)
    {
        return;
    }

    f.read((char*)&mHeader.numChannels, sizeof(mHeader.numChannels));
    if (mHeader.numChannels != 1 && mHeader.numChannels != 2)
    {
        return;
    }

    f.read((char*)&mHeader.sampleRate, sizeof(mHeader.sampleRate));
    f.read((char*)&mHeader.byteRate, sizeof(mHeader.byteRate));
    f.read((char*)&mHeader.blockAlign, sizeof(mHeader.blockAlign));
    f.read((char*)&mHeader.bitsPerSample, sizeof(mHeader.bitsPerSample));

    if (mHeader.fmtChunkSize == 18)
    {
        f.read((char*)&mHeader.cbSize, sizeof(mHeader.cbSize));
        if (mHeader.cbSize != 0)
        {
            return;
        }
    }

    f.read((char*)&mHeader.dataId, sizeof(mHeader.dataId));
    if (strncmp(mHeader.dataId, "data", 4) != 0)
    {
        return;
    }

    f.read((char*)&mHeader.dataChunkSize, sizeof(mHeader.dataChunkSize));
    mData = new short[mHeader.dataChunkSize / 2];
    unsigned int size = f.read((char*)mData, mHeader.dataChunkSize);
    if (size != mHeader.dataChunkSize)
    {
        return;
    }

}

WavFile::~WavFile()
{
    delete mData;
}

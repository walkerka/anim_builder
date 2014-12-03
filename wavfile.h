#ifndef WAVFILE_H
#define WAVFILE_H
#include <QString>

typedef struct
{
    // RIFF chunk
    char riffId[4];//RIFF
    unsigned int chunkSize;//36 + fmtChunkSize
    char waveId[4];//WAVE
    // fmt chunk
    char fmtId[4];//"fmt "
    unsigned int fmtChunkSize;//=16 or 18 for PCM
    unsigned short audioFormat;//=1 for PCM
    unsigned short numChannels;//=1 for mono, 2 for stereo
    unsigned int sampleRate;//8000, 44100
    unsigned int byteRate;//=SampleRate * NumChannels * BitsPerSample/8
    unsigned short blockAlign;//=NumChannels * BitsPerSample/8
    unsigned short bitsPerSample;//8 bits = 8, 16 bits = 16, etc.
    unsigned short cbSize;//=0 for PCM
    // data chunk
    char dataId[4];//"data"
    unsigned int dataChunkSize;//=NumSamples * NumChannels * BitsPerSample/8
} WavHeader;

class WavFile
{
public:
    WavFile(const QString& path);
    ~WavFile();

    bool IsReady() const { return mData != NULL; }
    const WavHeader& GetHeader() const { return mHeader; }
    const short* GetData() const { return mData; }
    size_t GetSampleCount() const { return mHeader.dataChunkSize / (mHeader.numChannels * mHeader.bitsPerSample / 8); }
    unsigned int GetSample(size_t idx) const { return mData[idx * mHeader.numChannels]; }
    unsigned int GetSampleRate() const { return mHeader.sampleRate; }

private:
    WavHeader mHeader;
    short* mData;
};

#endif // WAVFILE_H

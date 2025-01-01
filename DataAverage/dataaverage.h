#ifndef DATAAVERAGE_H
#define DATAAVERAGE_H

#include "Arduino.h"

class DataAverage
{
public:
    DataAverage(uint8_t sampleNumber);
    ~DataAverage();
    void addSample(float sample);
    float average();

    uint8_t sampleCount() const;

private:
    uint8_t mMaxSampleNumber;
    uint8_t mCurrentSample;
    uint8_t mSampleCount;
    float* mSamples;
};


#endif // DATAAVERAGE_H

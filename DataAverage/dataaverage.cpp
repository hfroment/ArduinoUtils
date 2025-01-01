#include "dataaverage.h"

DataAverage::DataAverage(uint8_t sampleNumber) :
    mMaxSampleNumber(sampleNumber),
    mCurrentSample(0),
    mSampleCount(0),
    mSamples(new float[sampleNumber])
{

}

DataAverage::~DataAverage()
{
    if (mSamples != 0)
    {
        delete [] mSamples;
    }
}

void DataAverage::addSample(float sample)
{
    mSamples[mCurrentSample] = sample;
    mCurrentSample++;
    mCurrentSample %= mMaxSampleNumber;
    if (mSampleCount < mMaxSampleNumber)
    {
        mSampleCount++;
    }
}

float DataAverage::average()
{
    float retour = 0;

    if (mSampleCount > 0)
    {
        for (int i = 0; i < mSampleCount; i++)
        {
            retour += mSamples[i];
        }
        retour /= mSampleCount;
        mSampleCount = 0;
        mCurrentSample = 0;
    }
    return retour;
}

uint8_t DataAverage::sampleCount() const
{
    return mSampleCount;
}

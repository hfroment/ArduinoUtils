
#include "mystgenerator.h"

MystGenerator::MystGenerator(uint8_t pin, uint16_t maxPeriodInSeconds) :
    mDuration(maxPeriodInSeconds),
    mOnDuration(0),
    mOffDuration(maxPeriodInSeconds),
    mStarted(false),
    mOn(false),
    mGeneratorPin(pin)
{
    pinMode(mGeneratorPin, OUTPUT);
}

void MystGenerator::cadencer1s()
{
    if (outputOn())
    {
        if (mOnDuration < mDuration)
        {
            mOnDuration++;
        }
    }
    else
    {
        if (mOffDuration < mDuration)
        {
            mOffDuration++;
        }
    }

    if (mStarted)
    {
        if (outputOn())
        {
            if (mOnDuration >= mDuration)
            {
                changeOutput(false);
            }
        }
        else
        {
            if (mOffDuration >= mDuration)
            {
                changeOutput(true);
            }
        }
    }
    else
    {
        if (outputOn())
        {
            changeOutput(false);
        }
    }
}
void MystGenerator::changeOutput(bool on)
{
    if (!outputOn() && on)
    {
        // On reset la durée de mise à ON
        mOnDuration = 0;
    }
    else if (outputOn() && !on)
    {
        mOffDuration = 0;
    }
    mOn = on;
    digitalWrite(mGeneratorPin, mOn);
}

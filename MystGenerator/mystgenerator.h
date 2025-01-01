#ifndef MYSTGENERATOR_H
#define MYSTGENERATOR_H

#include "Arduino.h"

class MystGenerator
{
public:
    MystGenerator(uint8_t pin, uint16_t maxPeriodInSeconds = defaultDurationInSeconds);
    void start()
    {
        mStarted = true;
    }

    void stop()
    {
        mStarted = false;
    }

    bool started()
    {
        return mStarted;
    }

    bool outputOn()
    {
        return mOn;
    }

    void durations(uint16_t& onDuration, uint16_t& offDuration)
    {
        onDuration = mOnDuration;
        offDuration = mOffDuration;
    }

    void cadencer1s();

private:
    static const uint16_t defaultDurationInSeconds = 15 * 60;
    uint16_t mDuration;
    uint16_t mOnDuration;
    uint16_t mOffDuration;
    bool mStarted;
    bool mOn;
    uint8_t mGeneratorPin;

    void changeOutput(bool on);
};

#endif // MYSTGENERATOR_H

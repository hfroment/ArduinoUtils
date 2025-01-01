#ifndef PALUDARIUM_H
#define PALUDARIUM_H

class DevBoardShaton;

class Paludarium
{
public:
    Paludarium();

    void init();
    void gerer();

private:
    DevBoardShaton& mBoard;

    static const uint8_t temperatureChar = 0;
    static const uint8_t humidityChar = 1;
    static const uint8_t mystOnChar = 2;

    static const uint8_t mHumidityThreshold = 70;
};

#endif // PALUDARIUM_H

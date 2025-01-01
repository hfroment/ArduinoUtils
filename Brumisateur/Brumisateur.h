#ifndef BRUMISATEUR_H
#define BRUMISATEUR_H

class MiniWeatherBoard;

class Brumisateur
{
public:
    Brumisateur();

    void init();
    void gerer();

private:
    MiniWeatherBoard& mBoard;

    static const uint8_t temperatureChar = 0;
    static const uint8_t humidityChar = 1;
    static const uint8_t mystOnChar = 2;

    static const uint8_t mHumidityThreshold = 70;
};

#endif // BRUMISATEUR_H

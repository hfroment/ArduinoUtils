/*
 * Board.h
 *
 *  Created on: 21 déc. 2013
 *      Author: flea_new
 */

#ifndef WEATHERBOARD_H_
#define WEATHERBOARD_H_

#include "Arduino.h"

class Ds18b20;
#include "dht.h"
class Adafruit_BMP085_Unified;

class WeatherBoard
{
public:

    WeatherBoard(bool useRtc = false, bool useVccio = false);
    virtual ~WeatherBoard();

    void initDht(dht::dhtmodels type = dht::DHT11, uint8_t dhht11Pin = defaultDhtPin);
    bool readTU(uint8_t& temperature, uint8_t& humidity);
    // DS18b20
    void initDs18b20(uint8_t ds18B20Pin = defaultDs18b20Pin);
    bool readDs18b20(float& temp);
    // BMP085
    void initBmp085();
    bool readBmp085(float& pressure, float& temp, float &seaPressure);
    bool readBmp085(float& pressure, float& temp, float &seaPressure, int& forecast);

    // Fonction périodique
    bool gerer(bool top, bool pretop, bool& refresh);
    bool acqStage1();
    bool acqStage2();

//    void cadencer1s()
//    {
//        mCompteurIt++;
//    }

    static WeatherBoard *instance;

    enum FORECAST
    {
        STABLE = 0,            // "Stable Weather Pattern"
        SUNNY = 1,            // "Slowly rising Good Weather", "Clear/Sunny "
        CLOUDY = 2,            // "Slowly falling L-Pressure ", "Cloudy/Rain "
        UNSTABLE = 3,        // "Quickly rising H-Press",     "Not Stable"
        THUNDERSTORM = 4,    // "Quickly falling L-Press",    "Thunderstorm"
        UNKNOWN = 5            // "Unknown (More Time needed)
    };

private:
    // TU
    static const uint8_t defaultDhtPin = A3;
    dht* mDht;
    bool mDhtDataOk;
    bool readTU();

    bool mDs3231Present;
    
    static const uint8_t defaultDs18b20Pin = A2;
    Ds18b20* mDs18b20;
    bool mDs18b20DataOk;
    bool startDs18b20();
    bool readDs18b20();

    Adafruit_BMP085_Unified* mBmp085;
    bool mBmp085DataOk;
    float mPressure;
    float mPressureTemperature;
    bool readBmp085();
    int mForecast;
    // forecast computing
    float getLastPressureSamplesAverage();
    int sample(float pressure);
    static const int maxSample = 5;
    float lastPressureSamples[maxSample];
    // this CONVERSION_FACTOR is used to convert from hPa to kPa in forecast algorithm
    // get kPa/h be dividing hPa by 10
    static const float conversionFactor = (1.0/10.0);
    int minuteCount;
    bool firstRound;
    // average value is used in forecast algorithm.
    float pressureAvg;
    // average after 2 hours is used as reference value for the next iteration.
    float pressureAvg2;
    float dP_dt;


//    static const uint8_t delaisLecure = 60; // s
//    uint8_t mCompteurLecture;

//    uint8_t mCompteurIt;
//    uint8_t mCompteurItPrecedent;

    bool mUseRtc;
    bool mUseVccio;

    //static const uint8_t mLedPin = 13;
    static const uint8_t mVccioPin = 8;

    void ioPower(bool on = true);

    static const float mAltitudeReference;

};

#endif /* MINIWEATHERBOARD_H_ */

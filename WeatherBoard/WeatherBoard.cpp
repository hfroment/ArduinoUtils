/*
 * Board.cpp
 *
 *  Created on: 21 déc. 2013
 *      Author: flea_new
 */

#include <dht.h>

#include "ds18b20.h"

#include <Wire.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

#include <Sodaq_DS3231.h>

#include "WeatherBoard.h"

WeatherBoard *WeatherBoard::instance = NULL;

//const float WeatherBoard::mAltitudeReference = 158.0; // Pins-Justaret
static const float WeatherBoard::mAltitudeReference = 240; // Puydaniel


WeatherBoard::WeatherBoard(bool useRtc, bool useVccio):
    mDht(0),
    mDhtDataOk(false),
    mDs3231Present(false),
    mDs18b20(0),
    mDs18b20DataOk(false),
    mPressure(-1000),
    mPressureTemperature(-1000),
    mForecast(UNKNOWN),
    minuteCount(0),
    firstRound(true),
    pressureAvg(0),
    pressureAvg2(0),
    dP_dt(0),
    mBmp085(0),
    mBmp085DataOk(false),
//    mCompteurIt(0),
//    mCompteurItPrecedent(255),
//    mCompteurLecture(0),
    mUseRtc(useRtc),
    mUseVccio(useVccio)//,
  //mMystGenerator(0)
{
    instance = this;
    // TODO Auto-generated constructor stub
    if (mUseVccio)
    {
        pinMode(mVccioPin, OUTPUT);
    }

    if (mUseRtc)
    {
        Wire.begin();
        rtc.begin();
    }
}

WeatherBoard::~WeatherBoard()
{
    // TODO Auto-generated destructor stub
    if (mDs18b20 == 0)
    {
        delete mDs18b20;
    }
    if (mDht == 0)
    {
        delete mDht;
    }
    if (mBmp085 == 0)
    {
        delete mBmp085;
    }
}

bool WeatherBoard::acqStage1()
{
    // Pour la température, il faut la lire en avance de phase
    mDs18b20DataOk = startDs18b20();
    return mDs18b20DataOk;
}

bool WeatherBoard::acqStage2()
{
    // On lit tout
    mDhtDataOk = readTU();
    mBmp085DataOk = readBmp085();
    if (mBmp085DataOk)
    {
        mForecast = sample(mPressure);
    }
    mDs18b20DataOk = readDs18b20();
    return true;
}

bool WeatherBoard::gerer(bool top, bool pretop, bool& refresh)
{
    bool retour = false;
    refresh = false;

//    if (seconde)
//    {
//        cadencer1s();
//    }
    if (top || pretop)
    {
        if (pretop)
        {
            // Pour la température, il faut la lire en avance de phase
            mDs18b20DataOk = startDs18b20();
//            mCompteurLecture--;
        }
        if (top)
        {
//            mCompteurLecture = delaisLecure;
            // On lit tout
            mDhtDataOk = readTU();
            mBmp085DataOk = readBmp085();
            if (mBmp085DataOk)
            {
                mForecast = sample(mPressure);
            }
            mDs18b20DataOk = readDs18b20();
            refresh = true;
        }
//        mCompteurItPrecedent = mCompteurIt;
        retour = true;
    }
    return retour;
}

void WeatherBoard::initDht(dht::dhtmodels type, uint8_t dhht11Pin)
{
    if (mDht == 0)
    {
        if (type < dht::DHT12)
        {
            mDht = new dht1wire(dhht11Pin, type);
        }
        else if (type == dht::DHT12)
        {
            mDht = new dht12();
        }
        // On allume
        ioPower();
    }
}

void WeatherBoard::initDs18b20(uint8_t ds18B20Pin)
{
    if (mDs18b20 == 0)
    {
        mDs18b20 = new Ds18b20(ds18B20Pin);
    }
}

bool WeatherBoard::readTU(uint8_t& temperature, uint8_t& humidity)
{
    if (mDhtDataOk)
    {
        temperature = mDht->getTemperature();
        humidity = mDht->getHumidity();
    }
    return mDhtDataOk;
}

bool WeatherBoard::readTU()
{
    bool retour = false;
    if (mDht != 0)
    {
        if (dht::OK == mDht->read())
        {
            retour = true;
        }
    }
    return retour;
}

// Fonction récupérant la température depuis le DS18B20
// Retourne true si tout va bien, ou false en cas d'erreur
bool WeatherBoard::readDs18b20(float& temp)
{
    bool retour = false;

    if (mDs18b20 != 0)
    {
        retour = mDs18b20->temperature(temp);
    }

    return retour;
}

// Fonction récupérant la température depuis le DS18B20
// Retourne true si tout va bien, ou false en cas d'erreur
bool WeatherBoard::readDs18b20()
{
    bool retour = false;

    if (mDs18b20 != 0)
    {
        // Un cycle après la lecture, on récupére les datas
        mDs18b20DataOk = mDs18b20->getData();
        retour = mDs18b20DataOk;
    }

    return retour;
}

bool WeatherBoard::startDs18b20()
{
    bool retour = false;

    if (mDs18b20 != 0)
    {
        // une seconde avant les traitements cycliques
//        if (mCompteurLecture == 1)
        {
            retour = mDs18b20->startRead();
        }
    }

    return retour;
}
void WeatherBoard::initBmp085()
{
    if (mBmp085 == 0)
    {
        // On allume
        ioPower();
        mBmp085 = new Adafruit_BMP085_Unified(10085);
        if(!mBmp085->begin())
        {
            delete mBmp085;
            mBmp085 = 0;
        }
    }
}

bool WeatherBoard::readBmp085(float& pressure, float& temp, float& seaPressure)
{
    if (mBmp085DataOk)
    {
        pressure = mPressure;
        temp = mPressureTemperature;
        seaPressure = mPressure * pow((mPressureTemperature + 273.15 + 0.0065 * mAltitudeReference) / (mPressureTemperature + 273.15), 5.2561);
    }

    return mBmp085DataOk;
}

bool WeatherBoard::readBmp085(float& pressure, float& temp, float &seaPressure, int& forecast)
{
    bool retour = readBmp085(pressure, temp, seaPressure);
    if (retour)
    {
        forecast = mForecast;
    }
    return retour;
}

bool WeatherBoard::readBmp085()
{
    bool retour = false;

    if (mBmp085 != 0)
    {
        sensors_event_t event;
        mBmp085->getEvent(&event);
        if (event.pressure)
        {
            retour = true;
            mPressure = event.pressure;
            mBmp085->getTemperature(&mPressureTemperature);
        }
    }

    return retour;
}

void WeatherBoard::ioPower(bool on)
{
    if (mUseVccio)
    {
        if (on)
        {
            digitalWrite(mVccioPin, 0);
        }
        else
        {
            digitalWrite(mVccioPin, 1);
        }
    }
}

float WeatherBoard::getLastPressureSamplesAverage()
{
    float lastPressureSamplesAverage = 0;
    for (int i = 0; i < maxSample; i++)
    {
        lastPressureSamplesAverage += lastPressureSamples[i];
    }
    lastPressureSamplesAverage /= maxSample;

    return lastPressureSamplesAverage;
}



// Algorithm found here
// http://www.freescale.com/files/sensors/doc/app_note/AN3914.pdf
// Pressure in hPa -->  forecast done by calculating kPa/h
int WeatherBoard::sample(float pressure)
{
    // Calculate the average of the last n minutes.
    int index = minuteCount % maxSample;
    lastPressureSamples[index] = pressure;

    minuteCount++;
    if (minuteCount > 185)
    {
        minuteCount = 6;
    }

    if (minuteCount == 5)
    {
        pressureAvg = getLastPressureSamplesAverage();
    }
    else if (minuteCount == 35)
    {
        float lastPressureAvg = getLastPressureSamplesAverage();
        float change = (lastPressureAvg - pressureAvg) * conversionFactor;
        if (firstRound) // first time initial 3 hour
        {
            dP_dt = change * 2; // note this is for t = 0.5hour
        }
        else
        {
            dP_dt = change / 1.5; // divide by 1.5 as this is the difference in time from 0 value.
        }
    }
    else if (minuteCount == 65)
    {
        float lastPressureAvg = getLastPressureSamplesAverage();
        float change = (lastPressureAvg - pressureAvg) * conversionFactor;
        if (firstRound) //first time initial 3 hour
        {
            dP_dt = change; //note this is for t = 1 hour
        }
        else
        {
            dP_dt = change / 2; //divide by 2 as this is the difference in time from 0 value
        }
    }
    else if (minuteCount == 95)
    {
        float lastPressureAvg = getLastPressureSamplesAverage();
        float change = (lastPressureAvg - pressureAvg) * conversionFactor;
        if (firstRound) // first time initial 3 hour
        {
            dP_dt = change / 1.5; // note this is for t = 1.5 hour
        }
        else
        {
            dP_dt = change / 2.5; // divide by 2.5 as this is the difference in time from 0 value
        }
    }
    else if (minuteCount == 125)
    {
        float lastPressureAvg = getLastPressureSamplesAverage();
        pressureAvg2 = lastPressureAvg; // store for later use.
        float change = (lastPressureAvg - pressureAvg) * conversionFactor;
        if (firstRound) // first time initial 3 hour
        {
            dP_dt = change / 2; // note this is for t = 2 hour
        }
        else
        {
            dP_dt = change / 3; // divide by 3 as this is the difference in time from 0 value
        }
    }
    else if (minuteCount == 155)
    {
        float lastPressureAvg = getLastPressureSamplesAverage();
        float change = (lastPressureAvg - pressureAvg) * conversionFactor;
        if (firstRound) // first time initial 3 hour
        {
            dP_dt = change / 2.5; // note this is for t = 2.5 hour
        }
        else
        {
            dP_dt = change / 3.5; // divide by 3.5 as this is the difference in time from 0 value
        }
    }
    else if (minuteCount == 185)
    {
        float lastPressureAvg = getLastPressureSamplesAverage();
        float change = (lastPressureAvg - pressureAvg) * conversionFactor;
        if (firstRound) // first time initial 3 hour
        {
            dP_dt = change / 3; // note this is for t = 3 hour
        }
        else
        {
            dP_dt = change / 4; // divide by 4 as this is the difference in time from 0 value
        }
        pressureAvg = pressureAvg2; // Equating the pressure at 0 to the pressure at 2 hour after 3 hours have past.
        firstRound = false; // flag to let you know that this is on the past 3 hour mark. Initialized to 0 outside main loop.
    }

    int forecast = UNKNOWN;
    if (minuteCount < 35 && firstRound) //if time is less than 35 min on the first 3 hour interval.
    {
        forecast = UNKNOWN;
    }
    else if (dP_dt < (-0.25))
    {
        forecast = THUNDERSTORM;
    }
    else if (dP_dt > 0.25)
    {
        forecast = UNSTABLE;
    }
    else if ((dP_dt > (-0.25)) && (dP_dt < (-0.05)))
    {
        forecast = CLOUDY;
    }
    else if ((dP_dt > 0.05) && (dP_dt < 0.25))
    {
        forecast = SUNNY;
    }
    else if ((dP_dt >(-0.05)) && (dP_dt < 0.05))
    {
        forecast = STABLE;
    }
    else
    {
        forecast = UNKNOWN;
    }

    // uncomment when debugging
    //Serial.print(F("Forecast at minute "));
    //Serial.print(minuteCount);
    //Serial.print(F(" dP/dt = "));
    //Serial.print(dP_dt);
    //Serial.print(F("kPa/h --> "));
    //Serial.println(weather[forecast]);

    return forecast;
}

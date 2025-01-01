#include "WeatherBoard.h"
#include <Sodaq_DS3231.h>
#include <LiquidCrystal_I2C.h>
#include <core/MySensorsCore.h>

#include "WeatherStation.h"

const String weatherForecast[] = { "stable", "sunny", "cloudy", "unstable", "thunderstorm", "unknown" };

static const uint8_t iconTemperature[8] PROGMEM = {0x4, 0xa, 0xa, 0xe, 0xe, 0x1f, 0x1f, 0xe}; // 8
static const uint8_t iconHumidity[8] PROGMEM = {0x0, 0x4, 0xa, 0x11, 0x11, 0xe, 0x0, 0x0}; // 8
//static const uint8_t iconMist[8] PROGMEM = {
//    0b00000,
//    0b00100,
//    0b01110,
//    0b11111,
//    0b11111,
//    0b01110,
//    0b00000,
//    0b00000
//};
static const uint8_t iconNoInformation[8] PROGMEM = {0x1f, 0x11, 0xa, 0x4, 0xa, 0x11, 0x1f};
// Double flèche verticale
static const uint8_t iconLink[8] PROGMEM = {
    0b00100,
    0b01110,
    0b10101,
    0b00100,
    0b00100,
    0b10101,
    0b01110,
    0b00100
};

static const uint8_t iconNoClock[8] PROGMEM = {
    0b00000,
    0b01110,
    0b10001,
    0b10101,
    0b10001,
    0b01110,
    0b00000,
    0b00000
};

WeatherStation::WeatherStation(bool useRtc, bool temperaturePresent, uint8_t lcdAddress, eTypeLcd typeLcd, uint8_t humidityType, bool pressurePresent, bool useLcd) :
    mBoard(*new WeatherBoard(useRtc)),
    mLcdAddress(lcdAddress),
    mTypeLcd(typeLcd),
    mUseRtc(useRtc),
    mLcdPresent(lcdAddress != 0),
    mPressurePresent(pressurePresent),
    mHumidityType(humidityType),
    mTemperaturePresent(temperaturePresent),
    mLcd(0),
    mUseLcd(useLcd)
{
}

void WeatherStation::init()
{
    //Serial.print("DHT11 : ");
    //Serial.println(mHumidityType);
    if (mHumidityType > 0)
    {
        // DHT11 présent
        mBoard.initDht(mHumidityType - 1);
    }
    //Serial.print("DS18B20 : ");
    //Serial.println(mTemperaturePresent);
    if (mTemperaturePresent)
    {
        // DS18B20 présent
        mBoard.initDs18b20();
    }
    //Serial.print("BMP085 : ");
    //Serial.println(mPressurePresent);
    if (mPressurePresent)
    {
        // BMP085 présent
        mBoard.initBmp085();
    }
    //Serial.print("LCD : ");
    //Serial.println(mLcdPresent);
    if (mLcdPresent)
    {
        if (mTypeLcd == LCD_SSD1306)
        {
            //            mBoard.initOled();
        }
        else
        {
            // Ecran LCD présent
            mLcdNbCols = 16;
            mLcdNbLines = 2;
            if (mTypeLcd == LCD_2004)
            {
                mLcdNbCols = 20;
                mLcdNbLines = 4;
            }
            initLcd(mLcdAddress, mLcdNbCols, mLcdNbLines);
            // caractère spéciaux
            if (mLcd != 0)
            {
                mLcd->createCharFromFlash(temperatureChar, iconTemperature);
                mLcd->createCharFromFlash(humidityChar, iconHumidity);
                //mLcd->createCharFromFlash(mystOnChar, iconMist);
                mLcd->createCharFromFlash(noInformationChar, iconNoInformation);
                mLcd->createCharFromFlash(linkChar, iconLink);
                mLcd->createCharFromFlash(noClockChar, iconNoClock);
            }
        }
    }
}

bool WeatherStation::getTemperature(float& temperature)
{
    bool retour = false;

    if (mTemperaturePresent)
    {
        retour = mBoard.readDs18b20(temperature);
    }
    else if (mHumidityType)
    {
        uint8_t humidity;
        uint8_t temp;
        retour = mBoard.readTU(temp, humidity);
        temperature = (float)temp;
    }
    return retour;
}

bool WeatherStation::getHumidity(uint8_t& humidity)
{
    bool retour = false;

    if (mHumidityType)
    {
        uint8_t temp;
        retour = mBoard.readTU(temp, humidity);
    }
    return retour;
}

bool WeatherStation::getPressure(float& stationPressure, float& seaPressure, int& forecast)
{
    bool retour = false;

    if (mPressurePresent)
    {
        float tempPressure = -1000;
        retour = mBoard.readBmp085(stationPressure, tempPressure, seaPressure, forecast);
    }
    return retour;
}

void WeatherStation::setTime(uint32_t epoch)
{
    if (mUseRtc)
    {
        rtc.setEpoch(epoch);
    }
}

bool WeatherStation::getTime(DateTime &datetime)
{
    bool retour = false;
    if (mUseRtc)
    {
        datetime = rtc.now();
        retour = true;
    }
    return retour;
}

bool WeatherStation::gerer(bool top, bool pretop)
{
    bool refresh = false;
    if (mLcdPresent)
    {
        if (mUseRtc)
        {
            DateTime currentDate = rtc.now();

            if (mLcd != 0)
            {
                if (mTypeLcd == LCD_2004)
                {
                    mLcd->setCursor(0, mLcdNbLines - 1);
                    mLcd->print(currentDate.date() / 10, DEC);
                    mLcd->print(currentDate.date() % 10, DEC);
                    mLcd->write('/');
                    mLcd->print(currentDate.month() / 10, DEC);
                    mLcd->print(currentDate.month() % 10, DEC);
                    mLcd->print('/');
                    mLcd->print(currentDate.year() / 10, DEC);
                    mLcd->print(currentDate.year() % 10, DEC);
                    mLcd->print(' ');
                    mLcd->print(currentDate.hour() / 10, DEC);
                    mLcd->print(currentDate.hour() % 10, DEC);
                    mLcd->write(':');
                    mLcd->print(currentDate.minute() / 10, DEC);
                    mLcd->print(currentDate.minute() % 10, DEC);
                    //            mLcd->print(':');
                    //            mLcd->print(currentDate.second() / 10, DEC);
                    //            mLcd->print(currentDate.second() % 10, DEC);
                    mLcd->print(' ');
                }
                else
                {
                    mLcd->setCursor(mLcdNbCols - 5, mLcdNbLines - 1);
                    mLcd->print(currentDate.hour() / 10, DEC);
                    mLcd->print(currentDate.hour() % 10, DEC);
                    mLcd->write(':');
                    mLcd->print(currentDate.minute() / 10, DEC);
                    mLcd->print(currentDate.minute() % 10, DEC);
                }
            }
        }
    }
    if (pretop)
    {
        mBoard.acqStage1();
    }
    if (top)
    {
        // si pretop, on laisse le temps de convertir
        if (pretop)
        {
            sleep(800);
        }
        refresh = mBoard.acqStage2();
        if (refresh)
        {
            if (mLcdPresent && mUseLcd)
            {
                float temperatureDs18B20;
                bool temperatureNominal = false;

                if (mTemperaturePresent)
                {
                    if (mLcd != 0)
                    {
                    mLcd->setCursor(0, 0);
                    if (mBoard.readDs18b20(temperatureDs18B20))
                    {
                        mLcd->write(temperatureChar);
                        if (mTypeLcd == LCD_2004)
                        {
                            mLcd->write(' ');
                        }
                        // Affiche la température
                        mLcd->print(temperatureDs18B20 + 0.05, 1);
                        mLcd->write(223); // caractère °
                        mLcd->write('C');
                        mLcd->write(' ');
                        temperatureNominal = true;
                        ////Serial.print("température = ");
                        ////Serial.println(temperatureDs18B20);
                    }
                    else
                    {
                        mLcd->write(noInformationChar);
                    }
                    }
                }

                uint8_t temperature;
                uint8_t humidity;

                float pressure = -1000;
                float tempPressure = -1000;
                float seaPressure = -1000;
                int forecast = WeatherBoard::UNKNOWN;
                if (mPressurePresent)
                {
                    if (!mBoard.readBmp085(pressure, tempPressure, seaPressure, forecast))
                    {
                        pressure = -1000;
                        tempPressure = -1000;
                        seaPressure = -1000;
                        if (mLcd != 0)
                        {
                        mLcd->write(noInformationChar);
                        }
                    }
                    ////Serial.print("pression = ");
                    ////Serial.println(seaPressure);
                }

                if (mHumidityType)
                {
                    if (mBoard.readTU(temperature, humidity))
                    {
                        if (mLcd != 0)
                        {
                        if (!temperatureNominal)
                        {
                            mLcd->setCursor(0, 0);
                            mLcd->write(temperatureChar);
                            if (mTypeLcd == LCD_2004)
                            {
                                mLcd->write(' ');
                            }
                            mLcd->print((float)temperature, 0);
                            mLcd->write(223); // caractère °
                            mLcd->write('C');
                            mLcd->write(' ');
                        }
                        mLcd->write(' ');
                        mLcd->write(humidityChar);
                        if (mTypeLcd == LCD_2004)
                        {
                            mLcd->write(' ');
                        }
                        mLcd->print((float)humidity, 0);
                        mLcd->write('%');
                        mLcd->write(' ');
                        mLcd->write(' ');
                        //Serial.print("humidité = ");
                        //Serial.println(humidity);
                        }
                    }
                    else
                    {
                        if (mLcd != 0)
                        {
                        if ((!temperatureNominal) && (tempPressure != -1000))
                        {
                            mLcd->setCursor(0, 0);
                            mLcd->write(temperatureChar);
                            mLcd->write(' ');
                            mLcd->print(tempPressure + 0.05, 1);
                            mLcd->write(223); // caractère °
                            mLcd->write('C');
                            mLcd->write(' ');
                        }
                        else
                        {
                            mLcd->write(noInformationChar);
                        }
                        }
                    }
                }
                if (mPressurePresent)
                {
                    if (mLcd != 0)
                    {
                    mLcd->setCursor(0, 1);
                    if (pressure != -1000)
                    {
                        mLcd->write('P');
                        if (mTypeLcd == LCD_2004)
                        {
                            mLcd->write(' ');
                        }
                        mLcd->print(seaPressure + 0.05, 1);
                        mLcd->print("hPa");
                        mLcd->write(' ');
                        if (mTypeLcd == LCD_2004)
                        {
                            mLcd->print(forecastString(forecast));
                            mLcd->write(' ');
                        }
                    }
                    else
                    {
                        mLcd->write(noInformationChar);
                    }
                    }
                }
            }
        }
    }
    return refresh;
}

void WeatherStation::log(uint8_t line, String &chaine)
{
    if (mLcd != 0)
    {
        mLcd->setCursor(0, line);
        mLcd->print(chaine.c_str());
    }
}

void WeatherStation::clearScreen()
{
    if (mLcd != 0)
    {
        mLcd->clear();
    }
}

void WeatherStation::setBacklight(bool on)
{
    if (mLcd != 0)
    {
        if (on)
        {
            mLcd->backlight();
        }
        else
        {
            mLcd->noBacklight();
        }
    }
}


const String &WeatherStation::forecastString(int forecast)
{
    if ((forecast > 0) && (forecast <= WeatherBoard::UNKNOWN))
    {
        return weatherForecast[forecast];
    }
    else
    {
        return weatherForecast[WeatherBoard::UNKNOWN];
    }
}

void WeatherStation::showLink(bool ok)
{
    if (mLcd != 0)
    {
        mLcd->setCursor(mLcdNbCols - 1, 0);
        if (ok)
        {
            mLcd->write(linkChar);
        }
        else
        {
            mLcd->write(noInformationChar);
        }
    }
}


void WeatherStation::showRtcSync(bool ok)
{
    if (mLcd != 0)
    {
        mLcd->setCursor(mLcdNbCols - 2, 0);
        if (ok)
        {
            mLcd->write(' ');
        }
        else
        {
            mLcd->write(noClockChar);
        }
    }
}

void WeatherStation::initLcd(uint8_t adresse, uint8_t nbCols, uint8_t nbLignes)
{
    if (mLcd == 0)
    {
        mLcd = new LiquidCrystal_I2C(adresse, nbCols, nbLignes); // set the LCD address to 0x20 for a 16 chars and 2 line display
        mLcd->init();                      // initialize the lcd
        mLcd->clear();
        mLcd->backlight();
    }
}

#include "MiniWeatherBoard.h"
#include <Sodaq_DS3231.h>
#include <LiquidCrystal_I2C.h>

#include "mystgenerator.h"
#include "Brumisateur.h"


uint8_t iconTemperature[8] = {0x4, 0xa, 0xa, 0xa, 0xa, 0x11, 0x11, 0xe}; // 8
uint8_t iconHumidity[8] = {0x0, 0x4, 0xa, 0x11, 0x11, 0xe, 0x0, 0x0}; // 8
uint8_t iconMist[8] = {
    0b00000,
    0b00100,
    0b01110,
    0b11111,
    0b11111,
    0b01110,
    0b00000,
    0b00000
};


Brumisateur::Brumisateur() :
    mBoard(*new MiniWeatherBoard())
{
}

void Brumisateur::init()
{
    // RTC toujour présente
//    rtc.begin();
    // DHT11 présent
    mBoard.initDht11();
    // DS18B20 présent
    //mBoard.initDs18b20();
    // BMP085 absent
    //mBoard.initBmp085();
    // Générateur de brume présent
    mBoard.initMystGenerator();
    // Ecran LCD présent
    mBoard.initLcd();

    // caractère spéciaux
    mBoard.lcd()->load_custom_character(temperatureChar, iconTemperature);
    mBoard.lcd()->load_custom_character(humidityChar, iconHumidity);
    mBoard.lcd()->load_custom_character(mystOnChar, iconMist);

    // Fréquence du PWN max
    uint8_t mode = 0x01;
    TCCR2B = TCCR2B & 0b11111000 | mode;
}

void Brumisateur::gerer()
{
    static uint8_t top = 0;
    if (mBoard.gerer())
    {
        top++;
//        DateTime currentDate = rtc.now();

        float temp;
        bool temperatureNominal = false;

        mBoard.lcd()->setCursor(0, 0);
        mBoard.lcd()->write(temperatureChar);
        mBoard.lcd()->write(' ');
        if (mBoard.readDs18b20(temp))
        {
            // Affiche la température
//            mBoard.lcd()->print(temp + 0.05, 1);
//            mBoard.lcd()->write(223); // caractère °
//            mBoard.lcd()->write('C');
//            mBoard.lcd()->write(' ');
//            temperatureNominal = true;
        }

        uint8_t temperature;
        uint8_t humidity;

        if (mBoard.readTU(temperature, humidity))
        {
            if (!temperatureNominal)
            {
                mBoard.lcd()->print((float)temperature, 0);
                mBoard.lcd()->write(223); // caractère °
                mBoard.lcd()->write('C');
                mBoard.lcd()->write(' ');
            }
            mBoard.lcd()->write(' ');
            if (!mBoard.mystGenerator()->started())
            {
                mBoard.lcd()->write(humidityChar);
                //analogWrite(3, 0);
            }
            else
            {
                if (mBoard.mystGenerator()->outputOn())
                {
                    mBoard.lcd()->write(mystOnChar);
                    //analogWrite(3, analogRead(A6)/4);
                }
                else
                {
                    //analogWrite(3, 0);
                    if (top % 2)
                    {
                        mBoard.lcd()->write(humidityChar);
                    }
                    else
                    {
                        mBoard.lcd()->write(mystOnChar);
                    }
                }
            }
            mBoard.lcd()->write(' ');
            mBoard.lcd()->print((float)humidity, 0);
            mBoard.lcd()->write('%');
            mBoard.lcd()->write(' ');
            mBoard.lcd()->write(' ');
            if (humidity < mHumidityThreshold)
            {
                mBoard.mystGenerator()->start();
            }
            else
            {
                mBoard.mystGenerator()->stop();
            }
        }
        //analogWrite(10, analogRead(A3)/4);

        mBoard.lcd()->setCursor(0, 1);
//        mBoard.lcd()->print(currentDate.hour() / 10, DEC);
//        mBoard.lcd()->print(currentDate.hour() % 10, DEC);
//        mBoard.lcd()->write(':');
//        mBoard.lcd()->print(currentDate.minute() / 10, DEC);
//        mBoard.lcd()->print(currentDate.minute() % 10, DEC);
//        mBoard.lcd()->print(':');
//        mBoard.lcd()->print(currentDate.second() / 10, DEC);
//        mBoard.lcd()->print(currentDate.second() % 10, DEC);
//        mBoard.lcd()->print(' ');

        uint16_t onDuration;
        uint16_t offDuration;
        mBoard.mystGenerator()->durations(onDuration, offDuration);
        mBoard.lcd()->print((onDuration / 100) % 10, DEC);
        mBoard.lcd()->print((onDuration / 10) % 10, DEC);
        mBoard.lcd()->print(onDuration % 10, DEC);
        mBoard.lcd()->print(' ');
        mBoard.lcd()->print((offDuration / 100) % 10, DEC);
        mBoard.lcd()->print((offDuration / 10) % 10, DEC);
        mBoard.lcd()->print(offDuration % 10, DEC);
        mBoard.lcd()->print(' ');



        analogWrite(3, onDuration % 255);


    }
    else
    {
    }

}

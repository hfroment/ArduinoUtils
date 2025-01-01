#include "Board.h"
#include <Sodaq_DS3231.h>
#include <LiquidCrystal_I2C.h>

#include "paludarium.h"


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


Paludarium::Paludarium() :
    mBoard(*new DevBoardShaton())
{
}

void Paludarium::init()
{
    // RTC toujour présente
    rtc.begin();
    // DHT11 présent
    mBoard.initDht11();
    // DS18B20 présent
    mBoard.initDs18b20();
    // BMP085 absent
    //mBoard.initBmp085();
    // Ecran LCD présent
    mBoard.initLcd();

    // caractère spéciaux
    mBoard.lcd()->load_custom_character(temperatureChar, iconTemperature);
    mBoard.lcd()->load_custom_character(humidityChar, iconHumidity);
    mBoard.lcd()->load_custom_character(mystOnChar, iconMist);
}

void Paludarium::gerer()
{
    static uint8_t fakePwmCounter = 0;
    static uint8_t fakePwmValue = 0;

    if (mBoard.gerer())
    {
        DateTime currentDate = rtc.now();

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

        fakePwmValue = analogRead(A3)/40;

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
            if (!mBoard.mystGenerator().started())
            {
                mBoard.lcd()->write(humidityChar);
                analogWrite(10, 0);
            }
            else
            {
                if (mBoard.mystGenerator().outputOn())
                {
                    mBoard.lcd()->write(mystOnChar);
                    analogWrite(10, analogRead(A3)/4);
                }
                else
                {
                    analogWrite(10, 0);
                    if (currentDate.second() % 2)
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
                mBoard.mystGenerator().start();
            }
            else
            {
                mBoard.mystGenerator().stop();
            }
        }
        //analogWrite(10, analogRead(A3)/4);

        mBoard.lcd()->setCursor(0, 1);
        mBoard.lcd()->print(currentDate.hour() / 10, DEC);
        mBoard.lcd()->print(currentDate.hour() % 10, DEC);
        mBoard.lcd()->write(':');
        mBoard.lcd()->print(currentDate.minute() / 10, DEC);
        mBoard.lcd()->print(currentDate.minute() % 10, DEC);
        mBoard.lcd()->print(':');
        mBoard.lcd()->print(currentDate.second() / 10, DEC);
        mBoard.lcd()->print(currentDate.second() % 10, DEC);
        mBoard.lcd()->print(' ');

        uint16_t onDuration;
        uint16_t offDuration;
        mBoard.mystGenerator().durations(onDuration, offDuration);
        mBoard.lcd()->print((onDuration / 100) % 10, DEC);
        mBoard.lcd()->print((onDuration / 10) % 10, DEC);
        mBoard.lcd()->print(onDuration % 10, DEC);
        mBoard.lcd()->print(' ');
        mBoard.lcd()->print((offDuration / 100) % 10, DEC);
        mBoard.lcd()->print((offDuration / 10) % 10, DEC);
        mBoard.lcd()->print(offDuration % 10, DEC);
        mBoard.lcd()->print(' ');
        //        mBoard.lcd()->print(analogRead(A3));
        //        mBoard.lcd()->print(' ');
        //        mBoard.lcd()->print(analogRead(A3)/4);

        //      uint8_t warm;
        //      uint8_t cold;
        //      mBoard.lightValues(warm, cold);
        //      mBoard.lcd()->print(warm);
        //      mBoard.lcd()->write(' ');
        //      mBoard.lcd()->print(cold);
        //      mBoard.lcd()->write(' ');
        //      if (currentDate.minute() % 5 == 0)
        //      {
        //        if (currentDate.minute() % 2 == 0)
        //        {
        //          //mBoard.dawn();
        //        }
        //        else
        //        {
        //          //mBoard.twilight();
        //        }
        //      }

        //  mBoard.lcd()->setCursor(14, 1);
        //  if (mBoard.mystGenerator().outputOn())
        //  {
        //      mBoard.lcd()->write(255);
        //  }
        //  else
        //  {
        //      mBoard.lcd()->write(' ');
        //  }
        //  if (mBoard.mystGenerator().started())
        //  {
        //      mBoard.lcd()->write(255);
        //  }
        //  else
        //  {
        //      mBoard.lcd()->write(' ');
        //  }
    }
    else
    {
        fakePwmCounter++;
//        if ((fakePwmCounter % 2) == 0)
//        {
//            digitalWrite(10, 1);
//        }
//        else
//        {
//            digitalWrite(10, 0);
//        }
        //        if (mBoard.mystGenerator().outputOn())
        //        {
        //            if (1)//analogRead(A3)/4 < 128)
        //            {
//                      digitalWrite(10, !digitalRead(10));
        //            }
        //            else
        //            {
        //                digitalWrite(10, 1);
        //            }
        //        }
        //        else
        //        {
        //            digitalWrite(10, 0);
        //        }
    }
//    digitalWrite(10, 1);
//    delay(1);
//    digitalWrite(10, 0);
//    delay(fakePwmValue);

}

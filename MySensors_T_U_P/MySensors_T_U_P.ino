#include <PinChangeInt.h>

#include <WeatherStation.h>
#include <Sodaq_DS3231.h>

#include <dataaverage.h>

// Enable debug prints to serial monitor
//#define MY_DEBUG
#define MY_NODE_ID 1
// Enabled repeater feature for this node
#define MY_REPEATER_FEATURE

#include <MyConfigFlea.h>
//#undef MY_RF24_PA_LEVEL
//#define MY_RF24_PA_LEVEL RF24_PA_MIN

#include <MyConfig.h>
#include <MySensors.h>

enum {
    CHILD_ID_HUM,
    CHILD_ID_TEMP,
    CHILD_ID_BARO
};

const uint8_t secondeTop = 00;
const uint8_t secondePretop = 59;

WeatherStation* weatherStation;

bool metric = true;

MyMessage temperatureMsg(CHILD_ID_TEMP, V_TEMP);
MyMessage humidityMsg(CHILD_ID_HUM, V_HUM);
MyMessage pressureMsg(CHILD_ID_BARO, V_PRESSURE);
//MyMessage forecastMsg(CHILD_ID_BARO, V_FORECAST);

String chainePresentation = "Shaton meteo (";
uint8_t versionConfig = 0;
bool useRtc = false;
bool lcdPresent = false;
uint8_t lcdAddress = 0;
uint8_t lcdType = WeatherStation::LCD_1602;
bool pressurePresent = false;
bool humidityPresent = false;
uint8_t humidityType = 0;
bool temperaturePresent = false;

void before()
{
    
     saveState(versionConfig, 1);
    saveState(PositionConfigRtc, 1);
    saveState(PositionConfigAdresseLcd, 0x27);
    saveState(PositionConfigTypeLcd, WeatherStation::LCD_2004);
    saveState(PositionConfigPressure, 1);
    saveState(PositionConfigHumidity, 1);
    saveState(PositionConfigTemperature, 1);
  

    versionConfig = loadState(PositionVersionConfig);
    switch (versionConfig)
    {
    case 1:
        useRtc = (loadState(PositionConfigRtc) != 0);
        lcdAddress = loadState(PositionConfigAdresseLcd);
        lcdPresent = (lcdAddress != 0);
        lcdType = loadState(PositionConfigTypeLcd);
        pressurePresent = (loadState(PositionConfigPressure) != 0);
        humidityType = loadState(PositionConfigHumidity);
        humidityPresent = (humidityType != 0);
        temperaturePresent = (loadState(PositionConfigTemperature) != 0);
        break;
    }

    if (temperaturePresent)
    {
        chainePresentation += "T";
    }
    if (humidityPresent)
    {
        chainePresentation += "U";
    }
    if (pressurePresent)
    {
        chainePresentation += "P";
    }
    if (useRtc)
    {
        chainePresentation += "R";
    }
    if (lcdPresent)
    {
        chainePresentation += "L";
    }
    chainePresentation += ")";
    Serial.println("before()");
}
void presentation()
{
    Serial.println("presentation()");
    // Send the sketch version information to the gateway and Controller
    sendSketchInfo(chainePresentation.c_str(), "1.0");

    // Register all sensors to gateway (they will be created as child devices)
    if (temperaturePresent)
    {
        present(CHILD_ID_TEMP, S_TEMP);
    }
    if (humidityPresent)
    {
        present(CHILD_ID_HUM, S_HUM);
    }
    if (pressurePresent)
    {
        present(CHILD_ID_BARO, S_BARO);
    }
    metric = getControllerConfig().isMetric;
}

const byte interruptPin = A0;
bool timeReceived = false;
const byte interruptPinTtp223 = 2;

void setup()
{
    weatherStation = new WeatherStation(useRtc, temperaturePresent, lcdAddress, lcdType, humidityType, pressurePresent);
    weatherStation->init();
    if (lcdPresent)
    {
        weatherStation->log(0, chainePresentation);
        weatherStation->log(1, "Node ID = " + String(getNodeId()));
        delay(5000);
        weatherStation->clearScreen();
        Serial.println(chainePresentation);
        Serial.println("Node ID = " + String(getNodeId()));
    }
    else
    {
        Serial.println(chainePresentation);
        Serial.println("Node ID = " + String(getNodeId()));
    }
    // Request latest time from controller at startup
    if (useRtc)
    {
        pinMode(interruptPin, INPUT_PULLUP);  // Configure the pin as an input, and turn on the pullup resistor.
        // See http://arduino.cc/en/Tutorial/DigitalPins
        attachPinChangeInterrupt(interruptPin, interruptFunction, FALLING);
        //Enable Interrupt
        rtc.enableInterrupts(EveryMinute); //interrupt at  EverySecond, EveryMinute, EveryHour
        requestTime();
    }
    else
    {
        timeReceived = true;
    }
    pinMode(interruptPinTtp223, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptPinTtp223), backlightOn, CHANGE);
    weatherStation->setBacklight(false);
}

// This is called when a new time value was received
void receiveTime(unsigned long controllerTime)
{
    // Ok, set incoming time
    weatherStation->setTime(controllerTime);
    //Serial.println("weatherStation->setTime(controllerTime)");
    timeReceived = true;
}

static const uint8_t periodeEnvoi = 6;

DataAverage temperatureAverage(periodeEnvoi);
DataAverage humidityAverage(periodeEnvoi);
DataAverage pressureAverage(periodeEnvoi);

bool itRtcRecue = false;
bool setBacklightOn = false;

void loop()
{
    // Sleep time between reads (in milliseconds). Do not change this value as the forecast algorithm needs a sample every minute.
    const unsigned long sleepTime = 900;
    static uint8_t secondeCourante = 59;
    static bool linkLost = false;
    static uint8_t lastSentMinute = 255;
    static bool backlightState = true;

    bool top = false;
    bool pretop = false;

    static bool isMinute = false;

    if (isMinute)
    {
        bool linkOk = isTransportReady();
        weatherStation->showLink(linkOk);
        if (linkOk)
        {
            if (linkLost || !timeReceived)
            {
                requestTime();
                linkLost = false;
            }
        }
        else
        {
            linkLost = true;
        }

        bool okToSend = false;

        if (useRtc && timeReceived)
        {
            top = true;
            pretop = true;
            uint8_t currentMinute = rtc.now().minute();
            if (((currentMinute % periodeEnvoi) == 0) && (lastSentMinute != currentMinute))
            {
                lastSentMinute = currentMinute;
                okToSend = true;
            }
        }
        else
        {
            uint8_t seconde;
            secondeCourante = (secondeCourante + 1) % 60;
            seconde = secondeCourante;
            if (seconde == secondeTop)
            {
                top = true;
                okToSend = true;
            }
            else if (seconde == secondePretop)
            {
                pretop = true;
            }
        }

        if (weatherStation->gerer(top, pretop))
        {
            float temperature;
            if (weatherStation->getTemperature(temperature))
            {
                if (!metric)
                {
                    // Convert to fahrenheit
                    temperature = temperature * 9.0 / 5.0 + 32.0;
                }
                temperatureAverage.addSample(temperature);
            }
            float stationPressure;
            float seaPressure;
            int forecast;
            if (weatherStation->getPressure(stationPressure, seaPressure, forecast))
            {
                pressureAverage.addSample(seaPressure);
            }
            uint8_t humidity;
            if (weatherStation->getHumidity(humidity))
            {
                humidityAverage.addSample(humidity);
            }

            if (linkOk && okToSend)
            {
                send(temperatureMsg.set(temperatureAverage.average(), 1));
                send(pressureMsg.set(pressureAverage.average(), 1));
                //send(forecastMsg.set(weatherStation->forecastString(forecast).c_str()));
                send(humidityMsg.set(humidityAverage.average(), 0));
            }
        }
        //Serial.println(Read_Volts());
        weatherStation->showLink(linkOk);
        weatherStation->showRtcSync(timeReceived);
        weatherStation->setBacklight(setBacklightOn);
    }

    if (useRtc)
    {
        rtc.clearINTStatus();
        if (timeReceived)
        {
            sleep(A0, FALLING, interruptPinTtp223, CHANGE, 0);
            if (itRtcRecue)
            {
                isMinute = true;
                itRtcRecue = false;
            }
            else
            {
                isMinute = false;
            }
        }
        else
        {
            wait(sleepTime);
            isMinute = true;
        }
    }
    else
    {
        sleep(sleepTime);
        isMinute = true;
    }

    if (setBacklightOn != backlightState)
    {
        weatherStation->setBacklight(setBacklightOn);
        backlightState = setBacklightOn;
    }
}
void interruptFunction() // handle pin change interrupt for A0 to A5 here
{
    itRtcRecue = true;
    // Serial.println(" External Interrupt detected ");
}
void backlightOn()
{
    //    Serial.print("TTP223 Interrupt detected : ");
    //    Serial.println(digitalRead(interruptPinTtp223));
    if (HIGH == digitalRead(interruptPinTtp223))
    {
        setBacklightOn = true;
    }
    else
    {
        setBacklightOn = false;
    }
}


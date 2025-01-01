#include <WeatherStation.h>
#include <LiquidCrystal_I2C.h>

//#include <Vcc.h>


// Enable debug prints to serial monitor
//#define MY_DEBUG
#define MY_NODE_ID 3

#include <MyConfigFlea.h>
// On attend le contrôleur pour démarrer
#include <MyConfig.h>
#include <MySensors.h>
#include <dht.h>
#include <DallasTemperature.h>
#include <OneWire.h>

static const uint8_t ds18b20Pin = A2;
OneWire oneWire(ds18b20Pin); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature dsTemp(&oneWire); // Pass the oneWire reference to Dallas Temperature.
// arrays to hold device address
DeviceAddress* deviceTemperature;
uint8_t numberOfTemp = 0;

String chainePresentation = "Shaton nT";

MyMessage* temperatureMsg;
float* lastTemp;

WeatherStation* weatherStation;

enum {
    CHILD_ID_BATTERY,
    FIRST_CHILD_TEMP
};

void before()
{
    String logStr;
    weatherStation = new WeatherStation(false, false, 0x27, WeatherStation::LCD_1602, false, false);
    weatherStation->init();
    // Startup up the OneWire library
    dsTemp.begin();
    dsTemp.setResolution(12);
    numberOfTemp = dsTemp.getDeviceCount();
    if (numberOfTemp > 0)
    {
        deviceTemperature = new DeviceAddress[numberOfTemp];
        temperatureMsg = new MyMessage[numberOfTemp];
        lastTemp = new float[numberOfTemp];
        for (int i = 0; i < numberOfTemp; i++)
        {
            lastTemp[i] = 0;
            temperatureMsg[i].setSensor(FIRST_CHILD_TEMP + i);
            temperatureMsg[i].setType(V_TEMP);
            if (!dsTemp.getAddress(deviceTemperature[i], i))
            {
                logStr = "Unable to find address for DS18B20";
                weatherStation->log(1, logStr);
            }
        }
    }
    else
    {
        logStr = "No sensors !";
        weatherStation->log(1, logStr);
    }
}

bool metric = true;

void presentation()
{
    //    lcd.println(chainePresentation);
    // Send the sketch version information to the gateway and Controller
    sendSketchInfo(chainePresentation.c_str(), "1.0");

    // Register all sensors to gateway (they will be created as child devices)
    for (int i = 0; i < numberOfTemp; i++)
    {
        present(FIRST_CHILD_TEMP + i, S_TEMP);
    }
    present(CHILD_ID_BATTERY, S_MULTIMETER);
    metric = getControllerConfig().isMetric;
}

void setup()
{
    dsTemp.begin();
    // requestTemperatures() will not block current thread
    dsTemp.setWaitForConversion(false);
    weatherStation->log(0, chainePresentation);
    weatherStation->log(1, "Node ID = " + String(getNodeId()));
    wait(2000);
}

//const float VccExpected   = 5.0;
//const float VccCorrection = 2.860 / 2.92; // Measured Vcc by multimeter divided by reported Vcc
//Vcc vcc(VccCorrection);

MyMessage batteryMsg(CHILD_ID_BATTERY, V_VOLTAGE);
static const uint8_t maxCharLine = 16;

void loop()
{
    //static float lastVoltage = 0;
    //static float batteryMax = 5;
    //static float batteryMin = 3.0;
    static uint8_t nNoUpdatesTemp = 0;
    static const uint8_t forceUpdateNReads = 10;
    static const long cycleNormalMs = 60000;
    static const long cycleTurboMs = 6000;
    static const long preReadMs = 1000;
    static const float tempTurbo = 80; // °C

    dsTemp.requestTemperatures();
    sleep(preReadMs);

    String ligne1("T=");
    String ligne2("T=");
    bool sent = false;
    float tempMax = 0;
    for (int i = 0; i < numberOfTemp; i++)
    {
        float temperature = dsTemp.getTempC(deviceTemperature[i]);
        if (!isnan(temperature))
        {
            if (temperature > tempMax)
            {
                tempMax = temperature;
            }
            if (temperature != lastTemp[i] || nNoUpdatesTemp == forceUpdateNReads)
            {
                // Only send temperature if it changed since the last measurement or if we didn't send an update for n times
                lastTemp[i] = temperature;
                if (!metric)
                {
                    temperature = DallasTemperature::toFahrenheit(temperature);
                }
                // Reset no updates counter
                nNoUpdatesTemp = 0;
                sent = true;
                send(temperatureMsg[i].set(temperature, 1));

#ifdef MY_DEBUG
                Serial.print("T: ");
                Serial.println(temperature);
#endif
            }
        }
        String* ligne = &ligne1;
        if (i >= numberOfTemp / 2)
        {
            ligne = &ligne2;
        }
        // Affiche la température
        *ligne += String(temperature, 1) + (char)223 + "C ";
    }
    if (ligne1.length() > 3)
    {
        for (int i = ligne1.length(); i <= maxCharLine; i++)
        {
            ligne1 += ' ';
        }
        weatherStation->log(0, ligne1);
    }
    if (ligne2.length() > 3)
    {
        for (int i = ligne2.length(); i <= maxCharLine; i++)
        {
            ligne2 += ' ';
        }
        weatherStation->log(1, ligne2);
    }

    if (!sent)
    {
        // Increase no update counter if the temperature stayed the same
        nNoUpdatesTemp++;
    }

    bool linkOk = isTransportReady();
    weatherStation->showLink(linkOk);
    if (tempMax > tempTurbo)
    {
        sleep(cycleTurboMs - preReadMs);
    }
    else
    {
        sleep(cycleNormalMs - preReadMs);
    }
}


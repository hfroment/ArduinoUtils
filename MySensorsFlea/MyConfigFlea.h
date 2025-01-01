/*
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

/**
 * @file MyConfigFlea.h
 *
 * MySensors specific configurations
 */
#ifndef MyConfigFlea_h
#define MyConfigFlea_h
#include <stdint.h>

enum
{
    PositionVersionConfig = 0,
    PositionConfigRtc,
    PositionConfigAdresseLcd,
    PositionConfigTypeLcd,
    PositionConfigPressure,
    PositionConfigHumidity,
    PositionConfigTemperature,

    TailleConfiguration
};



/**********************************
*  Radio selection and node config
***********************************/
/**********************************
*  NRF24L01P Driver Defaults
***********************************/

#define MY_RADIO_NRF24
//#define MY_RADIO_RF24

/**
 * @def MY_RF24_CE_PIN
 * @brief Default RF24 chip enable pin setting. Override in sketch if needed.
 */
#define MY_RF24_CE_PIN 8

/**
 * @def MY_RF24_CS_PIN
 * @brief Default RF24 chip select pin setting. Override in sketch if needed.
 */
#define MY_RF24_CS_PIN 7

/**
 * @def MY_RF24_PA_LEVEL
 * @brief Default RF24 PA level. Override in sketch if needed.
 */
#ifndef MY_RF24_PA_LEVEL
#define MY_RF24_PA_LEVEL RF24_PA_MAX
#endif

/**
 * @def MY_RF24_CHANNEL
 * @brief RF channel for the sensor net, 0-125.
 * Frequence: 2400 Mhz - 2525 Mhz Channels: 126
 * http://www.mysensors.org/radio/nRF24L01Plus.pdf
 * 0 => 2400 Mhz (RF24 channel 1)
 * 1 => 2401 Mhz (RF24 channel 2)
 * 76 => 2476 Mhz (RF24 channel 77)
 * 83 => 2483 Mhz (RF24 channel 84)
 * 124 => 2524 Mhz (RF24 channel 125)
 * 125 => 2525 Mhz (RF24 channel 126)
 * In some countries there might be limitations, in Germany for example only the range 2400,0 - 2483,5 Mhz is allowed
 * http://www.bundesnetzagentur.de/SharedDocs/Downloads/DE/Sachgebiete/Telekommunikation/Unternehmen_Institutionen/Frequenzen/Allgemeinzuteilungen/2013_10_WLAN_2,4GHz_pdf.pdf
 */
#define MY_RF24_CHANNEL	1

/**
 * @def MY_RF24_DATARATE
 * @brief RF24 datarate (RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps or RF24_2MBPS for 2Mbps).
 */
#define MY_RF24_DATARATE RF24_250KBPS

/**
* @def MY_TRANSPORT_WAIT_READY_MS
* @brief Timeout in MS until transport is ready during startup, set to 0 for no timeout
*/
#define MY_TRANSPORT_WAIT_READY_MS (5000ul)

#endif

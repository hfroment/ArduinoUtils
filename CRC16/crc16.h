#ifndef CRC16_H
#define CRC16_H

#include <Arduino.h>

class Crc16
{
public:
    Crc16();

    static uint16_t Calculer(uint8_t* donnees, uint16_t taille, uint16_t valeurInitiale = 0xFFFF);

private:
};

#endif // CRC16_H

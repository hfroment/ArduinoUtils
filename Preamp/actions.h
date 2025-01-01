#ifndef ACTIONS_H
#define ACTIONS_H

//#include "options.h"

class ActionsPreampli
{
public:
    typedef enum
    {
        AucuneAction = 0,
        VolumePlus = 0x0001,
        VolumeMoins = 0x0002,
        SelectionSuivante = 0x0004,
        SelectionPrecedente = 0x0008,
        VolumeParDefaut = 0x0010,
        BalanceGauche = 0x0020,
        BalanceDroite = 0x0040,
        Refresh = 0x0080,
        // 0x0100
        // 0x0200
        // 0x0400
        // 0x0800
        ActiverEntreeCourante = 0x1000,

        ToggleMute = 0x2000,

        PowerOff = 0x4000,

        Retour = 0x8000
    }
    teActionPreampli;
};

class ActionsServitudes
{
public:
    typedef enum
    {
        AlimKodi,
        AlimAmpliSalon,
        AlimAmpliTerrasse,
        AlimAmpliVeranda,
        AlimAux1,
        AlimAux2,

        NombreCibles
    }
    teCibleActionServitudes;

    typedef enum
    {
        Off,
        On,
        Toggle
    }
    teTypeActionServitudes;

};

class DialogDefinition
{
public:
    static const uint8_t servitudesI2cId = 8;
};

#endif // ACTIONS_H

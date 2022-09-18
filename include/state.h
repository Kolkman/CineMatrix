#ifndef MATRIXSTATE_H
#define MATRIX_STATE_H

#include "config.h"
#include <MD_Parola.h>

class MatrixState
{
public:
    MatrixState();
    char * getText();
    bool init();
    textPosition_t getPosition();
    textEffect_t getEffect();
    uint16_t getSpeed();
    char * getWifiSSID();
    char * getWifiPass();
    bool stateChange();
private:
    int elementID;
    int elementItterator=0;
    MatrixConfig myConfig;
};

#endif
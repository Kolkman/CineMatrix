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
    MatrixConfig myConfig;  
    textPosition_t getPosition();
    textEffect_t getEffect();
    uint16_t getSpeed();
    bool stateChange();
private:
    int elementID;
    int elementItterator=0;

};

#endif
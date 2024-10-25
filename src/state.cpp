#include "state.h"
#include "config.h"
#include "debug.h"
MatrixState::MatrixState(MatrixConfig * mc)
{
    // Empty
    myConfig= mc;
    elementID = 0;
    elementItterator = 0;
}


bool MatrixState::stateChange()
{
    if (elementItterator < myConfig->element[elementID].repeat ){
        elementItterator++;
    }else{
        elementItterator=1;
        if (elementID<MAXTEXTELEMENTS){
            elementID++;
        }else{
            elementID=0;
        }
    }
    return true;    
}



char *MatrixState::getText()
{
    return myConfig->element[elementID].matrixtext;
}

textPosition_t MatrixState::getPosition()
{
    return myConfig->element[elementID].position;
}

textEffect_t MatrixState::getEffect()
{
    return myConfig->element[elementID].effect;
}

uint16_t MatrixState::getSpeed()
{
    return myConfig->element[elementID].speed;
}

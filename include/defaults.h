#ifndef MATRIXDEFAULT_H
#define MATRIXDEFAULT_H

// HARDWARE
#define MAX_DEVICES 12 // Number of 8x8 Matrices



#define DATA_PIN 12
#define CS_PIN 14
#define CLK_PIN 27

#define DEFAULT_SPEED 40

#define CONFIGPORTAL_TIMEOUT 60 * 1000 // When configured the Configportal will stay up for a wee bt
#define ALWAYS_START_WITH_PORTAL true


#define RFC952_HOSTNAME "CineMatrix"

#define SHOWINFODURATION 60 // time to show the information t startup in seconds


#define MAXTEXTELEMENTS 5 // Amount of different sentences to display

// Note you FIELDS have to match what is in LINES
#define DEFAULTLINE0 "U kijkt naar [[film]]"
#define DEFAULTLINE1 "De pauze voor deze film is om [[pauze]]"
#define DEFAULTLINE2 "De film eindigt om [[eind]]"
#define DEFAULTFIELD0 "film"
#define DEFAULTFIELD1 "pauze"
#define DEFAULTFIELD2 "eind"
#define DEFAULTVALUE0 "Sneeuwwitje"
#define DEFAULTVALUE1 "23:30"
#define DEFAULTVALUE2 "02:00"

#define TEXTLENGTH 256 // Maximum length of text to be displayed


#endif
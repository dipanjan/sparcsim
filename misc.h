#ifndef MISC_H
#define MISC_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <constants.h>



union 
{
        unsigned long floatToHex;
        float hexToFloat;
}convertFloat;



union 
{
        unsigned long doubleToHex[2];
        double hexToDouble;
}convertDouble;



long findByToken(char* token);
char* trim(char* stringToBeTrimmed);
void colorize(const char *str, const char *colorCode);
void color(const char *str, int colorIndex);
int isSNaN32(float float32);
int isQNaN32(float float32);
int isSNaN64(double float64);
int isQNaN64(double float64);

#endif

#ifndef MATRIXPORTALDRIVER_MATRIXPORTALM4_H
#define MATRIXPORTALDRIVER_MATRIXPORTALM4_H

#include "samd51/include/samd51j19a.h"

#define _MD_PORT_OUTPUT PORT->Group[1].DIRSET.reg
#define _MD_PORT_SET PORT->Group[1].OUTSET.reg
#define _MD_PORT_CLEAR PORT->Group[1].OUTCLR.reg
#define _MD_R1 0
#define _MD_G1 1
#define _MD_B1 2
#define _MD_R2 3
#define _MD_G2 4
#define _MD_B2 5
#define _MD_A 7
#define _MD_B 8
#define _MD_C 9
#define _MD_D 15
#define _MD_E 13
#define _MD_CLK 6
#define _MD_LAT 14
#define _MD_OE 12

#endif //MATRIXPORTALDRIVER_MATRIXPORTALM4_H

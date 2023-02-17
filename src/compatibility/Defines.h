//This file is part of the MatrixDriver library.
//It is used to define MCU specific behaviour to standard functions.
//All definitions in this file are prefixed with _MD_ to prevent conflicts with other libraries.
//If you want to add support for a new MCU, add the definitions for the following:
// - _MD_PORT_OUTPUT - The register that controls the output direction of the pins
// - _MD_PORT_SET - The register that sets the pins to high
// - _MD_PORT_CLEAR - The register that sets the pins to low
// - _MD_R1 - The offset of the R1 pin in the register
// - _MD_G1 - The offset of the G1 pin in the register
// - _MD_B1 - The offset of the B1 pin in the register
// - _MD_R2 - The offset of the R2 pin in the register
// - _MD_G2 - The offset of the G2 pin in the register
// - _MD_B2 - The offset of the B2 pin in the register
// - _MD_A - The offset of the A pin in the register
// - _MD_B - The offset of the B pin in the register
// - _MD_C - The offset of the C pin in the register
// - _MD_D - The offset of the D pin in the register
// - _MD_E - The offset of the E pin in the register
// - _MD_CLK - The offset of the CLK pin in the register
// - _MD_LAT - The offset of the LAT pin in the register
// - _MD_OE - The offset of the OE pin in the register
// Make sure that the PORT registers are mapped to the registers which control the pins of the MCU you are using,
// as opposed to the registers that directly connect to the GPIO pins.
// This is because the code uses = instead of |= and &= to set the pins, which would otherwise overwrite the other pins.

#ifndef MATRIXDRIVER_DEFINES_H
#define MATRIXDRIVER_DEFINES_H

#define MASK(x) (1 << (x))
#define _MD_BITDEPTH 5
#define _MD_GAMMA 2

#define BOARD_adafruit_matrix_portal_m4

#ifdef BOARD_adafruit_matrix_portal_m4
    #include "MatrixPortalM4.h"
#endif

#endif //MATRIXDRIVER_DEFINES_H

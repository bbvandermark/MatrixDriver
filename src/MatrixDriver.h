//Driver for LED matrices that use the HUB75 protocol.
//This driver is initially designed for the Adafruit MatrixPortal M4, but you adapt it to any board that meets the following requirements:
// - 2 sets of 3 digital outputs for RGB data (R1, G1, B1 and R2, G2, B2)
// - 5 digital outputs for the address lines (A, B, C, D, E)
// - 3 digital outputs for the control lines (CLK, LAT, OE)
// - all pins must be mapped to the same GPIO port
// - the board is used to drive a 64x32 LED matrix

#ifndef MATRIXDRIVER_MATRIXDRIVER_H
#define MATRIXDRIVER_MATRIXDRIVER_H

#include <cstdint>
#include "compatibility/Defines.h"

//notes on the internal representation of the matrix:
//the matrix is stored as _MD_BITDEPTH bits per color channel, each bit stored in a separate plane
//each plane has 32 rows, which each have 3 colors (RGB).
//each color is represented by a 64-bit integer (since we are only dealing with one bit due to the division into planes)
//we represent this 64-bit integer as 2 32-bit integers, because of performance issues when using 64-bit integers directly
//we need to allocate these dynamically, as allocating them statically will give the arrays addresses that belong to other memory regions
//this is why we use a quadruple pointer, which is a bit less readable, but allows us to use calloc to assign the memory
//I have left a comment in the class definition that shows the dimensions of the array, which is easier to read

class MatrixDriver {
    public:
        MatrixDriver();
        void display_matrix();
        void draw_bitmap(int* bmp);
        void set_pixel(int x, int y, int r, int g, int b);
    private:
        //uint32_t matrix[_MD_BITDEPTH][32][3][2];
        uint32_t**** matrix;
        uint32_t**** buffer;
        void clock_row(uint8_t address, uint32_t **row_one, uint32_t **row_two);
        static int convert_bitdepth(int value);
        void clear_buffer();
        void swap_buffer();
};


#endif //MATRIXDRIVER_MATRIXDRIVER_H

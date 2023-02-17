#include "MatrixDriver.h"
#include <cmath>

uint32_t _MD_RGB_MASK = MASK(_MD_R1) | MASK(_MD_G1) | MASK(_MD_B1) | MASK(_MD_R2) | MASK(_MD_G2) | MASK(_MD_B2);
uint32_t _MD_ADR_MASK = MASK(_MD_A) | MASK(_MD_B) | MASK(_MD_C) | MASK(_MD_D) | MASK(_MD_E);

MatrixDriver::MatrixDriver() {
    //set pin modes
    _MD_PORT_OUTPUT = _MD_RGB_MASK | _MD_ADR_MASK | MASK(_MD_CLK) | MASK(_MD_LAT) | MASK(_MD_OE);

    //initialize matrix & buffer
    matrix = (uint32_t****) calloc(_MD_BITDEPTH, sizeof(uint32_t***));
    buffer = (uint32_t****) calloc(_MD_BITDEPTH, sizeof(uint32_t***));
    for (int i = 0; i < _MD_BITDEPTH; i++) {
        matrix[i] = (uint32_t***) calloc(32, sizeof(uint32_t**));
        buffer[i] = (uint32_t***) calloc(32, sizeof(uint32_t**));
        for (int j = 0; j < 32; j++) {
            matrix[i][j] = (uint32_t**) calloc(3, sizeof(uint32_t*));
            buffer[i][j] = (uint32_t**) calloc(3, sizeof(uint32_t*));
            for (int k = 0; k < 3; k++) {
                matrix[i][j][k] = (uint32_t*) calloc(2, sizeof(uint32_t));
                buffer[i][j][k] = (uint32_t*) calloc(2, sizeof(uint32_t));
            }
        }
    }
}

//to prevent the matrix freezing while writing pixels, we will call display_matrix() periodically throughout the function
//this does cause a bit of a slowdown, but it shouldn't matter much as bitmaps are usually drawn once
//I would've preferred doing this in threads, but the appropriate thread libraries don't seem to work here.
//the length of bmp is assumed to be 64*32*3
void MatrixDriver::draw_bitmap(int* bmp) {
    clear_buffer();

    for(int i = 0; i < 32; i++){
        for(int j = 0; j < 64; j++){
            if(j % 4 == 0){ //this seems like a good compromise between minimizing flickering and response time
                display_matrix();
            }
            int index = (i * 64 + j) * 3;
            set_pixel(j, i, bmp[index], bmp[index + 1], bmp[index + 2]);
        }

    }

    swap_buffer();
}

//r, g, and b should be in the range 0-255
//writes a pixel to the buffer
void MatrixDriver::set_pixel(int x, int y, int r, int g, int b) {
    //reduce r, g and b to fit in BITDEPTH bits
    //we need to write 0 to plane 0, 1 to plane 1, 1 to plane 2, etc.
    //we have 2 32-bit integers, located at matrix[plane][row][color][0/1] with 0 being 0-31 and 1 being 32-63
    //if x > 31, we use index 1, otherwise 0. Then we take the value to be set in the plane and shift it left by x % 32
    r = MatrixDriver::convert_bitdepth(r);
    g = MatrixDriver::convert_bitdepth(g);
    b = MatrixDriver::convert_bitdepth(b);

    int section = 0;
    if(x > 31){
        section = 1;
        x -= 32;
    }

    for(int z = 0; z < _MD_BITDEPTH; z++){
        buffer[z][y][0][section] |= (r % 2) << x;
        buffer[z][y][1][section] |= (g % 2) << x;
        buffer[z][y][2][section] |= (b % 2) << x;
        r >>= 1;
        g >>= 1;
        b >>= 1;
    }
}

//address must be less than 16
//row_one will be displayed on the address row, row_two will be displayed at an offset of 16
//we use 2 32-bit integers instead of 1 64-bit integer because the cortex M4 is 32-bit, meaning 64-bit operations are slower
//this is marginally faster than using an array of pixel structs. There would be 3 load operations per pixel, giving 192 load operations per row, instead of the 6 operations done currently.
//however, because the cortex is able to pipeline sequential load operations, this optimization is only 5% faster than using structs.
void MatrixDriver::clock_row(uint8_t address, uint32_t **row_one, uint32_t **row_two){
    //get RGB values from memory
    uint32_t r1 = row_one[0][0];
    uint32_t g1 = row_one[1][0];
    uint32_t b1 = row_one[2][0];
    uint32_t r2 = row_two[0][0];
    uint32_t g2 = row_two[1][0];
    uint32_t b2 = row_two[2][0];

    //clock in the first 32 pixels
    for(int i = 0; i < 32; i++){
        //clear all RGB values that were stored previously
        _MD_PORT_CLEAR = _MD_RGB_MASK;
        //set RGB pins and clock pin
        _MD_PORT_SET =  ((r1 >> i) & 1) << _MD_R1 |
                        ((g1 >> i) & 1) << _MD_G1 |
                        ((b1 >> i) & 1) << _MD_B1 |
                        ((r2 >> i) & 1) << _MD_R2 |
                        ((g2 >> i) & 1) << _MD_G2 |
                        ((b2 >> i) & 1) << _MD_B2 |
                        MASK(_MD_CLK);

        //add a small delay so the digital logic on the matrix can properly propagate
        __asm__("nop");

        //pull clock low
        _MD_PORT_CLEAR = MASK(_MD_CLK);
    }
    //clock in the last 32 pixels
    //we could do this in the same loop as the first 32 pixels, but that adds non-insignificant overhead
    r1 = row_one[0][1];
    g1 = row_one[1][1];
    b1 = row_one[2][1];
    r2 = row_two[0][1];
    g2 = row_two[1][1];
    b2 = row_two[2][1];
    for(int i = 0; i < 32; i++){
        //clear all RGB values that were stored previously
        _MD_PORT_CLEAR = _MD_RGB_MASK;
        //set RGB pins and clock pin
        _MD_PORT_SET =  ((r1 >> i) & 1) << _MD_R1 |
                        ((g1 >> i) & 1) << _MD_G1 |
                        ((b1 >> i) & 1) << _MD_B1 |
                        ((r2 >> i) & 1) << _MD_R2 |
                        ((g2 >> i) & 1) << _MD_G2 |
                        ((b2 >> i) & 1) << _MD_B2 |
                        MASK(_MD_CLK);

        //add a small delay so the digital logic on the matrix can properly propagate
        __asm__("nop");

        //pull clock low
        _MD_PORT_CLEAR = MASK(_MD_CLK);
    }

    //latch the data, switch the row address and disable the display output while doing so
    _MD_PORT_CLEAR = _MD_ADR_MASK;

    _MD_PORT_SET =  MASK(_MD_LAT) | MASK(_MD_OE) |
                            (address & 1) << _MD_A |
                     ((address >> 1) & 1) << _MD_B |
                     ((address >> 2) & 1) << _MD_C |
                     ((address >> 3) & 1) << _MD_D |
                     ((address >> 4) & 1) << _MD_E;

    __asm__("nop");
    __asm__("nop");

    //enable the output and disable the latch
    _MD_PORT_CLEAR =  MASK(_MD_LAT) | MASK(_MD_OE);
}

void MatrixDriver::display_matrix() {
    int time = 1;
    for(int i = 0; i < _MD_BITDEPTH; i++) {
        for (int x = 0; x < (time << i); x++) {
            for (int j = 0; j < 16; j++) {
                clock_row(j, matrix[i][j], matrix[i][j + 16]);
            }
        }
    }
}

//value should have a maximum value of 255
int MatrixDriver::convert_bitdepth(int value){
    //apply gamma correction
    value = pow(value / 255.0, _MD_GAMMA) * 255;
    //return the value with the correct bitdepth
    return value * (pow(2, _MD_BITDEPTH) - 1) / 255;
}

//clears the buffer
void MatrixDriver::clear_buffer() {
    for(int i = 0; i < _MD_BITDEPTH; i++){
        for(int j = 0; j < 32; j++){
            for(int c = 0; c < 3; c++){
                buffer[i][j][c][0] = 0;
                buffer[i][j][c][1] = 0;
            }
        }
    }
}

//swaps the buffer
void MatrixDriver::swap_buffer() {
    uint32_t**** temp = buffer;
    buffer = matrix;
    matrix = temp;
}
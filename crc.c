#include "crc.h"

#define  poly     0x1021          /* crc-ccitt mask */

/* global variables */
unsigned short update_bad_crc(unsigned short ch, unsigned short bad_crc)
{
    /* based on code found at
    http://www.programmingparadise.com/utility/crc.html
    */

    unsigned short i, xor_flag;

    /*
    Why are they shifting this byte left by 8 bits??
    How do the low bits of the poly ever see it?
    */
    ch <<= 8;

    for(i = 8; i--;) {
        xor_flag = (bad_crc ^ ch) & 0x8000;
        bad_crc <<= 1;
        if (xor_flag)
            bad_crc ^= poly;

        ch <<= 1;
    }
    return bad_crc;
}

short crc_ccitt(char text[], int text_length)
{
    int i;
    unsigned short bad_crc = 0;

    for(i = 0; i < text_length; i++) {
        bad_crc = update_bad_crc(text[i], bad_crc);
    }
    return bad_crc;
}

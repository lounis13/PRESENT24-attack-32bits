
#include "dechiffrement.h"

#include <stdio.h>
#include <stdlib.h>


const uint8_t INV_S [16] = 
{
    0x5, 0xe, 0xf, 0x8,
    0xc, 0x1, 0x2, 0xd, 
    0xb, 0x4, 0x6, 0x3, 
    0x0, 0x7, 0x9, 0xa
};

const uint8_t INV_P [24] =
{
    0,  4 ,  8 ,  12,  16,  20,
    1,  5 ,  9 ,  13,  17,  21, 
    2,  6 ,  10,  14,  18,  22,
    3,  7 ,  11,  15,  19,  23 
};

uint32_t inv_permutation (uint32_t etat)
{
    uint32_t res = 0;
    for (int  i  = 0; i < 24; i++)
    {
        res |= (etat & 1)<< INV_P[i];
        etat >>=1;
    }
    return res;
}


uint32_t inv_substitution (uint32_t etat) 
{
    uint8_t w0   = INV_S[etat  & 0xf];
    uint8_t w1   = INV_S[(etat & 0xf0)>>4];
    uint8_t w2   = INV_S[(etat & 0xf00)>>8];
    uint8_t w3   = INV_S[(etat & 0xf000)>>12];           
    uint8_t w4   = INV_S[(etat & 0xf0000)>>16];
    uint8_t w5   = INV_S[(etat & 0xf00000)>>20];    

    return (w0) | w1<<4 | w2<<8 | w3<<12 | w4<<16 | w5<<20;  
}

uint32_t dechiffrement (uint32_t message, uint32_t *k)
{
    uint32_t  etat = message;

    for (uint8_t i = 0; i < 10; i++)
    {
        etat ^= k[10-i];
        etat  = inv_permutation(etat);
        etat  = inv_substitution(etat);
    }
    
    etat ^= k[0];

    return etat;
}

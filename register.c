
#include "register.h"

#include <stdio.h>
#include <stdlib.h>

uint8_t S[16]   = {0xc, 0x5, 0x6, 0xb, 0x9, 0x0, 0xa, 0xd, 0x3, 0xe, 0xf, 0x8, 0x4, 0x7, 0x1, 0x2};

// Tableau de permutation //
const uint8_t P[24] = 
{
    0,6,12,18,1,7,
    13,19,2,8,14,20,
    3,9,15,21,4,10,
    16,22,5,11,17,23
};



cle_80 cle_80_init (uint32_t cle_maitre)
{
    cle_80 cle;
    cle.left = cle_maitre << 8;
    cle.mid = cle.right = 0;
    return cle;
}


cle_80 cle_80_rotate_61 (cle_80 cle)
{
    uint32_t mid   = cle.left << 13 | cle.mid >> 19;
    cle.left       = cle.left >> 19 | (cle.mid << 29) | (cle.right<<13);
    cle.right = cle.mid >> 3 & 0xffff;
    cle.mid   = mid;
    
    return cle;   
}

cle_24 cle_80_get_cle_24(cle_80 cle)
{
    return (cle_24) cle.mid & 0xffffff;
}

uint8_t cle_80_get_most4_left (cle_80 cle)
{   
    return cle.left >> 28;
}

cle_80 cle_80_remplace_with_s (cle_80 cle)
{
    uint8_t x = cle_80_get_most4_left(cle);
    uint32_t left = S[x] << 28;
    left |= (cle.left<<4) >> 4;
    cle.left = left;
    return cle;
}

cle_80  cle_80_xor_most_right (cle_80 cle, uint8_t i)
{
    cle.right ^= i << 0xf;
    cle.mid   ^= i >> 0x1;

    return cle;
}


void cle_80_print (cle_80 cle)
{
    printf(" %.8X%.8X%.4X\n", cle.left, cle.mid, cle.right);
}

uint32_t substitution (uint32_t etat) 
{
    uint8_t w0   = S[etat  & 0xf];
    uint8_t w1   = S[(etat & 0xf0)>>4];
    uint8_t w2   = S[(etat & 0xf00)>>8];
    uint8_t w3   = S[(etat & 0xf000)>>12];           
    uint8_t w4   = S[(etat & 0xf0000)>>16];
    uint8_t w5   = S[(etat & 0xf00000)>>20];    

    return (w0) | w1<<4 | w2<<8 | w3<<12 | w4<<16 | w5<<20;  
}

uint32_t permutation (uint32_t etat)
{
    uint32_t res = 0;
    for (int  i  = 0; i < 24; i++)
    {
        res |= (etat & 1)<< P[i];
        etat >>=1;
    }
    return res;
} 

uint32_t * all_keys (uint32_t cle_maitre)
{
    uint32_t * ks = malloc(sizeof(uint32_t) * 11);

    cle_80 cles = cle_80_init(cle_maitre);
    for (short i = 0; i < 11; i++)
    {
        ks[i] = cle_80_get_cle_24(cles);
        cles = cle_80_rotate_61(cles);
        cles = cle_80_remplace_with_s(cles);
        cles = cle_80_xor_most_right(cles, i+1);
    }
    return ks;
    
}

uint32_t chiffrement (uint32_t message, uint32_t *k)
{
    uint32_t  etat = message;

    for (uint8_t i = 0; i < 10; i++)
    {
        etat ^= k[i];
        etat  = substitution(etat);
        etat  = permutation(etat);
    }
    
    etat ^= k[10];

    return etat;
}
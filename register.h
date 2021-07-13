#ifndef __REGISTER_H_
#define __REGISTER_H_


#include <stdint.h>

#define cle_24 uint32_t


/**
 * 80 bits
 **/
typedef struct cle_80_str
{
    uint32_t left;
    uint32_t mid;
    uint16_t right;    
} cle_80;

/**
 * 
 **/
cle_80 
    cle_80_init (uint32_t cle_maitre);

/**
 * @brief
 **/
cle_24
    cle_80_get_cle_24(cle_80 cle);

cle_80 
    cle_80_rotate_61 (cle_80 cle);

uint8_t 
    cle_80_get_most4_left (cle_80 cle);

cle_80 
    cle_80_remplace_with_s (cle_80 cle);

cle_80 
    cle_80_xor_most_right (cle_80 cle, uint8_t i);

void 
    cle_80_print (cle_80 cle);

uint32_t 
    substitution (uint32_t etat); 

uint32_t 
    permutation (uint32_t etat);


uint32_t *
    all_keys (uint32_t cle_maitre);

uint32_t 
    chiffrement (uint32_t message, uint32_t *k);
#endif
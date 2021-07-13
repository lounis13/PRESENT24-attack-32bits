#ifndef _DECHIFF_H_
#define _DECHIFF_H_

#include <stdint.h>

uint32_t 
    inv_permutation (uint32_t etat);

uint32_t
     inv_substitution (uint32_t etat) ;

uint32_t 
    dechiffrement (uint32_t message, uint32_t *k);

    
#endif
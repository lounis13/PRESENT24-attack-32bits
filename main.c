
#include "register.h"
#include "dechiffrement.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>



const uint32_t SIZE         = 1<<24;
const uint32_t NB_THREADS   = 8;
const uint32_t SIZE_PER_TID = SIZE/NB_THREADS;

uint32_t ** list_of_keys  = NULL;


uint32_t message1 = 0x0;
uint32_t chiffre1 = 0x0;

uint32_t message2 = 0x0;
uint32_t chiffre2 = 0x0;


typedef struct keys
{
    uint32_t k;
    struct keys * next;
}*key_list;

/**
 * hash - une hash map qui stock toutes les clés condidates
 * PS : variable gloabal (acces depuis les thread routine).
 **/
key_list * hash;
/**
 * la liste des sous clés.
 **/
uint32_t ** master_key_list;


/**
 * @brief
 * 
 * @param
 **/
void fast_get_secret_key
    (uint32_t m1, uint32_t c1, uint32_t m2, uint32_t c2);


void  get_secret_key 
    (uint32_t m1, uint32_t c1, uint32_t m2, uint32_t c2);


// (m1,c1) = (220b5f, 0c2e1c) (m2, c2) = (9d733b, c4b297)
// (m1,c1) = (3e8e7e, 6a705f) (m2,c2) = (835541, 3ad4d3)
//(m1,c1)  = (220b5f, 0c2e1c) (m2,c2) = (9d733b, c4b297)
// (m1,c1) = (3e8e7e, 6a705f) (m2,c2) = (835541, 3ad4d3)


int main(int argc, char const *argv[])
{   

    fast_get_secret_key (0x2e03aa, 0x776603, 0xd3b2f8, 0xe18428);
    return 0;
}


key_list key_list_init ()
{
    return NULL;
}

key_list key_list_add (key_list l, uint32_t value)
{
    key_list new = malloc(sizeof(struct keys));
    new->next = l;
    new->k = value;

    return new;
}

void * thread_get_condidate (void * args)
{
    uint8_t thread_id = *(uint8_t*) args;
    uint32_t i = ((uint32_t) (thread_id)) * SIZE_PER_TID;
    uint32_t n = (uint32_t) i + SIZE_PER_TID;

    for (uint32_t k1 = i ; k1 < n; k1++)
    {
        uint32_t * ktmp = all_keys(k1);
        master_key_list[k1]   = ktmp;
        uint32_t res    = chiffrement(message1, ktmp);
        hash[res]       = key_list_add(hash[res], k1);
    }

    pthread_exit(NULL);
}

void * thread_get_same_keys (void * args)
{
    uint8_t thread_id = *(uint8_t*) args;
    uint32_t i = ((uint32_t) (thread_id)) * SIZE_PER_TID;
    uint32_t n  = (uint32_t) i + SIZE_PER_TID;
    
    for (uint32_t k2 = i ; k2 < n; k2++)
    {
        uint32_t res = dechiffrement(chiffre1, master_key_list[k2]);
        key_list ltmp = hash[res]; 
        while (ltmp)
        {
            if (chiffrement(chiffrement(message2, master_key_list[ltmp->k]), master_key_list[k2]) == chiffre2)
                printf("K1 : %.6X K2 : %.6X \n", ltmp->k, k2);
            
            ltmp = ltmp->next;

        }
    }
    pthread_exit(NULL);

}

void fast_get_secret_key(uint32_t m1, uint32_t c1, uint32_t m2, uint32_t c2)
{
    message1 = m1;  chiffre1 = c1;
    message2 = m2;  chiffre2 = c2;

    pthread_t tids [NB_THREADS];
    /* Utiliser juste piur passé la valeur de i 
     * dans la gestionnaire de threads
     */
    uint8_t thread_id [NB_THREADS];
    
    master_key_list = malloc(sizeof(uint32_t*)*(SIZE));
    hash   = malloc(sizeof(key_list) * SIZE);

    for (uint8_t k1 = 0; k1 < NB_THREADS; k1++)
    {
        thread_id[k1] = k1;
        pthread_create(tids + k1, NULL, thread_get_condidate, thread_id + k1);
    }


    for (uint8_t i = 0; i < NB_THREADS; i++)
        pthread_join (tids[i], NULL);


    for (uint8_t k2 = 0; k2 < NB_THREADS; k2++)
        pthread_create(tids + k2, NULL, thread_get_same_keys, thread_id + k2);
    
    
    for (uint8_t i = 0; i < NB_THREADS; i++)
        pthread_join (tids[i], NULL);

}

void  get_secret_key (uint32_t m1, uint32_t c1, uint32_t m2, uint32_t c2)
{
    
    master_key_list = malloc(sizeof(uint32_t*)*(SIZE));
    hash   = malloc(sizeof(key_list) * SIZE);

    for (uint32_t k1 = 0; k1 < SIZE; k1++)
    {
        uint32_t * ktmp = all_keys(k1);
        master_key_list[k1] = ktmp;
        uint32_t res = chiffrement(m1, ktmp);
        hash[res] = key_list_add(hash[res], k1);
    }

    for (uint32_t k2 = 0; k2 < SIZE; k2++)
    {
        uint32_t res = dechiffrement(c1, master_key_list[k2]);
        key_list ltmp = hash[res]; 
        while (ltmp)
        {
            if (chiffrement(chiffrement(m2, master_key_list[ltmp->k]), master_key_list[k2]) == c2)
                printf("K1 : %.6X K2 : %.6X \n", ltmp->k, k2);
            
            ltmp = ltmp->next;
        }
        
    }
}

#ifndef AES_MODES_H
#define AES_MODES_H

//predproceroske direktive da se osigura poklapanje i sa C i sa C++
#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include "aes.h"
#include <string.h>

#define AES_DEFAULT_THREADS 4

typedef struct {
    const uint8_t* input;
    uint8_t* output;
    uint8_t key[AES_KEY_SIZE];
    int start_block;    //broj pocetnog bloka koji thread dobija
    int num_blocks;     //broj blokova koji thread dobija
}thread_128_arg_t;


typedef struct {
    const uint8_t* input;
    uint8_t* output;
    uint8_t key[AES_256_KEY_SIZE];
    int start_block;    //broj pocetnog bloka koji thread dobija
    int num_blocks;     //broj blokova koji thread dobija
}thread_256_arg_t;

//funkcije za ECB - ELECTRONIC CODEBOOK, sekvencijalni, sa nitima, opcije za kljuc od 128 i 256 bita
void aes_128_ecb_encrypt_seq(const uint8_t *input, uint8_t *output,
                              const uint8_t key[AES_KEY_SIZE], int num_blocks);

void aes_128_ecb_decrypt_seq(const uint8_t *input, uint8_t *output,
                              const uint8_t key[AES_KEY_SIZE], int num_blocks);

void aes_128_ecb_encrypt_thrd(const uint8_t *input, uint8_t *output,
                              const uint8_t key[AES_KEY_SIZE],
                              int num_blocks, int num_threads);

void aes_128_ecb_decrypt_thrd(const uint8_t *input, uint8_t *output,
                              const uint8_t key[AES_KEY_SIZE], 
                              int num_blocks, int num_threads);


void aes_256_ecb_encrypt_seq(const uint8_t *input, uint8_t *output,
                              const uint8_t key[AES_256_KEY_SIZE], int num_blocks);

void aes_256_ecb_decrypt_seq(const uint8_t *input, uint8_t *output,
                              const uint8_t key[AES_256_KEY_SIZE], int num_blocks);

void aes_256_ecb_encrypt_thrd(const uint8_t *input, uint8_t *output,
                              const uint8_t key[AES_256_KEY_SIZE], 
                              int num_blocks, int num_threads);

void aes_256_ecb_decrypt_thrd(const uint8_t *input, uint8_t *output,
                              const uint8_t key[AES_256_KEY_SIZE], 
                              int num_blocks, int num_threads);


//funkcije za CBC - Cipher block chaining
//enkripcija nije pogodna za paralelizaciju, pa se ni dekripcija ne paralelizuje 
void aes_128_cbc_encrypt(const uint8_t *input, uint8_t *output,
                          const uint8_t key[AES_KEY_SIZE], 
                          const uint8_t iv[AES_BLOCK_SIZE],
                          int num_blocks);

void aes_128_cbc_decrypt(const uint8_t *input, uint8_t *output,
                          const uint8_t key[AES_KEY_SIZE], 
                          const uint8_t iv[AES_BLOCK_SIZE],
                          int num_blocks);

void aes_256_cbc_encrypt(const uint8_t *input, uint8_t *output,
                          const uint8_t key[AES_256_KEY_SIZE], 
                          const uint8_t iv[AES_BLOCK_SIZE],
                          int num_blocks);

void aes_256_cbc_decrypt(const uint8_t *input, uint8_t *output,
                          const uint8_t key[AES_256_KEY_SIZE], 
                          const uint8_t iv[AES_BLOCK_SIZE],
                          int num_blocks);


#ifdef __cplusplus
}
#endif  //__cplusplus

#endif //AES_MODES_H
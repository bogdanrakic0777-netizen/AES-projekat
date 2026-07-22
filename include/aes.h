#ifndef AES_H
#define AES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "gf.h"

#define AES_BLOCK_SIZE 16

#define AES_KEY_SIZE 16     //default je za aes 128
#define AES_256_KEY_SIZE 32
//dve konstante kako bi moglao da pokrene i aes 128 i aes 256

#define AES_ROUNDS 10       //default je za aes 128
#define AES_256_ROUNDS 14
//dve konstante kako bi moglao da pokrene i aes 128 i aes 256

extern uint8_t SBOX[256];
extern uint8_t INV_SBOX[256];

uint8_t affine_transform(uint8_t a);

// generisanje lookup tabela za sbox i inv_sbox
void generate_sbox(void);

//funkcije koje rade preko lookup tabela za sbox i inv_sbox
uint8_t sbox(uint8_t a);
uint8_t inv_sbox(uint8_t a);

//funkcije koje za matricu 4x4 svaki element zamene njegovom vrednosti preko funkcija sbox i inv_sbox
void subBytes(uint8_t state[4][4]);
void inv_subBytes(uint8_t state[4][4]);

//funkcije za shiftovanje redova matrice 4x4
void shiftRows(uint8_t state[4][4]);
void invShiftRows(uint8_t state[4][4]);

//funkije koje mesaju kolone matrice
void mixColumns(uint8_t state[4][4]);
void mixcolumns_asm(uint8_t state[4][4]);
void invMixColumns(uint8_t state[4][4]);


//srz AES-a - RIJNDAL ALGORITAM key scheduling
void key_expansion_128(const uint8_t key[AES_KEY_SIZE], uint8_t roundKey[11][4][4]);
void key_expansion_256(const uint8_t key[AES_256_KEY_SIZE], uint8_t roundKey[14][4][4]);

void addRoundKey(uint8_t state[4][4], uint8_t roundKey[4][4]);

void loadState(const uint8_t input[16], uint8_t state[4][4]);
void storeState(const uint8_t state[4][4], uint8_t output[16]);

void aes_128_encrypt(const uint8_t plaintext[16], const uint8_t key[AES_KEY_SIZE], uint8_t ciphertext[16]);
void aes_128_decrypt(uint8_t plaintext[16], const uint8_t key[AES_KEY_SIZE], const uint8_t ciphertext[16]);


void aes_256_encrypt(const uint8_t plaintext[16], const uint8_t key[AES_256_KEY_SIZE], uint8_t ciphertext[16]);
void aes_256_decrypt(uint8_t plaintext[16], const uint8_t key[AES_256_KEY_SIZE], const uint8_t ciphertext[16]);

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  //AES_H
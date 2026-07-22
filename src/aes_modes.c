#include "aes_modes.h"
#include <string.h>

void aes_128_ecb_encrypt_seq(const uint8_t* input, uint8_t* output, const uint8_t key[AES_KEY_SIZE], int num_blocks) {
    for(int i = 0; i < num_blocks; i++) {
        aes_128_encrypt(input + i*AES_BLOCK_SIZE, key, output + i*AES_BLOCK_SIZE);
    }
}

void aes_128_ecb_decrypt_seq(const uint8_t* input, uint8_t* output, const uint8_t key[AES_KEY_SIZE], int num_blocks) {
    for(int i = 0; i < num_blocks; i++) {
        aes_128_decrypt(output + i*AES_BLOCK_SIZE, key , (uint8_t*)(input + i*AES_BLOCK_SIZE));
    }
}

void aes_256_ecb_encrypt_seq(const uint8_t* input, uint8_t* output, const uint8_t key[AES_256_KEY_SIZE], int num_blocks) {
    for(int i = 0; i < num_blocks; i++) {
        aes_256_encrypt(input + i*AES_BLOCK_SIZE, key, output + i*AES_BLOCK_SIZE);
    }
}

void aes_256_ecb_decrypt_seq(const uint8_t* input, uint8_t* output, const uint8_t key[AES_256_KEY_SIZE], int num_blocks) {
    for(int i = 0; i < num_blocks; i++) {
        aes_256_decrypt(output + i*AES_BLOCK_SIZE, key, (uint8_t*)(input + i*AES_BLOCK_SIZE));
    }
}

void aes_128_cbc_encrypt(const uint8_t* input, uint8_t* output, const uint8_t key[AES_KEY_SIZE], const uint8_t iv[AES_BLOCK_SIZE], int num_blocks) {
    
    uint8_t prev[AES_BLOCK_SIZE];
    memcpy(prev, iv, AES_BLOCK_SIZE);

    uint8_t temp[AES_BLOCK_SIZE];
    //prodje kroz ceo plaintext, blok po blok
    for(int i = 0; i < num_blocks; i++) {

        //svaki blok plaintexta se prvo XOR-uje sa prethodnim enkriptovanim blokom
        for(int j = 0; j < AES_BLOCK_SIZE; j++) {
            temp[j] = input[i*AES_BLOCK_SIZE + j] ^ prev[j];
        }

        aes_128_encrypt(temp, key, output + i*AES_BLOCK_SIZE);

        //sledeci prev je enkriptovan trenutni 
        memcpy(prev, output + i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
    }

}

void aes_128_cbc_decrypt(const uint8_t* input, uint8_t* output, const uint8_t key[AES_KEY_SIZE], const uint8_t iv[AES_BLOCK_SIZE], int num_blocks) {

    uint8_t prev[AES_BLOCK_SIZE];
    memcpy(prev, iv, AES_BLOCK_SIZE);

    uint8_t temp[AES_BLOCK_SIZE];

    for(int i = 0; i < num_blocks; i++) {
        aes_128_decrypt(temp, key, (uint8_t*)(input + i*AES_BLOCK_SIZE));

        for(int j = 0; j < AES_BLOCK_SIZE; j++) {
            output[i*AES_BLOCK_SIZE + j] = temp[j] ^ prev[j];
        }

        memcpy(prev, input + i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
    }
}

void aes_256_cbc_encrypt(const uint8_t* input, uint8_t* output, const uint8_t key[AES_256_KEY_SIZE], const uint8_t iv[AES_BLOCK_SIZE], int num_blocks) {
    uint8_t prev[AES_BLOCK_SIZE];
    memcpy(prev, iv, AES_BLOCK_SIZE);

    uint8_t temp[AES_BLOCK_SIZE];

    for(int i = 0; i < num_blocks; i++) {
        //svaki blok plaintexta se prvo XOR-uje sa prethodnim enkriptovanim blokom
        for(int j = 0; j < AES_BLOCK_SIZE; j++) {
            temp[j] = input[i*AES_BLOCK_SIZE + j] ^ prev[j];
        }

        aes_256_encrypt(temp, key, output + i*AES_BLOCK_SIZE);

        //sledeci prev je enkriptovan trenutni 
        memcpy(prev, output + i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
    }
}

void aes_256_cbc_decrypt(const uint8_t* input, uint8_t* output, const uint8_t key[AES_256_KEY_SIZE], const uint8_t iv[AES_BLOCK_SIZE], int num_blocks) {

    uint8_t prev[AES_BLOCK_SIZE];
    memcpy(prev, iv, AES_BLOCK_SIZE);

    uint8_t temp[AES_BLOCK_SIZE];

    for(int i = 0; i < num_blocks; i++) {
        aes_256_decrypt(temp, key, (uint8_t*)(input + i*AES_BLOCK_SIZE));

        for(int j = 0; j < AES_BLOCK_SIZE; j++) {
            output[i*AES_BLOCK_SIZE + j] = temp[j] ^ prev[j];
        }

        memcpy(prev, input + i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
    }
}
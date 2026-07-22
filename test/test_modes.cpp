#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "aes_modes.h"

static int passed = 0;
static int failed = 0;

static const uint8_t key128[AES_KEY_SIZE] = {
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};

static const uint8_t key256[AES_256_KEY_SIZE] = {
    0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE,
    0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81,
    0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7,
    0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4
};

static const uint8_t iv[AES_BLOCK_SIZE] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

#define NUM_BLOCKS 100
#define DATA_SIZE (NUM_BLOCKS * AES_BLOCK_SIZE)

static void check(const char *name, const uint8_t *original, const uint8_t *result, int size) {
    if (memcmp(original, result, size) == 0) {
        printf("[PASS] %s\n", name);
        passed++;
    } else {
        printf("[FAIL] %s\n", name);
        failed++;
    }
}

//==========ECB 128==========
static void test_ecb_128_seq(void) {
    uint8_t plaintext[DATA_SIZE];
    uint8_t ciphertext[DATA_SIZE];
    uint8_t decrypted[DATA_SIZE];

    for (int i = 0; i < DATA_SIZE; i++)
        plaintext[i] = (uint8_t)(i % 256);

    aes_128_ecb_encrypt_seq(plaintext, ciphertext, key128, NUM_BLOCKS);
    aes_128_ecb_decrypt_seq(ciphertext, decrypted, key128, NUM_BLOCKS);

    check("ECB-128 seq roundtrip", plaintext, decrypted, DATA_SIZE);
}

static void test_ecb_128_thrd(void) {
    uint8_t plaintext[DATA_SIZE];
    uint8_t ciphertext[DATA_SIZE];
    uint8_t decrypted[DATA_SIZE];

    for (int i = 0; i < DATA_SIZE; i++)
        plaintext[i] = (uint8_t)(i % 256);

    aes_128_ecb_encrypt_thrd(plaintext, ciphertext, key128, NUM_BLOCKS, AES_DEFAULT_THREADS);
    aes_128_ecb_decrypt_thrd(ciphertext, decrypted, key128, NUM_BLOCKS, AES_DEFAULT_THREADS);

    check("ECB-128 thrd roundtrip", plaintext, decrypted, DATA_SIZE);
}

static void test_ecb_128_seq_vs_thrd(void) {
    uint8_t plaintext[DATA_SIZE];
    uint8_t cipher_seq[DATA_SIZE];
    uint8_t cipher_thrd[DATA_SIZE];

    for (int i = 0; i < DATA_SIZE; i++)
        plaintext[i] = (uint8_t)(i % 256);

    aes_128_ecb_encrypt_seq(plaintext, cipher_seq, key128, NUM_BLOCKS);
    aes_128_ecb_encrypt_thrd(plaintext, cipher_thrd, key128, NUM_BLOCKS, AES_DEFAULT_THREADS);

    check("ECB-128 seq == thrd", cipher_seq, cipher_thrd, DATA_SIZE);
}

//==========ECB 256==========
static void test_ecb_256_seq(void) {
    uint8_t plaintext[DATA_SIZE];
    uint8_t ciphertext[DATA_SIZE];
    uint8_t decrypted[DATA_SIZE];

    for (int i = 0; i < DATA_SIZE; i++)
        plaintext[i] = (uint8_t)(i % 256);

    aes_256_ecb_encrypt_seq(plaintext, ciphertext, key256, NUM_BLOCKS);
    aes_256_ecb_decrypt_seq(ciphertext, decrypted, key256, NUM_BLOCKS);

    check("ECB-256 seq roundtrip", plaintext, decrypted, DATA_SIZE);
}

static void test_ecb_256_thrd(void) {
    uint8_t plaintext[DATA_SIZE];
    uint8_t ciphertext[DATA_SIZE];
    uint8_t decrypted[DATA_SIZE];

    for (int i = 0; i < DATA_SIZE; i++)
        plaintext[i] = (uint8_t)(i % 256);

    aes_256_ecb_encrypt_thrd(plaintext, ciphertext, key256, NUM_BLOCKS, AES_DEFAULT_THREADS);
    aes_256_ecb_decrypt_thrd(ciphertext, decrypted, key256, NUM_BLOCKS, AES_DEFAULT_THREADS);

    check("ECB-256 thrd roundtrip", plaintext, decrypted, DATA_SIZE);
}

static void test_ecb_256_seq_vs_thrd(void) {
    uint8_t plaintext[DATA_SIZE];
    uint8_t cipher_seq[DATA_SIZE];
    uint8_t cipher_thrd[DATA_SIZE];

    for (int i = 0; i < DATA_SIZE; i++)
        plaintext[i] = (uint8_t)(i % 256);

    aes_256_ecb_encrypt_seq(plaintext, cipher_seq, key256, NUM_BLOCKS);
    aes_256_ecb_encrypt_thrd(plaintext, cipher_thrd, key256, NUM_BLOCKS, AES_DEFAULT_THREADS);

    check("ECB-256 seq == thrd", cipher_seq, cipher_thrd, DATA_SIZE);
}

//==========CBC 128==========
static void test_cbc_128(void) {
    uint8_t plaintext[DATA_SIZE];
    uint8_t ciphertext[DATA_SIZE];
    uint8_t decrypted[DATA_SIZE];

    for (int i = 0; i < DATA_SIZE; i++)
        plaintext[i] = (uint8_t)(i % 256);

    aes_128_cbc_encrypt(plaintext, ciphertext, key128, iv, NUM_BLOCKS);
    aes_128_cbc_decrypt(ciphertext, decrypted, key128, iv, NUM_BLOCKS);

    check("CBC-128 roundtrip", plaintext, decrypted, DATA_SIZE);
}

//==========CBC 256==========
static void test_cbc_256(void) {
    uint8_t plaintext[DATA_SIZE];
    uint8_t ciphertext[DATA_SIZE];
    uint8_t decrypted[DATA_SIZE];

    for (int i = 0; i < DATA_SIZE; i++)
        plaintext[i] = (uint8_t)(i % 256);

    aes_256_cbc_encrypt(plaintext, ciphertext, key256, iv, NUM_BLOCKS);
    aes_256_cbc_decrypt(ciphertext, decrypted, key256, iv, NUM_BLOCKS);

    check("CBC-256 roundtrip", plaintext, decrypted, DATA_SIZE);
}

//==========CBC nije isto što i ECB==========
static void test_cbc_differs_from_ecb(void) {
    uint8_t plaintext[DATA_SIZE];
    uint8_t cbc_out[DATA_SIZE];
    uint8_t ecb_out[DATA_SIZE];

    // isti blok ponovljen — ECB daje iste blokove, CBC različite
    for (int i = 0; i < DATA_SIZE; i++)
        plaintext[i] = 0xAA;

    aes_128_cbc_encrypt(plaintext, cbc_out, key128, iv, NUM_BLOCKS);
    aes_128_ecb_encrypt_seq(plaintext, ecb_out, key128, NUM_BLOCKS);

    // CBC i ECB moraju biti različiti
    if (memcmp(cbc_out, ecb_out, DATA_SIZE) != 0) {
        printf("[PASS] CBC se razlikuje od ECB\n");
        passed++;
    } else {
        printf("[FAIL] CBC je isti kao ECB\n");
        failed++;
    }
}

int main(void) {
    printf("===AES modes testovi===\n\n");

    printf("---ECB 128---\n");
    test_ecb_128_seq();
    test_ecb_128_thrd();
    test_ecb_128_seq_vs_thrd();

    printf("\n---ECB 256---\n");
    test_ecb_256_seq();
    test_ecb_256_thrd();
    test_ecb_256_seq_vs_thrd();

    printf("\n---CBC 128---\n");
    test_cbc_128();

    printf("\n---CBC 256---\n");
    test_cbc_256();

    printf("\n---CBC vs ECB---\n");
    test_cbc_differs_from_ecb();

    printf("\n===Rezultat: %d passed, %d failed===\n", passed, failed);
    return failed > 0 ? 1 : 0;
}
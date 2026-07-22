#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "gf.h"
#include "aes.h"

static int passed = 0;
static int failed = 0;

// ==========AES-128 (FIPS 197)==========
static void test_aes_128(void) {
    printf("\n===AES-128 (FIPS 197)===\n");

    uint8_t plaintext[16] = {
        0x32, 0x43, 0xF6, 0xA8, 0x88, 0x5A, 0x30, 0x8D,
        0x31, 0x31, 0x98, 0xA2, 0xE0, 0x37, 0x07, 0x34
    };
    uint8_t key[16] = {
        0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
        0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
    };
    uint8_t expected[16] = {
        0x39, 0x25, 0x84, 0x1D, 0x02, 0xDC, 0x09, 0xFB,
        0xDC, 0x11, 0x85, 0x97, 0x19, 0x6A, 0x0B, 0x32
    };

    uint8_t ciphertext[16];
    uint8_t decrypted[16];

    aes_128_encrypt(plaintext, key, ciphertext);
    aes_128_decrypt(decrypted, key, ciphertext);

    printf("Ciphertext: ");
    for(int i = 0; i < 16; i++) printf("%02X ", ciphertext[i]);
    printf("\n");

    if(memcmp(ciphertext, expected, 16) == 0) {
        printf("[PASS] AES-128 enkripcija (FIPS 197)\n");
        passed++;
    } else {
        printf("[FAIL] AES-128 enkripcija\n");
        failed++;
    }

    if(memcmp(decrypted, plaintext, 16) == 0) {
        printf("[PASS] AES-128 dekripcija\n");
        passed++;
    } else {
        printf("[FAIL] AES-128 dekripcija\n");
        failed++;
    }
}

// ==========AES-256 (FIPS 197)==========
static void test_aes_256(void) {
    printf("\n===AES-256 (FIPS 197)===\n");

    uint8_t plaintext[16] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };
    uint8_t key[32] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };
    uint8_t expected[16] = {
        0x8E, 0xA2, 0xB7, 0xCA, 0x51, 0x67, 0x45, 0xBF,
        0xEA, 0xFC, 0x49, 0x90, 0x4B, 0x49, 0x60, 0x89
    };

    uint8_t ciphertext[16];
    uint8_t decrypted[16];

    aes_256_encrypt(plaintext, key, ciphertext);
    aes_256_decrypt(decrypted, key, ciphertext);

    printf("Ciphertext: ");
    for(int i = 0; i < 16; i++) printf("%02X ", ciphertext[i]);
    printf("\n");

    if(memcmp(ciphertext, expected, 16) == 0) {
        printf("[PASS] AES-256 enkripcija (FIPS 197)\n");
        passed++;
    } else {
        printf("[FAIL] AES-256 enkripcija\n");
        failed++;
    }

    if(memcmp(decrypted, plaintext, 16) == 0) {
        printf("[PASS] AES-256 dekripcija\n");
        passed++;
    } else {
        printf("[FAIL] AES-256 dekripcija\n");
        failed++;
    }
}

int main(void) {
    printf("===AES osnovni testovi===\n");

    test_aes_128();
    test_aes_256();

    printf("\n===Rezultat: %d passed, %d failed===\n", passed, failed);
    
    generate_sbox();
    printf("SBOX[0x00] = %02X (treba 0x63)\n", SBOX[0x00]);
    printf("SBOX[0x53] = %02X (treba 0xED)\n", SBOX[0x53]);
    

    return failed > 0 ? 1 : 0;
}
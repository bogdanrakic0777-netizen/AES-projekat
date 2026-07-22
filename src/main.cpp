#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <random>
#include <iomanip>
#include <array>
#include <cstring>
extern "C" {
    #include "aes.h"
    #include "aes_modes.h"
}
using namespace std;

//priprema plaintext-a: kopira poruku u padded bafer (nule za padding)
static int prepare_plaintext(const string &msg, uint8_t** out) {
    int num_blocks = (msg.size() + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE;
    if (num_blocks == 0) num_blocks = 1;
    int padded_size = num_blocks * AES_BLOCK_SIZE;

    *out = new uint8_t[padded_size]();  // () inicijalizuje na nulu
    memcpy(*out, msg.data(), msg.size());

    return num_blocks;
}

static void generate_random(uint8_t* buf, int size) {
    random_device rd;
    for(int i = 0; i < size; ++i) {
        buf[i] = (uint8_t)(rd() & 0xFF);
    }
}

static void print_hex(const string &label, const uint8_t* data, int size) {
    cout << label << ":\t";
    for(int i = 0; i < size; ++i) {
        cout << hex << setw(2) << setfill('0') << (int) data[i] << "\t"; 
    }
    cout << dec << endl;
}

int main() {

    srand(time(NULL));

    string ulaz;
    uint8_t* plaintext;

    cout << "Unesite poruku za enkriptovanje: ";
    getline(cin, ulaz);
    cout << endl;

    int num_blocks = prepare_plaintext(ulaz, &plaintext);
    int size = num_blocks * AES_BLOCK_SIZE;

    uint8_t* ciphertext_ecb_128 = new uint8_t[size];
    uint8_t* ciphertext_ecb_256 = new uint8_t[size];
    uint8_t* ciphertext_cbc_128 = new uint8_t[size];
    uint8_t* ciphertext_cbc_256 = new uint8_t[size];

    uint8_t* decrypted = new uint8_t[size];

    random_device rd;
    uint8_t key128[AES_KEY_SIZE];
    uint8_t key256[AES_256_KEY_SIZE];
    uint8_t iv[AES_BLOCK_SIZE];
    generate_random(key128, AES_KEY_SIZE);
    generate_random(key256, AES_256_KEY_SIZE);
    generate_random(iv, AES_BLOCK_SIZE);

    print_hex("Plaintext", plaintext, size);

    cout << "*****AES 128 ECB SEQ*****" <<endl;
    print_hex("Kljuc    ", key128, AES_KEY_SIZE);
    aes_128_ecb_encrypt_seq(plaintext, ciphertext_ecb_128, key128, num_blocks);
    print_hex("Ciphertext", ciphertext_ecb_128, size);
    aes_128_ecb_decrypt_seq(ciphertext_ecb_128, decrypted, key128, num_blocks);
    print_hex("Decrypted", decrypted, size);
    cout << endl;

    cout << "*****AES 256 ECB SEQ*****" <<endl;
    print_hex("Kljuc    ", key256, AES_256_KEY_SIZE);
    aes_256_ecb_encrypt_seq(plaintext, ciphertext_ecb_256, key256, num_blocks);
    print_hex("Ciphertext", ciphertext_ecb_256, size);
    aes_256_ecb_decrypt_seq(ciphertext_ecb_256, decrypted, key256, num_blocks);
    print_hex("Decrypted", decrypted, size);
    cout << endl;


    cout << "*****AES 128 ECB THRD*****" <<endl;
    print_hex("Kljuc    ", key128, AES_KEY_SIZE);
    aes_128_ecb_encrypt_thrd(plaintext, ciphertext_ecb_128, key128, num_blocks, 6);
    print_hex("Ciphertext", ciphertext_ecb_128, size);
    aes_128_ecb_decrypt_thrd(ciphertext_ecb_128, decrypted, key128, num_blocks, 7);
    print_hex("Decrypted", decrypted, size);
    cout << endl;

    cout << "*****AES 256 ECB THRD*****" <<endl;
    print_hex("Kljuc    ", key256, AES_256_KEY_SIZE);
    aes_256_ecb_encrypt_thrd(plaintext, ciphertext_ecb_256, key256, num_blocks, 4);
    print_hex("Ciphertext", ciphertext_ecb_256, size);
    aes_256_ecb_decrypt_thrd(ciphertext_ecb_256, decrypted, key256, num_blocks, 4);
    print_hex("Decrypted", decrypted, size);
    cout << endl;


    cout << "*****AES 128 CBC*****" <<endl;
    print_hex("Kljuc    ", key128, AES_KEY_SIZE);
    print_hex("Inicijalni vektor", iv, AES_BLOCK_SIZE);
    aes_128_cbc_encrypt(plaintext, ciphertext_cbc_128, key128, iv, num_blocks);
    print_hex("Ciphertext", ciphertext_cbc_128, size);
    aes_128_cbc_decrypt(ciphertext_cbc_128, decrypted, key128, iv, num_blocks);
    print_hex("Decrypted", decrypted, size);
    cout << endl;

    cout << "*****AES 256 CBC*****" <<endl;
    print_hex("Kljuc    ", key256, AES_256_KEY_SIZE);
    print_hex("Inicijalni vektor", iv, AES_BLOCK_SIZE);
    aes_256_cbc_encrypt(plaintext, ciphertext_cbc_256, key256, iv, num_blocks);
    print_hex("Ciphertext", ciphertext_cbc_256, size);
    aes_256_cbc_decrypt(ciphertext_cbc_256, decrypted, key256, iv, num_blocks);
    print_hex("Decrypted", decrypted, size);
    cout << endl;

    delete[] plaintext;
    delete[] ciphertext_ecb_128;
    delete[] ciphertext_ecb_256;
    delete[] ciphertext_cbc_128;
    delete[] ciphertext_cbc_256;

    return 0;
}
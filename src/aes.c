#include "aes.h"

uint8_t SBOX[256];
uint8_t INV_SBOX[256];


uint8_t affine_transform(uint8_t a) {
    uint8_t rezultat = a;

    //mnozenje matricom A iz Afine transformacije
    //  | 1 0 0 0 1 1 1 1 |
    //  | 1 1 0 0 0 1 1 1 |
    //  | 1 1 1 0 0 0 1 1 |
    //  | 1 1 1 1 0 0 0 1 |
    //  | 1 1 1 1 1 0 0 0 |
    //  | 0 1 1 1 1 1 0 0 |
    //  | 0 0 1 1 1 1 1 0 |
    //  | 0 0 0 1 1 1 1 1 |

    //zato sto je svaki red ciklicna rotacija prethodnog, moze se implementirati kao rotacija bajtova

    rezultat ^= (uint8_t) ((a << 1) | (a >> 7));
    rezultat ^= (uint8_t) ((a << 2) | (a >> 6));
    rezultat ^= (uint8_t) ((a << 3) | (a >> 5));
    rezultat ^= (uint8_t) ((a << 4) | (a >> 4));

    //sabiranje sa vektorom b iz Afine transformacijom

    rezultat ^= 0b01100011;

    return rezultat;
}

//generisanje lookup tabela za sbox i inv_sbox
void generate_sbox(void) {
    for(int i = 0; i < 256; i++) {
        SBOX[i] = affine_transform(gf_inv_table(i));
        INV_SBOX[SBOX[i]] = i;
    }
}

uint8_t sbox(uint8_t a) {
    return SBOX[a];
}

uint8_t inv_sbox(uint8_t a) {
    return INV_SBOX[a];
}

//primenjuje sbox nad svakim elementom matrice state
void subBytes(uint8_t state[4][4]) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            state[i][j] = sbox(state[i][j]);
        }
    }
}
//primenjuje inv_sbox nad svakim elementom matrice state
void invSubBytes(uint8_t state[4][4]) {
    for(int i = 0; i <  4; i++) {
        for(int j = 0; j < 4; j++) {
            state[i][j] = inv_sbox(state[i][j]);
        }
    }
}

void shiftRows(uint8_t state[4][4]) {
    uint8_t temp[4];

    for(int i = 1; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            temp[j] = state[i][(j + i) % 4];
        }
        for(int k = 0; k < 4; k++) {
            state[i][k] = temp[k];
        }
    }
}

void invShiftRows(uint8_t state[4][4]) {
    uint8_t temp[4];

    for(int i = 1; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            temp[j] = state[i][(j - i + 4) % 4];
        }
        for(int k = 0; k < 4; k++) {
            state[i][k] = temp[k];
        }
    }
}

void mixColumns(uint8_t state[4][4]) {
    //mnozenje sa matricom
    //  | 2 3 1 1 |
    //  | 1 2 3 1 |
    //  | 1 1 2 3 |
    //  | 3 1 1 2 |

    //cuvanje originala svake kolone i na osnovu originala se racuna rezultat.
    for(int i = 0; i < 4; i++) {
        //sacuvam kolonu pre mnozenja martricom
        uint8_t s0 = state[0][i];
        uint8_t s1 = state[1][i];
        uint8_t s2 = state[2][i];
        uint8_t s3 = state[3][i];

        //na osnovu starih vrednosti iz vrste racunam nove.
        state[0][i] = gf_mul(2, s0) ^ gf_mul(3, s1) ^ gf_mul(1, s2) ^ gf_mul(1, s3); 
        state[1][i] = gf_mul(1, s0) ^ gf_mul(2, s1) ^ gf_mul(3, s2) ^ gf_mul(1, s3);
        state[2][i] = gf_mul(1, s0) ^ gf_mul(1, s1) ^ gf_mul(2, s2) ^ gf_mul(3, s3);
        state[3][i] = gf_mul(3, s0) ^ gf_mul(1, s1) ^ gf_mul(1, s2) ^ gf_mul(2, s3);
    }
}

void invMixColumns(uint8_t state[4][4]) {
    //mnozenje fiksnom matricom kako bi se postiglo suprotno mesanje od onog iz mixColumns
    //  | E B D 9 |
    //  | 9 E B D |
    //  | D 9 E B |
    //  | B D 9 E |

    //cuvanje originala svake kolone i na osnovu originala se racuna rezultat.
    for(int i = 0; i < 4; i++) {
        //sacuvam kolonu pre mnozenja martricom
        uint8_t s0 = state[0][i];
        uint8_t s1 = state[1][i];
        uint8_t s2 = state[2][i];
        uint8_t s3 = state[3][i];

        //na osnovu starih vrednosti iz vrste racunam nove.
        state[0][i] = gf_mul(0xE, s0) ^ gf_mul(0xB, s1) ^ gf_mul(0xD, s2) ^ gf_mul(0x9, s3); 
        state[1][i] = gf_mul(0x9, s0) ^ gf_mul(0xE, s1) ^ gf_mul(0xB, s2) ^ gf_mul(0xD, s3);
        state[2][i] = gf_mul(0xD, s0) ^ gf_mul(0x9, s1) ^ gf_mul(0xE, s2) ^ gf_mul(0xB, s3);
        state[3][i] = gf_mul(0xB, s0) ^ gf_mul(0xD, s1) ^ gf_mul(0x9, s2) ^ gf_mul(0xE, s3);
    }

}

//round constand - konstanta koja se dodaje u svakoj rundi kako bi se kljucevi razlikovali
//radi kao lookup tabela
static const uint8_t R_CON[11] = {0x0, 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};


//ciklicna rotacija reci za 1 baj ulevo
static void rot_word(uint8_t word[4]) {
    uint8_t temp = word[0];
    word[0] = word[1];
    word[1] = word[2];
    word[2] = word[3];
    word[3] = temp; 
}

//primenjuje sbox na svaki bajt reci
static void sub_word(uint8_t word[4]) {
    for(int i = 0; i < 4 ; i++) {
        //BITNO JE DA SBOX LOOKUP TABELA VEC BUDE INICIJALIZOVANA JER JE FUNKCIJA sbox KORISTI 
        //ODNOSNO DA JE FUNKCIJA generate_sbox vec pozvana
        word[i] = sbox(word[i]);
    }
}

//RIJNDAL algoritam key scheduling za aes 128
void key_expansion_128(const uint8_t key[AES_KEY_SIZE], uint8_t roundKey[11][4][4]) {
    //kljucevi se generisu kao 2d matrica pa se zatim pretvaraju u 3d "tenzor"
    uint8_t pomocni_niz[44][4];

    //prve 4 reci direktno iz kljuca
    for(int i = 0; i < 4; i++) {
        pomocni_niz[i][0] = key[i * 4 ];
        pomocni_niz[i][1] = key[i * 4 + 1];
        pomocni_niz[i][2] = key[i * 4 + 2];
        pomocni_niz[i][3] = key[i * 4 + 3];
    }

    //ostalih 40n reci po algoritmu
    for(int i = 4; i < 44; i++) {
        //pravi se kopija 
        uint8_t temp[4];
        temp[0] = pomocni_niz[i - 1][0];
        temp[1] = pomocni_niz[i - 1][1];
        temp[2] = pomocni_niz[i - 1][2];
        temp[3] = pomocni_niz[i - 1][3];

        //svaku 4. iteraciju na temp se primene rot_word, sub_word i XOR sa roundKey-om
        if(i % 4 == 0) {
            rot_word(temp);
            sub_word(temp);
            //svaki round key ima oblik kljuc(8 bita) i ostatak su nule, tako da je XOR sa 0 nepotreban.
            temp[0] ^= R_CON[i / 4];
        }

        //xor sa reci koja je 4 pozicije ranije
        pomocni_niz[i][0] = pomocni_niz[i - 4][0] ^ temp[0];
        pomocni_niz[i][1] = pomocni_niz[i - 4][1] ^ temp[1];
        pomocni_niz[i][2] = pomocni_niz[i - 4][2] ^ temp[2];
        pomocni_niz[i][3] = pomocni_niz[i - 4][3] ^ temp[3];

    }

    //kopiranje pomocni_niz[44][4] u roundKey[11][4][4]
    for(int round = 0; round < 11; round++) {
        for(int vrsta = 0; vrsta < 4; vrsta++) {
            for(int kolona = 0; kolona < 4; kolona ++) {
                roundKey[round][vrsta][kolona] = pomocni_niz[round * 4 + kolona][vrsta];
            }
        }
    }

}


//RIJNDAL algoritam key scheduling za aes 256
void key_expansion_256(const uint8_t key[AES_256_KEY_SIZE], uint8_t roundKey[15][4][4]) {
    //kljucevi se generisu kao 2d matrica pa se zatim pretvaraju u 3d "tenzor"
    uint8_t pomocni_niz[60][4];

    //prve 8 reci direktno iz kljuca
    for(int i = 0; i < 8; i++) {
        pomocni_niz[i][0] = key[i * 4 ];
        pomocni_niz[i][1] = key[i * 4 + 1];
        pomocni_niz[i][2] = key[i * 4 + 2];
        pomocni_niz[i][3] = key[i * 4 + 3];
    }

    //ostalih 52 reci po algoritmu
    for(int i = 8; i < 60; i++) {
        //pravi se kopija 
        uint8_t temp[4];
        temp[0] = pomocni_niz[i - 1][0];
        temp[1] = pomocni_niz[i - 1][1];
        temp[2] = pomocni_niz[i - 1][2];
        temp[3] = pomocni_niz[i - 1][3];

        //svaku 8. iteraciju na temp se primene rot_word, sub_word i XOR sa roundKey-om
        if(i % 8 == 0) {
            rot_word(temp);
            sub_word(temp);
            //svaki round key ima oblik kljuc(8 bita) i ostatak su nule, tako da je XOR sa 0 nepotreban.
            temp[0] ^= R_CON[i / 8];
        }

        //svaka 4. iteracija, ali ne ako je 8.  
        if((i % 8 != 0) && (i % 4 == 0)) {
            sub_word(temp);
        }

        //xor sa reci koja je 4 pozicije ranije
        pomocni_niz[i][0] = pomocni_niz[i - 8][0] ^ temp[0];
        pomocni_niz[i][1] = pomocni_niz[i - 8][1] ^ temp[1];
        pomocni_niz[i][2] = pomocni_niz[i - 8][2] ^ temp[2];
        pomocni_niz[i][3] = pomocni_niz[i - 8][3] ^ temp[3];

    }

    //kopiranje pomocni_niz[60][4] u roundKey[15][4][4]
    for(int round = 0; round < 15; round++) {
        for(int vrsta = 0; vrsta < 4; vrsta++) {
            for(int kolona = 0; kolona < 4; kolona ++) {
                roundKey[round][vrsta][kolona] = pomocni_niz[round * 4 + kolona][vrsta];
            }
        }
    }

}

void addRoundKey(uint8_t state[4][4], uint8_t roundKey[4][4]) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            //xor maskiranje sa odgovarajucim bajtom kljuca
            //xor je ekvivalent i sabiranju i oduzimanju u GF(2^8),
            //tako da se addRoundKey koristi i za enkripcijui i za dekripciju.
            state[i][j] ^= roundKey[i][j];
        }
    }
}

void loadState(const uint8_t input[16], uint8_t state[4][4]) {
    for(int i = 0; i < 16; i++) {
        state[i % 4][i / 4] = input[i];
    }
}

void storeState(const uint8_t state[4][4], uint8_t output[16]) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            output[i + 4 * j] = state[i][j];
        }
    }
}

void aes_128_encrypt(const uint8_t plaintext[16], const uint8_t key[AES_KEY_SIZE], uint8_t ciphertext[16]) {
    uint8_t state[4][4];
    uint8_t roundKeys[11][4][4];

    generate_sbox();    //OBAVEZNO PRE POZIVA KEY EXPANSION

    key_expansion_128(key, roundKeys);  //sada je roundKeys popunjen

    loadState(plaintext, state);

    //u prvom krugu se samo doda prvi kljuc
    addRoundKey(state, roundKeys[0]);

    //u ostalim krugovima se smenjuju bajtu, shiftuju vrste, mesaju kolone i dodaje se odgovarajuci kljuc 
    for(int i = 1; i < AES_ROUNDS; i++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        //mixcolumns_asm(state);
        addRoundKey(state, roundKeys[i]);
    }

    //u poslednjem krugu bez mixcolumns
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys[AES_ROUNDS]);

    storeState(state, ciphertext);
}

void aes_128_decrypt(uint8_t plaintext[16], const uint8_t key[AES_KEY_SIZE], const uint8_t ciphertext[16]) {
    uint8_t state[4][4];
    uint8_t roundKeys[11][4][4];

    generate_sbox();//OBAVEZNO PRE KEY SCHEDULING-a

    key_expansion_128(key, roundKeys);

    loadState(ciphertext, state);

    addRoundKey(state, roundKeys[AES_ROUNDS]);
    invShiftRows(state);
    invSubBytes(state);

    for(int i = AES_ROUNDS - 1; i > 0; i--) {
        addRoundKey(state, roundKeys[i]);
        invMixColumns(state);
        invShiftRows(state);
        invSubBytes(state);
    }

    addRoundKey(state, roundKeys[0]);

    storeState(state, plaintext);
}


void aes_256_encrypt(const uint8_t plaintext[16], const uint8_t key[AES_256_KEY_SIZE], uint8_t ciphertext[16]) {
    uint8_t state[4][4];
    uint8_t roundKeys[15][4][4];

    generate_sbox();    //OBAVEZNO PRE POZIVA KEY EXPANSION

    key_expansion_256(key, roundKeys);  //sada je roundKeys popunjen

    loadState(plaintext, state);

    //u prvom krugu se samo doda prvi kljuc
    addRoundKey(state, roundKeys[0]);

    //u ostalim krugovima se smenjuju bajtu, shiftuju vrste, mesaju kolone i dodaje se odgovarajuci kljuc 
    for(int i = 1; i < AES_256_ROUNDS; i++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        //mixcolumns_asm(state);
        addRoundKey(state, roundKeys[i]);
    }

    //u poslednjem krugu bez mixcolumns
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys[AES_256_ROUNDS]);

    storeState(state, ciphertext);
}

void aes_256_decrypt(uint8_t plaintext[16], const uint8_t key[AES_256_KEY_SIZE], const uint8_t ciphertext[16]) {
    uint8_t state[4][4];
    uint8_t roundKeys[15][4][4];

    generate_sbox();//OBAVEZNO PRE KEY SCHEDULING-a

    key_expansion_256(key, roundKeys);

    loadState(ciphertext, state);

    addRoundKey(state, roundKeys[AES_256_ROUNDS]);
    invShiftRows(state);
    invSubBytes(state);

    for(int i = AES_256_ROUNDS - 1; i > 0; i--) {
        addRoundKey(state, roundKeys[i]);
        invMixColumns(state);
        invShiftRows(state);
        invSubBytes(state);
    }

    addRoundKey(state, roundKeys[0]);

    storeState(state, plaintext);
}
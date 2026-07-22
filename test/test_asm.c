#include <stdio.h>
#include "aes.h"
#include "gf.h"
#include <stdint.h>

void mixcolumns_asm(uint8_t state[4][4]);

int main() {

    uint8_t state[4][4] = {
        {0x01, 0x02, 0x08, 0x10},
        {0x03, 0x05, 0x0A, 0x12},
        {0x06, 0x08, 0x0C, 0x14},
        {0x08, 0x0A, 0x0F, 0x16}
    };

    mixcolumns_asm(state);

    printf("ASM:\n");
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            printf("%02x\t", state[i][j]);
        }
        printf("\n");
    }

    uint8_t state_c[4][4] = {
        {0x01, 0x02, 0x08, 0x10},
        {0x03, 0x05, 0x0A, 0x12},
        {0x06, 0x08, 0x0C, 0x14},
        {0x08, 0x0A, 0x0F, 0x16}
    };

    mixColumns(state_c);

    printf("\nC:\n");
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            printf("%02x\t", state_c[i][j]);
        }
        printf("\n");
    }


    return 0;
}
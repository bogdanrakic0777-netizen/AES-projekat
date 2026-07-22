#ifndef GF_H
#define GF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define IRREDUBLE 0x11B
#define REDUCE 0x1B

uint8_t xtime(uint8_t a);

uint8_t gf_mul(uint8_t a, uint8_t b);

uint8_t gf_inv_table(uint8_t a);

uint8_t gf_inv_fermat(uint8_t a);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // GF_H
#ifndef _MY_MISC_H_
#define _MY_MISC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Macros to make a string from a macro
#define STRINGIFY_(M) #M
#define STRINGIFY(M) STRINGIFY_(M)

#define NOBEYOND(min, v, max) _MAX(min, _MIN(v, max))

uint8_t inRange(int cur, int tag , int range);

uint8_t string_2_uint8_t(const uint8_t *string);
uint8_t *uint8_2_string(uint8_t num, uint8_t *string);
uint32_t string_2_uint32(const uint8_t *string, const uint8_t bytes_num);
uint8_t *uint32_2_string(uint32_t num, uint8_t bytes_num, uint8_t *string);

#ifdef __cplusplus
}
#endif

#endif

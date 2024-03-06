// stdint.h // ~ eylon

#if !defined(__LIBC_STDINT_H)
#define __LIBC_STDINT_H

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef          char  int8_t;
typedef          short int16_t;
typedef          int   int32_t;

// Get lowest 16 bits out of 32 bit data
#define low32(value32) ((uint16_t) ((value32) & 0xFFFF))

// Get highest 16 bits out of 32 bit data
#define high32(value32) ((uint16_t) (((value32) >> 16) & 0xFFFF))

// Get lowest 8 bits out of 16 bit data
#define low16(value16) ((uint8_t) ((value16) & 0xFF))
// Get highest 8 bits out of 16 bit data
#define high16(value16) ((uint8_t) (((value16) >> 8) & 0xFF))

#endif
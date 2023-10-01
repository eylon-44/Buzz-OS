// Type Defining // ~ eylon

#if !defined(TYPE_H)
#define TYPE_H

typedef unsigned int   u32_t;
typedef          int   s32_t;
typedef unsigned short u16_t;
typedef          short s16_t;
typedef unsigned char  u8_t;
typedef          char  s8_t;

// Get low or high value out of 32 bit data
#define LOW_16 (b32_d) (u16_t) ((b32_d) & 0xFFFF)
#define HIGH_16(b32_d) (u16_t) (((b32_d) >> 16) & 0xFFFF) 


#endif

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
#define LOW_16(arg32) ((u16_t) ((arg32) & 0xFFFF))
#define HIGH_16(arg32) ((u16_t) (((arg32) >> 16) & 0xFFFF))


#endif
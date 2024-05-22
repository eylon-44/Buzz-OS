// bitfield.h // ~ eylon

#if !defined(__LIBC_BITFIELD_H)
#define __LIBC_BITFIELD_H


// Get bit number [bit] from [map] bitmap
#define BITMAP_GET_BIT(map, bit) \
    ( *((map) + (bit) /((sizeof(map[0])*8))) \
    >> ((bit) % (sizeof(map[0])*8)) \
    &  0x1 )

// Set bit number [bit] with [value: 0/1] at [map] bitmap
#define BITMAP_SET_BIT(map, bit, value) \
    ( *((map) + (bit) / (sizeof(map[0])*8)) \
    ^= (BITMAP_GET_BIT(map, bit) != (value & 0x01)) << ((bit) % (sizeof(map[0])*8)) )


// Get bit number [bit] from [field] bitfield
#define BITFIELD_GET_BIT(field, bit) \
    ( ((field) >> (bit)) & 0x1 )

// Set bit number [bit] with [value: 0/1] at [field] bitfield
#define BITFIELD_SET_BIT(field, bit, value) \
    (field) ^= (BITFIELD_GET_BIT(field, bit) != (value & 0x01)) << ((bit))

#endif


/*
BITMAP/BITFIELD_SET_BIT logic {
    ## TURN ON ##
    <xor 1 with 0 (1!=0 -> 1) will turn on the bit>
        |        [0]                              [1]
    [0b1010] ^= (BITFIELD_GET_BIT(map, bit) != (value & 0x01)) << ((bit) % sizeof(map[0]))
    <xor 1 with 0 (1!=1 -> 0) won't change anything>
        |        [1]                              [1]
    [0b1110] ^= (BITFIELD_GET_BIT(map, bit) != (value & 0x01)) << ((bit) % sizeof(map[0]))

    ## TURN OFF ##
    <xor 1 with 1 (1!=0 -> 1) will turn off the bit>
        |        [1]                              [0]
    [0b1110] ^= (BITFIELD_GET_BIT(map, bit) != (value & 0x01)) << ((bit) % sizeof(map[0]))
    <xor 0 with 0 (0!=0 -> 0) won't change anything>
        |        [0]                              [0]
    [0b1010] ^= (BITFIELD_GET_BIT(map, bit) != (value & 0x01)) << ((bit) % sizeof(map[0]))
}
*/
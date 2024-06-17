// LIBC Heap Header File // ~ eylon

#if !defined(__LIBC_HEAP_H)
#define __LIBC_HEAP_H

#include <stdint.h>
#include <stddef.h>
#include <mm.h>

// Heap data structure
typedef struct
{
    size_t start;
    size_t top;
} heap_t;

// Heap chunk header data structure
typedef struct
{
    uint32_t size;         // size & 0x1 = free/used
    uint32_t prev_size;
} heap_header_t;

// Max heap size
#define HEAP_MAX_SIZE MB(2)

// Heap chunks alignment
#define HEAP_ALIGNMENT 8

// Calculate heap size
#define HEAP_SIZE(heap) ((heap).top - (heap).start)

#define CHUNK_SIZE(header_ptr)            ((header_ptr)->size & ~0x1)
#define CHUNK_PREV_SIZE(header_ptr)       ((header_ptr)->prev_size & ~0x1)

#define CHUNK_USED(header_ptr)            ((header_ptr)->size & 0x1)
#define CHUNK_PREV_USED(header_ptr)       ((header_ptr)->prev_size & 0x1)

// Get a pointer to the next chunk
#define NEXT_CHUNK(header_ptr)            ((heap_header_t*) ((uint8_t*) (header_ptr) + CHUNK_SIZE(header_ptr)))
// Get a pointer to the previous chunk
#define PREV_CHUNK(header_ptr)            ((heap_header_t*) ((uint8_t*) (header_ptr) - CHUNK_PREV_SIZE(header_ptr)))
// Get a pointer to the start of the data in the chunk
#define CHUNK_DATA(header_ptr)            ((void*) ((uint8_t*) (header_ptr) + sizeof(heap_header_t)))
// Get a poniter to the chunk's header by a pointer to its data
#define CHUNK_HEADER(header_ptr)          ((heap_header_t*) ((uint8_t*) (header_ptr) - sizeof(heap_header_t)))

#endif
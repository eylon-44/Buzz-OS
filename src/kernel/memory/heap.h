// Kernel Heap Header File // ~ eylon

#if !defined(KHEAP_H)
#define KHEAP_H

#include <kernel/memory/mm.h>
#include <libc/stddef.h>

// Heap data structure
typedef struct heap_t heap_t;
struct heap_t
{
    vaddr_t start;
    vaddr_t top;
    int (*extension) (heap_t* heap, vaddr_t new_top);
};

// Heap chunk header data structure
typedef struct
{
    uint32_t size;         // size & 0x1 = free/used
    uint32_t prev_size;
} heap_header_t;

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

void init_kheap();
void* kmalloc(size_t size);
void kfree(void* ptr);

#endif
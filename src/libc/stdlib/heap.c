// stdlib/heap.c // ~ eylon

#include "heap.h"
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <mm.h>
#include <string.h>

/* Allocate and free dynamic memory.

    #include <stdlib.h>
    void* malloc(size_t size);
    void free(void* ptr);
    void* calloc(size_t nmemb, size_t size);
    void* realloc(void* ptr, size_t size);
    void* reallocarray(void* ptr, size_t nmemb, size_t size);
*/

// The heap structure
static heap_t heap;

/* Heap extension function; set a new top for the heap.
    On sucess returns 0. On error returns non-zero. */
static int heap_extend(size_t new_top)
{   
    // [new_top] must be 4KB aligned
    if (new_top % MM_PAGE_SIZE != 0) return 1;

    // if [new_top] is out of the heap boundaries
    if (new_top <= heap.start || new_top > heap.start + HEAP_MAX_SIZE) return 1;

    // If [new_top] is equal to [heap.top], then this function has no use
    if (new_top == heap.top) return 0;

    // Allocate or deallocate memory as needed
    new_top = (size_t) sbrk(new_top - heap.top);
    if ((void*) new_top == NULL) return 1;

    heap.top = new_top;

    return 0;
}

/* The malloc() function allocates size bytes and returns a pointer
    to the allocated memory. The memory is not initialized.
*/
void* malloc(size_t size)
{
    // Calculate padding and the new chunk's size
    size_t padding = (HEAP_ALIGNMENT - (size % HEAP_ALIGNMENT)) % HEAP_ALIGNMENT;
    size_t new_chunk_size = sizeof(heap_header_t) + size + padding;

    // [best_chunk] variable will store the choosen chunk at the end of the loop
    heap_header_t* best_chunk = NULL;
    heap_header_t* chunk_ptr = (heap_header_t*) heap.start;

    // Go over all chunks in the heap
    for (;;) {

        // if [chunk_ptr] is free and its size is greater than or equal to the required size
        if (!CHUNK_USED(chunk_ptr) && CHUNK_SIZE(chunk_ptr) >= new_chunk_size) 
        {
            // if we found a chunk with perfect match in size, set it as the [best_chunk] and break the loop
            if (CHUNK_SIZE(chunk_ptr) == new_chunk_size) { 
                best_chunk = chunk_ptr;   
                break;
            }
            // else, check if its size is smaller than the size of the [best_chunk], if it is, then it's a better chunk
            if (best_chunk == NULL || CHUNK_SIZE(chunk_ptr) < CHUNK_SIZE(best_chunk)) {
                best_chunk = chunk_ptr;
            }
        }

        // If it's the last chunk in the heap, break
        if ((void*) NEXT_CHUNK(chunk_ptr) == (void*) heap.top) break;

        // Step the heap
        chunk_ptr = NEXT_CHUNK(chunk_ptr);
    }
    
    // If we did not find a chunk
    if (best_chunk == NULL) {
        /* In this case, the variable [chunk_ptr] represents the last (highest) chunk in the heap.
            We need to allocate more memory for the heap, so will increase it by 4KB. */
        if (!heap_extend((size_t) heap.top + MM_PAGE_SIZE)) {
            // If the last chunk (the highest in the heap) is free, merge it with the newly allocated space
            if (!CHUNK_USED(chunk_ptr)) {
                // Add to its size the newly allocated memory and set it as the best chunk
                chunk_ptr->size += MM_PAGE_SIZE;
                best_chunk = chunk_ptr;
            }
            // If the last chunk is not free, create a new header at the beginning of the newly allocated space and set it as the best chunk
            else {
                // Get the next chunk
                best_chunk = NEXT_CHUNK(chunk_ptr);
                // Set the header
                *best_chunk = (heap_header_t) { .size = MM_PAGE_SIZE, .prev_size = chunk_ptr->size };
            }
        }
        // If allocation failed, we can't increase the heap size, aborting, return NULL
        else {
            return NULL;
        }
    }

    // Check if we can split the chunk; we can if it can fit another chunk in the unused area
    if (CHUNK_SIZE(best_chunk) - new_chunk_size  >= sizeof(heap_header_t) + HEAP_ALIGNMENT) {
        // Split the chunks by placing the newly allocated chunk is at the bottom of the larger chunk
        size_t total_size = CHUNK_SIZE(best_chunk);
        best_chunk->size = new_chunk_size;
        chunk_ptr = NEXT_CHUNK(best_chunk);
        chunk_ptr->size = total_size - CHUNK_SIZE(best_chunk);
        chunk_ptr->prev_size = best_chunk->size;
    }

    // Mark the [best_chunk] as used
    best_chunk->size |= 1;
    // Ff this is not the last chunk, set the [prev_size] of the next chunk with the size of [best_chunk]
    if ((void*) NEXT_CHUNK(best_chunk) != (void*) heap.top) {
        chunk_ptr = NEXT_CHUNK(best_chunk);
        chunk_ptr->prev_size = best_chunk->size;
    }

    // Return a pointer to the start of the data section in the [best_chunk]
    return CHUNK_DATA(best_chunk);
}


/* The free() function frees the memory space pointed to by [ptr],
    which must have been returned by a previous call to malloc() or
    related functions.  Otherwise, or if [ptr] has already been freed,
    undefined behavior occurs. If ptr is NULL, no operation is
    performed.
*/
void free(void* ptr)
{
    heap_header_t* chunk_ptr = CHUNK_HEADER(ptr);

    /* Check for chunk corruption or invalidation */

    // if NULL pointer
    if (ptr == NULL) return;

    // if chunk is already free
    if (!CHUNK_USED(chunk_ptr)) return;

    // if chunk size or previous chunk size is not aligned
    if (CHUNK_SIZE(chunk_ptr) % HEAP_ALIGNMENT != 0 || CHUNK_PREV_SIZE(chunk_ptr) % HEAP_ALIGNMENT != 0) return;
    
    // if previous size is 0 and chunk is not at the heap start or chunk is at the heap start but previous size is not 0 
    if ((CHUNK_PREV_SIZE(chunk_ptr) == 0) != ((void*) chunk_ptr == (void*) heap.start)) return;

    // if the size of the previous chunk that is registered in the current chunk does not match the actual size of the previous chunk
    if (CHUNK_PREV_SIZE(chunk_ptr) != 0 && chunk_ptr->prev_size != PREV_CHUNK(chunk_ptr)->size) return;

    // if the previous size attribute of the next chunk is not equal to the actual size of the current chunk
    if ((void*) NEXT_CHUNK(chunk_ptr) != (void*) heap.top && chunk_ptr->size != NEXT_CHUNK(chunk_ptr)->prev_size) return;
    
    // if pointer is not in heap range
    if ((void*) chunk_ptr >= (void*) heap.top || (void*) chunk_ptr < (void*) heap.start) return;


    /* Check for merging opportunities with adjacent chunks */

    // if can merge with top chunk
    if ((void*) NEXT_CHUNK(chunk_ptr) != (void*) heap.top && !CHUNK_USED(NEXT_CHUNK(chunk_ptr))) {
        chunk_ptr->size += CHUNK_SIZE(NEXT_CHUNK(chunk_ptr));
    }
    
    // if can merge with bottom chunk
    if ((void*) chunk_ptr != (void*) heap.start && !CHUNK_USED(PREV_CHUNK(chunk_ptr))) {
        PREV_CHUNK(chunk_ptr)->size += CHUNK_SIZE(chunk_ptr);
        chunk_ptr = PREV_CHUNK(chunk_ptr);
    }

    // free the chunk
    chunk_ptr->size &= ~1;
    // if this is not the last chunk, set the [prev_size] of the next chunk with the size of the freed chunk
    if ((void*) NEXT_CHUNK(chunk_ptr) != (void*) heap.top) {
        // set the [prev_size] attribute of the next chunk
        NEXT_CHUNK(chunk_ptr)->prev_size = chunk_ptr->size;    
    }
}

/* The calloc() function allocates memory for an array of [nmemb] elements
    of [size] bytes each and returns a pointer to the allocated memory. 
    The memory is set to zero. calloc() is equivalent to memset(malloc(nmemb * size), 0, nmemb * size);
*/
void* calloc(size_t nmemb, size_t size)
{
    return memset(malloc(nmemb * size), 0, nmemb * size);   
}

/* The realloc() function changes the size of the memory block pointed to
    by [ptr] to [size] bytes. The contents of the memory will be unchanged
    in the range from the start of the region up to the minimum of the old
    and new sizes. If the new size is larger than the old size, the added
    memory will not be initialized.

    If [ptr] is NULL, then the call is equivalent to malloc(size), for all
    values of size.

    Unless [ptr] is NULL, it must have been returned by an earlier call to malloc
    or related functions. If the area pointed to was moved, a free(ptr) is done.
*/
void* realloc(void* ptr, size_t size)
{
    void* chunk;
    size_t cpy_size;

    // If [ptr] is NULL, function acts as malloc
    if (ptr == NULL) {
        return malloc(size);
    }

    // Get the number of bytes to copy from the old to the new chunk
    cpy_size  = CHUNK_SIZE(CHUNK_HEADER(ptr));
    if (cpy_size > size) cpy_size = size;

    // Allocate a chunk with size [size] and copy the old chunk's contents into it
    chunk = malloc(size);
    memcpy(chunk, ptr, cpy_size);

    // Free the old chunk and return the new chunk
    free(ptr);
    return chunk;
}

// Initiate the heap
void __libc_init_heap()
{
    // Initiate the heap structure
    size_t pbrk = (size_t) sbrk(0);
    heap.start = ALIGN_UP(pbrk, MM_PAGE_SIZE);
    heap.top   = heap.start;

    // Extend the heap by one page
    heap_extend(heap.start + MM_PAGE_SIZE);

    // Create the first header in the heap
    *(heap_header_t*) heap.start = (heap_header_t) { .size = HEAP_SIZE(heap), .prev_size = 0 };
}
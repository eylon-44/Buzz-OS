// Kernel Heap Implementation // ~ eylon

#include "heap.h"
#include "vmm.h"
#include "pmm.h"
#include <kernel/memory/mm.h>
#include <kernel/memory/mmlayout.h>
#include <libc/stdint.h>
#include <libc/stddef.h>

// The kernel heap is initiated with a single page
static heap_t kheap = { .start = MM_KHEAP_START, .top = MM_KHEAP_START + MM_PAGE_SIZE };

/* Heap extension function :: set a new top for the heap.
    Return [0] for success and [1] for error.
*/
int heap_extend(heap_t* heap, vaddr_t new_top)
{   
    // [new_top] must be 4KB aligned
    if (new_top % MM_PAGE_SIZE != 0) return 1;

    // if [new_top] is out of the heap boundaries
    if (new_top <= heap->start || new_top > MM_KHEAP_START + MM_KHEAP_SIZE) return 1;

    // If [new_top] is equal to [heap->top], there is nothing to allocate or deallocate
    if (new_top == heap->top) return 0;

    // If [new_top] is greater than [heap->top] we need to allocate memory
    if (new_top > heap->top) {
        for (vaddr_t p = heap->top; p < new_top; p += MM_PAGE_SIZE) {
            vmm_map_page(vmm_get_pd(), pmm_get_page(), p, 1, 0, 0, 1);
        }
    }

    // else, [new_top] is less than [heap->top], meaning we need to deallocate memory
    else if (new_top < heap->top) {
        for (vaddr_t p = heap->top; p > new_top; p -= MM_PAGE_SIZE) {
            pmm_free_page(MM_GET_PHYSICAL(p - MM_PAGE_SIZE, vmm_get_pd()));
            vmm_unmap_page(vmm_get_pd(), p - MM_PAGE_SIZE);
        }
    }

    heap->top = new_top;
    return 0;
}


// Allocate [size] bytes for the on the kearnel heap and return a pointer to the allocated data
void* kmalloc(uint32_t size)
{
    // calculate padding and the new chunk's size
    uint32_t padding = HEAP_ALIGNMENT - (size % HEAP_ALIGNMENT);
    uint32_t new_chunk_size = sizeof(heap_header_t) + size + padding;

    // [best_chunk] variable will store the choosen chunk at the end of the loop
    heap_header_t* best_chunk = NULL;
    heap_header_t* chunk_ptr = (heap_header_t*) kheap.start;

    // go over all the chunks in the heap
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

        // if it's the last chunk in the heap
        if ((void*) NEXT_CHUNK(chunk_ptr) == (void*) kheap.top) break;
        // iterate over the chunks: get and set the next chunk in the heap
        chunk_ptr = NEXT_CHUNK(chunk_ptr);
    }
    

    // if we did not find a chunk
    if (best_chunk == NULL) {
        // in this case the variable [chunk_ptr] represents the last (highest) chunk in the heap
        // alocate more space for the heap; increase the heap size by 4KB
        if (heap_extend(&kheap, (uint32_t) kheap.top + MM_PAGE_SIZE)) {
            // if the last chunk (the highest in the address space) is free, merge it with the newly allocated space
            if (!CHUNK_USED(chunk_ptr)) {
                // add to its size the allocated space and set it as the best chunk
                chunk_ptr->size += MM_PAGE_SIZE;
                best_chunk = chunk_ptr;
            }
            // if the last chunk is not free, create a new header at the beginning of the newly allocated space and set it as the best chunk
            else {
                // get the next chunk
                best_chunk = NEXT_CHUNK(chunk_ptr);
                // set the header
                *best_chunk = (heap_header_t) { .size = MM_PAGE_SIZE, .prev_size = chunk_ptr->size };
            }
        }
        // if allocation failed, we can't increase the heap size, aborting, return NULL
        else {
            return NULL;
        }
    }


    // check if we can split the chunk; if it can fit another chunk in the unused area
    if (CHUNK_SIZE(best_chunk) - new_chunk_size  >= sizeof(heap_header_t) + HEAP_ALIGNMENT) {
        // split the chunks that the newly allocated chunk is at the bottom
        uint32_t total_size = CHUNK_SIZE(best_chunk);
        best_chunk->size = new_chunk_size;
        chunk_ptr = NEXT_CHUNK(best_chunk);
        chunk_ptr->size = total_size - CHUNK_SIZE(best_chunk);
        chunk_ptr->prev_size = best_chunk->size;
    }

    // mark the [best_chunk] as used
    best_chunk->size |= 1;
    // if this is not the last chunk, set the [prev_size] of the next chunk with the size of [best_chunk]
    if ((void*) NEXT_CHUNK(best_chunk) != (void*) kheap.top) {
        chunk_ptr = NEXT_CHUNK(best_chunk);
        chunk_ptr->prev_size = best_chunk->size;
    }

    // return a pointer to the start of the data section in the [best_chunk]
    return CHUNK_DATA(best_chunk);
}


// Free and alocated chunk of data on the kernel heap
void kfree(void* ptr)
{
    heap_header_t* chunk_ptr = (heap_header_t*) ((char*) ptr - sizeof(heap_header_t));

    /* Check for chunk corruption or invalidation */

    // if NULL pointer
    if (ptr == NULL) return;

    // if chunk is already free
    if (!CHUNK_USED(chunk_ptr)) return;

    // if chunk size or previous chunk size is not aligned
    if (CHUNK_SIZE(chunk_ptr) % HEAP_ALIGNMENT != 0 || CHUNK_PREV_USED(chunk_ptr) % HEAP_ALIGNMENT != 0) return;
    
    // if previous size is 0 and chunk is not at the heap start or chunk is at the heap start but previous size is not 0 
    if ((CHUNK_PREV_SIZE(chunk_ptr) == 0) != ((void*) chunk_ptr == (void*) kheap.start)) return;

    // if the size of the previous chunk that is registered in the current chunk does not match the actual size of the previous chunk
    if (CHUNK_PREV_SIZE(chunk_ptr) != 0 && chunk_ptr->prev_size != PREV_CHUNK(chunk_ptr)->size) return;

    // if the previous size attribute of the next chunk is not equal to the actual size of the current chunk
    if ((void*) NEXT_CHUNK(chunk_ptr) != (void*) kheap.top && chunk_ptr->size != NEXT_CHUNK(chunk_ptr)->prev_size) return;
    
    // if pointer is not in heap range
    if ((void*) chunk_ptr >= (void*) kheap.top || (void*) chunk_ptr < (void*) kheap.start) return;


    /* Check for merging opportunities with adjacent chunks */

    // if can merge with top chunk
    if ((void*) NEXT_CHUNK(chunk_ptr) != (void*) kheap.top && !CHUNK_USED(NEXT_CHUNK(chunk_ptr))) {
        chunk_ptr->size += CHUNK_SIZE(NEXT_CHUNK(chunk_ptr));
    }
    
    // if can merge with bottom chunk
    if ((void*) chunk_ptr != (void*) kheap.start && !CHUNK_USED(PREV_CHUNK(chunk_ptr))) {
        PREV_CHUNK(chunk_ptr)->size += CHUNK_SIZE(chunk_ptr);
        chunk_ptr = PREV_CHUNK(chunk_ptr);
    }

    // free the chunk
    chunk_ptr->size &= ~1;
    // if this is not the last chunk, set the [prev_size] of the next chunk with the size of the freed chunk
    if ((void*) NEXT_CHUNK(chunk_ptr) != (void*) kheap.top) {
        // set the [prev_size] attribute of the next chunk
        NEXT_CHUNK(chunk_ptr)->prev_size = chunk_ptr->size;    
    }
}

// void* kmalloc_aligned(uint32_t size, uint32_t alignment)
// {

// }

// void* kcalloc(uint32_t nmemb, uint32_t size)
// {

// }

// void* krealloc(void* ptr, uint32_t size)
// {

// }




void kheap_init()
{
    // allocated a physical page and map it to the start of the heap [TODO] use the extension function instead
    kheap.extension = heap_extend;
    kheap.extension(&kheap, kheap.top);
    // create the first header in the heap
    *(heap_header_t*) kheap.start = (heap_header_t) { .size = HEAP_SIZE(kheap), .prev_size = 0 };
}
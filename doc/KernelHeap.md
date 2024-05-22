# Kernel Heap

The kernel heap data structure is defined as follows:
```c
typedef struct heap_t heap_t;
struct heap_t
{
    vaddr_t start;
    vaddr_t top;
    int (*extension) (heap_t* heap, vaddr_t new_top);
};
```

`start` is the start address of the heap, `top` is the end of it, and `extension` is a function pointer that can be used to allocate or deallocate memory for it as needed.

Each allocated block of data in the heap is called a **chunk**. A chunk is built out of three components:

#### Header

The header stores the chunk's metadata. It marks the start of the chunk and is located immediately before its [data](#data).

```c
typedef struct
{
    uint32_t size;      // size & 0x1 = free/used
    uint32_t prev_size;
} heap_header_t;
```

The header contains the size of the chunk, as well as the size of the chunk that comes before of it (we will soon discuss the importance of this). Because chunks are 8 bytes aligned, the first three bits in the `size` attribute are always set to zero — at least until now. We are going to use the first bit in `size` attribute of each chunk to indicate whether it is free (0) or used (1).

As all the chunks are stacked on top of each other, we can iterate over them by using the following logic:

* The first header is located at the `start` address of the heap, read it.

* Now that we have the address of the first chunk and know its size, the next chunk is located at `[current_address] + [chunk_size]`.

* The last chunk is marked by the quality of `[current_adddress] + [chunk_size] = [heap_top]`

If we would like to move backwards in the list, we can do so by using the `prev_size` attribute by subtracting it from the address of the current chunk until `[current_adddress] = [heap_start]`.

#### Data

This is where the allocated data is being stored.

#### Padding

As each chunk must be 8-byte aligned, we round up the size of unaligned data by adding padding to it. The size of the padding can be calculated with the following equation: `[padding] = 8 - [data_size] % 8`.

We can now calculate the size of a chunk by adding together the size of these three components: `[chunk_size] = [header_size] + [data_size] + [padding]`.


### Allocation, Deallocation & Merging of Chunks

The initial heap has a single free block in it with the size of the entire allocated heap. For the sake of explanation, let's assume that our heap starts at address 0 and ends at address 800—it would look like this:

```c
heap_t kernel_heap = { .start = 0, .top = 800 };
```

| Address   | Data Type | Size (bytes) | Values       |
| :-------: | :------:  | :----------: | :----------: |
| 800       | Padding   | 000          |              |
| 008       | Data      | 792          |              |
| 000       | Header    | 008          | s=800 p=0    |

> s = size | p = prev_size | (s % 8 == 1) = used | (s % 8 == 0) = free

Notice that the first bit in `s=800` is off, indicating that this chunk is free. Also notice that the size of the previous chunk is `p=0`, indicating that this is indeed the first chunk in the heap.

#### Allocating a Chunk
If we would like to allocate some data, say, 13 bytes, we will have to start a search. What are we searching for? The smallest free chunk that we can fit our data in.

We will iterate over our chunks using the method described earlier until finding the best-matching chunk, or until reaching the end of the heap. In our current situation, we only have a single free chunk, so will have to use it. This chunk is way to big for our cute little 13 bytes allocation, so it would be best to split it. The process of splitting a chunk goes as follows:

* Set the size of the splitted chunk with the new size. In our case it is `[header] + [data] + [padding] = 8 + 13 + 11 = 32`. Keep the previous size as it is.

* Create a new header at address `[current_chunk_address] + [chunk_size]`. In our example, it should be at `0 + 32 = 32`.

* Set the size of the new chunk with the initial size of the splitted chunk, minus the size of the other chunk: `[size] = [initial_size] - [lower_chunk_size]`. If we substitute this expression with our current values, we get that the size of our upper new chunk is `800 - 32 = 768`.

* Set the previous size attribute of the upper chunk to the size of the lower chunk. 

We now have 2 chunks in our heap, both are marked as free. We are going to mark the lower one of them as used by setting the first bit of its size attribute to 1. We will also return the user a pointer to the allocated data, which can be calculated by adding the address of the chunk with the size of the header.

| Address   | Data Type | Size (bytes) | Values       |
| :-------: | :------:  | :----------: | :----------: |
| 800       | Padding   | 000          |              |
| 040       | Data      | 760          |              |
| 032       | Header    | 008          | s=768 p=33   |
| 021       | Padding   | 011          |              |
| 008       | Data      | 013          |              |
| 000       | Header    | 008          | s=33 p=0     |

> s = size | p = prev_size | (s % 8 == 1) = used | (s % 8 == 0) = free

Note that the size of the first chunk is set to `s=33`, while in reality it's 32. Recall that the first bit in the size attribute indicates if the chunk is being in use or not, and is ignored for size calculations. In this scenario, we can see that the first chunk is used (`s&1=1`), opposed to the chunk above it which is free (`s&1=0`).

The user will receive a pointer to address `008` — start of data.

If we can't find any free chunk, we will use the heap `extension` function to get more memory, and if that fails too, we return a `NULL` pointer as an error.

#### Freeing a Chunk

Freeing a chunk is quite simple. The `free` function takes a pointer to the start of the chunk’s data and subtracts the size of a header from it, resulting in the base address of the chunk’s header. Now, all there is left to do is to mark the chunk as free. We do that by setting the first bit of the size attribute to zero. Following the previous example, the heap would now look like this:

| Address   | Data Type | Size (bytes) | Values       |
| :-------: | :------:  | :----------: | :----------: |
| 800       | Padding   | 000          |              |
| 040       | Data      | 760          |              |
| 032       | Header    | 008          | s=768 p=32   |
| 021       | Padding   | 011          |              |
| 008       | Data      | 013          |              |
| 000       | Header    | 008          | s=32 p=0     |

> s = size | p = prev_size | (s % 8 == 1) = used | (s % 8 == 0) = free

We have only changed the `size` of the first chunk and the `prev_size` of the chunk that comes after it.

#### Merging Chunks

We now have 2 free chunks and we would like to merge them into a single one. After detecting 2 adjacent free chunks, we can merge them together by adding the size of the upper chunk into the size of the lower chunk. That's it—that’s all there is to it.

The heap would now look like this:

| Address   | Data Type | Size (bytes) | Values       |
| :-------: | :------:  | :----------: | :----------: |
| 800       | Padding   | 000          |              |
| 040       | Data      | 760          |              |
| 032       | Header    | 008          | s=768 p=32   |
| 021       | Padding   | 011          |              |
| 008       | Data      | 013          |              |
| 000       | Header    | 008          | s=800 p=0    |

> s = size | p = prev_size | (s % 8 == 1) = used | (s % 8 == 0) = free

But in reality, the first chunk will now override the second one, so we can just ignore it and view the heap as follows:

| Address   | Data Type | Size (bytes) | Values       |
| :-------: | :------:  | :----------: | :----------: |
| 021       | Padding   | 011          |              |
| 008       | Data      | 013          |              |
| 000       | Header    | 008          | s=800 p=0    |

> s = size | p = prev_size | (s % 8 == 1) = used | (s % 8 == 0) = free
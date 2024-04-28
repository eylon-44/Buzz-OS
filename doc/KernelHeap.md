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

`start` is the start address of the heap, `top` is the end of it, and `extension` is a pointer to a function that can allocate or deallocate memory for the heap as needed.

Each allocated block of data in the heap is called a **chunk**. A chunk is built out of three components:

#### Header

The header is where the meta data of the chunk is being stored. The header marks the start of the chunk and is located right underneath the [data](#data).

```c
typedef struct
{
    u32_t size;         // size & 0x1 = free/used
    u32_t prev_size;
} heap_header_t;
```

The header includes the size of its own chunk ([header](#header) + [data](#data) + [padding](#padding)), as well as the size of the chunk that comes right before of it—we will soon discuss the importance of this. As each chunk is 8 bytes aligned, the first bit in the `size` attribute is never used—at least not until now! We are going to use the first bit in `size` to indicate whether a page is free (0) or used (1). This is big, as we now achieved two significant things:

* Our header is exactly 8 bytes—we avoided the overhead of adding a 1 byte boolean `used`, and by that we don't need to add (16-9=) 7 bytes of padding to the header in order to keep it 8 bytes aligned.

* Remember that each header also keeps track of the size of the header that comes before of it. That means, that while looking over a single header we can determine if both this header and the header before of it are free, and if so, merge them together.

As all the chunks are stacked together, we can iterate over all the headers in the heap (all the chunks) using the logic described below:

* The first header is located at the `start` address of the heap, read it.

* Now that we have the address of the first chunk and we know its size, the next chunk is located at `[current_address] + [chunk_size]`.

* The last header in the heap is marked by the quality of `[current_adddress] + [chunk_size] = [heap_top]`

If we want to move backwards in the list, we can use the `prev_size` attribute to subtract it from the current chunk address until `[current_adddress] = [heap_start]`.

#### Data

This is where the allocated data is being stored.

#### Padding

As each chunk must be 8 bytes aligned we add padding at the end of the data. Padding size can be calculated by the following equation: `[padding] = 8 - [data_size] % 8`. Using this value, the data, and the header size, we can calculate the size of a chunk: `[chunk_size] = 8 + [data_size] + [padding]`.


### Allocation, Deallocation & Merging

The initial heap has a single free block in it with the size of the entire allocated heap. For the sake of explanation, let's assume that our heap starts at address 0 and ends at address 800—it will look like this:

```c
heap_t kernel_heap = { .start = 0, .top = 800 };
```

| Address   | Heap     | Size (bytes) | Values       |
| :-------: | :------: | :----------: | :----------: |
| 800       | Padding  | 000          |              |
| 008       | Data     | 792          |              |
| 000       | Header   | 008          | s=800 p=0    |

>s = size | p = prev_size | (s % 8 == 1) = used | (s % 8 == 0) = free

Notice that the first bit in `s=800` is off, meaning that this chunk is free. Also notice that the size of the previous chunk is `p=0`—this is because this chunk is the first chunk in the heap and there is no chunk before of it.

#### Allocating a Chunk
If we want to allocate a some data, say, 13 bytes, we will have to start a hunt! What are we hunting? The smallest free chunk that we can fit our data in.

We will iterate over our chunks using the method described earlier until finding a perfect-match chunk, or until reaching the end of the heap. In our current situation, we only have a single free chunk, so will have to use it. This chunk is way to big for our cute little 13 bytes allocation, so we will have to split it. The process of splitting a chunk goes as follows:

* Set the size of the splitted chunk with the new size, in our case it is `[header] + [data] + [padding] = 8 + 13 + 11 = 32`. Keep the previous size as it is.

* Create a new header at `[current_address] + [chunk_size]`, in our example it is at `0 + 32 = 32`.

* Set the size of the new header with the initial size of the splitted chunk, minus the size of the other chunk: `[size] = [initial_size] - [lower_chunk_size]`. If we substitute this expression with our current values we can get that the size of our upper new chunk, which is `800 - 32 = 768`

* Set the previous size attribute of the upper chunk to the size of the lower chunk. 

We now have 2 chunks in our heap, both are marked as free. We are going to mark the lower one of them as used by setting the first bit of its size attribute to 1. We will also return the user a pointer of the allocated data, which can be calculated by adding the address of the chunk with the size of the header.

| Address   | Heap     | Size (bytes) | Values       |
| :-------: | :------: | :----------: | :----------: |
| 800       | Padding  | 000          |              |
| 032       | Data     | 768          |              |
| 024       | Header   | 008          | s=776 p=25   |
| 021       | Padding  | 003          |              |
| 008       | Data     | 013          |              |
| 000       | Header   | 008          | s=25 p=0     |


>s = size | p = prev_size | (s % 8 == 1) = used | (s % 8 == 0) = free

A few things to pay attention to. First, notice that the size of the first chunk is set to `s=33`, while in reality it's 32 bytes. Recall that the first bit in the size attribute is reserved for the `used` flag, meaning that this chunk is being used, opposed to the free chunk above it where in it the first bit of `s` is off, meaning free. Second, `p=33` in the second chunk is now set with the size of the first chunk.

The user will get a pointer to address `008`.

If we can't find any free chunk we will use the heap `extension` function to get more memory, and if that fails too, return NULL as error.

#### Freeing a Chunk

Freeing a chunk is really easy. The `free` function gets a pointer to the start of the data section in a chunk, by subtracting the size of a header (8) from it we can find its header. And now, all there is left to do is mark the chunk as free. We do that by turning the first bit of the size attribute off. Using the previous example the heap will now look like this: 

| Address   | Heap     | Size (bytes) | Values       |
| :-------: | :------: | :----------: | :----------: |
| 800       | Padding  | 000          |              |
| 032       | Data     | 768          |              |
| 024       | Header   | 008          | s=776 p=24   |
| 021       | Padding  | 003          |              |
| 008       | Data     | 013          |              |
| 000       | Header   | 008          | s=24 p=0     |

>s = size | p = prev_size | (s % 8 == 1) = used | (s % 8 == 0) = free

The only visible change is in the `size` of the first chunk and in the `prev_size` of the next chunk.

#### Merging Chunks

We now have 2 free chunks and we would like to merge them into a single one. After detecting 2 adjacent free chunks, we can merge them together by adding the size of the upper chunk into the size of the lower chunk. That's it.

Memory will look like this:

| Address   | Heap     | Size (bytes) | Values       |
| :-------: | :------: | :----------: | :----------: |
| 800       | Padding  | 000          |              |
| 032       | Data     | 768          |              |
| 024       | Header   | 008          | s=776 p=24   |
| 800       | Padding  | 000          |              |
| 008       | Data     | 792          |              |
| 000       | Header   | 008          | s=800 p=0    |


>s = size | p = prev_size | (s % 8 == 1) = used | (s % 8 == 0) = free

But in reality the first chunk will now override the second one so we can ignore it and view the heap as follows:

| Address   | Heap     | Size (bytes) | Values       |
| :-------: | :------: | :----------: | :----------: |
| 800       | Padding  | 000          |              |
| 008       | Data     | 792          |              |
| 000       | Header   | 008          | s=800 p=0    |

>s = size | p = prev_size | (s % 8 == 1) = used | (s % 8 == 0) = free
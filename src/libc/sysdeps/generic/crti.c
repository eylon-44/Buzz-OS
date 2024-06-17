// sys/generic/crti.c // ~ eylon

extern void __libc_init_heap();

/* LIBC runtime initialization.
*/
void __libc_init()
{
    __libc_init_heap();
}
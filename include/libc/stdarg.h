// stdarg.h // ~ eylon

#if !defined(__LIBC_STDARG_H)
#define __LIBC_STDARG_H

typedef char* va_list;

/* void va_start(va_list ap, last_arg)
    Initialize the [ap] variable to be used with the [va_arg] and [va_end] macros.
    [last_arg] is the last fixed argument in the function (the argument before the ellipsis). */
#define va_start(ap, last_arg)  \
    ap = (char*) (&(last_arg) + 1)   // [ap] points to the first argument after [last_arg]   

/* type va_arg(va_list ap, type)
    Retrieve the next argument of type [type] from the argument list.
    Note that the macro's return value must be received by a variable and not to another
    another function as a parameter. */
#define va_arg(ap, type)        \
    *(type*) ap;                \
    ap += sizeof(type)

/* void va_end(va_list ap)
    Call before returning from the function. */
#define va_end(ap)              \
    ap = NULL


#endif
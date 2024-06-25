// stdio/vasprintf.c // ~ eylon

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define FORMATTER_CHAR '%'
#define BUFF_JUMP  512

// Write to [str] to buffer [buff] while allocating memory for it as needed.
static void write_to_buff(char** buff, size_t* buff_size, size_t* buff_len, char* str, size_t len)
{
    // Allocate more memory for the buffer if needed
    if (len + *buff_len > *buff_size) {
        *buff_size = ALIGN_UP(len+*buff_len, BUFF_JUMP);
        *buff = realloc(*buff, *buff_size);
    }

    // Write [str] into the buffer
    memcpy(*buff+*buff_len, str, len);
    *buff_len += len;
}

/* https://linux.die.net/man/3/vasprintf */
int vasprintf(char **strp, const char *fmt, va_list ap)
{
    const char* c  = fmt;                   // character pointer
    size_t len     = 0;                     // formatted string length
    size_t size    = 0;                     // formatted string buffer size 

    *strp     = NULL;                       // formatted string buffer initiated with NULL

    // Iterate over the given string
    while (*c != '\0')
    {
        // If the character is a formatter, insert the argument after it
        if (*c == FORMATTER_CHAR) {
            c++;
            switch (*c) {
                // Number formatter
                case 'd': {
                    int num = va_arg(ap, int);
                    char str[16];
                    
                    itoa(num, str);
                    write_to_buff(strp, &size, &len, str, strlen(str));
                    break;
                }
                // String formatter
                case 's': {
                    char* str = va_arg(ap, char*);

                    write_to_buff(strp, &size, &len, str, strlen(str));
                    break;
                }
                // Character formatter
                case 'c': {
                    char str = va_arg(ap, int);

                    write_to_buff(strp, &size, &len, &str, 1);
                    break;
                }
                // No formatter
                default: {
                    write_to_buff(strp, &size, &len, (char*) c, 1);
                    break;
                }
            }
        }
        // Else, insert the character
        else {
            write_to_buff(strp, &size, &len, (char*) c, 1);
        }
        c++;
    }
    // Insert null terminator
    write_to_buff(strp, &size, &len, "", 1);

    // Optimize buffer size
    *strp = realloc(*strp, len);

    return len-1;
}
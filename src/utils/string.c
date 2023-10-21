// String Utils // ~ eylon

#include <utils/string.h>
#include <utils/type.h>

// String length :: get the length of a null terminated string
u16_t strlen(char* str)
{
    u16_t i = 0;
    // iterate until null terminator
    while (str[i] != '\0') {
        i++;
    }
    return i;
}
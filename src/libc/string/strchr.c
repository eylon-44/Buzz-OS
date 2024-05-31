// string/strchr.c // ~ eylon

#include <string.h>

/*  Locate a given character in a string.

    The strchr() function returns a pointer to the first occurrence of
    the character [c] in the string [s] or NULL if the character is not found.

    #include <string.h>
    char* strchr(const char* str, int c);
*/
char* strchr(const char *str, int c)
{
    while (*str != '\0') {
        if (*str == (char) c) {
            return (char*) str;
        }
        str++;
    }

    if ((char) c == '\0') {
        return (char*) str;
    }

    return NULL;
}
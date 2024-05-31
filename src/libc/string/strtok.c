// string/strtok.c // ~ eylon

#include <string.h>
#include <stddef.h>

/*  Split a string by a delimiter.
       
    The strtok() function breaks a string into a sequence of zero or
    more nonempty tokens. On the first call to strtok(), the string
    to be parsed should be specified in [str]. In each subsequent call
    that should parse the same string, [str] must be NULL. The [delim]
    argument specifies a set of bytes that delimit the tokens in the
    parsed string. The caller may specify different strings in [delim]
    in successive calls that parse the same string.

    A sequence of calls to strtok() that operate on the same string
    maintains a pointer that determines the point from which to start
    searching for the next token. The first call to strtok() sets this
    pointer to point to the first byte of the string. The start of the
    next token is determined by scanning forward for the next nondelimiter
    byte in [str]. If such byte is found, it is taken as the start of
    the next token. If no such byte is found, then there are no more
    tokens, and strtok() returns NULL.

    The end of each token is found by scanning forward until either
    the next delimiter byte is found or until the terminating null
    byte ('\0') is encountered. If a delimiter byte is found, it is
    overwritten with a null byte to terminate the current token, and
    strtok() saves a pointer to the following byte; that pointer will
    be used as the starting point when searching for the next token.
    In this case, strtok() returns a pointer to the start of the
    found token.
    
    Each call to strtok() returns a pointer to a null-terminated string
    containing the next token. This string does not include the delimiting
    byte. If no more tokens are found, strtok() returns NULL.

    #include <string.h>
    char* strcpy(char* dest, const char* src);
*/
char* strtok(char* str, const char* delim)
{
    // Static variable to keep track of the current position in the string
    static char* last = NULL;
    char* token_start;

    // If a new string is passed, initialize the last pointer
    if (str != NULL) {
        last = str;
    }
    // If the last pointer is NULL, return NULL
    if (last == NULL) {
        return NULL;
    }
    
    // Skip leading delimiters
    while (*last != '\0' && strchr(delim, *last)) {
        last++;
    }
    // If we reached the end of the string, return NULL
    if (*last == '\0') {
        return NULL;
    }
    
    // Set the start of the token
    token_start = last;
    
    // Find the end of the token
    while (*last != '\0' && !strchr(delim, *last)) {
        last++;
    }
    
    // If we reached the end of the string, set last to NULL
    if (*last == '\0') {
        last = NULL;
    } else {
        *last = '\0';       // null-terminate the token
        last++;             // move to the next character after the null terminator
    }
    
    return token_start;
}
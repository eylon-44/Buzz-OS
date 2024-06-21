// string/basename.c // ~ eylon

#include <string.h>
#include <limits.h>

/*  Extract file name out of path.

    The basename() function extracts the file name out of a null-terminated
    [path] string.
    
    The basename() function returns the part of the string following the last
    '/' in it. If the path does not contain a slash or is equal to "/", the
    function returns a copy of the path. If the path is a NULL pointer or
    points to an empty string, the string "." shall be returned.

    DO NOT FREE THE RETURNED POINTERS; The function return a pointer to a
    statically allocated buffer. This buffer may be overwritten by subsequent calls.
       
    #include <string.h>
    char* basename(char* path);
*/
char* basename(const char* path) {
    static char buff[FNAME_LEN_MAX];
    char* last_slash;

    // If path is NULL or empty, return "."
    if (path == NULL || *path == '\0') {
        strcpy(buff, ".");
        return buff;
    }

    // Copy the path into the buffer
    strcpy(buff, path);

    // Find the last slash in the string
    last_slash = strrchr(buff, '/');

    // If the path does not contain a slash or is equal to "/", return it as is
    if (last_slash == NULL || strcmp(buff, "/") == 0) {
        return buff;
    }

    /* This code executes only if the path contains a slash and is not equal to "/" */

    // If the slash is a trailing slash, find the previous slash
    if (*(last_slash+1) == '\0')
    {
        // Remove the trailing slash
        while (*last_slash == '/' && last_slash > buff) {
            *last_slash = '\0';
            last_slash--;
        }
        // Find the new last slash, and if it exists, return the part that comes after it
        last_slash = strrchr(buff, '/');
        if (last_slash != NULL) {
            return last_slash + 1;
        }
    }
    // Return the part the comes after the last slash
    else {
        return last_slash + 1;
    }

    // Return the buffer
    return buff;
}
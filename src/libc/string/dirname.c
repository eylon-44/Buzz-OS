// string/dirname.c // ~ eylon

#include <string.h>
#include <limits.h>

/*  Extract directory name out of path.

    The dirname() function extracts the directory name out of a null-terminated
    [path] string.
    
    The dirname() function returns the part of the string that comes before of
    the last '/' in it. If the path does not contain a slash or is equal to "/", the
    function returns a copy of the path. If the path is a NULL pointer or points to
    an empty string, the string "." shall be returned.

    DO NOT FREE THE RETURNED POINTERS; The function return a pointer to a
    statically allocated buffer. This buffer may be overwritten by subsequent calls.
       
    #include <string.h>
    char* dirname(const char* path);
*/
char* dirname(const char* path)
{
    static char buff[PATH_MAX];
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

    // If the only slash is at the start of the string
    if (last_slash == buff) {
        return "/";
    }

    /* This code executes only if the path contains a slash and is not equal to "/" */
    
    // If the slash is a trailing slash, find the previous one in the string
    if (*(last_slash+1) == '\0')
    {
        // Remove the trailing slash
        while (*last_slash == '/' && last_slash > buff) {
            *last_slash = '\0';
            last_slash--;
        }
        // Find the new last slash, and if it exists, set it to null
        last_slash = strrchr(buff, '/');
        if (last_slash != NULL) {
            *last_slash = '\0';
        }
    }
    // Else, set the slash as the null-terminator
    else {
        *last_slash = '\0';
    }

    // Return the buffer
    return buff;
}
// Terminal's Input Handler // ~ eylon

#include <input.h>

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* Get argv and argc from user input. The function takes [argv], which is an array of strings, fill
    it up with the user input terminated with a NULL pointer and returns the number of strings in [argv].
    [argv] will hold no more than ARGC_MAX-1 values; if the number of arguments exceed ARGC_MAX-1, then
    the argument the comes before of the NULL pointer (argv[ARGC_MAX-2]) will contain the rest of the buffer
    as is. With end of use, [argv] should be passed to the free_input() function. */
int get_input(char* argv[ARGC_MAX])
{
    int argc = 0;
    char buff[512];
    char* input = NULL;
    size_t inputlen = 1;
    
    // Clear the input buffer
    stdin_flush();

    // Read user input and add it to the input buffer until encountering a '\n' character
    do
    {
        size_t bufflen;

        fgets(buff, sizeof(buff), stdin);                   // read user input into [buff]
        bufflen = strlen(buff);                             // get the length of the read input

        input = (char*) realloc(input, inputlen + bufflen); // allocate memory for the new input in the input buffer
        strcpy(input + inputlen - 1, buff);                 // add the new input into the input buffer; strcpy puts a null terminator at the end
        inputlen += bufflen;                                // increase the input buffer's size with the new input size
    }
    while (input[inputlen-2] != '\n');

    // Remove the new-line character
    input[inputlen-2] = '\0';

    // Split the input into [argv]
    char* token = strtok(input, " ");
    while (token != NULL)
    {
        argv[argc] = (char*) malloc(strlen(token)+1);
        strcpy(argv[argc], token);
        argc++;

        token = strtok(NULL, argc < ARGC_MAX-2 ? " " : "");
    }

    // Terminate the list with a NULL pointer
    argv[argc] = NULL;

    free(input);
    return argc;
}

// Free an [argv] array created by the get_input() function
void free_input(char* argv[ARGC_MAX])
{
    for (int i = 0; i < ARGC_MAX; i++)
    {
        if (argv[i] == NULL) break;
        free(argv[i]);
    }
}
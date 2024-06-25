// Input Handler // ~ eylon

#include <input.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Get user input. The function inputs and the user returns a null-terminated
    string of the result. With end of use, the string should be passed to free(). */
char* get_input()
{
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

    // Remove the new-line character and return a pointer to the input
    input[inputlen-2] = '\0';
    return input;
}

// Block until enter is pressed
void block_enter()
{
    stdin_flush();
    while ((char) fgetc(stdin) != '\n');
}
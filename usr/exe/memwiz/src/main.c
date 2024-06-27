// Memory Game // ~ eylon

#include <input.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define STARTING_COUNT 1
#define STARTING_RATE  7

// Game structure
struct
{
    int points;         // points; round number
    int count;          // amount of numbers to remember 
    int rate;           // amount of numbers displayed per TEN second
    char* numbers;      // string of the random numbers, that is, the correct user input
    bool running;       // is the game running?
} game;

// Take the user input and exit the program if it equals to "exit"
static void handle_input_exit(char* str)
{
    if (strcmp(str, "exit") == 0) {
        printf("\f");
        exit(0);
    }
}

/* Short term number memorization game. */
int main()
{
    char* input;

    // Initiate random
    srand(militime());

    // Main game loop
    for (;;)
    {
        /* Rules. */
        printf("\f");           // clear the screen
        printf(" *** Welcome to Memwiz ***\n");
        printf(" - How to play:\n");
        printf("  * At each round, a list of single digit numbers will apear one after another\n");
        printf("  * The rate and amount of numbers displayed at each round varies\n");
        printf("  * The higher the rate and number count, the more difficult the game becomes\n");
        printf("  * Your goal is to recall and enter the numbers displayed in order\n");
        printf("  * The game continues until \"exit\" is inputed\n");
        printf(" - To start, press Enter.\n");
        input = get_input();        // block until Enter
        handle_input_exit(input);   // handle exit input
        printf("\f");               // clear the screen

        // Sub game loop
        game.running = true;
        game.points  = 0;
        game.count   = STARTING_COUNT;
        game.rate    = STARTING_RATE;
        while (game.running)
        {
            // Allocate memory for the random numbers list
            game.numbers = (char*) malloc(game.count + 1);
            game.numbers[game.count] = '\0';

            // Generate and print random numbers to be rememberd
            for (int i = 0; i < game.count; i++) {
                char rnum = (rand() % 10) + 48;                 // generate a random number and convert it into a character
                game.numbers[i] = rnum;                         // save the random number in the numbers list
                for (int k = 0; k < i; k++) { printf(" "); }    // Offset the random number
                printf("%c", rnum);                             // print the random number
                milisleep(10000/game.rate);                     // sleep 10/rate seconds
                printf("\f");                                   // clear the screen
            }

            // Get and proccess user input
            printf(" - Recall: ");
            char* input = get_input();
            handle_input_exit(input);

            // If input is correct, increase difficulty and points
            if (strcmp(input, game.numbers) == 0) {
                printf(" Correct!\n");
                game.points++;
                game.count++;
                game.rate++;
            }
            // If input is incorrect, restart the game
            else {
                printf(" Incorrect!\n");
                game.running = false;
            }

            printf("Points: %d\n", game.points);
            printf(" * Correct sequence: %s\n", game.numbers);
            printf(" * You inputed:      %s\n", input);

            free(input);
            free(game.numbers);

            printf("Press Enter to continue.\n");
            input = get_input();    // block
            handle_input_exit(input);
            printf("\f");
        }
    }
}
// Memory Game // ~ eylon

#include <input.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

// Game structure
struct
{
    int points;         // points; round number
    int count;          // amount of numbers to remember 
    int rate;           // amount of numbers displayed per TEN second
    char* numbers;      // string of the random numbers, that is, the correct user input
    bool running;       // is the game running?
} game = {
    .points=0, .count=1, .rate=5, .numbers=NULL, .running=true
};

/* Short term number memorization game. */
int main()
{
    /* Rules. */
    printf("\f");           // clear the screen
    printf(" *** Welcome to the Memory Game ***\n");
    printf(" - How to play:\n");
    printf("  * At each round, a list of single digit numbers will apear one after another\n");
    printf("  * The rate and amount of numbers displayed at each round varies\n");
    printf("  * The higher the rate and number count, the more difficult the game becomes\n");
    printf("  * Your goal is to recall and enter the numbers displayed in order\n");
    printf("  * The game continues until a wrong or \"exit\" input is received\n");
    printf(" - To start, press Enter.\n");
    block_enter();   // block until Enter
    printf("\f");           // clear the screen

    // Initiate random
    srand(militime());

    // Game loop
    while(game.running)
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
            milisleep(10000/game.rate);                     // sleep 1/rate seconds
            printf("\f");                                   // clear the screen
        }

        // Get and proccess user input
        printf(" - Recall: ");
        char* input = get_input();
        
        // Exit command
        if (strcmp(input, "exit") == 0) {
            printf("\f");   // clear the screen
            exit(0);
        }

        // If input is correct, increase difficulty and points
        if (strcmp(input, game.numbers) == 0) {
            printf(" Correct!\n");
            game.points++;
            game.count++;
            game.rate++;
        }
        // If input is incorrect, stop the game
        else {
            printf(" Incorrect!\n");
            game.running = false;
        }

        printf("Points: %d\n", game.points);
        printf(" * Correct sequence: %s\n", game.numbers);
        printf(" * You inputed:      %s\n", input);

        free(input);
        free(game.numbers);

        printf("Press Enter to continue.");
        block_enter();          // block until Enter
        printf("\f");           // clear the screen
    }
}
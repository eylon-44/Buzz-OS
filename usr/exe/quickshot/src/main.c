// Quickshot Game // ~ eylon

#include <input.h>
#include <graphics.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>

#define SHOTS_PER_ROUND     10
#define REACT_TIME_MIN      100     // minimum react time in miliseconds
#define WAIT_TIME_MIN       50      // minimum wait time in miliseconds
#define WAIT_TIME_MAX       6000    // maximum wait time in miliseconds
#define SHOT_DISPLAY_TIME   200     // time in miliseconds the shot is displayed on the screen

// Game structure
struct
{
    size_t best_time;
    size_t best_avrg_time;
    size_t react_time[SHOTS_PER_ROUND];      // string of the random numbers, that is, the correct user input
    bool running;       // is the game running?
} game = {
    .best_time=UINT_MAX, .best_avrg_time=UINT_MAX, .running=true
};

// Check if user inputed "exit", and if so quit
static void handle_input_exit(char* str) {
    if (strcmp(str, "exit") == 0) {
        printf("\f");
        exit(0);
    }
}

/* Fast reflex game. */
int main()
{
    // Initiate random
    srand(militime());

    // Main game loop
    while(game.running)
    {
        char* input;                    // user input
        size_t best_time = UINT_MAX;    // best reaction time this round
        size_t avrg_t;                  // average reaction time
        // Buffers to hold number strings
        char num1_str[16];
        char num2_str[16];
        
        /* Rules. */
        printf("\f");           // clear the screen
        printf(" *** Welcome to Quickshot ***\n");
        printf(" - How to play:\n");
        printf("  * In each round, %d enemies will appear one after another at random times\n", SHOTS_PER_ROUND);
        printf("  * Your job is to shoot them as fast as you can by pressing Enter\n");
        printf("  * The game continues until \"exit\" is inputed\n");
        printf(" - To start, press Enter.\n");
        input = get_input();    // block until Enter
        handle_input_exit(input);
        printf("\f");           // clear the screen

        // Take reaction time of [SHOTS_PER_ROUND] shots 
        for (int i = 0; i < SHOTS_PER_ROUND; i++) {
            size_t wait_t, enemy_t;

            // Calculate a random time to sleep between WAIT_TIME_MIN and WAIT_TIME_MAX miliseconds and sleep it
            wait_t = (rand() % (WAIT_TIME_MAX - WAIT_TIME_MIN + 1)) + WAIT_TIME_MIN;
            milisleep(wait_t);

            // Print a random enemy, record the time, and wait for the user to hit Enter
            printf("\f%s", enemies_str[rand() % (sizeof(enemies_str)/sizeof(enemies_str[0]))]);
            enemy_t = militime();

            // Block and wait for user reaction, then calculate reaction time
            do
            {
                input = get_input();
                game.react_time[i] = militime() - enemy_t;
            } while(game.react_time[i] < REACT_TIME_MIN);
            
            handle_input_exit(input);

            // Update stats
            avrg_t += game.react_time[i];
            if (game.react_time[i] < game.best_time) game.best_time = game.react_time[i];
            if (game.react_time[i] < best_time) best_time = game.react_time[i];

            // Show shooting animation
            lseek(stdout, 0, 0);
            printf("%s", shot_str);
            milisleep(SHOT_DISPLAY_TIME);
            printf("\f");
        }

        // Update stats
        avrg_t /= SHOTS_PER_ROUND;
        if (avrg_t < game.best_avrg_time) game.best_avrg_time = avrg_t;

        // Display results
        printf(" - Results:\n");
        printf(" SHOT    REACTION TIMEms\n");
        for (int i =0; i < SHOTS_PER_ROUND; i++)
        {
            itoapad(i+1, num1_str, 2);
            itoapad(game.react_time[i], num2_str, 6);

            printf(" %s      %s\n", num1_str, num2_str);
        }
        itoapad(best_time, num1_str, 6);
        itoapad(game.best_time, num2_str, 6);
        printf("                CURRENT ROUND    RECORD\n");
        printf(" BEST TIMEms    %s           %s\n", num1_str, num2_str);
        itoapad(avrg_t, num1_str, 6);
        itoapad(game.best_avrg_time, num2_str, 6);
        printf(" AVERAGE TIMEms %s           %s\n\n", num1_str, num2_str);


        printf("Press Enter to continue.\n");
        input = get_input();    // block
        handle_input_exit(input);
        printf("\f");           // clear the screen
        free(input);
    }
}
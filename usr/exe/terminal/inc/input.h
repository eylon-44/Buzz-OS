// Terminal's Input Handler Header File // ~ eylon

#if !defined(__INPUT_H)
#define __INPUT_H

#include <limits.h>

int get_input(char* argv[ARGC_MAX]);
void free_input(char* argv[ARGC_MAX]);

#endif
// stdlib/itoa.c // ~ eylon

#include <stdlib.h>

/* Convert an integer into a null terminated string.

    The itoa() function converts integer [n] into a string. The string
    buffer [s] should be 16 bytes long.

    #include <stdlib.h>
    void itoa(int n, char s[16]);
*/
void itoa(int n, char s[16]) {
    int sign, i = 0;

    // If [n] is negative, makr it as so and make it positive
    if ((sign = n) < 0) {
        n = -n;
    }

    // Generate digits in reverse order
    do {
        s[i] = n % 10 + '0';
        i++;
    } while ((n /= 10) > 0);

    // Add the sign if [n] was negative
    if (sign < 0)
        s[i++] = '-';

    // Terminate the string
    s[i] = '\0';

    // Reverse the string
    for (int j = 0, k = i-1; j < k; j++, k--) {
        char tmp;
        tmp = s[j];
        s[j] = s[k];
        s[k] = tmp;
    }
}
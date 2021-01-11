/* str2's value overflow to str1
Compiled by "gcc buffer_overflow_1.c"

case 1:
EVILINPUTVALUE
buffer: str1(TVALUE), str2(EVILINPUTVALUE), valid(0)

case 2:
BADINPUTBADINPUT
buffer: str1(BADINPUT), str2(BADINPUTBADINPUT), valid(1)
*/

#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int valid = 0;
    char str1[8];
    char str2[8];
    // gets() function does not include any checking on the amount of data copied.
    gets(str2);
    if (strncmp(str1, str2, 8) == 0)
        valid = 1;

    printf("buffer: str1(%s), str2(%s), valid(%d)\n", str1, str2, valid);
    return 0;
}

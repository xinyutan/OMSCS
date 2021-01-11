/* buffer overflow can contaminate the return address.
In the following example, if the input is much longer than
16, then it overflows to the return address to the main function.
When it is about to return, the program pointer will points to 
illegal address, and will have a segmentation fault.
*/
#include <stdio.h>

void hello(char *tag) {
    char input[16];
    printf("Enter value for %s\n", tag);
    gets(input);
    printf("Hello your %s is %s\n", tag, input);
}

int main() {
    hello("name");
    return 0;
}
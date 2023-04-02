#include <inttypes.h>
#include <stdio.h>

int main()
{
    int a = 5;
    int b = 6;
    int c = 7;

    int x = a ^ b & c;

    return x;
}
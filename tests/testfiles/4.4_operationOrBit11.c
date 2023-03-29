#include <inttypes.h>
#include <stdio.h>

int main()
{
    int a = 5;
    int b = 6;
    int p = 7 | a | b;

    return p;
}
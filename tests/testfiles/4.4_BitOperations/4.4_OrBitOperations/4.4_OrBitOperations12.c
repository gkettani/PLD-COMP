#include <inttypes.h>
#include <stdio.h>

int main()
{
    int a = 5;
    int b = 6;
    int x = a | b;
    int o = a | b | x;

    return o;
}
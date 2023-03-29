#include <inttypes.h>
#include <stdio.h>

int main()
{
    int a = 5;
    int b = 6;
    int x = a | b;
    int y = 15 | b;
    int z = a | 4;
    int w = 1 | 4;

    int v = 5 | 6 | 7;
    int u = a | 6 | 7;
    int t = 8 | b | 9;
    int s = 8 | 6 | b;
    int r = a | b | 7;
    int q = a | 7 | b;
    int p = 7 | a | b;
    int o = a | b | x;

    return o;
}
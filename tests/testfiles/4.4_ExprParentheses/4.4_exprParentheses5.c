#include <inttypes.h>
int main()
{
    int a = 5;
    int b = 6;
    int c = 7;
    int z = (a | b & c);

    return z;
}
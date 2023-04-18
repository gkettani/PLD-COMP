#include <inttypes.h>
int main()
{
    int a = 1;
    int b = 2;
    int d = 5;
    int c = (a != b) != (5 != d);
    return c;
}
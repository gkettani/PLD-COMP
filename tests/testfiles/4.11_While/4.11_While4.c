int main()
{
    int a = 1;
    int b = 1;
    int c = 0;

    while (a < 3)
    {
        a = 100;
        while (b < 2)
        {
            b = b + 1;
            a = a + 1;
        }
        a = a + 1;
    }

    return a;
}
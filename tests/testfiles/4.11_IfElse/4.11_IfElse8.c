int main()
{
    int a = 4;
    int b = 1;

    if (a > 5)
    {
        if (b == 0)
        {
            b = 1;
        }
        else
        {
            b = 3;
        }
    }
    else
    {
        if (b == 0)
        {
            b = 2;
        }
        else
        {
            b = 4;
        }
    }

    return b;
}
int main()
{
    int a = 6;
    int b = 0;

    if (a > 5)
    {
        if(b == 0){
            b = 1;
        }
    }
    else
    {
        if (b == 0) {
            b = 2;
        }
    }

    return b;
}
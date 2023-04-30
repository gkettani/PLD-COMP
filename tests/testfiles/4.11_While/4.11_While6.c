int main()
{
    int a = 1;
    int b = 0;

    while(a<3){
        if(a<2){
            b = b + 1;
        }else{
            b = 100;
        }
        a = a + 1;
    }

    return b;
}
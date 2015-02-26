void func()
{
    static int i=0;
    int arr[1024]={0};
    printf("%ld KB pushed on stack!\n",++i*sizeof(int));
    func();
}

int main()
{
    func();
    return 0;
}

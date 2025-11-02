#include "../io/iofn.c"
#include "../io/dmafn.c"

int main() {
    int *x = malloc(sizeof(int));
    
    if (x == 0) {
        print("malloc returned 0!\n");
        return 1;
    }
    
    print("malloc succeeded\n");
    
    *x = 42;
    print_int(*x);
    free(x);
    return 0;
}
#include <clist.h>

enum consts {
    J = (unsigned)1000
};

int main(int argc, char* argv[])
{
    unsigned long index = 1;
    unsigned long div = index / J;
    printf("%d\n", div);
    List* l = new_list();
    list_add(l, (void*)0);
    list_add(l, (void*)1);
    printf("%p\n", list_get(l, 1));
    free_list(l);
}
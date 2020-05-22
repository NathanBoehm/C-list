#include <clist.h>

enum consts {
    J = (unsigned)1000
};

int main(int argc, char* argv[])
{
    List* l = new_list();
    size_t i = 0;
    for (; i < 10000; i++) {
        list_add(l, (void*)i);
    }
    free_list(l);
}
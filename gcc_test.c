#include <stdio.h>

#define LIST_DATA_TYPE int
#define ERROR_RETURN_VALUE -1
#include <clist.h>

int main(int argc, char* argv[])
{
    List* l = new_list();

    //list_add(l, 3);
    list_add(l, 2);
    list_add(l, 1);
    list_add(l, 0);

    sort_list(l);

    free_list(l);
}
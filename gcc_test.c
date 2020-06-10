#include <stdio.h>

#define LIST_DATA_TYPE int
#define ERROR_RETURN_VALUE -1
#include <clist.h>

int main(int argc, char* argv[])
{
    List* l = new_list();
    long i = 3;
    for (; i >= 0; --i)
    {
        list_add(l, i);
        printf("i: %d\n", i);
        //TEST_CHECK(l->tail->value == i);
    }

    sort_list(l);

    free_list(l);
}
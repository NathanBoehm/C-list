#include <stdio.h>

#define LIST_DATA_TYPE int
#define ERROR_RETURN_VALUE -1
#include <clist.h>

int main(int argc, char* argv[])
{
    List* l = new_list();
    size_t i = 9999;
    for (; i >= 0; i--)
        list_insert(l, 0, i);

    printf("after insert\n");

    for (i = 0; i < 10000; i++)
    {
        int a = list_get(l, i);
    }
    printf("head value: %d\n", l->head->value);

    for (i = 0; i < 10; i++)
        printf("jump_table[%d] = %d\n", i, l->jump_table[i]->value);

    free_list(l);
}
#include <clist.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    List* l = new_list();
    size_t i = 0, j = 0;
    for (; i < 10001; i++)
    {
        list_add(l, (void*)i);
    }
    for (i = 0; i < 10001; i++)
    {
        size_t index = rand() % (10000 - i);
        size_t init_value = (size_t)list_get(l, index);
        list_remove(l, index);
        size_t new_value = (size_t)list_get(l, index);
        if (new_value <= init_value) return -1;
    }
    /*for (i = 0; i < 11; i++)
        printf("jump_table[%d]: %lu\n", i, (size_t)l->_jump_table[i]->value);

    for (j = 10; j > 1; j--)
    {
        for (i = 0; i < 1000; i++)
            list_pop(l);
        printf("l->size after 1000 pops: %d\n", l->size);f
        printf("jump_table[%d]: %lu\n", j, l->_jump_table[j]->value);
        printf("jump_table[%d-1]: %lu\n", j, l->_jump_table[j-1]->value);
    }
    for (i = 0; i < 999; i++)
        list_pop(l);
    printf("%d\n", l->_jump_table[1] == NULL);
    printf("l-size: %d\n", l->size);
    list_pop(l);
    printf("%d\n", l->_jump_table[0]);
    printf("%d\n", l->size == 0);*/
    free_list(l);
}
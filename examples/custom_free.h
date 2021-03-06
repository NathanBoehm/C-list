//////////////////////////////////////////////////////////////////////////////
//
// custom_free.h
// Example of dealing with freeing a list of lists / multiple list value
// types.
//
// Created by Nathan Boehm, 2020.  
//
//////////////////////////////////////////////////////////////////////////////


#include <stdlib.h>


struct list;

typedef struct data
{
    int is_list;
    int is_int;

    union
    {
        struct list* l;
        int i;
    };
} data_t;

data_t Data_t(struct list* l, int i)
{
    if (l != NULL)
    {
        data_t d = {1, 0, {.l = l}};
        return d;
    }
    else
    {
        data_t d = {0, 1, {.i = i}};
        return d;
    }
}

#define LIST_DATA_TYPE data_t
#define ERROR_RETURN_VALUE Data_t(NULL, 0)
int dummy(data_t a, data_t b) {return 1;}
#define LIST_COMPARATOR dummy

#include "../include/clist.h"



void custom_list_free(List* parent_list)
{
    _ListNode* current_node = parent_list->head;
    list_index_t i = 0;
    for (; i < parent_list->size; i++)
    {
        if (current_node->value.is_list)
            custom_list_free(current_node->value.l);
        //no special actions to take if current_node is an int.  
        //else if (current_node->value...)
            //other type cases here.  
        _ListNode* old_node = current_node;
        if (current_node->next != NULL)
            current_node = current_node->next;
        _free_list_node(old_node);
    }
    free(parent_list->jump_table);
    free(parent_list);
}
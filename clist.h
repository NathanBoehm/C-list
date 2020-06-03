//////////////////////////////////////////////////////////////////////////////
//
// clist.h
// Defines a header only, sortable list structure with 
// constant element access time and constant add() time.  
// API includes standard list operations, get(), insert(), remove(), add(),
// pop(), etc.  
//
// Created by Nathan Boehm, 2020.  
//
//////////////////////////////////////////////////////////////////////////////


#ifndef CLIST_H
#define CLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum Constants
{
    JT_INCREMENT = (int)1000,
    INITIAL_JT_SIZE = (unsigned)10
};

#ifndef LIST_DATA_TYPE
#define LIST_DATA_TYPE void*
#endif

/*
Default sorting function, items are sorted smallest first. If a < b, then
it will come earlier in the list.  
*/
int _default_less_than(LIST_DATA_TYPE a, LIST_DATA_TYPE b) { return a < b; }
#define LEFT_BEFORE_RIGHT _default_less_than

//convience option to have list items freed with the list.  
#ifndef FREE_LIST_ITEMS
#define FREE_LIST_ITEMS 0
#endif

/*
Default error handling callback function, if one is not defined.  
Attempts to print an error message to stderr.  
A user defined handler must have the same signature as the below function.  
*/
int _default_error_handler(char* func, char* arg, char* msg)
{
    fprintf(stderr, "list error:\nin function: %s\nargument(s): %s\n%s",
            func, arg, msg);
    return -1;
}

#ifndef ERROR_HANDLER
#define ERROR_HANDLER _default_error_handler
#endif

typedef struct _list_node
{
    LIST_DATA_TYPE value;
    struct _list_node* next;
    struct _list_node* prev;
} _ListNode;

/*
Linked list structure. Do not modify internal contents.  
*/
typedef struct list
{
    unsigned long size;
    _ListNode*    head;
    _ListNode*    tail;
    _ListNode**   jump_table;
    unsigned long jt_size;
} List;

_ListNode* new_list_node(LIST_DATA_TYPE value)
{
    _ListNode* new_le = (_ListNode*)calloc(1, sizeof(_ListNode));
    new_le->value = value;
    return new_le;
}

typedef unsigned long list_index_t;
typedef int (*filter_function) (LIST_DATA_TYPE);

//API functions
List*          new_list(void);
void           free_list(List*);
void           list_add(List*, LIST_DATA_TYPE);
void           list_remove(List*, list_index_t);
void           list_insert(List*, list_index_t);
void           sort_list(List* l);
list_index_t   list_size(List*);
LIST_DATA_TYPE list_pop(List*);
LIST_DATA_TYPE list_get(List*, list_index_t);

//Internal functions
_ListNode* _list_pointer_at(List*, list_index_t);
void _free_list_node(_ListNode*);
void _list_add_jump_table_node(List*, _ListNode*);
void _list_remove(List*, _ListNode*, list_index_t);
void _list_adjust_jump_table_up(List*, list_index_t);
void _list_adjust_jump_table_down(List*, list_index_t);
void _merge_sort_list(List*, _ListNode*);


/*
Returns a newly allocated list on success or NULL if memory allocation failed.  
User must free with free_list if the value returned is not NULL.  
Does not call the ERROR_HANDLER function.  
*/
List*
new_list(void)
{
    //Allocate list struct.  
    List* l = (List*)calloc(1, sizeof(List));
    if (!l) return NULL;

    //Allocate jump table.  
    l->jump_table = (_ListNode**)\
    calloc(INITIAL_JT_SIZE, sizeof(_ListNode*));
    if (!l->jump_table)
    {
        free(l);
        return NULL;
    }
    l->jt_size = INITIAL_JT_SIZE;

    return l;
}

/*
Frees memory associated with the given list.  
*/
void
free_list(List* l)
{
    _ListNode* current = l->head;
    list_index_t i;
    for (i = 0; i < list_size(l); i++) 
    {
        #if FREE_LIST_ITEMS
        free(current->value)
        #endif

        _ListNode* next = current->next;
        _free_list_node(current);
        current = next;
    }

    free(l->jump_table);
    l->jump_table = NULL;
    l->head = NULL;
    l->tail = NULL;
    free(l);
}

list_index_t
list_size(List* l)
{
    return l->size;
}

/*
Frees memory associated with the given _ListNode*.  
*/
void
_free_list_node(_ListNode* le)
{
    le->next = NULL;
    le->prev = NULL;
    free(le);
}

/*
Returns the value at the given index, if the index is valid.  
Otherwise, calls ERROR_HANDLER and returns NULL.  
*/
LIST_DATA_TYPE
list_get(List* l, list_index_t index)
{
    _ListNode* ptr = _list_pointer_at(l, index);
    return ptr ? ptr->value : (LIST_DATA_TYPE)NULL;
}

/*
Internal function that returns a struct _list_node* at the given index,
if the index is valid.  Otherwise, calls ERROR_HANDLER and returns NULL.  
*/
_ListNode*
_list_pointer_at(List* l, list_index_t index)
{
    //Start at the closest multiple of JT_INCREMENT,
    //then iterate to reach the desired index.
    if (index < list_size(l)) 
    {
        if (index == list_size(l) - 1)
            return l->tail;

        list_index_t jump_location = index / JT_INCREMENT;
        _ListNode* start = l->jump_table[jump_location];
        list_index_t distance_to_destination = index % JT_INCREMENT;

        _ListNode* destination = start;
        list_index_t i;
        for (i = 0; i < distance_to_destination; i++)
            destination = destination->next;
        return destination;
    }
    //Error, index out of range.  
    else
    {
        char arg_as_string[20];
        sprintf(arg_as_string, "(%ld)", index);
        ERROR_HANDLER("list_at()", arg_as_string, "Index out of bounds!\n");
        return NULL;
    }
}

/*
Adds the given value to the given list.  
Calls ERROR_HANDLER if there is a memory allocation error,
User must free list on a memory allocation error in list_add().  
*/
void
list_add(List* l, LIST_DATA_TYPE value)
{
    _ListNode* le = new_list_node(value);
    if (list_size(l) == 0)
        l->head = le;
    else
    {
        l->tail->next = le;
        le->prev = l->tail;
    }
    l->tail = le;

    ++l->size;
    _list_add_jump_table_node(l, l->tail);
}

/*
Set _jump_table node if there have been JT_INCREMENT additions
since the last jump table node (or this is the first node).  
*/
void
_list_add_jump_table_node(List* l, _ListNode* jte)
{
    if ((list_size(l) - 1) / JT_INCREMENT > (l->jt_size - 1))
    {
        //_ListNode** new_table = (_ListNode**)\
        realloc(l->jump_table, (l->jt_size * 2));
        /*for whatever reason the above method of reallocating memory will
          result in _jump_table memory being overwritten (consistently on
          the 12th node in the _jump_table) which eventually cuases a
          sigabrt. But the below method works - need to investigate what
          is happening here, it seems like realloc call is failing but it
          is not returning a NULL pointer like it is supposed to. */
        _ListNode** new_table =\
        (_ListNode**)calloc(sizeof(_ListNode*), l->jt_size * 2);

        if (!new_table)
        {
            ERROR_HANDLER("_list_add_jump_table_node",
                          "NA",
                          "Memory allocation error");
        }
        else
        {
            memcpy(new_table, l->jump_table, l->jt_size * sizeof(_ListNode*));
            free(l->jump_table);
            l->jump_table = new_table;
            l->jt_size *= 2;
        }
    }
    if ((list_size(l) - 1) % JT_INCREMENT == 0)
        l->jump_table[(list_size(l) - 1) / JT_INCREMENT] = jte;
}

/*
Removes the last node from the list and returns its value.  
*/
LIST_DATA_TYPE
list_pop(List* l)
{
    if (list_size(l) < 1)
    {
        ERROR_HANDLER("list_pop()", "NA", "List contains no items!");
        return (LIST_DATA_TYPE)NULL;
    }
    else
    {
        LIST_DATA_TYPE value = l->tail->value;
        struct _list_node* former_tail = l->tail;

        if (list_size(l) == 1)
        {
            l->head = NULL;
            l->tail = NULL;
        }
        else
        {
            l->tail = l->tail->prev;
            l->tail->next = NULL;
        }

        if ((list_size(l) - 1) % JT_INCREMENT == 0)
            l->jump_table[(list_size(l) - 1) / JT_INCREMENT] = NULL;

        --l->size;
        _free_list_node(former_tail);
        return value;
    }
}

void
list_remove(List* l, list_index_t index)
{
    _list_remove(l, _list_pointer_at(l, index), index);
}

void
_list_remove(List* l, _ListNode* le, list_index_t index)
{
    if (index >= list_size(l))
    {
        char arg_as_string[20];
        sprintf(arg_as_string, "(%ld)", index);
        ERROR_HANDLER("list_remove()", arg_as_string, "Index out of bounds!\n");
    }
    else
    {
        if (le == l->tail) 
        {
            list_pop(l);
            return;
        }
        else if (le == l->head) //l->head != l->tail
        {
            l->head = l->head->next;
            l->head->prev = NULL;
        }
        else 
        {
            le->prev->next = le->next;
            le->next->prev = le->prev;
        }
        _list_adjust_jump_table_up(l, index);
        _free_list_node(le);
        --l->size;
    }
}

void
_list_adjust_jump_table_up(List* l, list_index_t index)
{
    //Affected _jump_table entries starting index.  
    list_index_t i = index / JT_INCREMENT;
    list_index_t largest_jt_index = (list_size(l) - 1) / JT_INCREMENT;

    for (; i < (largest_jt_index); i++)
    {
        //only advance ptr if index really does come before the jt node.  
        //for case exapmle: list_size(l) == 10001 and index == 9001,
        //dont advance l->jump_table[9]
        if (index <= (i*1000))
            l->jump_table[i] = l->jump_table[i]->next;
    }

    //handle final jump_table node if necessary.  
    if ((list_size(l) - 1) % JT_INCREMENT == 0) //largest_jt_index * 1000 == list_size(l) - 1
        //in the case of a remove:
        //if the last element in the list ends on an index location,
        //repace it with NULL because an element is being removed.
        l->jump_table[largest_jt_index] = NULL;
    else if (index <= largest_jt_index * 1000)
        l->jump_table[largest_jt_index] =\
        l->jump_table[largest_jt_index]->next;
}

void
_list_adjust_jump_table_down(List* l, list_index_t index)
{
    //Affected _jump_table entries starting index.  
    list_index_t i = index / JT_INCREMENT;
    list_index_t largest_jt_index = list_size(l) / JT_INCREMENT;

    for (; i < (largest_jt_index-1); i++)
        l->jump_table[i] = l->jump_table[i]->prev;

    if (list_size(l) + 1 % JT_INCREMENT)
        //In the case of an insert:
        //The last element is being pushed into a _jump_table node position.  
        _list_add_jump_table_node(l, l->tail);
}

/*
Preforms a mergesort on the given list without requiring O(n) extra memory.
*/
void
sort_list(List* l)
{
    if (list_size(l) == 0) return;
    _merge_sort_list(l, l->head);
}

void
_merge_sort_list(List* l, _ListNode* current_head)
{
    /***Not Yet Implemented***/
    _ListNode* new_head, new_tail;
}

#endif
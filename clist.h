/////////////////////////////////////////////////////////////////////////////
//
// clist.h
// Defines an optionally sorted, header only, list structure with 
// constant element access time and constant add() (on average), insert() 
// and remove() time.  API includes standard list operations, get(), insert(),
// remove(), add(), pop(), etc.  A sorted list will be doubly linked,
// otherwise the list will singly linked.  
//
// Created by Nathan Boehm, 2020.  
//
/////////////////////////////////////////////////////////////////////////////


#ifndef CLIST_H
#define CLIST_H

#include <stdio.h>
#include <stdlib.h>

enum Constants
{
    _JUMP_TABLE_INCREMENT = (int)1000,
    _INITIAL_JUMP_TABLE_SIZE = (unsigned)10
};

#ifndef LIST_DATA_TYPE
#define LIST_DATA_TYPE void*
#endif

#ifndef LIST_SORTED
#define LIST_SORTED 1
#endif

#define DOUBLY_LINKED LIST_SORTED

/*
Default sorting function, items are sorted smallest first. If a < b, then
it will come earlier in the list.  
*/
int _default_less_than(LIST_DATA_TYPE a, LIST_DATA_TYPE b) { return a < b; }

#if LIST_SORTED && !(defined LEFT_BEFORE_RIGHT)
//Sorting function
#define LEFT_BEFORE_RIGHT _default_less_than
#endif

//convience option to have list items freed with the list.  
#ifndef FREE_LIST_ITEMS
#define FREE_LIST_ITEMS 0
#endif

/*
Default error handling callback function, if one is not defined.  
Attempts to print an error message to stderr.  
A user defined hanlder must have the same signature as the below function.  
*/
int default_error_handler(char* func, char* arg, char* msg)
{
    fprintf(stderr, "list error:\nin function: %s\nargument(s): %s\n%s",
            func, arg, msg);
    return -1;
}

#ifndef ERROR_HANDLER
#define ERROR_HANDLER default_error_handler
#endif

typedef struct _list_entry {
    LIST_DATA_TYPE value;
    struct _list_entry* next;

    //a sorted list will require it to be doubly linked.  
    #if DOUBLY_LINKED
    struct _list_entry* prev;
    #endif
} _ListEntry;

typedef struct list {
    unsigned long size;

    _ListEntry* _head;
    _ListEntry* _tail;
    _ListEntry** _jump_table;
} List;

_ListEntry* new_list_entry(LIST_DATA_TYPE value)
{
    _ListEntry* new_le = (_ListEntry*)calloc(1, sizeof(_ListEntry));
    new_le->value = value;
    return new_le;
}

typedef int (*filter_function) (LIST_DATA_TYPE);


/*
Returns a newly allocated list on success or NULL if memory allocation failed.  
User must free with free_list if the value returned is not NULL.  
Does not call the ERROR_HANDLER function.  
*/
struct list* 
new_list(void)
{
    //Allocate list struct.  
    List* l = (List*)calloc(1, sizeof(List));
    if (!l) return NULL;

    //Allocate jump table.  
    l->_jump_table = (_ListEntry**)\
    calloc(_INITIAL_JUMP_TABLE_SIZE, sizeof(_ListEntry*));
    if (!l->_jump_table) {
        free(l);
        return NULL;
    }

    return l;
}

/*
Internal function that returns a struct _list_entry* at the given index,
if the index is valid.  Otherwise, calls ERROR_HANDLER and returns NULL.  
*/
_ListEntry*
_list_pointer_at(List* l, unsigned long index)
{
    //Start at the closest multiple of _JUMP_TABLE_INCREMENT,
    //then iterate to reach the desired index.
    if (index < l->size) 
    {
        unsigned long jump_location = index / _JUMP_TABLE_INCREMENT;
        _ListEntry* start = l->_jump_table[jump_location];
        unsigned long distance_to_destination = index % _JUMP_TABLE_INCREMENT;

        _ListEntry* destination = start;
        unsigned long i;
        for (i = 0; i < distance_to_destination; i++) {
            destination = destination->next;
        }
        return destination;
    }
    //Error, index out of range.  
    else
    {
        char arg_as_string[20];
        sprintf(arg_as_string, "(%ld)", index);
        ERROR_HANDLER("list_at()", arg_as_string, "Index out of range!");
        return NULL;
    }
}

/*
Returns the value at the given index, if the index is valid.  
Otherwise, calls ERROR_HANDLER and returns NULL.  
*/
LIST_DATA_TYPE
list_get(List* l, unsigned long index)
{
    _ListEntry* ptr = _list_pointer_at(l, index);
    return ptr ? ptr->value : ptr;
}

/*
Internal function that checks if the position of the _ListEntry currently 
being sorted within the list (or the _ListEntry prev to it) corresponds to
a _jump_table location, if so, it updates the _jump_table accordingly.  
*/
void _list_adjust_jump_table(_ListEntry* le, List* l, unsigned long list_pos)
{
    unsigned long list_pos_mod =  list_pos % _JUMP_TABLE_INCREMENT;
    unsigned long jump_table_pos = list_pos / _JUMP_TABLE_INCREMENT;

    if (list_pos_mod == 0) {
        //le is a jump table entry.  
        l->_jump_table[jump_table_pos] = le->prev; 
    }
    else if (list_pos_mod == 1) {
        //le->prev is a jump table entry.  
        l->_jump_table[jump_table_pos] = le;
    }
}

/*
Internal function that swaps the given _ListEntry with the one previous to it
in the list and updates the _jump_table, _head and _tail, if necessary.  
*/
void _list_swap_back(_ListEntry* le_to_sort, List* l, unsigned long list_pos)
{
    _list_adjust_jump_table(le_to_sort, l, list_pos);

    _ListEntry* former_prev = le_to_sort->prev;
    if (le_to_sort->next == NULL) { //le_to_sort is the tail.  
        former_prev->next = NULL;
        l->_tail = former_prev;
    }
    else {
        former_prev->next = le_to_sort->next;
        le_to_sort->next->prev = former_prev;
    }
    le_to_sort->next = former_prev;

    if (former_prev->prev == NULL) { //former_prev is the head.  
        le_to_sort->prev = NULL;  
        l->_head = le_to_sort;
    }
    else {
        le_to_sort->prev = former_prev->prev;
        former_prev->prev->next = le_to_sort;
    }
    former_prev->prev = le_to_sort;
}

/*
Internal function that Sorts the current list->_tail node into place,
according to the LEFT_BEFORE_RIGHT rule. Replaces list->tail if necessary.
*/
void
_list_sort_last(List* l)
{
    unsigned long list_pos = l->size - 1;
    _ListEntry* le_to_sort = l->_tail;

    while (le_to_sort != l->_head &&
           LEFT_BEFORE_RIGHT(le_to_sort->value, le_to_sort->prev->value))
    {
        _list_swap_back(le_to_sort, l, list_pos);
        --list_pos;
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
    _ListEntry* le = new_list_entry(value);
    if (l->size == 0) {
        l->_head = le;
    }
    else {
        l->_tail->next = le;
        le->prev = l->_tail;
    }
    l->_tail = le;

    //Set jump table entry if there have been _JUMP_TABLE_INCREMENT additions
    //since the last jump table entry (or this is the first entry).  
    if (l->size % _JUMP_TABLE_INCREMENT == 0) {
        l->_jump_table[l->size / _JUMP_TABLE_INCREMENT] = l->_tail;
    }
    l->size++;

    #if LIST_SORTED
    if (l->size > 1) _list_sort_last(l);
    #endif
}

/*
Frees a memory associated with the given list.  
*/
void
free_list(List* l)
{
    _ListEntry* current = l->_head;
    unsigned long i;
    for (i = 0; i < l->size; i++) 
    {
        _ListEntry* next = current->next;
        current->next = NULL;
        current->prev = NULL;

        #if FREE_LIST_ITEMS
        free(current->value)
        #endif

        free(current);
        current = next;
    }

    free(l->_jump_table);
    l->_jump_table = NULL;
    l->_head = NULL;
    l->_tail = NULL;
    free(l);
}

#endif
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
    _JUMP_TABLE_INCREMENT = (int)1000,
    _INITIAL_JUMP_TABLE_SIZE = (unsigned)10
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

typedef struct _list_entry
{
    LIST_DATA_TYPE value;
    struct _list_entry* next;
    struct _list_entry* prev;
} _ListEntry;

typedef struct list
{
    unsigned long size;

    _ListEntry* _head;
    _ListEntry* _tail;
    _ListEntry** _jump_table;
    unsigned long _jump_table_size;
} List;

_ListEntry* new_list_entry(LIST_DATA_TYPE value)
{
    _ListEntry* new_le = (_ListEntry*)calloc(1, sizeof(_ListEntry));
    new_le->value = value;
    return new_le;
}

typedef int (*filter_function) (LIST_DATA_TYPE);

//API functions
List* new_list(void);
void  free_list(List*);
void  list_add(List*, LIST_DATA_TYPE);
void  list_remove(List*, unsigned long);
void  sort_list(List* l);
LIST_DATA_TYPE list_pop(List*);
LIST_DATA_TYPE list_get(List*, unsigned long);

//Internal functions
_ListEntry* _list_pointer_at(List*, unsigned long);
void _free_list_entry(_ListEntry*);
void _list_add_jump_table_entry(List*, _ListEntry*);
void _list_remove(List*, _ListEntry*, unsigned long);
void _list_adjust_jump_table(_ListEntry*, List*, unsigned long);
void _list_adjust_jump_table_up(List*, unsigned long);
void _list_adjust_jump_table_down(List*, unsigned long);
void _merge_sort_list(List*, _ListEntry*);


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
    l->_jump_table = (_ListEntry**)\
    calloc(_INITIAL_JUMP_TABLE_SIZE, sizeof(_ListEntry*));
    if (!l->_jump_table)
    {
        free(l);
        return NULL;
    }
    l->_jump_table_size = _INITIAL_JUMP_TABLE_SIZE;

    return l;
}

/*
Frees memory associated with the given list.  
*/
void
free_list(List* l)
{
    _ListEntry* current = l->_head;
    unsigned long i;
    for (i = 0; i < l->size; i++) 
    {
        #if FREE_LIST_ITEMS
        free(current->value)
        #endif

        _ListEntry* next = current->next;
        _free_list_entry(current);
        current = next;
    }

    free(l->_jump_table);
    l->_jump_table = NULL;
    l->_head = NULL;
    l->_tail = NULL;
    free(l);
}

/*
Frees memory associated with the given _ListEntry*.  
*/
void _free_list_entry(_ListEntry* le)
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
list_get(List* l, unsigned long index)
{
    _ListEntry* ptr = _list_pointer_at(l, index);
    return ptr ? ptr->value : (LIST_DATA_TYPE)NULL;
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
        if (index == l->size - 1)
            return l->_tail;

        unsigned long jump_location = index / _JUMP_TABLE_INCREMENT;
        _ListEntry* start = l->_jump_table[jump_location];
        unsigned long distance_to_destination = index % _JUMP_TABLE_INCREMENT;

        _ListEntry* destination = start;
        unsigned long i;
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
    _ListEntry* le = new_list_entry(value);
    if (l->size == 0)
        l->_head = le;
    else
    {
        l->_tail->next = le;
        le->prev = l->_tail;
    }
    l->_tail = le;

    ++l->size;
    _list_add_jump_table_entry(l, l->_tail);
}

/*
Set _jump_table entry if there have been _JUMP_TABLE_INCREMENT additions
since the last jump table entry (or this is the first entry).  
*/
void
_list_add_jump_table_entry(List* l, _ListEntry* jte)
{
    if ((l->size - 1) / _JUMP_TABLE_INCREMENT > (l->_jump_table_size - 1))
    {
        //_ListEntry** new_table = (_ListEntry**)\
        realloc(l->_jump_table, (l->_jump_table_size * 2));
        /*for whatever reason the above method of reallocating memory will
          result in _jump_table memory being overwritten (consistently on
          the 12th entry in the _jump_table) which eventually cuases a
          sigabrt. But the below method works - need to investigate what
          is happening here, it seems like realloc call is failing but it
          is not returning a NULL pointer like it is supposed to. */
        _ListEntry** new_table =\
        (_ListEntry**)calloc(sizeof(_ListEntry*), l->_jump_table_size * 2);

        if (!new_table)
        {
            ERROR_HANDLER("_list_add_jump_table_entry",
                          "NA",
                          "Memory allocation error");
        }
        else
        {
            memcpy(new_table, l->_jump_table,
                   (l->_jump_table_size) * sizeof(_ListEntry*));
            free(l->_jump_table);
            l->_jump_table = new_table;
            l->_jump_table_size *= 2;
        }
    }
    if ((l->size - 1) % _JUMP_TABLE_INCREMENT == 0)
        l->_jump_table[(l->size - 1) / _JUMP_TABLE_INCREMENT] = jte;
}

/*
Removes the last entry from the list and returns its value.  
*/
LIST_DATA_TYPE
list_pop(List* l)
{
    if (l->size < 1)
    {
        ERROR_HANDLER("list_pop()", "NA", "List contains no items!");
        return (LIST_DATA_TYPE)NULL;
    }
    else
    {
        LIST_DATA_TYPE value = l->_tail->value;
        struct _list_entry* former_tail = l->_tail;

        if (l->size == 1)
        {
            l->_head = NULL;
            l->_tail = NULL;
        }
        else
        {
            l->_tail = l->_tail->prev;
            l->_tail->next = NULL;
        }

        //_list_adjust_jump_table_up(l, l->size); //does the same as below - but seems a little hacky.
        if ((l->size - 1) % _JUMP_TABLE_INCREMENT == 0)
            l->_jump_table[(l->size - 1) / _JUMP_TABLE_INCREMENT] = NULL;

        --l->size;
        _free_list_entry(former_tail);
        return value;
    }
}

void
list_remove(List* l, unsigned long index)
{
    _list_remove(l, _list_pointer_at(l, index), index);
}

void
_list_remove(List* l, _ListEntry* le, unsigned long index)
{
    if (index >= l->size)
    {
        char arg_as_string[20];
        sprintf(arg_as_string, "(%ld)", index);
        ERROR_HANDLER("list_remove()", arg_as_string, "Index out of bounds!\n");
    }
    else
    {
        if (le == l->_tail) 
        {
            list_pop(l);
            return;
        }
        else if (le == l->_head) //l->_head != l->_tail
        {
            l->_head = l->_head->next;
            l->_head->prev = NULL;
        }
        else 
        {
            le->prev->next = le->next;
            le->next->prev = le->prev;
        }
        _list_adjust_jump_table_up(l, index);
        _free_list_entry(le);
        --l->size;
    }
}

void
_list_adjust_jump_table_up(List* l, unsigned long index)
{
    //Affected _jump_table entries starting index.  
    unsigned long i = index / _JUMP_TABLE_INCREMENT;
    unsigned long largest_jt_index = (l->size - 1) / _JUMP_TABLE_INCREMENT;

    for (; i < (largest_jt_index); i++)
    {
        //only advance ptr if index really does come before the jt entry.  
        //for case exapmle: l->size == 10001 and index == 9001,
        //dont advance l->jump_table[9]
        if (index <= (i*1000))
            l->_jump_table[i] = l->_jump_table[i]->next;
    }

    //handle final jump_table entry if necessary.  
    if ((l->size - 1) % _JUMP_TABLE_INCREMENT == 0) //largest_jt_index * 1000 == l->size - 1
        //in the case of a remove:
        //if the last element in the list ends on an index location,
        //repace it with NULL because an element is being removed.
        l->_jump_table[largest_jt_index] = NULL;
    else if (index <= largest_jt_index * 1000)
        l->_jump_table[largest_jt_index] =\
        l->_jump_table[largest_jt_index]->next;
}

void
_list_adjust_jump_table_down(List* l, unsigned long index)
{
    //Affected _jump_table entries starting index.  
    unsigned long i = index / _JUMP_TABLE_INCREMENT;
    unsigned long largest_jt_index = l->size / _JUMP_TABLE_INCREMENT;

    for (; i < (largest_jt_index-1); i++)
        l->_jump_table[i] = l->_jump_table[i]->prev;

    if (l->size + 1 % _JUMP_TABLE_INCREMENT)
        //In the case of an insert:
        //The last element is being pushed into a _jump_table entry position.  
        _list_add_jump_table_entry(l, l->_tail);
}

/*
Preforms a mergesort on the given list without requiring O(n) extra memory.
*/
void
sort_list(List* l)
{
    if (l->size == 0) return;
    _merge_sort_list(l, l->_head);
}

void
_merge_sort_list(List* l, _ListEntry* current_head)
{
    /***Not Yet Implemented***/
    _ListEntry* new_head, new_tail;
}

#endif
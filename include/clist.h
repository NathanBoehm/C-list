//////////////////////////////////////////////////////////////////////////////
//
// clist.h
// Defines a sortable linked list structure with 
// constant element access time and constant add() time (on average).  
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


#ifndef LIST_DATA_TYPE
#define LIST_DATA_TYPE void*
#endif

#ifndef ERROR_RETURN_VALUE
#define ERROR_RETURN_VALUE NULL
#endif

#ifndef LIST_COMPARATOR
static inline int _default_less_than(LIST_DATA_TYPE a, LIST_DATA_TYPE b)
{
    return a < b;
}
#define LIST_COMPARATOR _default_less_than
#endif

//Convience option to have list items freed with the list.  
#ifndef FREE_LIST_ITEMS
#define FREE_LIST_ITEMS 0
#endif


enum Constants
{
    JT_INCREMENT = (int)1000,
    INITIAL_JT_SIZE = (unsigned)10
};


//Linked list structure. Do not modify internal contents.  
typedef struct list List;
//List node structure.  
typedef struct _list_node _ListNode;
//List indexing type.  
typedef unsigned long list_index_t;
//Filter function signature.  
typedef int (*filter_function) (LIST_DATA_TYPE);
//Error handler func signature.  
typedef int (*error_handler_func) (char*, char*, char*);
//Comparator func signature.  
typedef int (*comparator_func) (LIST_DATA_TYPE, LIST_DATA_TYPE);


//API functions
/*
Returns a newly allocated list on success or NULL if memory allocation failed.  
User must free with free_list if the value returned is not NULL.  
Does not call the list_error_handler function.  
*/
static inline List*           new_list(void);

/*
Frees memory associated with the given list.  
*/
static inline void            free_list(List*);

/*
Returns the number of elements in the given list.  
*/
static inline list_index_t    list_size(List*);

/*
Adds the given value to the given list.  
Calls list_error_handler if there is a memory allocation error,
User must free the list on a memory allocation error.  
*/
static inline void            list_add(List*, LIST_DATA_TYPE);

/*
Removes the last node from the list and returns its value.  
If the list has no items to pop, calls list_error_handler and returns
ERROR_RETURN_VALUE.  
*/
static inline LIST_DATA_TYPE  list_pop(List*);

/*
Returns the value at the given index, if the index is valid.  
Otherwise, calls list_error_handler and returns ERROR_RETURN_VALUE.  
*/
static inline LIST_DATA_TYPE  list_get(List*, list_index_t);

/*
Inserts the given value at the specified position in the list.  
Calls list_error_handler if the index is out of range.  
*/
static inline void            list_insert(List*, list_index_t, LIST_DATA_TYPE);

/*
Removes the list entry at the given index and returns its value,
if the inedx is valid.  Otherwise calls list_error_handler and returns
ERROR_RETURN_VALUE.  
*/
static inline LIST_DATA_TYPE  list_remove(List*, list_index_t);

/*
Sorts the given list.  
*/
static inline void            sort_list(List* l);

/*
If the argument is not NULL, sets the list_error_handler function to be called
when the list encounters an error.   Returns the current list_error_handler.  
*/
static inline error_handler_func list_error_handler(error_handler_func f);



//Internal functions
/*
Internal function that returns a newly allocated _list_node structure.  
*/
static inline _ListNode*     _new_list_node(LIST_DATA_TYPE);

/*
Internal fucntion that frees memory associated with the given _ListNode*.  
*/
static inline void           _free_list_node(_ListNode*);

/*
Internal function that returns a struct _list_node* at the given index.
*/
static inline _ListNode*     _list_pointer_at(List*, list_index_t);

/*
Internal function that sets _jump_table node if there have been JT_INCREMENT
additions since the last jump table node (or this is the first node).  
*/
static inline void           _list_add_jump_table_node(List*, _ListNode*);

/*
Internal function that deadvances every jump table node, after the given index,
to it's ->next.  For use when removing a node.  
*/
static inline void           _list_adjust_jump_table_up(List*, list_index_t);

/*
Internal function that advances every jump table node, after the given index,
to it's ->prev.  For use when inserting a node.  
*/
static inline void           _list_adjust_jump_table_down(List*, list_index_t);

/*
Internal function that preforms a space optimized mergesort on the given list.  
*/
static inline _ListNode*     _merge_sort_list(_ListNode*, list_index_t);

/*
Internal function that adjusts the given list's internal data to
remove the given node and then free it.  
*/
static inline LIST_DATA_TYPE _list_remove(List*, _ListNode*, list_index_t);

/*
Internal function that removes that last node of the given list.  
*/
static inline LIST_DATA_TYPE _list_pop(List*);

/*
Internal function that inserts the given node 
at the specified location in the list.  
*/
static inline void _list_insert(List*, list_index_t, _ListNode*);

/*
Internal function that appends a node to the ->next of another.  
*/
static inline void _append(_ListNode**, _ListNode**, _ListNode*);

/*
Default error handling callback function, if one is not defined.  
Attempts to print an error message to stderr.  
A user defined handler must have the same signature as the function below.  
*/
static inline int _default_error_handler(char* func, char* arg, char* msg);



struct _list_node
{
    LIST_DATA_TYPE value;
    struct _list_node* next;
    struct _list_node* prev;
};

struct list
{
    unsigned long size;
    _ListNode*    head;
    _ListNode*    tail;
    _ListNode**   jump_table;
    unsigned long jt_size;
};


static inline int
_default_error_handler(char* func, char* arg, char* msg)
{
    fprintf(stderr, "list error:\nin function: %s\nargument(s): %s\n%s",
            func, arg, msg);
    return -1;
}

static inline error_handler_func
list_error_handler(error_handler_func f)
{
    static error_handler_func handler = _default_error_handler;
    if (f != NULL)
        handler = f;
    return handler;
}


static inline List*
new_list(void)
{
    //Allocate list struct.  
    List* l = (List*)calloc(1, sizeof(List));
    if (!l) return NULL;

    //Allocate jump table.  
    l->jump_table = (_ListNode**)calloc(INITIAL_JT_SIZE, sizeof(_ListNode*));
    if (!l->jump_table)
    {
        free(l);
        return NULL;
    }
    l->jt_size = INITIAL_JT_SIZE;

    return l;
}


static inline void
free_list(List* l)
{
    _ListNode* current = l->head;
    list_index_t i;
    for (i = 0; i < l->size; ++i) 
    {
        #if FREE_LIST_ITEMS
            free(current->value);
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


static inline list_index_t
list_size(List* l)
{
    return l->size;
}


static inline LIST_DATA_TYPE
list_get(List* l, list_index_t index)
{
    if (index < l->size)
        return _list_pointer_at(l, index)->value;
    else
    {
        char arg_as_string[20];
        sprintf(arg_as_string, "(%ld)", index);
        list_error_handler(NULL)\
        ("list_at()", arg_as_string, "Index out of range!\n");
        return ERROR_RETURN_VALUE;
    }
}


static inline _ListNode*
_list_pointer_at(List* l, list_index_t index)
{
    if (index == l->size - 1)
        return l->tail;

    //Start at the closest multiple of JT_INCREMENT,
    //then iterate to reach the desired index.  
    list_index_t jump_location = index / JT_INCREMENT;
    _ListNode* start = l->jump_table[jump_location];
    list_index_t distance_to_destination = index % JT_INCREMENT;

    _ListNode* destination = start;
    list_index_t i;
    for (i = 0; i < distance_to_destination; ++i)
        destination = destination->next;
    return destination;
}


static inline void
list_add(List* l, LIST_DATA_TYPE value)
{
    _ListNode* le = _new_list_node(value);
    if (l->size == 0)
        l->head = le;
    else
    {
        l->tail->next = le;
        le->prev = l->tail;
    }
    l->tail = le;

    _list_add_jump_table_node(l, l->tail);
    ++l->size;
}


static inline _ListNode*
_new_list_node(LIST_DATA_TYPE value)
{
    _ListNode* new_le = (_ListNode*)calloc(1, sizeof(_ListNode));
    new_le->value = value;
    return new_le;
}


static inline void
_free_list_node(_ListNode* le)
{
    le->next = NULL;
    le->prev = NULL;
    free(le);
}


static inline void
_list_add_jump_table_node(List* l, _ListNode* jte)
{
    if (l->size / JT_INCREMENT > (l->jt_size - 1))
    {
        /*_ListNode** new_table = (_ListNode**)
        realloc(l->jump_table, (l->jt_size * 2));
        for whatever reason the above method of reallocating memory will
          result in _jump_table memory being overwritten (consistently on
          the 12th node in the _jump_table) which eventually cuases a
          sigabrt. But the below method works - need to investigate what
          is happening here, it seems like realloc call is failing but it
          is not returning a NULL pointer like it is supposed to. */
        _ListNode** new_table =\
        (_ListNode**)calloc(sizeof(_ListNode*), l->jt_size * 2);

        if (!new_table)
        {
            list_error_handler(NULL)("_list_add_jump_table_node",
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
    if (l->size % JT_INCREMENT == 0)
        l->jump_table[l->size / JT_INCREMENT] = jte;
}


static inline void
_list_adjust_jump_table_up(List* l, list_index_t index)
{
    //Affected _jump_table entries starting index.  
    list_index_t i = index / JT_INCREMENT;
    list_index_t largest_jt_index = (l->size - 1) / JT_INCREMENT;

    for (; i < (largest_jt_index); ++i)
    {
        //only advance ptr if index really does come before the jt node.  
        //for case exapmle: l->size == 10001 and index == 9001,
        //dont advance l->jump_table[9]
        if (index <= (i*1000))
            l->jump_table[i] = l->jump_table[i]->next;
    }

    //Handle final jump_table node if necessary.  
    //largest_jt_index * 1000 == l->size - 1.    
    if ((l->size - 1) % JT_INCREMENT == 0)
        //if the last element in the list ends on an index location,
        //repace it with NULL because an element is being removed.
        l->jump_table[largest_jt_index] = NULL;
    else if (index <= largest_jt_index * 1000)
    {
        l->jump_table[largest_jt_index] =\
        l->jump_table[largest_jt_index]->next;
    }
}


static inline void
_list_adjust_jump_table_down(List* l, list_index_t index)
{
    //l->size incr/decr is always last operation so size is +1 current.  
    if (l->size > 0) //list_insert() will catch the size == 0 case.  
    {
        //Affected jump_table entries starting index.  
        list_index_t i = index / JT_INCREMENT;
        list_index_t largest_jt_index = (l->size - 1) / JT_INCREMENT;
    
        for (; i <= largest_jt_index; ++i)
        {
            if (index <= (i * 1000))
                l->jump_table[i] = l->jump_table[i]->prev;
        }
    }

    if (l->size % JT_INCREMENT == 0)
        //In the case of an insert:
        //The last element is being pushed into a _jump_table node position.  
        _list_add_jump_table_node(l, l->tail);
}


static inline LIST_DATA_TYPE
list_pop(List* l)
{
    if (l->size < 1)
    {
        list_error_handler(NULL)\
        ("list_pop()", "NA", "List contains no items!\n");
        return ERROR_RETURN_VALUE;
    }
    else
        return _list_pop(l);
}


static inline LIST_DATA_TYPE
list_remove(List* l, list_index_t index)
{
    if (index >= l->size)
    {
        char arg_as_string[20];
        sprintf(arg_as_string, "(%ld)", index);
        list_error_handler(NULL)("list_remove()",
                                 arg_as_string,
                                 "Index out of bounds!\n");
        return ERROR_RETURN_VALUE;
    }
    else
        return _list_remove(l, _list_pointer_at(l, index), index);
}


static inline LIST_DATA_TYPE
_list_pop(List* l)
{
    LIST_DATA_TYPE value = l->tail->value;
    struct _list_node* former_tail = l->tail;

    if (l->size == 1)
    {
        l->head = NULL;
        l->tail = NULL;
    }
    else
    {
        l->tail = l->tail->prev;
        l->tail->next = NULL;
    }

    if ((l->size - 1) % JT_INCREMENT == 0)
        l->jump_table[(l->size - 1) / JT_INCREMENT] = NULL;

    _free_list_node(former_tail);
    --l->size;
    return value;
}


static inline LIST_DATA_TYPE
_list_remove(List* l, _ListNode* le, list_index_t index)
{
    if (le == l->tail)
        return _list_pop(l);
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
    LIST_DATA_TYPE value = le->value;

    _list_adjust_jump_table_up(l, index);
    _free_list_node(le);
    --l->size;

    return value;
}


static inline void
list_insert(List* l, list_index_t index, LIST_DATA_TYPE value)
{
    if (index == l->size)
        list_add(l, value);
    else if (index > l->size)
    {
        char arg_as_string[20];
        sprintf(arg_as_string, "(%ld)", index);
        list_error_handler(NULL)\
        ("list_at()", arg_as_string, "Index out of range!\n");
    }
    else
    {
        _ListNode* new_node = _new_list_node(value);
        _list_insert(l, index, new_node);
    }
}


static inline void
_list_insert(List* l, list_index_t index, _ListNode* new_node)
{
    if (index == 0)
    {
        l->head->prev = new_node;
        new_node->next = l->head;
        l->head = new_node;
    }
    else
    {
        _ListNode* current_node = _list_pointer_at(l, index);
        new_node->prev = current_node->prev;
        new_node->next = current_node;
        current_node->prev->next = new_node;
        current_node->prev = new_node;
    }
    
    _list_adjust_jump_table_down(l, index);
    ++l->size;
}


static inline void
sort_list(List* l)
{
    if (l->size == 0) return;
    l->head = _merge_sort_list(l->head, 1);

    _ListNode* current = l->head;
    list_index_t i = 0;
    while(current->next != NULL)
    {
        if (i % JT_INCREMENT == 0)
            l->jump_table[i / JT_INCREMENT] = current;
        current = current->next;
        ++i;
    }
    l->tail = current;

    //Handle last jump_table node if necessary.  
    if (i % JT_INCREMENT == 0)
            l->jump_table[i / JT_INCREMENT] = current;
}


static inline _ListNode*
_merge_sort_list(_ListNode* current_head, list_index_t sublist_size)
{
    //Space-optimized mergesort based on the algorithm description found here:
    //https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html

    _ListNode* new_head = NULL; 
    _ListNode* new_tail = NULL;
    _ListNode* first_list = current_head;
    _ListNode* second_list = current_head;
    int n_merges = 0;

    while (first_list != NULL)
    {
        ++n_merges;
        list_index_t f_size = 0;
        while (f_size < sublist_size && second_list != NULL)
        {
            second_list = second_list->next;
            ++f_size;
        }
        list_index_t s_size = sublist_size;


        while(f_size > 0 || (s_size > 0 && second_list != NULL))
        {
            if (f_size == 0)
            {
                _append(&new_head, &new_tail, second_list);
                second_list = second_list->next;
                --s_size;
            }
            else if (s_size == 0 || second_list == NULL)
            {
                _append(&new_head, &new_tail, first_list);
                first_list = first_list->next;
                --f_size;
            }
            else if (LIST_COMPARATOR(second_list->value, first_list->value))
            {
                _append(&new_head, &new_tail, second_list);
                second_list = second_list->next;
                --s_size;
            }
            else
            {
                _append(&new_head, &new_tail, first_list);
                first_list = first_list->next;
                --f_size;
            }
        }
        first_list = second_list;
    }
    new_tail->next = NULL;

    if (n_merges == 1)
        return new_head;
    else
        return _merge_sort_list(new_head, sublist_size*2);
}


static inline void
_append(_ListNode** head, _ListNode** tail, _ListNode* next)
{
    if (*tail)
    {
        (*tail)->next = next;
        next->prev = *tail;
        *tail = next;
    }
    else
    {
        next->prev = NULL;
        *head = next;
        *tail = next;
    }
}



#endif
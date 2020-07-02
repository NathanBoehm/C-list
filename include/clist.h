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
typedef int (*filter_func) (LIST_DATA_TYPE);
//Error handler function signature.  
typedef int (*err_handler_ft) (char*, char*, char*);
//Comparator function signature.  
typedef int (*comparator_func) (LIST_DATA_TYPE, LIST_DATA_TYPE);


/// API functions ///
/*
Returns a newly allocated list on success or NULL if memory allocation failed.  
User must free with free_list if the value returned is not NULL.  
Does not call the list_error_handler function.  
*/
static inline List*           new_list(void);

/*
Frees the memory associated with the given list.  
*/
static inline void            free_list(List*);

/*
Returns the number of elements in the given list.  
*/
static inline list_index_t    list_size(List*);

/*
Adds the given value to the given list.  
Calls list_error_handler if there is a memory allocation error;
user must free the list on a memory allocation error.  
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
if the index is valid.  Otherwise calls list_error_handler and returns
ERROR_RETURN_VALUE.  
*/
static inline LIST_DATA_TYPE  list_remove(List*, list_index_t);

/*
Sorts the given list.  
*/
static inline void            sort_list(List*);

/*
If the argument is not NULL, sets the list_error_handler function to be called
when the list encounters an error.   Returns the current list_error_handler.  
*/
static inline err_handler_ft  list_error_handler(err_handler_ft);

/*
Returns a newly allocated array containing all list elements that meet the
requirements of the given filter function.  The size of the returned array
is stored in the given list_index_t pointer.  Returns NULL on memory allocation
failure.  
*/
static inline LIST_DATA_TYPE*  list_where(List*, filter_func, list_index_t*);



/// Internal functions ///
/*
Internal function that returns a pointer to a
newly allocated _list_node structure.  
*/
static inline _ListNode*      _new_list_node(LIST_DATA_TYPE);

/*
Internal fucntion that frees memory associated with the given _ListNode*.  
*/
static inline void            _free_list_node(_ListNode*);

/*
Internal function that returns the _ListNode* at the given index.
*/
static inline _ListNode*      _list_pointer_at(List*, list_index_t);

/*
Internal function that sets _jump_table node if there have been JT_INCREMENT
additions since the last jump table node (or this is the first node).  
*/
static inline void            _list_add_jump_table_node(List*, _ListNode*);

/*
Internal function that advances every jump table node, after the given index,
to it's ->next.  For use when removing a node.  
*/
static inline void            _list_adjust_jump_table_up(List*, list_index_t);

/*
Internal function that deadvances every jump table node, after the given index,
to it's ->prev.  For use when inserting a node.  
*/
static inline void            _list_adjust_jump_table_down(List*, list_index_t);

/*
Internal function that preforms a space optimized mergesort on the given list.  
Returns the new head node of the sorted list.  
*/
static inline _ListNode*      _merge_sort_list(_ListNode*, list_index_t);

/*
Internal function that removes the _ListNode at the given index
from the given list and returns its value.  
*/
static inline LIST_DATA_TYPE  _list_remove(List*, _ListNode*, list_index_t);

/*
Internal function that removes the last node of the given list
and returns its value.  
*/
static inline LIST_DATA_TYPE  _list_pop(List*);

/*
Internal function that inserts the given node 
at the specified location in the list.  
*/
static inline void            _list_insert(List*, list_index_t, _ListNode*);

/*
Internal function that appends a node to the ->next of another.  
*/
static inline void            _append(_ListNode**, _ListNode**, _ListNode*);

/*
Internal function that returns the node nearest to the one requested.  
Either a jump_table node or the previouisly accessed node, l->current.  
Sets the long* argument to the distance between the nearest node and
the one requested.
*/
static inline _ListNode*      _get_start_node(List*, list_index_t, long*);

/*
Internal function that returns the jump_table node closest to the given index.  
Sets the long* argument to the distance between the jump_table node and the
one at the provided index.  
*/
static inline _ListNode*      _get_closest_jt_node(List*, list_index_t, long*);

/*
Internal function that updates the list->current if it is equal to the
given _ListNode.  Updates to le->next if it is not NULL otherwise updates to
le->prev if it is not NULL, otherwise l->current is changed to NULL.  For
Use when removing a node.  Also ajusts the numerical position of current_index
according to the index of the remove operation (list_index_t).  
*/
static inline void  _update_list_current(List*, _ListNode*, list_index_t);

/*
Default error handling callback function, if one is not defined.  
Attempts to print an error message to stderr and returns -1.  
A user defined handler must have the same signature as the function below.  
*/
static inline int   _default_error_handler(char* func, char* arg, char* msg);


struct _list_node
{
    LIST_DATA_TYPE value;
    struct _list_node* next;
    struct _list_node* prev;
};

struct list
{
    list_index_t size;
    list_index_t jt_size;
    list_index_t current_index;
    _ListNode*   head;
    _ListNode*   tail;
    _ListNode**  jump_table;
    _ListNode*   current;
};


static inline int
_default_error_handler(char* func, char* arg, char* msg)
{
    fprintf(stderr, "list error:\nin function: %s\nargument(s): %s\n%s",
            func, arg, msg);
    return -1;
}

static inline err_handler_ft
list_error_handler(err_handler_ft f)
{
    static err_handler_ft handler = _default_error_handler;
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
    {
        _ListNode* node = _list_pointer_at(l, index);
        l->current = node;
        l->current_index = index;
        return node->value;
    }
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
    long dist_to_dest;
    _ListNode* start = _get_start_node(l, index, &dist_to_dest);
    int iterate_backward = dist_to_dest < 0;
    dist_to_dest = labs(dist_to_dest);

    _ListNode* destination = start;
    list_index_t i;
    for (i = 0; i < dist_to_dest; ++i)
    {
        if (iterate_backward)
            destination = destination->prev;
        else
            destination = destination->next;
    }
    return destination;
}


static inline _ListNode*
_get_start_node(List* l, list_index_t index, long* dist)
{
    long jump_location_dist;
    _ListNode* jump_table_node = _get_closest_jt_node(l,
                                                      index,
                                                      &jump_location_dist);

    long current_location_dist = (long)index - (long)l->current_index;

    if (labs(current_location_dist) < labs(jump_location_dist) &&
        (l->current != NULL))
    {
        *dist = current_location_dist;
        return l->current;
    }
    
    *dist = jump_location_dist;
    return jump_table_node;
}


static inline _ListNode*
_get_closest_jt_node(List* l, list_index_t index, long* jump_loc_dist)
{
    long lower_jump_location = (long)index / JT_INCREMENT;
    long upper_jump_location = lower_jump_location + 1;

    if (l->jt_size > upper_jump_location && //jt has entry that is not null
        (l->jump_table[upper_jump_location] != NULL))
    {
        if (labs(index - (upper_jump_location * JT_INCREMENT)) < 
            labs(index - (lower_jump_location * JT_INCREMENT)))
            {
                *jump_loc_dist = index - (upper_jump_location * JT_INCREMENT);
                return l->jump_table[upper_jump_location];
            }
    }

    *jump_loc_dist = index - (lower_jump_location * JT_INCREMENT);
    return l->jump_table[lower_jump_location];
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
    //Check if more space is needed in the jump_table.  
    if (l->size / JT_INCREMENT > (l->jt_size - 1))
    {
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
    //Check if new node is needed.  
    if (l->size % JT_INCREMENT == 0)
        l->jump_table[l->size / JT_INCREMENT] = jte;
}


static inline void
_list_adjust_jump_table_up(List* l, list_index_t index)
{
    //Affected _jump_table entries starting index.  
    list_index_t i = index / JT_INCREMENT;
    list_index_t largest_jt_index = (l->size - 1) / JT_INCREMENT;

    //Don't change last jump_table node yet.  
    for (; i < largest_jt_index; ++i)
    {
        //Only advance ptr if index really does come before the jt node.  
        //Exapmle: index == 9001, dont advance l->jump_table[9].  
        if (index <= (i*1000))
            l->jump_table[i] = l->jump_table[i]->next;
    }

    if ((l->size - 1) % JT_INCREMENT == 0)
        //If the last element in the list ends on a jump_table location,
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
    //Must update b4 pointers change.  
     _update_list_current(l, former_tail, l->size);

    if (l->size == 1) //l->head == l->tail
    {
        l->head = NULL;
        l->tail = NULL;
    }
    else
    {
        l->tail = l->tail->prev;
        l->tail->next = NULL;
    }

    //Will only remove last node, if necessary.  
    _list_adjust_jump_table_up(l, l->size-1);

    _free_list_node(former_tail);
    --l->size;
    return value;
}


static inline void
_update_list_current(List* l, _ListNode* le, list_index_t index)
{
    if (le == l->current && le != NULL)
    {
        if (le->next != NULL)
            l->current = le->next;
        else if (le->prev != NULL)
        {
            l->current = le->prev;
            --l->current_index;
        }
        else
        {
            l->current = NULL;
            l->current_index = 0;
        }
    }
    //Remove/Insert before this node will adjust its position.  
    else if (index < l->current_index)
        --l->current_index;
}


static inline LIST_DATA_TYPE
_list_remove(List* l, _ListNode* le, list_index_t index)
{
    if (le == l->tail)
        return _list_pop(l);
    else if (le == l->head) //l->head != l->tail
    {
        _update_list_current(l, le, index); //Must update b4 pointers change.  
        l->head = l->head->next;
        l->head->prev = NULL;
    }
    else
    {
        _update_list_current(l, le, index); //Must update b4 pointers change.  
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

    if (l->current_index >= index)
        ++l->current_index; //Insert will push node forward by one.  
    
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
        if (current == l->current) //Update current_position.  
            l->current_index = i;

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
    //https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html.  

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


static inline LIST_DATA_TYPE*
list_where(List* l, filter_func filter, list_index_t* size)
{
    list_index_t collection_size = 10;
    LIST_DATA_TYPE* collection = (LIST_DATA_TYPE*)\
    calloc(collection_size, sizeof(LIST_DATA_TYPE));
    if (!collection)
        return NULL;

    _ListNode* current = l->head;
    list_index_t count = 0;
    while (current != NULL)
    {
        if (count >= collection_size) //Grow array.  
        {
            LIST_DATA_TYPE* new_collection = (LIST_DATA_TYPE*)\
            calloc(collection_size * 2, sizeof(LIST_DATA_TYPE));
            if (!new_collection)
            {
                free(collection);
                return NULL;
            }

            memcpy(new_collection, collection, collection_size * sizeof(LIST_DATA_TYPE));
            free(collection);
            collection = new_collection;
            collection_size *= 2;
        }

        if (filter(current->value))
        {
            collection[count] = current->value;
            ++count;
        }

        current = current->next;
    }

    *size = count;
    return collection;
}



#endif
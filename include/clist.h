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



//Linked list structure. Do not modify internal contents.  
typedef struct list List;
//List node structure.  
typedef struct _list_node _ListNode;
//List indexing type.  
typedef unsigned long list_index_t;
//Filter function signature.  
typedef int (*filter_func) (LIST_DATA_TYPE);
//Error handler function signature.  
typedef int (*err_handler_ft) (const char*, const char*, const char*);
//Comparator function signature.  
typedef int (*comparator_func) (LIST_DATA_TYPE, LIST_DATA_TYPE);


enum Constants
{
    JT_INCREMENT = (int)1000,
    INITIAL_JT_SIZE = (unsigned)10,
    INDEX_ERR_RETURN_VALUE = (list_index_t)-1,
};


//NOTE: Error checking is handled by API functions, Internal functions assume
//that the given parameters are correct.  



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
Returns a newly created list containing all list elements that meet the
requirements of the given filter function.  Returns NULL on memory allocation
failure.  
*/
static inline List*           list_where(List*, filter_func);

/*
Merges the first and second list into the first list.  Second list is freed and
its pointer becomes invalid.   New list will not be sorted.
*/
static inline void            list_merge(List*, List*);

/*
Splits the given list at the specified index.  Leaving the first half of the
elements, prior to the given index, in the given list and 
returning a new list containing the second half of the elements.  
*/
static inline List*           list_split(List*, list_index_t);

/*
Splits the given list according to the filter function.  Elements that pass the
filter are put in the newly returned list, all others remain in the original
list.  
*/
static inline List*           list_split_where(List*, filter_func);

/*
If the argument is not NULL, sets the list_error_handler function to be called
when the list encounters an error.   Returns the current list_error_handler.  
*/
static inline err_handler_ft  list_error_handler(err_handler_ft);



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
Frees all memory associated with the list strucutre,
but not the nodes (if any).  
*/
static inline void            _free_list_structures(List*);

/*
Internal function that returns the _ListNode* at the given index.
*/
static inline _ListNode*      _list_pointer_at(List*, list_index_t);

/*
Internal function that iterates starting at the given node until it reaches the
node that is the specified distance away.  Advances on node->next if the second
parameter is false, otherwise advances on node->prev.
*/
static inline _ListNode*      _advance_to(_ListNode*, const int, list_index_t);

/*
Internal function that sets _jump_table node if there have been JT_INCREMENT
additions since the last jump table node (or this is the first node).  
*/
static inline void            _list_add_jump_table_node(List*, _ListNode*);

/*
Internal function that advances the jump_table node at the specified index
to its ->next if it comes after the first index specified.  For use with
list_remove().  
*/
static inline void _advance_jt_entry_if_affected(List*, list_index_t, list_index_t);

/*
Internal function that removes the last_jump_table index if it is the last node
in the list.  Or advances that node if it comes after the index specified.  
Does nothing if neither of the previous conditions are true.  
*/
static inline void _remove_or_advance_last_jt_entry(List*, list_index_t, list_index_t);

/*
Internal function that expands the jump_table of the given list,
to the specified size.  
*/
static inline void            _list_grow_jump_table(List*, list_index_t);

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
Internal function that advances the jump_table node at the specified index
to its ->prev if it comes after the first index specified.  For use with 
list_insert().  
*/
static inline void _deadvance_jt_entry_if_affected(List*, list_index_t, list_index_t);

/*
Internal function that preforms a space optimized mergesort on the given list.  
Returns the new head node of the sorted list.  
*/
static inline _ListNode*      _merge_sort_list(_ListNode*, list_index_t);

/*
Internal function that modifies pointers to link the given node into
the given list at the specified index.  
*/
static inline void            _link_node(List*, list_index_t, _ListNode*);

/*
Internal function that links the given node into the specified list as its
first element (l->head = node, l->tail = node).  
*/
static inline void            _link_first(List*, _ListNode*);

/*
Internal function that links the given node into the specified list as its
new head node.  
*/
static inline void            _link_head(List*, _ListNode*);

/*
Internal function that links the given node into the specified list as its
new tail node.  
*/
static inline void            _link_tail(List*, _ListNode*);

/*
Internal function that links the given node into the specified list, behind
the given current_node and after current_node->prev.  
*/
static inline void            _link_middle(List*, _ListNode*, _ListNode*);

/*
Internal function that alters list structure and _ListNode structure pointers
to remove links to/from the specified node and the given list.  
*/
static inline void            _unlink_node(List* l, _ListNode*);

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
static inline void       _update_list_current(List*, _ListNode*, list_index_t);

/*
Internal function that reassignes all jump_table nodes after 
(index / JT_INCREMENT) by iterating starting from the given node 
(assumed to be at the specified index).  Returns the final node iterated to.  
*/
static inline _ListNode* _reassign_jump_table(List*, list_index_t, _ListNode*);

/*
Internal function add all values of the first list, that pass the filter, to 
the second list.  
*/
static inline void _add_filtered_values_to_new_list(List*, List*, filter_func);

/*
Internal function that appends to the given list, a series of connected nodes, 
starting with the first provided node and ending with the second provided node,
of the specified size.  Does not affect jump table.  
*/
static inline void       _link_range(List*, _ListNode*, _ListNode*);

/*
Internal function that unlinks a range of connected nodes from the given list.  
The nodes start at the first provided node and end at the second provided node,
and are the specified length.  
*/
static inline void       _unlink_range(List*, _ListNode*, _ListNode*);

/*
Internal function that adds a range of connected nodes, starting at the first
provided node, ending at the second provided node and of the specified
length.  
*/
static inline void       _add_range(List*, _ListNode*, _ListNode*, list_index_t);

/*
Internal function that splits the given list into two lists at the given
index.  
*/
static inline List*      _list_split(List*, List*, list_index_t);

/*
Internal function that cuts off the given list at the given index and node.  
Invalidates jump table entries after the index.  
*/
static inline void       _list_chop(List*, _ListNode*, list_index_t);

/*
Internal function that sets the structure of a list to the given head,
tail and size.  
*/
static inline void       _list_set_new(List*, _ListNode*, _ListNode*, list_index_t);

/*
Internal function that NULLs out all jump table entries after the given
index.  
*/
static inline void       _remove_invalid_jt_entries(List*, list_index_t);

/*
Internal function that puts all of the first lists' values that pass the filter
into the second list and leaves all others in the original list.  
*/
static inline List*      _list_split_where(List*, List*, filter_func);

/*
Internal functiont hat moves the _ListNode from from the first list 
to the second.  Avoids the calls to free and alloc of list_add()
and list_remove().  
*/
static inline void      _move_node(List*, List*, _ListNode*, list_index_t);

/*
Default error handling callback function, if one is not defined.  
Attempts to print an error message to stderr and returns -1.  
A user defined handler must have the same signature as the function below.  
*/
static inline int _default_error_handler(const char*, const char*, const char*);

/*
Error handling wrapper to check for a NULL list.  
*/
static inline int _list_null_arg_error(List*, const char*);

/*
Error handling wrapper to check for an out of bounds index.  
*/
static inline int _list_index_error(List*, list_index_t, const char*);

/*
Error handling wrapper to check for list to small to be popped.  
*/
static inline int _list_size_error(List*, const char*);

/*
Error handling wrapper to check for failed memory allocation of a new list.  
*/
static inline int _list_allocation_error(void*, const char*);


//Error checking macros.  
#define NULL_ARG_ERROR(l)           _list_null_arg_error(l, __func__)
#define INDEX_ERROR(l, index)       _list_index_error(l, index, __func__)
#define SIZE_ERROR(l)               _list_size_error(l, __func__)
#define ALLOC_ERROR(ptr)            _list_allocation_error(ptr, __func__)


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
_default_error_handler(const char* func, const char* arg, const char* msg)
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


static inline int
_list_null_arg_error(List* l, const char* func)
{
    if (!l)
    {
        list_error_handler(NULL)\
        (func, "NA", "Given list was NULL!\n");
        return -1;
    }
    return 0;
}


static inline int
_list_index_error(List* l, list_index_t index, const char* func)
{
    if (index >= l->size)
    {
        char arg_as_string[20];
        sprintf(arg_as_string, "(%ld)", index);
        list_error_handler(NULL)\
        (func, arg_as_string, "Index out of range!\n");
        return -1;
    }
    return 0;
}


static inline int
_list_size_error(List* l, const char* func)
{
    if (l->size < 1)
    {
        list_error_handler(NULL)\
        (func, "NA", "List contains no items!\n");
        return -1;
    }
    return 0;
}


static inline int
_list_allocation_error(void* ptr, const char* func)
{
    if (!ptr)
    {
        list_error_handler(NULL)\
        (func, "NA", "Memory allocation error!\n");
        return -1;
    }
    return 0;
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
    if (!l) return;

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

    _free_list_structures(l);
}


static inline void
_free_list_structures(List* l)
{
    free(l->jump_table);
    l->jump_table = NULL;
    l->head = NULL;
    l->tail = NULL;
    free(l);
}


static inline list_index_t
list_size(List* l)
{
    if (NULL_ARG_ERROR(l)) return INDEX_ERR_RETURN_VALUE;

    return l->size;
}


#define set_list_current(l, n, i) l->current = n; l->current_index = i
static inline LIST_DATA_TYPE
list_get(List* l, list_index_t index)
{ 
    if (NULL_ARG_ERROR(l)) return ERROR_RETURN_VALUE;
    if (INDEX_ERROR(l, index)) return ERROR_RETURN_VALUE;

    _ListNode* node = _list_pointer_at(l, index);
    set_list_current(l, node, index);
    return node->value;
}


static inline _ListNode*
_list_pointer_at(List* l, list_index_t index)
{
    if (index == l->size - 1) return l->tail;

    //Start at the closest multiple of JT_INCREMENT or l->current,
    //then iterate to reach the desired index.  
    long dist_to_dest;
    _ListNode* start = _get_start_node(l, index, &dist_to_dest);

    const int iterate_backward = dist_to_dest < 0;
    dist_to_dest = labs(dist_to_dest);

    return _advance_to(start, iterate_backward, dist_to_dest);
}


static inline _ListNode*
_advance_to(_ListNode* start, const int backward, list_index_t dist)
{
    _ListNode* destination = start;
    list_index_t i;
    for (i = 0; i < dist; ++i)
        destination = backward ? destination->prev : destination->next;

    return destination;
}


static inline _ListNode*
_get_start_node(List* l, list_index_t index, long* dist)
{
    long jt_loc_dist;
    _ListNode* jump_table_node = _get_closest_jt_node(l, index, &jt_loc_dist);

    long current_loc_dist = (long)index - (long)l->current_index;

    int use_current = labs(current_loc_dist) < labs(jt_loc_dist) &&
                      l->current != NULL;

    *dist = (use_current ? current_loc_dist : jt_loc_dist);
    return  (use_current ? l->current : jump_table_node);
}


static inline _ListNode*
_get_closest_jt_node(List* l, list_index_t index, long* jump_loc_dist)
{
    long lower_jump_loc = (long)index / JT_INCREMENT;
    long upper_jump_loc = lower_jump_loc + 1;

    long upper_dist = labs(index - (upper_jump_loc * JT_INCREMENT));
    long lower_dist = labs(index - (lower_jump_loc * JT_INCREMENT));

    int use_upper = l->jt_size > upper_jump_loc && 
                    l->jump_table[upper_jump_loc] != NULL &&
                    upper_dist < lower_dist;

    long jump_location = (use_upper ? upper_jump_loc : lower_jump_loc);
    *jump_loc_dist = index - (jump_location * JT_INCREMENT);
    return l->jump_table[jump_location];
}


static inline void
list_add(List* l, LIST_DATA_TYPE value)
{
    //Error checking.  
    if (NULL_ARG_ERROR(l)) return;
    _ListNode* le = _new_list_node(value);
    if (ALLOC_ERROR(le)) return;

    _link_node(l, l->size, le);
    _list_add_jump_table_node(l, l->tail);
    ++(l->size);
}


static inline _ListNode*
_new_list_node(LIST_DATA_TYPE value)
{
    _ListNode* new_le = (_ListNode*)calloc(1, sizeof(_ListNode));
    if (!new_le) return NULL;

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
_list_add_jump_table_node(List* l, _ListNode* jt_entry)
{
    //Check if more space is needed in the jump_table.  
    list_index_t largest_required_jt_entry_index = l->size / JT_INCREMENT;
    if (largest_required_jt_entry_index > l->jt_size-1)
        _list_grow_jump_table(l, l->jt_size * 2);

    //Check if new node is needed.  
    if (l->size % JT_INCREMENT == 0)
        l->jump_table[largest_required_jt_entry_index] = jt_entry;
}


static inline void
_list_grow_jump_table(List* l, list_index_t new_size)
{
    _ListNode** new_table =\
    (_ListNode**)calloc(sizeof(_ListNode*), new_size);

    //Error checking.  
    if (ALLOC_ERROR(new_table)) return;

    memcpy(new_table, l->jump_table, l->jt_size * sizeof(_ListNode*));
    free(l->jump_table);
    l->jump_table = new_table;
    l->jt_size = new_size;
}


static inline void
_list_adjust_jump_table_up(List* l, list_index_t index)
{
    list_index_t affected_jt_indicies_start = index / JT_INCREMENT;
    list_index_t final_jt_index = (l->size - 1) / JT_INCREMENT;
 
    list_index_t i = affected_jt_indicies_start;
    //Don't change last jump_table node yet. 
    for (; i < final_jt_index; ++i)
        _advance_jt_entry_if_affected(l, index, i);

    _remove_or_advance_last_jt_entry(l, index, final_jt_index);
}


static inline void
_advance_jt_entry_if_affected(List* l, list_index_t index, list_index_t table_index)
{
    //Only advance ptr if index really does come before the jt node.  
    //Exapmle: index == 9001, dont advance l->jump_table[9].  
    if (index <= (table_index * JT_INCREMENT))
        l->jump_table[table_index] = l->jump_table[table_index]->next;
}


static inline void
_remove_or_advance_last_jt_entry(List* l, list_index_t index, list_index_t final_jt_index)
{
    if ((l->size - 1) % JT_INCREMENT == 0)
        //If the last element in the list ends on a jump_table location,
        //repace it with NULL because an element is being removed.  
        l->jump_table[final_jt_index] = NULL;

    else if (index <= final_jt_index * 1000)
        l->jump_table[final_jt_index] = l->jump_table[final_jt_index]->next;
}


static inline void
_list_adjust_jump_table_down(List* l, list_index_t index)
{
    //l->size incr/decr is always last operation so size is +1 current.  
    if (l->size > 0) //list_insert() will catch the size == 0 case.  
    {
        list_index_t affected_jt_indicies_start_index = index / JT_INCREMENT;
        list_index_t final_jt_index = (l->size - 1) / JT_INCREMENT;
    
        list_index_t i = affected_jt_indicies_start_index;
        for (; i <= final_jt_index; ++i)
            _deadvance_jt_entry_if_affected(l, index, i);
    }

    if (l->size % JT_INCREMENT == 0)
        //In the case of an insert:
        //The last element is being pushed into a _jump_table node position.       
        _list_add_jump_table_node(l, l->tail);
}


static inline void
_deadvance_jt_entry_if_affected(List* l, list_index_t index, list_index_t table_index)
{
    //Only advance ptr if index really does come before the jt node.  
    //Exapmle: index == 9001, dont deadvance l->jump_table[9].  
    if (index <= (table_index * JT_INCREMENT))
            l->jump_table[table_index] = l->jump_table[table_index]->prev;
}


static inline LIST_DATA_TYPE
list_pop(List* l)
{
    //Error checking.  
    if (NULL_ARG_ERROR(l)) return ERROR_RETURN_VALUE;
    if (SIZE_ERROR(l)) return ERROR_RETURN_VALUE;

    return _list_pop(l);
}


static inline LIST_DATA_TYPE
list_remove(List* l, list_index_t index)
{
    //Error checking.  
    if (NULL_ARG_ERROR(l)) return ERROR_RETURN_VALUE;
    if (INDEX_ERROR(l, index)) return ERROR_RETURN_VALUE;

    return _list_remove(l, _list_pointer_at(l, index), index);
}


static inline LIST_DATA_TYPE
_list_pop(List* l)
{
    LIST_DATA_TYPE value = l->tail->value;
    struct _list_node* former_tail = l->tail;

    //Must update b4 pointers change.  
    _update_list_current(l, former_tail, l->size);
    _unlink_node(l, former_tail);
    //Will only remove last node, if necessary.  
    _list_adjust_jump_table_up(l, l->size-1);

    --(l->size);

    _free_list_node(former_tail);
    return value;
}


static inline void
_unlink_node(List* l, _ListNode* le)
{
    if (l->size == 1)
    {
        l->head = NULL;
        l->tail = NULL;
    }
    else if (le == l->head) //l->head != l->tail
    {
        l->head = l->head->next;
        l->head->prev = NULL;
    }
    else if (le == l->tail)
    {
        l->tail = l->tail->prev;
        l->tail->next = NULL;
    }
    else
    {
        le->prev->next = le->next;
        le->next->prev = le->prev;
    }
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
            --(l->current_index);
        }
        else
        {
            l->current = NULL;
            l->current_index = 0;
        }
    }
    //Remove before this node will adjust its position.  
    else if (index < l->current_index)
        --(l->current_index);
}


static inline LIST_DATA_TYPE
_list_remove(List* l, _ListNode* le, list_index_t index)
{
    LIST_DATA_TYPE value = le->value;

    if (le == l->tail)
        return _list_pop(l);
    else
    {
        _update_list_current(l, le, index);
        _unlink_node(l, le);
        _list_adjust_jump_table_up(l, index);
    }

    --(l->size);

    _free_list_node(le);
    return value;
}


static inline void
list_insert(List* l, list_index_t index, LIST_DATA_TYPE value)
{
    //Error Checking.  
    if (NULL_ARG_ERROR(l)) return;
    if (l->size != 0)
        if (INDEX_ERROR(l, index)) return;

    _ListNode* new_node = _new_list_node(value);
    _list_insert(l, index, new_node);
}


static inline void
_list_insert(List* l, list_index_t index, _ListNode* new_node)
{
    _link_node(l, index, new_node);
    
    _list_adjust_jump_table_down(l, index);

    if (l->current_index >= index)
        ++(l->current_index); //Insert will push node forward by one.  
    ++(l->size);
}


static inline void
_link_node(List* l, list_index_t index, _ListNode* node)
{
    if (l->size == 0)
        _link_first(l, node);
    else if (index == l->size)
        _link_tail(l, node);
    else if (index == 0)
        _link_head(l, node);
    else
        _link_middle(l, _list_pointer_at(l, index), node);
}


static inline void
_link_tail(List* l, _ListNode* node)
{
    l->tail->next = node;
    node->prev = l->tail;
    node->next = NULL;
    l->tail = node;
}


static inline void
_link_head(List* l, _ListNode* node)
{
    node->next = l->head;
    node->prev = NULL;
    l->head->prev = node;
    l->head = node;
}


static inline void
_link_first(List* l, _ListNode* node)
{
    node->prev = NULL;
    node->next = NULL;
    l->head = node;
    l->tail = node;
}


static inline void
_link_middle(List* l, _ListNode* current_node, _ListNode* node)
{
    node->prev = current_node->prev;
    node->next = current_node;
    current_node->prev->next = node;
    current_node->prev = node;
}


static inline void
sort_list(List* l)
{
    if (NULL_ARG_ERROR(l)) return;

    if (l->size == 0) return;
    l->head = _merge_sort_list(l->head, 1);
    l->tail = _reassign_jump_table(l, 0, l->head);
}


static inline _ListNode*
_reassign_jump_table(List* l, list_index_t index, _ListNode* le)
{
    _ListNode* current = le;
    while(current->next != NULL)
    {
        if (current == l->current) //Update current_position.  
            l->current_index = index;

        if (index % JT_INCREMENT == 0)
            l->jump_table[index / JT_INCREMENT] = current;
        current = current->next;
        ++index;
    }

    //Handle last jump_table node if necessary.  
    if (index % JT_INCREMENT == 0)
            l->jump_table[index / JT_INCREMENT] = current;

    return current;
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


static inline List*
list_where(List* l, filter_func filter)
{
    if (NULL_ARG_ERROR(l)) return NULL;
    List* new_collection = new_list();
    if (ALLOC_ERROR(l)) return NULL;

    _add_filtered_values_to_new_list(l, new_collection, filter);
    return new_collection;
}


static inline void
_add_filtered_values_to_new_list(List* l, List* nl, filter_func filter)
{
    _ListNode* current = l->head;
    while (current != NULL)
    {
        if (filter(current->value))
            list_add(nl, current->value);
        current = current->next;
    }
}


static inline void
list_merge(List* first, List* second)
{
    if (NULL_ARG_ERROR(first)) return;
    if (second == NULL || second->size == 0) return;

    _add_range(first, second->head, second->tail, second->size);
    _free_list_structures(second);
}


static inline void
_add_range(List* l, _ListNode* start, _ListNode* end, list_index_t size)
{
    list_index_t last_jt_index  = (l->size-1) / JT_INCREMENT;

    _link_range(l, start, end);
    l->size += size;
    
    list_index_t new_table_size = l->size / JT_INCREMENT;
    if (l->jt_size < new_table_size)
        _list_grow_jump_table(l, new_table_size * 2);

    _ListNode* start_node = l->jump_table[last_jt_index];
    _reassign_jump_table(l, last_jt_index * JT_INCREMENT, start_node);
}


static inline void
_link_range(List* l, _ListNode* start, _ListNode* end)
{
    l->tail->next = start;
    start->prev = l->tail;
    l->tail = end;
    l->tail->next = NULL;
}


static inline void
_unlink_range(List* l, _ListNode* start, _ListNode* end)
{
    if (start == l->head)
        l->head = end->next;
    else
        start->prev->next = end->next;
    
    if (end == l->tail)
        l->tail = NULL;
    else
        end->next->prev = start->prev;
}


static inline List*
list_split(List* l, list_index_t index)
{
    if (NULL_ARG_ERROR(l)) return NULL;
    if (INDEX_ERROR(l, index)) return NULL;
    if (index == 0)
        return new_list();

    List* new_l = new_list();
    if (ALLOC_ERROR(new_l)) return NULL;

    return _list_split(l, new_l, index);
}


static inline List*
_list_split(List* l, List* nl, list_index_t index)
{
    _ListNode*   new_head = _list_pointer_at(l, index);
    list_index_t new_size = l->size - index;
    _ListNode*   new_tail = l->tail;

    _list_chop(l, new_head->prev, index);
    _list_set_new(nl, new_head, new_tail, new_size);

    return nl;
}


static inline void
_list_set_new(List* nl, _ListNode* head, _ListNode* tail, list_index_t size)
{
    nl->head = head;
    nl->head->prev = NULL;
    nl->tail = tail;
    nl->size = size;
    _reassign_jump_table(nl, 0, nl->head);
}


static inline void
_list_chop(List* l, _ListNode* new_tail, list_index_t index)
{
    l->tail = new_tail;
    new_tail->next = NULL;
    l->size -= (l->size - index);
    _remove_invalid_jt_entries(l, index);
}


static inline void
_remove_invalid_jt_entries(List* l, list_index_t index)
{
    list_index_t invalid_jt_index = index / JT_INCREMENT;
    for (; invalid_jt_index < l->jt_size; ++invalid_jt_index)
    {
        if (index <= invalid_jt_index * JT_INCREMENT)
            l->jump_table[invalid_jt_index] = NULL;
    }
}


static inline List*
list_split_where(List* l, filter_func filter)
{
    //Error checking.  
    if (NULL_ARG_ERROR(l)) return NULL;
    List* nl = new_list();
    if (ALLOC_ERROR(nl)) return NULL;

    return _list_split_where(l, nl, filter);
}


static inline List*
_list_split_where(List* l, List* nl, filter_func filter)
{
    int i = 0;
    _ListNode* current = l->head;
    while (current != NULL)
    {
        _ListNode* next = current->next;
        if (filter(current->value))
        {
            _move_node(l, nl, current, i);
            --i;
        }

        current = next;
        ++i;
    }

    return nl;
}


static inline void
_move_node(List* l1, List* l2, _ListNode* ln, list_index_t index)
{
    //Remove from l1.  
    _update_list_current(l1, ln, index);
    _unlink_node(l1, ln);
    _list_adjust_jump_table_up(l1, index);
    --(l1->size);

    //Add to l2.  
    _link_node(l2, l2->size, ln);
    _list_add_jump_table_node(l2, l2->tail);
    ++(l2->size);
}



#endif
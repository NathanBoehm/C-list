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
typedef struct list list;
//list node structure.  
typedef struct _node _node;
//list indexing type.  
typedef unsigned long lindex;
//Filter function signature.  
typedef int (*filter_func) (LIST_DATA_TYPE);
//Error handler function signature.  
typedef int (*err_handler_ft) (const char*, const char*, const char*);
//Comparator function signature.  
typedef int (*comparator_func) (LIST_DATA_TYPE, LIST_DATA_TYPE);

//Header only function.  
#define HOF static inline


enum Constants
{
    JT_INCREMENT = (int)1000,
    INITIAL_JT_SIZE = (unsigned)10,
    INDEX_ERR_RETURN_VALUE = (lindex)-1,
};


//NOTE: Error checking is handled by API functions, Internal functions assume
//that the given parameters are correct.  



/// API functions ///


/*
Returns a newly allocated list on success or NULL if memory allocation failed.  
User must free with free_list if the value returned is not NULL.  
Does not call the list_error_handler function.  
*/
HOF list*
new_list(void);

/*
Frees the memory associated with the given list, 'l'.  
*/
HOF void
free_list(list* l);

/*
Returns the number of elements in the given list, 'l'.  
*/
HOF lindex
list_size(const list* l);

/*
Adds the given value to the given list.  
Calls list_error_handler if there is a memory allocation error;
user must free the list on a memory allocation error.  
*/
HOF void
list_add(list* l, LIST_DATA_TYPE value);

/*
Removes the last value from 'l' and returns it.  
If the list has no items to pop, calls list_error_handler and returns
ERROR_RETURN_VALUE.  
*/
HOF LIST_DATA_TYPE
list_pop(list* l);

/*
Returns the value at the given index, if the index is valid.  
Otherwise, calls list_error_handler and returns ERROR_RETURN_VALUE.  
*/
HOF LIST_DATA_TYPE
list_get(list* l, lindex index);

/*
Inserts the given value at the specified index in the list.  
Calls list_error_handler if the index is out of range.  
*/
HOF void
list_insert(list* l, lindex index, LIST_DATA_TYPE value);

/*
Removes the value at the given index and returns it, if the index is valid.  
Otherwise calls list_error_handler and returns ERROR_RETURN_VALUE.  
*/
HOF LIST_DATA_TYPE
list_remove(list* l, lindex index);

/*
Sorts the given list.  
*/
HOF void
sort_list(list* l);

/*
Returns a newly created list containing all list elements of 'l' that meet the
requirements of the filter function.  Returns NULL on memory allocation
failure.  
*/
HOF list*
list_where(list* l, filter_func filter);

/*
Merges the first and second list into the first list.  The Second list is freed
and its pointer becomes invalid.   
*/
HOF void
list_merge(list* first, list* second);

/*
Splits the given list at the specified index.  Leaving the first half of the
elements, prior to the given index, in the original list 'l' and
returning a new list containing the second half of the elements.  
*/
HOF list*
list_split(list* l, lindex index);

/*
Splits the given list according to the filter function.  Elements that pass the
filter are put in the newly returned list, all others remain in the original
list.  
*/
HOF list*
list_split_where(list* l, filter_func filter);

/*
If the argument is not NULL, sets the list_error_handler function to be called
when the list encounters an error.   Returns the current list_error_handler.  
*/
HOF err_handler_ft
list_error_handler(err_handler_ft f);



/// Internal functions ///


/*
Internal function that returns a pointer to a
newly allocated _node structure whose value is set to the given value.  
*/
HOF _node*
_new_list_node(LIST_DATA_TYPE value);

/*
Internal fucntion that frees memory associated with the given _node*.  
*/
HOF void
_free_list_node(_node* n);

/*
Frees all memory associated with the list strucutre,
but not the nodes (if any).  
*/
HOF void
_free_list_structures(list* l);

/*
Internal function that returns the _node* at the given index.  
*/
HOF _node*
_list_pointer_at(list* l, lindex index);

/*
Internal function that iterates starting at the given node, 'start', until it
reaches the node that is 'dist' nodes away.  Advances on node->prev if the
second parameter, 'backward', is true, otherwise advances on node->next.
*/
HOF _node*
_advance_to(_node* start, const int backward, lindex dist);

/*
Internal function that sets the next jump_table node to 'jt_entry'
if there have been JT_INCREMENT additions since the last jump table node 
(or this is the first node).  
*/
HOF void
_list_add_jump_table_node(list* l, _node* jt_entry);

/*
Internal function that advances the jump_table node at the specified 
table_index to its ->next, if it comes after the first index specified.  
For use with list_remove().  
*/
HOF void
_advance_jt_entry_if_affected(list* l, lindex index, lindex table_index);

/*
Internal function that removes the last_jump_table index if it is the last node
in the list.  Or advances that node if it comes after the index specified.  
Does nothing if neither of the previous conditions are true.  
*/
HOF void
_remove_or_advance_last_jt_entry(list* l, lindex index, lindex final_jt_index);

/*
Internal function that expands the jump_table of the given list,
to the specified size.  
*/
HOF void
_list_grow_jump_table(list* l, lindex new_size);

/*
Internal function that advances every jump table node, after the given index,
to it's ->next.  For use when removing a node.  
*/
HOF void
_list_adjust_jump_table_up(list* l, lindex index);

/*
Internal function that deadvances every jump table node, after the given index,
to it's ->prev.  For use when inserting a node.  
*/
HOF void
_list_adjust_jump_table_down(list* l, lindex index);

/*
Internal function that advances the jump_table node at the specified 
table_index to its ->prev if it comes after the first index specified.  
For use with list_insert().  
*/
HOF void
_deadvance_jt_entry_if_affected(list* l, lindex index, lindex table_index);

/*
Internal function that preforms a space optimized mergesort on the given list,
represented by the provided head node.  
Returns the new head node of the sorted list.  
*/
HOF _node*
_merge_sort_list(_node* current_head, lindex sublist_size);

/*
Internal function that modifies l's pointers to link the given node into
l at the specified index.  
*/
HOF void
_link_node(list* l, lindex index, _node* node);

/*
Internal function that links the given node into the specified list as its
first element (l->head = node, l->tail = node).  
*/
HOF void
_link_first(list* l, _node* node);

/*
Internal function that links the given node into the specified list as its
new head node.  
*/
HOF void
_link_head(list* l, _node* node);

/*
Internal function that links the given node into the specified list as its
new tail node.  
*/
HOF void            
_link_tail(list* l, _node* node);

/*
Internal function that links the given node into the specified list, behind
current_node and after current_node->prev.  
*/
HOF void            
_link_middle(list* l, _node* current_node, _node* node);

/*
Internal function that alters list structure and _node structure pointers
to remove links to/from the given node and list.  
*/
HOF void
_unlink_node(list* l, _node* node);

/*
Internal function that removes the _node at the given index
from the list and returns its value.  
*/
HOF LIST_DATA_TYPE
_list_remove(list* l, _node* node, lindex index);

/*
Internal function that removes the last node of the given list
and returns its value.  
*/
HOF LIST_DATA_TYPE
_list_pop(list* l);

/*
Internal function that inserts the given node 
at the specified location in the list.  
*/
HOF void
_list_insert(list* l, lindex index, _node* new_node);

/*
Internal function that appends to a chain of other nodes.  
*/
HOF void
_append(_node** head, _node** tail, _node* next);

/*
Internal function that returns the node nearest to the one requested.  
Either a jump_table node or the previouisly accessed node, l->current.  
Sets the dist argument to the distance between the nearest node and
the one requested.
*/
HOF _node*
_get_start_node(list* l, lindex pos, long* dist);

/*
Internal function that returns the jump_table node closest to the given 
position.  Sets the dist argument to the distance between the jump_table
node and the one at the given index.  
*/
HOF _node*
_get_closest_jt_node(list* l, lindex pos, long* dist);

/*
Internal function that updates the list->current if it is equal to the
given _node.  Updates to le->next if it is not NULL otherwise updates to
le->prev if it is not NULL, otherwise l->current is changed to NULL.  For
Use when removing a node.  Also ajusts the numerical position of current_index
according to the index of the remove operation specified by 'index'.  
*/
HOF void
_update_list_current(list* l, _node* node, lindex index);

/*
Internal function that reassignes all jump_table nodes after 
(index / JT_INCREMENT) by iterating starting from the given node 
(assumed to be at the specified index).  Returns the final node iterated to.  
*/
HOF _node*
_reassign_jump_table(list* l, lindex index, _node* node);

/*
Internal function that adds all values of the original list,
that pass the filter, to the new list.  
*/
HOF void
_add_filtered_values_to_new_list(list* l, list* nl, filter_func filter);

/*
Internal function that appends to the given list, a series of connected nodes, 
starting with the 'start' and ending with 'end'.  Does not affect jump table.  
*/
HOF void
_link_range(list* l, _node* start, _node* end);

/*
Internal function that unlinks a range of connected nodes from the given list.  
The range begins with 'start' and ends with 'end'.  
*/
HOF void
_unlink_range(list* l, _node* start, _node* end);

/*
Internal function that adds a range of connected nodes, starting at 'start'
and ending at 'end' and of length 'size'.  
*/
HOF void
_add_range(list* l, _node* start, _node* end, lindex size);

/*
Internal function that puts all nodes that come after 'index' in 'nl'
and leaves the remaining nodes in 'l'.  
*/
HOF list*
_list_split(list* l, list* nl, lindex index);

/*
Internal function that cuts off the list at the given index and node.  
Invalidates jump table entries after the index.  
*/
HOF void
_list_chop(list* l, _node* new_tail, lindex new_tail_index);

/*
Internal function that sets the structure of a list to the given head,
tail and size.  
*/
HOF void
_list_set_new(list* l, _node* head, _node* tail, lindex size);

/*
Internal function that NULLs out all of the list's jump table entries
after the given index.  
*/
HOF void
_remove_invalid_jt_entries(list* l, lindex index);

/*
Internal function that puts all of the first list's values that pass the filter
into the new list and leaves all others in the first list.  
*/
HOF list*
_list_split_where(list* l, list* nl, filter_func filter);

/*
Internal functiont hat moves the _node from  the first list 
to the second.  Avoids the calls to free and alloc of list_add()
and list_remove().  
*/
HOF void
_move_node(list* l1, list* l2, _node* node, lindex node_index);

/*
Default error handling callback function, if one is not defined.  
Attempts to print an error message to stderr and returns -1.  
A user defined handler must have the same signature as the function below.  
*/
HOF int
_default_error_handler(const char* func, const char* arg, const char* msg);

/*
Error handling wrapper to check for a NULL list.  
*/
HOF int
_list_null_arg_error(const list* l, const char* func);

/*
Error handling wrapper to check for an out of bounds index.  
*/
HOF int
_list_index_error(const list* l, lindex, const char* func);

/*
Error handling wrapper to check for list to small to be popped.  
*/
HOF int
_list_size_error(const list* l, const char* func);

/*
Error handling wrapper to check for failed memory allocation of a new list.  
*/
HOF int
_list_allocation_error(const void* ptr, const char* func);


//Error checking macros.  
#define NULL_ARG_ERROR(l)           _list_null_arg_error(l, __func__)
#define INDEX_ERROR(l, index)       _list_index_error(l, index, __func__)
#define SIZE_ERROR(l)               _list_size_error(l, __func__)
#define ALLOC_ERROR(ptr)            _list_allocation_error(ptr, __func__)


struct _node
{
    LIST_DATA_TYPE value;
    struct _node* next;
    struct _node* prev;
};

struct list
{
    lindex   size;
    lindex   jt_size;
    lindex   current_index;
    _node*   head;
    _node*   tail;
    _node**  jump_table;
    _node*   current;
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
_list_null_arg_error(const list* l, const char* func)
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
_list_index_error(const list* l, lindex index, const char* func)
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
_list_size_error(const list* l, const char* func)
{
    if (l->size < 1)
    {
        list_error_handler(NULL)\
        (func, "NA", "list contains no items!\n");
        return -1;
    }
    return 0;
}


static inline int
_list_allocation_error(const void* ptr, const char* func)
{
    if (!ptr)
    {
        list_error_handler(NULL)\
        (func, "NA", "Memory allocation error!\n");
        return -1;
    }
    return 0;
}


static inline list*
new_list(void)
{
    //Allocate list struct.  
    list* l = (list*)calloc(1, sizeof(list));
    if (!l) return NULL;

    //Allocate jump table.  
    l->jump_table = (_node**)calloc(INITIAL_JT_SIZE, sizeof(_node*));
    if (!l->jump_table)
    {
        free(l);
        return NULL;
    }
    l->jt_size = INITIAL_JT_SIZE;

    return l;
}


static inline void
free_list(list* l)
{
    if (!l) return;

    _node* current = l->head;
    lindex i;
    for (i = 0; i < l->size; ++i) 
    {
        #if FREE_LIST_ITEMS
            free(current->value);
        #endif

        _node* next = current->next;
        _free_list_node(current);
        current = next;
    }

    _free_list_structures(l);
}


static inline void
_free_list_structures(list* l)
{
    free(l->jump_table);
    l->jump_table = NULL;
    l->head = NULL;
    l->tail = NULL;
    free(l);
}


static inline lindex
list_size(const list* l)
{
    if (NULL_ARG_ERROR(l)) return INDEX_ERR_RETURN_VALUE;

    return l->size;
}


#define set_list_current(l, n, i) l->current = n; l->current_index = i
static inline LIST_DATA_TYPE
list_get(list* l, lindex index)
{ 
    if (NULL_ARG_ERROR(l)) return ERROR_RETURN_VALUE;
    if (INDEX_ERROR(l, index)) return ERROR_RETURN_VALUE;

    _node* node = _list_pointer_at(l, index);
    set_list_current(l, node, index);
    return node->value;
}


static inline _node*
_list_pointer_at(list* l, lindex index)
{
    if (index == l->size - 1) return l->tail;

    //Start at the closest multiple of JT_INCREMENT or l->current,
    //then iterate to reach the desired index.  
    long dist_to_dest;
    _node* start = _get_start_node(l, index, &dist_to_dest);

    const int iterate_backward = dist_to_dest < 0;
    dist_to_dest = labs(dist_to_dest);

    return _advance_to(start, iterate_backward, dist_to_dest);
}


static inline _node*
_advance_to(_node* start, const int backward, lindex dist)
{
    _node* destination = start;
    lindex i;
    for (i = 0; i < dist; ++i)
        destination = backward ? destination->prev : destination->next;

    return destination;
}


static inline _node*
_get_start_node(list* l, lindex pos, long* dist)
{
    long jt_loc_dist;
    _node* jump_table_node = _get_closest_jt_node(l, pos, &jt_loc_dist);

    long current_loc_dist = (long)pos - (long)l->current_index;

    int use_current = labs(current_loc_dist) < labs(jt_loc_dist) &&
                      l->current != NULL;

    *dist = (use_current ? current_loc_dist : jt_loc_dist);
    return  (use_current ? l->current : jump_table_node);
}


static inline _node*
_get_closest_jt_node(list* l, lindex pos, long* jump_loc_dist)
{
    long lower_jump_loc = (long)pos / JT_INCREMENT;
    long upper_jump_loc = lower_jump_loc + 1;

    long upper_dist = labs(pos - (upper_jump_loc * JT_INCREMENT));
    long lower_dist = labs(pos - (lower_jump_loc * JT_INCREMENT));

    int use_upper = l->jt_size > upper_jump_loc && 
                    l->jump_table[upper_jump_loc] != NULL &&
                    upper_dist < lower_dist;

    long jump_location = (use_upper ? upper_jump_loc : lower_jump_loc);
    *jump_loc_dist = pos - (jump_location * JT_INCREMENT);
    return l->jump_table[jump_location];
}


static inline void
list_add(list* l, LIST_DATA_TYPE value)
{
    if (NULL_ARG_ERROR(l)) return;
    _node* le = _new_list_node(value);
    if (ALLOC_ERROR(le)) return;

    _link_node(l, l->size, le);
    _list_add_jump_table_node(l, l->tail);
    ++(l->size);
}


static inline _node*
_new_list_node(LIST_DATA_TYPE value)
{
    _node* new_le = (_node*)calloc(1, sizeof(_node));
    if (!new_le) return NULL;

    new_le->value = value;
    return new_le;
}


static inline void
_free_list_node(_node* le)
{
    le->next = NULL;
    le->prev = NULL;
    free(le);
}


static inline void
_list_add_jump_table_node(list* l, _node* jt_entry)
{
    //Check if more space is needed in the jump_table.  
    lindex largest_required_jt_entry_index = l->size / JT_INCREMENT;
    if (largest_required_jt_entry_index > l->jt_size-1)
        _list_grow_jump_table(l, l->jt_size * 2);

    //Check if new node is needed.  
    if (l->size % JT_INCREMENT == 0)
        l->jump_table[largest_required_jt_entry_index] = jt_entry;
}


static inline void
_list_grow_jump_table(list* l, lindex new_size)
{
    _node** new_table =\
    (_node**)calloc(sizeof(_node*), new_size);

    if (ALLOC_ERROR(new_table)) return;

    memcpy(new_table, l->jump_table, l->jt_size * sizeof(_node*));
    free(l->jump_table);
    l->jump_table = new_table;
    l->jt_size = new_size;
}


static inline void
_list_adjust_jump_table_up(list* l, lindex index)
{
    lindex affected_jt_indicies_start = index / JT_INCREMENT;
    lindex final_jt_index = (l->size - 1) / JT_INCREMENT;
 
    lindex i = affected_jt_indicies_start;
    //Don't change last jump_table node yet. 
    for (; i < final_jt_index; ++i)
        _advance_jt_entry_if_affected(l, index, i);

    _remove_or_advance_last_jt_entry(l, index, final_jt_index);
}


static inline void
_advance_jt_entry_if_affected(list* l, lindex index, lindex table_index)
{
    //Only advance ptr if index really does come before the jt node.  
    //Exapmle: index == 9001, dont advance l->jump_table[9].  
    if (index <= (table_index * JT_INCREMENT))
        l->jump_table[table_index] = l->jump_table[table_index]->next;
}


static inline void
_remove_or_advance_last_jt_entry(list* l, lindex index, lindex final_jt_index)
{
    if ((l->size - 1) % JT_INCREMENT == 0)
        //If the last element in the list ends on a jump_table location,
        //repace it with NULL because an element is being removed.  
        l->jump_table[final_jt_index] = NULL;

    else if (index <= final_jt_index * 1000)
        l->jump_table[final_jt_index] = l->jump_table[final_jt_index]->next;
}


static inline void
_list_adjust_jump_table_down(list* l, lindex index)
{
    //l->size incr/decr is always last operation so size is +1 current.  
    if (l->size > 0) //list_insert() will catch the size == 0 case.  
    {
        lindex affected_jt_indicies_start_index = index / JT_INCREMENT;
        lindex final_jt_index = (l->size - 1) / JT_INCREMENT;
    
        lindex i = affected_jt_indicies_start_index;
        for (; i <= final_jt_index; ++i)
            _deadvance_jt_entry_if_affected(l, index, i);
    }

    if (l->size % JT_INCREMENT == 0)
        //In the case of an insert:
        //The last element is being pushed into a _jump_table node position.       
        _list_add_jump_table_node(l, l->tail);
}


static inline void
_deadvance_jt_entry_if_affected(list* l, lindex index, lindex table_index)
{
    //Only advance ptr if index really does come before the jt node.  
    //Exapmle: index == 9001, dont deadvance l->jump_table[9].  
    if (index <= (table_index * JT_INCREMENT))
            l->jump_table[table_index] = l->jump_table[table_index]->prev;
}


static inline LIST_DATA_TYPE
list_pop(list* l)
{
    if (NULL_ARG_ERROR(l)) return ERROR_RETURN_VALUE;
    if (SIZE_ERROR(l)) return ERROR_RETURN_VALUE;

    return _list_pop(l);
}


static inline LIST_DATA_TYPE
list_remove(list* l, lindex index)
{
    if (NULL_ARG_ERROR(l)) return ERROR_RETURN_VALUE;
    if (INDEX_ERROR(l, index)) return ERROR_RETURN_VALUE;

    return _list_remove(l, _list_pointer_at(l, index), index);
}


static inline LIST_DATA_TYPE
_list_pop(list* l)
{
    LIST_DATA_TYPE value = l->tail->value;
    struct _node* former_tail = l->tail;

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
_unlink_node(list* l, _node* node)
{
    if (l->size == 1)
    {
        l->head = NULL;
        l->tail = NULL;
    }
    else if (node == l->head) //l->head != l->tail
    {
        l->head = l->head->next;
        l->head->prev = NULL;
    }
    else if (node == l->tail)
    {
        l->tail = l->tail->prev;
        l->tail->next = NULL;
    }
    else
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
}


static inline void
_update_list_current(list* l, _node* node, lindex index)
{
    if (node == l->current && node != NULL)
    {
        if (node->next != NULL)
            l->current = node->next;
        else if (node->prev != NULL)
        {
            l->current = node->prev;
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
_list_remove(list* l, _node* node, lindex index)
{
    LIST_DATA_TYPE value = node->value;

    if (node == l->tail)
        return _list_pop(l);
    else
    {
        _update_list_current(l, node, index);
        _unlink_node(l, node);
        _list_adjust_jump_table_up(l, index);
    }

    --(l->size);

    _free_list_node(node);
    return value;
}


static inline void
list_insert(list* l, lindex index, LIST_DATA_TYPE value)
{
    if (NULL_ARG_ERROR(l)) return;
    if (l->size != 0)
        if (INDEX_ERROR(l, index)) return;

    _node* new_node = _new_list_node(value);
    _list_insert(l, index, new_node);
}


static inline void
_list_insert(list* l, lindex index, _node* new_node)
{
    _link_node(l, index, new_node);
    
    _list_adjust_jump_table_down(l, index);

    if (l->current_index >= index)
        ++(l->current_index); //Insert will push node forward by one.  
    ++(l->size);
}


static inline void
_link_node(list* l, lindex index, _node* node)
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
_link_tail(list* l, _node* node)
{
    l->tail->next = node;
    node->prev = l->tail;
    node->next = NULL;
    l->tail = node;
}


static inline void
_link_head(list* l, _node* node)
{
    node->next = l->head;
    node->prev = NULL;
    l->head->prev = node;
    l->head = node;
}


static inline void
_link_first(list* l, _node* node)
{
    node->prev = NULL;
    node->next = NULL;
    l->head = node;
    l->tail = node;
}


static inline void
_link_middle(list* l, _node* current_node, _node* node)
{
    node->prev = current_node->prev;
    node->next = current_node;
    current_node->prev->next = node;
    current_node->prev = node;
}


static inline void
sort_list(list* l)
{
    if (NULL_ARG_ERROR(l)) return;

    if (l->size == 0) return;
    l->head = _merge_sort_list(l->head, 1);
    l->tail = _reassign_jump_table(l, 0, l->head);
}


static inline _node*
_reassign_jump_table(list* l, lindex index, _node* node)
{
    _node* current = node;
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


static inline _node*
_merge_sort_list(_node* current_head, lindex sublist_size)
{
    //Space-optimized mergesort based on the algorithm description found here:
    //https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html.  

    _node* new_head = NULL; 
    _node* new_tail = NULL;
    _node* first_list = current_head;
    _node* second_list = current_head;
    int n_merges = 0;

    while (first_list != NULL)
    {
        ++n_merges;
        lindex f_size = 0;
        while (f_size < sublist_size && second_list != NULL)
        {
            second_list = second_list->next;
            ++f_size;
        }
        lindex s_size = sublist_size;


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
_append(_node** head, _node** tail, _node* next)
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


static inline list*
list_where(list* l, filter_func filter)
{
    if (NULL_ARG_ERROR(l)) return NULL;
    list* new_collection = new_list();
    if (ALLOC_ERROR(l)) return NULL;

    _add_filtered_values_to_new_list(l, new_collection, filter);
    return new_collection;
}


static inline void
_add_filtered_values_to_new_list(list* l, list* nl, filter_func filter)
{
    _node* current = l->head;
    while (current != NULL)
    {
        if (filter(current->value))
            list_add(nl, current->value);
        current = current->next;
    }
}


static inline void
list_merge(list* first, list* second)
{
    if (NULL_ARG_ERROR(first)) return;
    if (second == NULL || second->size == 0) return;

    _add_range(first, second->head, second->tail, second->size);
    _free_list_structures(second);
}


static inline void
_add_range(list* l, _node* start, _node* end, lindex size)
{
    lindex last_jt_index  = (l->size-1) / JT_INCREMENT;

    _link_range(l, start, end);
    l->size += size;
    
    lindex new_table_size = l->size / JT_INCREMENT;
    if (l->jt_size < new_table_size)
        _list_grow_jump_table(l, new_table_size * 2);

    _node* start_node = l->jump_table[last_jt_index];
    _reassign_jump_table(l, last_jt_index * JT_INCREMENT, start_node);
}


static inline void
_link_range(list* l, _node* start, _node* end)
{
    l->tail->next = start;
    start->prev = l->tail;
    l->tail = end;
    l->tail->next = NULL;
}


static inline void
_unlink_range(list* l, _node* start, _node* end)
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


static inline list*
list_split(list* l, lindex index)
{
    if (NULL_ARG_ERROR(l)) return NULL;
    if (INDEX_ERROR(l, index)) return NULL;
    if (index == 0)
        return new_list();

    list* new_l = new_list();
    if (ALLOC_ERROR(new_l)) return NULL;

    return _list_split(l, new_l, index);
}


static inline list*
_list_split(list* l, list* nl, lindex index)
{
    _node*   new_head = _list_pointer_at(l, index);
    lindex new_size = l->size - index;
    _node*   new_tail = l->tail;

    _list_chop(l, new_head->prev, index);
    _list_set_new(nl, new_head, new_tail, new_size);

    return nl;
}


static inline void
_list_set_new(list* nl, _node* head, _node* tail, lindex size)
{
    nl->head = head;
    nl->head->prev = NULL;
    nl->tail = tail;
    nl->size = size;
    _reassign_jump_table(nl, 0, nl->head);
}


static inline void
_list_chop(list* l, _node* new_tail, lindex new_tail_index)
{
    l->tail = new_tail;
    new_tail->next = NULL;
    l->size -= (l->size - new_tail_index);
    _remove_invalid_jt_entries(l, new_tail_index);
}


static inline void
_remove_invalid_jt_entries(list* l, lindex index)
{
    lindex invalid_jt_index = index / JT_INCREMENT;
    for (; invalid_jt_index < l->jt_size; ++invalid_jt_index)
    {
        if (index <= invalid_jt_index * JT_INCREMENT)
            l->jump_table[invalid_jt_index] = NULL;
    }
}


static inline list*
list_split_where(list* l, filter_func filter)
{
    if (NULL_ARG_ERROR(l)) return NULL;
    list* nl = new_list();
    if (ALLOC_ERROR(nl)) return NULL;

    return _list_split_where(l, nl, filter);
}


static inline list*
_list_split_where(list* l, list* nl, filter_func filter)
{
    int i = 0;
    _node* current = l->head;
    while (current != NULL)
    {
        _node* next = current->next;
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
_move_node(list* l1, list* l2, _node* ln, lindex node_index)
{
    //Remove from l1.  
    _update_list_current(l1, ln, node_index);
    _unlink_node(l1, ln);
    _list_adjust_jump_table_up(l1, node_index);
    --(l1->size);

    //Add to l2.  
    _link_node(l2, l2->size, ln);
    _list_add_jump_table_node(l2, l2->tail);
    ++(l2->size);
}



#endif
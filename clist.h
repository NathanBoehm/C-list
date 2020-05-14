/////////////////////////////////////////////////////////////////////////////
//
// clist.h
// Defines a simple, optionally sorted, header only, list structure with 
// constant element access time and constant add() (on average), insert() 
// and remove() time.  API includes standard list operations, at(), insert(),
// remove(), add(), pop(), etc.  
//
// Created by Nathan Boehm, 2020.  
//
/////////////////////////////////////////////////////////////////////////////


#ifndef CLIST_H
#define CLIST_H

enum Constants
{
    //error codes
    MEMORY_ALLOCATION_ERROR = (int)-1,

    _LIST_BASE_SIZE = (size_t)10000,
    _JUMP_TABLE_INCREMENT = (unsigned)1000
};

#ifndef LIST_DATA_TYPE
#define LIST_DATA_TYPE void*
#endif

#ifndef LIST_SORTED
#define LIST_SORTED 0
#endif

#ifndef FREE_LIST_ITEMS
#define FREE_LIST_ITEMS 0
#endif

/*
Default error handling callback function, if one is not defined.  
Attempts to print an error message to stderr.  
A user defined hanlder must have the same signature as the below function.  
*/
int defalut_error_handler(char* func, char* arg, char* msg)
{
    fprintf(stderr, "list error:\nin function: %s\narguments: %s\n%s",
            func, arg, msg);
    return -1;
}

#ifndef ERROR_HANDLER
#define ERROR_HANDLER default_error_handler
#endif

typedef struct _list_entry {
    LIST_DATA_TYPE value;
    struct _list_entry* next;
} _ListEntry;

typedef struct list {
    size_t size;

    _ListEntry* _head;
    size_t _max_size;
    _ListEntry** _jump_table;
} List;

/*
Internal function to free 'num_entries' nodes inside the list.  
This can be used when needing to free an entire list or just a part of one,
e.g. when memory allocation fails ona single node, partway through allocation.  
*/
void
_partial_free(List* l, size_t num_entries)
{
    _ListEntry* current = l->_head;
    _ListEntry* next;
    size_t i;
    for (i = 0; i < num_entries; i++) {
        next = current->next;
        if (FREE_LIST_ITEMS) {
            free(current->value);
            current->value = NULL;
        }
        free(current);
        current->next = NULL;
        current = next;
    }
    free(l->_jump_table);
    l->_jump_table = NULL;
    l->_max_size = 0;
    l->size = 0;
    free(l);
}

/*
Allocates 'n' number of nodes and appends them sequentially to 'start'.  
Returns -1 on success, otherwise returns the number of nodes that
were successfully allocated and need to be freed.  
*/
long long
_allocate_n_nodes(const _ListEntry* start, size_t n)
{
    _ListEntry* prev = start;

    size_t i; for (i = 0; i < n; i++) {
        _ListEntry* le = (_ListEntry*)calloc(1, sizeof(_ListEntry));
        if (!le) {
            return i;
        }
        prev->next = le;
        prev = le;
    }
    return -1;
}

/*
Returns a newly allocated list on success or NULL if memory allocation failed.  
User must free with free_list if the value returned is not NULL.  
Does not call the ERROR_HANDLER function.  
*/
struct list* new_list(void)
{
    //Allocate list struct.  
    List* l = (List*)calloc(1, sizeof(List));
    if (!l) return NULL;

    //Allocate jump table.  
    l->_jump_table = (_ListEntry**)\
    calloc(_LIST_BASE_SIZE % _JUMP_TABLE_INCREMENT, sizeof(_ListEntry*));
    if (!l->_jump_table) {
        free(l);
        return NULL;
    }

    //Allocate first node.  
    l->_head = (_ListEntry*)calloc(1, sizeof(_ListEntry));
    if (!l->_head) {
        free(l->_jump_table);
        free(l);
        return NULL;
    }
    _ListEntry* prev = l->_head;

    //Allocate _LIST_BASE_SIZE worth of connected nodes.  
    long long status = _allocate_n_nodes(l->_head, _LIST_BASE_SIZE);
    if (status != -1) {
        //Status will be the number of successfully allocated nodes.  
        _partial_free(l, status);
        return NULL;
    }
    l->size = 0;
    l->_max_size = _LIST_BASE_SIZE;
    return l;
}

/*
Internal function that returns a struct _list_entry* at the given index,
if the index is valid.  Otherwise, calls ERROR_HANDLER and returns NULL.  
*/
_ListEntry*
_list_pointer_at(List* l, size_t index)
{
    if (index < l->size)
    {
        size_t jump_location = index / _JUMP_TABLE_INCREMENT;
        _ListEntry* start = l->_jump_table[jump_location];
        size_t distance_to_destination = index % _JUMP_TABLE_INCREMENT;

        _ListEntry* destination = start;
        size_t i; for (i = 0; i < distance_to_destination; i++) {
            destination = destination->next;
        }
    }
    else //Error.  
    {
        char arg_as_string[20];
        sprintf(arg_as_string, "(%d)", index);
        ERROR_HANDLER(__FILE__, "list_at()", 
                      arg_as_string, "Index out of range!");
        return NULL;
    }
}

/*
Returns the value at the given index, if the index is valid.  
Otherwise, calls ERROR_HANDLER and returns NULL.  
*/
LIST_DATA_TYPE
list_at(List* l, size_t index)
{
    return _list_pointer_at(l, index)->value;
}

/*
Adds the given value to the given list.  
*/
void
list_add(List* l, LIST_DATA_TYPE value)
{
    if (l->size == l->_max_size) {
        long long status = _allocate_n_nodes(_list_pointer_at(l, l->size));
        if (status != -1) {

        }

    }
}

/*
Frees a memory associated with the given list.  
*/
void
free_list(List* l)
{
    _partial_free(l, l->size);
}

#endif
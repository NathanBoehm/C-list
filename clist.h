#ifndef CLIST_H
#define CLIST_H

//#define ERR_LIST_MEM -1
#define _LIST_BASE_SIZE (size_t)10000 
#define _JUMP_TABLE_INCR 1000

#ifndef DataType
#define DataType int
#endif

#ifndef FreeListItems
#define FreeListItems 0
#endif

typedef struct _list_entry {
    DataType data;
    struct _list_entry* next;
} _ListEntry;

typedef struct list {
    size_t size;

    _ListEntry* _head;
    size_t _max_size;
    _ListEntry** _jump_table;
} List;

void _partial_free(List* l, size_t num_entries)
{
    _ListEntry* current = l->_head;
    _ListEntry* next;
    size_t i;
    for (i = 0; i < num_entries; i++) {
        next = current->next;
        if (FreeListItems) {
            free(current->data);
            current->data = NULL;
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

struct list* new_list(void) 
{
    //allocate list struct
    List* l = (List*)calloc(1, sizeof(List));
    if (!l) return NULL;

    //allocate jump table
    l->_jump_table = (_ListEntry**)calloc(_LIST_BASE_SIZE % _JUMP_TABLE_INCR, sizeof(_ListEntry*));
    if (!l->_jump_table) {
        free(l);
        return NULL;
    }

    //allocate first node
    l->_head = (_ListEntry*)calloc(1, sizeof(_ListEntry));
    if (!l->_head) {
        free(l->_jump_table);
        free(l);
        return NULL;
    }
    _ListEntry* prev = l->_head;

    //allocate _LIST_BASE_SIZE worth of connected nodes
    size_t i; for (i = 0; i < _LIST_BASE_SIZE; i++) {
        _ListEntry* le = (_ListEntry*)calloc(1, sizeof(_ListEntry));
        if (!le) {
            _partial_free(l, i-1);
            return NULL;
        }
        prev->next = le;
        prev = le;
    }
    l->size = 0;
    l->_max_size = _LIST_BASE_SIZE;
    return l;
}

void free_list(List* l)
{
    _partial_free(l, l->size);
}

#endif
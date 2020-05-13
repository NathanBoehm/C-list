#include <stdio.h>

#define LIST_MEM -1

union Data {
    int i;
    float f;
    unsigned u;
    long l;
    long long ll;
    unsigned long ul;
    char c;
    char* s;
    void* ptr;
    //your data here
};

struct _ListEntry {
    Data data;
    _ListEntry* next;
};

#define _LIST_BASE_SIZE 10000 
struct list {
    _ListEntry* head;
    size_t size;
    size_t max_size;
    _ListEntry** _jump_table;
};

struct list* new_list(void) 
{
    struct list* l = (struct list*)calloc(1, sizeof(struct list));
    l->head = (struct _ListEntry*)calloc(1, sizeof(struct _ListEntry));
    struct _ListEntry* prev = l->head;
    size_t i; for (i = 0; i < _LIST_BASE_SIZE; i++) {
        struct _ListEntry* le = (struct _ListEntry*)calloc(1, sizeof(struct _ListEntry));
        prev->next = le;
        prev = le;
    }
    l->size = 0;
    l->max_size = _LIST_BASE_SIZE;
    return l;
}

int free_list(struct list* l) 
{
    
}
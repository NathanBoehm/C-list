//////////////////////////////////////////////////////////////////////////////
//
// custom_free.h
// Example app for running gdb.  
//
// Created by Nathan Boehm, 2020.  
//
//////////////////////////////////////////////////////////////////////////////


#include <stdio.h>


#define LIST_DATA_TYPE int
#define ERROR_RETURN_VALUE -1
#include "../include/clist.h"

#define CHECK(cond) check(cond, __LINE__)

void check(int cond, int line)
{
    if (!cond) printf("Failure on line %d\n", line);
}

enum ops
{
    Add = 0,
    Insert = 40,
    Pop = 65,
    Remove = 85,
    Sort = 99
};

void op_add(List* l)
{
    list_index_t prev_size = list_size(l);
    _ListNode* prev_tail = l->tail;
    int val = rand();
    list_add(l, val);
    CHECK(list_size(l) == prev_size+1);
    CHECK(l->tail->value == val);
    CHECK(list_get(l, list_size(l)-1) == val);
    CHECK(l->tail->prev == prev_tail);
}

void op_insert(List* l)
{
    if (list_size(l) > 0)
    {
        list_index_t prev_size = list_size(l);
        int val = rand();
        list_index_t index = rand() % list_size(l);
        _ListNode* prev = _list_pointer_at(l, index);
        list_insert(l, index, val);
        CHECK(list_size(l) == prev_size+1);
        _ListNode* current = _list_pointer_at(l, index);
        CHECK(current->value == val);
        CHECK(current->next == prev);
    }
}

void op_pop(List* l)
{
    if (list_size(l) > 0)
    {
        list_index_t prev_size = list_size(l);
        long expected_value = l->tail->value;
        _ListNode* prev = l->tail->prev;
        CHECK(list_pop(l) == expected_value);
        CHECK(list_size(l) == prev_size-1);
        CHECK(l->tail == prev);
    }
}

void op_remove(List* l)
{
    if (list_size(l) > 0)
    {
        list_index_t index = rand() % list_size(l);
        list_index_t prev_size = list_size(l);
        _ListNode* current = _list_pointer_at(l, index);
        long expected_value = current->value;
        _ListNode* next = current->next;
        CHECK(list_remove(l, index) == expected_value);
        CHECK(list_size(l) == prev_size-1);
        CHECK(_list_pointer_at(l, index) == next);
    }
}

void op_sort(List* l)
{
    sort_list(l);
    _ListNode* current = l->head;
    while (current->next != NULL)
    {
        CHECK(current->value <= current->next->value);
        current = current->next;
    }

    if (l->jt_size > 1)
    {
        list_index_t i = 0;
        for (; i < l->jt_size-1; ++i)
        {
            if (l->jump_table[i+1] != NULL)
                CHECK(l->jump_table[i]->value <= l->jump_table[i+1]->value);
        }
    }
}

void battery_op(List* l, int seed)
{
    int op = seed % 100;

    if (op >= Add && op < Insert)
        op_add(l);
    else if (op >= Insert && op < Pop)
        op_insert(l);
    else if (op >= Pop && op < Remove)
        op_pop(l);
    else if (op >= Remove && op < Sort)
        op_remove(l);
    else
        op_sort(l);
}

int main(int argc, char* agrv[])
{
    List* l = new_list();

    list_add(l, 0);
    list_add(l, 1);

    int i = 0;
    for (; i < 10000; ++i)
    {
        int index = rand() % (list_size(l)-1);
        list_insert(l, index, i);
        int val = list_get(l, index);
        if (val != i)
            printf("expected: %d, actual: %d\n", i, list_get(l, index));
    }
    free_list(l);
    /*
    List* l = new_list();
    int num_ops = 100000;
    int i = 0;
    for (; i < num_ops; ++i)
    {
        battery_op(l, rand());
    }

    free_list(l);
    */
}

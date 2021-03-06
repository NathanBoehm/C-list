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

void op_add(list* l)
{
    lindex prev_size = list_size(l);
    _node* prev_tail = l->tail;
    int val = rand();
    list_add(l, val);
    CHECK(list_size(l) == prev_size+1);
    CHECK(l->tail->value == val);
    CHECK(list_get(l, list_size(l)-1) == val);
    CHECK(l->tail->prev == prev_tail);
}

void op_insert(list* l)
{
    if (list_size(l) > 0)
    {
        lindex prev_size = list_size(l);
        int val = rand();
        lindex index = rand() % list_size(l);
        _node* prev = _list_pointer_at(l, index);
        list_insert(l, index, val);
        CHECK(list_size(l) == prev_size+1);
        _node* current = _list_pointer_at(l, index);
        CHECK(current->value == val);
        CHECK(current->next == prev);
    }
}

void op_pop(list* l)
{
    if (list_size(l) > 0)
    {
        lindex prev_size = list_size(l);
        long expected_value = l->tail->value;
        _node* prev = l->tail->prev;
        CHECK(list_pop(l) == expected_value);
        CHECK(list_size(l) == prev_size-1);
        CHECK(l->tail == prev);
    }
}

void op_remove(list* l)
{
    if (list_size(l) > 0)
    {
        lindex index = rand() % list_size(l);
        lindex prev_size = list_size(l);
        _node* current = _list_pointer_at(l, index);
        long expected_value = current->value;
        _node* next = current->next;
        CHECK(list_remove(l, index) == expected_value);
        CHECK(list_size(l) == prev_size-1);
        CHECK(_list_pointer_at(l, index) == next);
    }
}

void op_sort(list* l)
{
    sort_list(l);
    _node* current = l->head;
    while (current->next != NULL)
    {
        CHECK(current->value <= current->next->value);
        current = current->next;
    }

    if (l->jt_size > 1)
    {
        lindex i = 0;
        for (; i < l->jt_size-1; ++i)
        {
            if (l->jump_table[i+1] != NULL)
                CHECK(l->jump_table[i]->value <= l->jump_table[i+1]->value);
        }
    }
}

void battery_op(list* l, int seed)
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

int filter1to10(int x)
{
    return x > 0 && x <= 10;
}

int main(int argc, char* agrv[])
{
    /*
    list* l = new_list();
    int i = 0;
    for (; i < 10; ++i)
        list_add(l, i);

    list* new_l = list_split(l, 5);
    for (i = 0; i < 5; ++i)
    {
        list_get(l, i) == i;
        list_get(new_l, i) == i+5;
    }

    int is_true1 = l->jump_table[0]->value == 0;
    int is_true2 = new_l->jump_table[0]->value == 5;

    //check_error_status(not_in_error);
    free_list(l);
    free_list(new_l);
    */
    
    list* l = new_list();
    int num_ops = 300000;
    int i = 0;
    for (; i < num_ops; ++i)
    {
        battery_op(l, rand());
    }

    free_list(l);
    
}

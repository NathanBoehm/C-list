//////////////////////////////////////////////////////////////////////////////
//
// clist_test.c
// Verifies correct behavior of clist.h.  
//
// Created by Nathan Boehm, 2020.  
//
//////////////////////////////////////////////////////////////////////////////


#include <stdbool.h>
#include "../../acutest/include/acutest.h"

#define LIST_DATA_TYPE long
#define ERROR_RETURN_VALUE -1

#include "../include/clist.h"


bool ERROR_STATUS = false;

bool not_in_error = false;
bool in_error = true;

void check_error_status(bool should_be_error)
{
    bool current = ERROR_STATUS;
    ERROR_STATUS = false;
    //can't assert, need to free list first.  
    TEST_CHECK(current == should_be_error);
}

int error_handler(char* func, char* arg, char* msg)
{
    ERROR_STATUS = true;
    return 0;
}


void test_constants(void)
{
    TEST_CHECK(INITIAL_JT_SIZE == 10);
    TEST_CHECK(JT_INCREMENT == 1000);
}


void test_new_list_intial_values(void)
{
    List* l = new_list();
    TEST_ASSERT(l != NULL);
    TEST_CHECK(list_size(l) == 0);
    TEST_CHECK(l->head == NULL);
    TEST_CHECK(l->tail == NULL);
    TEST_CHECK(l->jump_table != NULL);
    TEST_CHECK(l->jt_size == INITIAL_JT_SIZE);
    TEST_CHECK(l->current_index == 0);
    TEST_CHECK(l->current == NULL);

    size_t i = 0;
    for (; i < INITIAL_JT_SIZE; ++i)
        TEST_CHECK(l->jump_table[i] == NULL);

    free_list(l);
}

void test_get_invalid_index(void)
{
    //Custom error handler must be set each time with this unitesting framework.  
    list_error_handler(error_handler);
    List* list = new_list();
    TEST_ASSERT(list != NULL);

    TEST_CHECK(list_get(list, 0) == ERROR_RETURN_VALUE);
    check_error_status(in_error);
    TEST_CHECK(list_get(list, 1) == ERROR_RETURN_VALUE);
    check_error_status(in_error);

    free_list(list);
}


/*
Note: in the below functions values are often added in such way that their
indecies match their values, to make testing simpler.  
*/

void test_get_sets_current(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    int i = 0;
    for (; i < 2001; ++i)
        list_add(l, i);
    TEST_CHECK(l->current == NULL);
    TEST_CHECK(l->current_index == 0);

    list_get(l, list_size(l)-1);
    TEST_CHECK(l->current == l->tail);
    TEST_CHECK(l->current_index == list_size(l) - 1);

    for (i = 0; i < 2001; ++i)
    {
        list_get(l, i);
        TEST_CHECK(l->current->value == i);
        TEST_CHECK(l->current_index == i);
    }

    free_list(l);
}

void test_ptr_at_not_set_current(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    list_add(l, 0);
    list_add(l, 1);
    list_add(l, 2);

    _list_pointer_at(l, 2);
    TEST_CHECK(l->current_index == 0);
    TEST_CHECK(l->current == NULL);

    list_get(l, 1);
    _list_pointer_at(l, 0);
    TEST_CHECK(l->current->value == 1);
    TEST_CHECK(l->current_index == 1);

    free_list(l);
}


void test_get_closest_jt_node(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    int i = 0;
    for (; i < 10999; ++i)
        list_add(l, i);

    long dummy;
    for (i = 0; i < 501; ++i)
        TEST_CHECK(_get_closest_jt_node(l, i, &dummy)->value == 0);

    for (i = 501; i < 1000; ++i)
        TEST_CHECK(_get_closest_jt_node(l, i, &dummy)->value == 1 * JT_INCREMENT);

    for (i = 2000; i < 2500; ++i)
        TEST_CHECK(_get_closest_jt_node(l, i, &dummy)->value == 2 * JT_INCREMENT);

    for (i = 10000; i < 11000; ++i)
        TEST_CHECK(_get_closest_jt_node(l, i, &dummy)->value == 10 * JT_INCREMENT);

    free_list(l);
}


void test_get_start_node(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    int i = 0;
    for (; i < 10999; ++i)
        list_add(l, i);

    long dummy;
    list_get(l, 500);
    for (i = 251; i < 750; ++i)
        TEST_CHECK(_get_start_node(l, i, &dummy) == l->current);
    for (i = 0; i < 251; ++i)
        TEST_CHECK(_get_start_node(l, i, &dummy) == l->jump_table[0]);
    for (i = 750; i < 1000; ++i)
        TEST_CHECK(_get_start_node(l, i, &dummy) == l->jump_table[1]);

    free_list(l);
}


void test_add_and_get(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    TEST_ASSERT(l != NULL);

    list_add(l, 0);
    TEST_CHECK(list_size(l) == 1);
    TEST_CHECK(l->head->value == 0);
    TEST_CHECK(l->tail->value == 0);
    TEST_CHECK(l->head == l->tail);
    TEST_CHECK(list_get(l, 0) == 0);

    //Basic add()/get() checks up to intial jump_table size.  
    size_t i = 1;
    for (; i < 10000; ++i)
    {
        list_add(l, i);
        TEST_CHECK(list_size(l) == i+1);
        TEST_CHECK(l->head->value == 0);
        TEST_CHECK(l->tail->value == i);
        TEST_CHECK(l->head != l->tail);

        size_t value = list_get(l, i);
        TEST_CHECK_(value == i, "expected: %lu got: %lu\n",
                    i, value);
    }

    check_error_status(not_in_error); //hopefully none of the adds failed.  

    //check jump_table values.  
    for (i = 9; i > 0; --i)
        //value = index, jump_table entries are assigned every 1000th index.  
        TEST_CHECK(l->jump_table[i]->value == (i * 1000));

    check_error_status(not_in_error);
    free_list(l);
}


void test_simple_pop(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    TEST_ASSERT(l != NULL);
    
    //Add 100 numbers, largest first.  
    size_t i = 100;
    for (; i >= 1; --i)
        list_add(l, i);
    TEST_CHECK(list_size(l) == 100);
    TEST_CHECK(l->tail->value == 1);

    //Basic pop() checks, pop all but last element.  
    for (i = 1; i <= 99; ++i)
    {
        size_t value = list_pop(l);
        TEST_CHECK_(value == i, "expected: %lu got: %lu\n",
                    i, value);
        TEST_CHECK(list_size(l) == 100 - i);
        TEST_CHECK(l->tail->value == i+1);
    }

    //Pop last.  
    TEST_CHECK(list_pop(l) == 100);
    TEST_CHECK(list_size(l) == 0);
    TEST_CHECK(l->head == NULL);
    TEST_CHECK(l->tail == NULL);

    check_error_status(not_in_error);
    free_list(l);
}


void test_pop_after_get(void)
{
    list_error_handler(error_handler);
    List* l = new_list();

    int i = 0;
    for (; i < 501; ++i)
        list_add(l, i);

    for (i = 500; i > 0; --i)
    {
        list_get(l, i);
        TEST_CHECK(l->current->value == i);
        TEST_CHECK(l->current_index == i);
        list_pop(l);
        TEST_CHECK(l->current->value == i - 1);
        TEST_CHECK(l->current_index == i - 1);
    }
    TEST_CHECK(list_pop(l) == 0);
    TEST_CHECK(list_size(l) == 0);
    TEST_CHECK(l->current == NULL);
    TEST_CHECK(l->current_index == 0);

    check_error_status(not_in_error);
    free_list(l);
}


void test_simple_remove(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    TEST_ASSERT(l != NULL);

    size_t i = 0;
    for (; i <= 100; ++i)
        list_add(l, i);
    TEST_CHECK(list_size(l) == 101);
    TEST_CHECK(l->tail->value == 100);
    TEST_CHECK(list_get(l, 0) == 0);
    TEST_CHECK(list_get(l, 50) == 50);

    //Remove in the middle.  
    for (i = 1; i < 50; ++i)
    {
        list_remove(l, 50);
        size_t value = list_get(l, 50);
        TEST_CHECK_(value == 50+i, "expected: %lu got: %lu\n",
                    50+i, value);
        TEST_CHECK(list_size(l) == 101 - i);
    }

    check_error_status(not_in_error);
    free_list(l);
}


void test_remove_pop_error_cases(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    TEST_ASSERT(l != NULL);

    TEST_CHECK(list_pop(l) == ERROR_RETURN_VALUE);
    check_error_status(in_error);

    TEST_CHECK(list_remove(l, 0) == ERROR_RETURN_VALUE);
    check_error_status(in_error);

    list_add(l, 0);
    TEST_CHECK(list_remove(l, 1) == ERROR_RETURN_VALUE);
    check_error_status(in_error);

    free_list(l);
}


void test_large_add(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    TEST_ASSERT(l != NULL);

    size_t i = 0;
    for (; i < 1000000; ++i)
        list_add(l, i);

    TEST_CHECK(list_size(l) == 1000000);
    TEST_CHECK(l->jt_size >= 1000);
    TEST_CHECK(l->jump_table[999]->value == 999000);

    check_error_status(not_in_error);
    free_list(l);
}


void test_pop_effect_on_jt(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    TEST_ASSERT(l != NULL);

    //Add 10001 numbers.  
    size_t i = 0, j = 0;
    for (; i <= 10000; ++i)
        list_add(l, i);
    TEST_CHECK(list_size(l) == 10001);

    //Check current values of the jump_table entries.  
    for (i = 0; i < 11; ++i)
    {
        TEST_CHECK_(l->jump_table[i]->value == (i * 1000),
                    "expected: %lu got: %lu\n",
                    i * 1000,
                    l->jump_table[i]->value);
    }

    for (j = 10; j > 1; --j)
    {
        //Pop 1000 items.  
        for (i = 0; i < 1000; ++i)
            list_pop(l);

        //jump_table entries every 1000 items, so check last entry is removed.  
        TEST_CHECK(l->jump_table[j] == NULL);
        //j-1 jump_table entry was not affected.  
        TEST_CHECK(l->jump_table[j-1]->value == ((j-1) * 1000));
        TEST_CHECK(list_get(l, ((j-1)*1000)) == ((j-1)*1000));
    }

    for (i = 0; i < 1000; ++i)
        list_pop(l);
    TEST_CHECK(l->jump_table[1] == NULL); //2nd to last jt entry rmeoved.  
    TEST_CHECK(list_size(l) == 1);

    //Pop last.  
    list_pop(l);
    TEST_CHECK(l->jump_table[0] == NULL);
    TEST_CHECK(list_size(l) == 0);
    TEST_CHECK(l->head == NULL);
    TEST_CHECK(l->tail == NULL);

    check_error_status(not_in_error);
    free_list(l);
}


void test_remove_effect_on_jt(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    TEST_ASSERT(l != NULL);

    size_t i = 0;
    for (; i <= 10000; ++i)
        list_add(l, i);
    TEST_CHECK(list_size(l) == 10001);

    //remove in the middle.  
    TEST_CHECK(l->jump_table[10] != NULL);
    for (i = 0; i < 1000; ++i)
    {
        list_remove(l, 5000);
        //Value should advance by 1 for each remove.  
        TEST_CHECK(list_get(l, 5000) == 5001+i);

        //every jump_table node after the 5th (removing at index 5000)
        //should be moved to the next node in the list.  
        size_t j = 5;
        for (; j < 10; ++j)
        {
            //The jump_table node value will have started out as j*1000,
            //each remove should cause it to increase by 1.
            TEST_ASSERT_(l->jump_table[j]->value == (j*1000)+(i+1),
                       "expected: %lu got: %lu\n",
                        (j*1000)+(i+1),
                        (size_t)l->jump_table[j]->value);
        }
    }
    //final jt node should be removed.  size < 10000 due to removes.  
    TEST_CHECK(l->jump_table[10] == NULL);

    //remove at the begining.  
    TEST_CHECK(l->jump_table[9] != NULL);
    for (i = 0; i < 1000; ++i)
    {
        list_remove(l, 0);
        //Removing the current head should cause the new_head to be the next
        //node in the list whose value should be 1 greater than the old head.  
        TEST_CHECK(list_get(l, 0) == i+1);
        TEST_CHECK(l->head->value == i+1);

        //Check first half of jump_table entries.  
        size_t j = 0;
        for (; j < 4; ++j)
        {
            //The jump_table node value will have started out as j*1000,
            //each remove should cause it to increase by 1.
            TEST_ASSERT_(l->jump_table[j]->value == (j*1000)+(i+1),
                        "expected: %lu got: %lu\n",
                        (size_t)l->jump_table[j]->value,
                        (j*1000)+(i+1));
        }

        //Second half should all have values exactly 1000 larger than their
        //index due to previous removes at index 5000 (first remove loop).  
        j = 5;
        for (; j < 9; ++j)
        {
            //The jump_table node value will have started out as j*1000,
            //the first remove loop will have cause each value to be 1000
            //greater than its index, each remove in this loop will cause
            //each value to increase by 1.  
            TEST_ASSERT_(l->jump_table[j]->value == ((j+1)*1000)+(i+1),
                        "expected: %lu got: %lu\n",
                        l->jump_table[j]->value,
                        ((j+1)*1000)+(i+1));
        }
    }
    TEST_CHECK(l->jump_table[9] == NULL);

    //Remove at end.  
    TEST_CHECK(l->jump_table[8] != NULL);
    for (i = 0; i < 1000; ++i)
    {
        list_remove(l, list_size(l)-1);
        TEST_CHECK(list_get(l, list_size(l)-1) == (10000 - i - 1));
        TEST_CHECK(l->tail->value == (10000 - i - 1));
        //Removing at the end will only affect the last jump_table node.  
    }
    TEST_CHECK(l->jump_table[8] == NULL);

    //Remove remaining nodes.   
    //we've removed 1000, 3 times so remove 7001 more.  
    for (i = 0; i <= 7000; ++i)
        list_remove(l, 0);
    TEST_CHECK(list_size(l) == 0);
    TEST_CHECK(l->head == NULL);
    TEST_CHECK(l->tail == NULL);
    for (i = 0; i <= 10; ++i)
        TEST_CHECK(l->jump_table[i] == NULL);

    check_error_status(not_in_error);
    free_list(l);
}


void test_random_remove_get(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    TEST_ASSERT(l != NULL);

    size_t i = 0;
    for (; i <= 10000; ++i)
        list_add(l, i);

    for (i = 0; i < 10000; ++i)
    {
        //Check that after a remove the same index will
        //have the value of the ->next of the previous node.  
        size_t index = rand() % (10000 - i);
        _ListNode* current_node = _list_pointer_at(l, index);
        size_t expected_value = current_node->next->value;

        list_remove(l, index);
        size_t new_value = list_get(l, index);
        TEST_CHECK_(expected_value == new_value,
                    "expected: %lu got: %lu\n",
                    expected_value,
                    new_value);
    }

    //Checks state is valid given we've removed all but one node.  
    TEST_CHECK(list_size(l) == 1);
    TEST_CHECK(l->head != NULL);
    TEST_CHECK(l->tail == l->head);
    TEST_CHECK(l->jump_table[0] == l->head);
    for (i = 1; i <= 10; ++i)
        TEST_CHECK(l->jump_table[i] == NULL);

    //Remove final node.  
    list_remove(l, 0);
    TEST_CHECK(list_size(l) == 0);
    TEST_CHECK(l->head == NULL);
    TEST_CHECK(l->tail == NULL);
    TEST_CHECK(l->jump_table[0] == NULL);

    check_error_status(not_in_error);
    free_list(l);
}


void test_basic_insert(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    list_add(l, 0);
    list_add(l, 100);

    size_t i = 99;
    for (; i > 0; --i)
        list_insert(l, 1, i);

    //check ordering
    for (i = 0; i < 101; ++i)
    {
        long actual = list_get(l, i);
        TEST_CHECK_(actual == i,
                    "expected: %lu got: %lu\n",
                    i,
                    actual);
    }
    
    check_error_status(not_in_error);
    free_list(l);
}

void test_insert_on_empty_list(void)
{
    list_error_handler(error_handler);
    List* l = new_list();

    list_insert(l, 0, 1);
    TEST_ASSERT(l->head != NULL);
    TEST_CHECK(l->head->value == 1);
    TEST_ASSERT(l->tail != NULL);
    TEST_CHECK(l->tail->value == 1);

    free_list(l);
}

void test_insert_adds_jt_nodes(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    int i = 9999;
    for (; i >= 0; --i)
        list_insert(l, 0, i);

    for (i = 0; i < 10000; ++i)
        TEST_CHECK(list_get(l, i) == i);
    TEST_CHECK(l->head->value == 0);

    for (i = 0; i < 10; ++i)
        TEST_CHECK(l->jump_table[i]->value == i * 1000);

    check_error_status(not_in_error);
    free_list(l);
}

void test_insert_expands_jt(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    int i = 20000;
    for (; i >= 0; --i)
        list_insert(l, 0, i);

    TEST_CHECK(list_size(l) == 20001);
    TEST_CHECK(l->jt_size == 40);
    for (i = 0; i <= 20; ++i)
        TEST_CHECK(l->jump_table[i]->value == i * 1000);

    check_error_status(not_in_error);
    free_list(l);
}

void test_insert_modifies_jt(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    int i = 0;
    for (; i < 10000; ++i)
        list_add(l, i);

    int insert_num = 77777;

    //insert at the end.  
    TEST_CHECK(l->jt_size == 10);
    for (i = 0; i < 1000; ++i)
    {
        list_insert(l, 10000, insert_num + i);
        TEST_CHECK(list_get(l, 10000) == insert_num + i);
        TEST_CHECK(l->jump_table[10]->value == insert_num + i);
    }

    //insert in the middle.  
    TEST_CHECK(list_get(l, 5000) == 5000);
    TEST_CHECK(l->jump_table[5]->value == 5000);
    for (i = 0; i < 1000; ++i)
    {
        list_insert(l, 5000, insert_num + i);
        TEST_CHECK(list_get(l, 5000) == insert_num + i);
        TEST_CHECK(l->jump_table[5]->value == insert_num + i);
    }
    TEST_CHECK(l->jump_table[6]->value == 5000);

    //insert at the begining.  
    TEST_CHECK(list_get(l, 0) == 0);
    for (i = 0; i < 1000; ++i)
    {
        list_insert(l, 0, insert_num + i);
        TEST_CHECK(list_get(l, 0) == insert_num + i);
        TEST_CHECK(l->jump_table[0]->value == insert_num + i);
    }
    TEST_CHECK(l->jump_table[1]->value == 0);

    for (i = 1; i < 6; ++i)
        TEST_CHECK(l->jump_table[i]->value == (i-1) * 1000);
    for (i = 7; i < 12; ++i)
        TEST_CHECK(l->jump_table[i]->value == (i-2) * 1000);
    TEST_CHECK(l->jump_table[12]->value == insert_num + 999);

    check_error_status(not_in_error);
    free_list(l);
}

void test_random_insert_get(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    list_add(l, 0);
    list_add(l, 1);

    int i = 0;
    for (; i < 10000; ++i)
    {
        int index = rand() % (list_size(l)-1);
        list_insert(l, index, i);
        TEST_CHECK(list_get(l, index) == i);
    }
    TEST_CHECK(list_size(l) == 10002);

    check_error_status(not_in_error);
    free_list(l);
}

void test_sorting(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    long i = 9999;
    for (; i >= 0; --i)
    {
        list_add(l, i);
        TEST_CHECK(l->tail->value == i);
    }

    //check initial jump_table values.  
    for (i = 0; i < 10; ++i)
        TEST_CHECK(l->jump_table[i]->value == ((10 - i) * 1000) - 1);

    sort_list(l);

    _ListNode* current = l->head->next;
    TEST_CHECK(l->head->value == 0);
    while (current != NULL)
    {
        TEST_CHECK(current->prev->value == current->value - 1);
        current = current->next;
    }

    //Check new jump_table values.  
    for (i = 0; i < 10; ++i)
        TEST_CHECK(l->jump_table[i]->value == i * 1000);

    check_error_status(not_in_error);
    free_list(l);
}


void test_sort_updates_current(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    int i = 5000;
    for (; i >= 0; --i)
        list_add(l, i);
    list_get(l, 1);
    TEST_CHECK(l->current->value == 4999);
    TEST_CHECK(l->current_index == 1);
    
    sort_list(l);
    TEST_CHECK(l->current->value == 4999);
    TEST_CHECK(l->current_index == 4999);

    check_error_status(not_in_error);
    free_list(l);
}


void test_sort_emtpy_and_single(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    sort_list(l);
    list_add(l, 0);
    sort_list(l);
    TEST_CHECK(l->size == 1);
    TEST_CHECK(l->head->value == 0);
    TEST_CHECK(l->jump_table[0]->value == 0);
    TEST_CHECK(l->tail->value == 0);
    TEST_CHECK(list_get(l, 0) == 0);

    check_error_status(not_in_error);
    free_list(l);
}


void test_sort_random(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    long i = 0;
    for (; i < 1000; ++i)
        list_add(l, rand() % 100000);

    sort_list(l);

    _ListNode* current = l->head->next;
    while (current != NULL)
    {
        TEST_CHECK(current->prev->value <= current->value);
        current = current->next;
    }

    for (i = 1; i < l->jt_size; ++i)
    {
        if (l->jump_table[i] == NULL)
            break;
        TEST_CHECK(l->jump_table[i]->value >= l->jump_table[i-1]->value);
    }

    check_error_status(not_in_error);
    free_list(l);
}


//this will probably take a while.  
void test_large_sort(void)
{
    list_error_handler(error_handler);
    List* l = new_list();

    long amnt = 1000000;
    long i = 0;
    for (; i < amnt; ++i)
    {
        int value = rand();
        list_add(l, value);
    }

    TEST_CHECK(l->size == amnt);
    TEST_CHECK(l->jt_size > 1000);
    long jt_size = l->jt_size;
    sort_list(l);
    TEST_CHECK(l->jt_size == jt_size);
    TEST_CHECK(l->size == amnt);

    _ListNode* current = l->head->next;
    long size = 0;
    while (current != NULL)
    {
        ++size;
        TEST_CHECK(current->prev->value <= current->value);
        current = current->next;
    }
    TEST_CHECK(size == amnt-1);

    for (i = 1; i < l->jt_size; ++i)
    {
        if (l->jump_table[i] == NULL)
            break;
        TEST_CHECK(l->jump_table[i]->value >= l->jump_table[i-1]->value);
    }

    check_error_status(not_in_error);
    free_list(l);
}

void test_get_after_sort(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    long i = 10000;
    for (; i >= 0; --i)
        list_add(l, i);

    sort_list(l);

    for (i = 0; i < 10000; ++i)
        TEST_CHECK(list_get(l, i) == i);
    
    for (i = 0; i <= 10; ++i)
        TEST_CHECK(l->jump_table[i]->value == i * 1000);

    check_error_status(not_in_error);
    free_list(l);
}

void test_sort_sorted_list(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    long i = 0;
    for (; i < 1000; ++i)
        list_add(l, i);

    sort_list(l);
    sort_list(l);
    sort_list(l);

    for (i = 0; i < 1000; ++i)
        TEST_CHECK(list_get(l, i) == i);
    
    TEST_CHECK(l->jump_table[0]->value == 0);

    check_error_status(not_in_error);
    free_list(l);
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
    TEST_CHECK(list_size(l) == prev_size+1);
    TEST_CHECK(l->tail->value == val);
    TEST_CHECK(list_get(l, list_size(l)-1) == val);
    TEST_CHECK(l->tail->prev == prev_tail);
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
        TEST_CHECK(list_size(l) == prev_size+1);
        _ListNode* current = _list_pointer_at(l, index);
        TEST_CHECK(current->value == val);
        TEST_CHECK(current->next == prev);
    }
}

void op_pop(List* l)
{
    if (list_size(l) > 0)
    {
        list_index_t prev_size = list_size(l);
        long expected_value = l->tail->value;
        _ListNode* prev = l->tail->prev;
        TEST_CHECK(list_pop(l) == expected_value);
        TEST_CHECK(list_size(l) == prev_size-1);
        TEST_CHECK(l->tail == prev);
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
        TEST_CHECK(list_remove(l, index) == expected_value);
        TEST_CHECK(list_size(l) == prev_size-1);
        TEST_CHECK(_list_pointer_at(l, index) == next);
    }
}

void op_sort(List* l)
{
    sort_list(l);
    _ListNode* current = l->head;
    while (current->next != NULL)
    {
        TEST_CHECK(current->value <= current->next->value);
        current = current->next;
    }

    if (l->jt_size > 1)
    {
        list_index_t i = 0;
        for (; i < l->jt_size-1; ++i)
        {
            if (l->jump_table[i+1] != NULL)
                TEST_CHECK(l->jump_table[i]->value <= l->jump_table[i+1]->value);
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

//This test takes a very long time.
void test_battery_of_operations(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    int num_ops = 100000;
    int i = 0;
    for (; i < num_ops; ++i)
    {
        battery_op(l, rand());
    }
    check_error_status(not_in_error);

    free_list(l);
}


int filter1to10(long x)
{
    return x > 0 && x <= 10;
}

int lessthan500(long x)
{
    return x < 500;
}

void test_where(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    int i = 0;
    for (; i < 10001; ++i)
    {
        list_add(l, i);
    }

    List* collection = list_where(l, filter1to10);
    TEST_CHECK(list_size(collection) == 10);
    for (i = 0; i < 10; ++i)
        TEST_CHECK(list_get(collection, i) == i+1);
    free_list(collection);

    collection = list_where(l, lessthan500);
    TEST_CHECK(list_size(collection) == 500);
    for (i = 0; i < 500; ++i)
        TEST_CHECK(list_get(collection, i) == i);
    free_list(collection);

    check_error_status(not_in_error);
    free_list(l);
}

void test_merge_small(void)
{
    list_error_handler(error_handler);
    List* l1 = new_list();
    List* l2 = new_list();

    int i = 1;
    for (; i < 11; ++i)
    {
        list_add(l1, i);
        list_add(l2, i+10);
    }
    list_index_t jt_size = l1->jt_size;
    _ListNode* current_jt_node = l1->jump_table[0];

    list_merge(l1, l2);
    TEST_CHECK(l1->size == 20);
    for (i = 0; i < 20; ++i)
        TEST_CHECK(list_get(l1, i) == i+1);
    TEST_CHECK(jt_size == l1->jt_size); //jump_table should not have increased.  
    TEST_CHECK(l1->jump_table[0] == current_jt_node);
    TEST_CHECK(l1->tail->value == 20);
    TEST_CHECK(l1->head->value == 1);

    check_error_status(not_in_error);
    free_list(l1);
}

void test_merge_null(void)
{
    list_error_handler(error_handler);
    List* l = new_list();
    list_add(l, 0);

    list_merge(l, NULL);
    check_error_status(not_in_error);
    TEST_CHECK(list_get(l, 0) == 0);
    TEST_CHECK(l->head == l->tail);
    TEST_CHECK(l->head->value == 0);

    list_merge(NULL, l);
    check_error_status(in_error);
    TEST_CHECK(list_get(l, 0) == 0);
    TEST_CHECK(l->head == l->tail);
    TEST_CHECK(l->head->value == 0);

    free_list(l);
}

void test_merge_doesnt_change_current(void)
{
    list_error_handler(error_handler);
    List* l1 = new_list();
    List* l2 = new_list();

    int i = 1;
    for (; i < 11; ++i)
    {
        list_add(l1, i);
        list_add(l2, i+10);
    }

    TEST_CHECK(list_get(l1, 5) == 6);
    TEST_CHECK(list_get(l2, 5) == 16);
    list_merge(l1, l2);
    TEST_CHECK(l1->current->value == 6);

    check_error_status(not_in_error);
    free_list(l1);
}

void test_merge_5k_lists(void)
{
    list_error_handler(error_handler);
    List* l1 = new_list();
    List* l2 = new_list();

    int i = 0;
    for (; i < 5000; ++i)
    {
        list_add(l1, i);
        list_add(l2, i+5000);
    }

    list_merge(l1, l2);
    TEST_CHECK(l1->jt_size == 10);
    TEST_CHECK(l1->size == 10000);
    for (i = 0; i < 10000; ++i)
        TEST_CHECK_(list_get(l1, i) == i, "expected: %d, actual:%d\n", i, list_get(l1, i));
    for (i = 0; i < 10; ++i)
        TEST_CHECK(l1->jump_table[i]->value == i * JT_INCREMENT);

    free_list(l1);
}

void test_merge_large_lists(void)
{
    list_error_handler(error_handler);
    List* l1 = new_list();
    List* l2 = new_list();

    int i = 0;
    for (; i < 100000; ++i)
    {
        list_add(l1, i);
        list_add(l2, 100000+i);
    }

    list_merge(l1, l2);
    TEST_CHECK(l1->jt_size == 400);
    TEST_CHECK(l1->size == 200000);
    for (i = 0; i < 200000; ++i)
        TEST_CHECK(list_get(l1, i) == i);
    for (i = 0; i < 200; ++i)
        TEST_CHECK(l1->jump_table[i]->value == i * JT_INCREMENT);

    check_error_status(not_in_error);
    free_list(l1);
}


TEST_LIST = {
    {"Constant values", test_constants},
    {"New list has correct intial values", test_new_list_intial_values},
    {"get on invalid index, is an error", test_get_invalid_index},
    {"get sets current",   test_get_sets_current},
    {"list_ptr_at doesnt set current", test_ptr_at_not_set_current},
    {"get closest jump table node", test_get_closest_jt_node},
    {"get iteration start node", test_get_start_node},
    {"Basic add and get checks", test_add_and_get},
    {"Basic pop checks", test_simple_pop},
    {"Pop after get changes current", test_pop_after_get},
    {"Basic remove checks", test_simple_remove},
    {"Remove/pop on invalid index is error", test_remove_pop_error_cases},
    {"Large add and effect on jump table", test_large_add},
    {"pops reorganize jump table", test_pop_effect_on_jt},
    {"removes reorganize jump table", test_remove_effect_on_jt},
    {"Series of random removes and gets", test_random_remove_get},
    {"Basic insert checks", test_basic_insert},
    {"Insert first entry", test_insert_on_empty_list},
    {"Inserts populate jump table", test_insert_adds_jt_nodes},
    {"Inserts expand jump table size", test_insert_expands_jt},
    {"Inserts modify the jump table", test_insert_modifies_jt},
    {"Random inserts and gets", test_random_insert_get},
    {"List sorting", test_sorting},
    {"sort after get updates currrent", test_sort_updates_current},  
    {"List sorting - empty and single", test_sort_emtpy_and_single},
    {"List sorting - random value", test_sort_random},
    {"List sorting - large", test_large_sort},
    {"List sorting - get after sort", test_get_after_sort},
    {"List sorting - repeat sorting list", test_sort_sorted_list},
    {"Battery of random list operations", test_battery_of_operations},
    {"Where", test_where},
    {"Merge with small list", test_merge_small},
    {"Merge with null cases", test_merge_null},
    {"Merge doesn't change current", test_merge_doesnt_change_current},
    {"Merge 5k lists has correct jump_table", test_merge_5k_lists},
    {"Merge large lists has correct jt", test_merge_large_lists},
    {NULL, NULL}
};

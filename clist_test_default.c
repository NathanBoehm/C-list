#include <../acutest/include/acutest.h>
#include <stdbool.h>

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
#define ERROR_HANDLER error_handler

#define LIST_DATA_TYPE long
#define ERROR_RETURN_VALUE -1

#include <clist.h>


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

    size_t i = 0;
    for (; i < INITIAL_JT_SIZE; i++)
        TEST_CHECK(l->jump_table[i] == NULL);

    free_list(l);
}

void test_get_invalid_index(void)
{
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
void test_add_and_get(void)
{
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
    for (; i < 10000; i++)
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
    for (i = 9; i > 0; i--)
        //value = index, jump_table entries are assigned every 1000th index.  
        TEST_CHECK(l->jump_table[i]->value == (i * 1000));

    free_list(l);
}


void test_simple_pop(void)
{
    List* l = new_list();
    TEST_ASSERT(l != NULL);
    
    //Add 100 numbers, largest first.  
    size_t i = 100;
    for (; i >= 1; i--)
        list_add(l, i);
    TEST_CHECK(list_size(l) == 100);
    TEST_CHECK(l->tail->value == 1);

    //Basic pop() checks, pop all but last element.  
    for (i = 1; i <= 99; i++)
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


void test_simple_remove(void)
{
    List* l = new_list();
    TEST_ASSERT(l != NULL);

    size_t i = 0;
    for (; i <= 100; i++)
        list_add(l, i);
    TEST_CHECK(list_size(l) == 101);
    TEST_CHECK(l->tail->value == 100);
    TEST_CHECK(list_get(l, 0) == 0);
    TEST_CHECK(list_get(l, 50) == 50);

    //Remove in the middle.  
    for (i = 1; i < 50; i++)
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
    List* l = new_list();
    TEST_ASSERT(l != NULL);

    size_t i = 0;
    for (; i < 1000000; i++)
        list_add(l, i);

    TEST_CHECK(list_size(l) == 1000000);
    TEST_CHECK(l->jt_size >= 1000);
    TEST_CHECK(l->jump_table[999]->value == 999000);

    check_error_status(not_in_error);
    free_list(l);
}


void test_pop_effect_on_jt(void)
{
    List* l = new_list();
    TEST_ASSERT(l != NULL);

    //Add 10001 numbers.  
    size_t i = 0, j = 0;
    for (; i <= 10000; i++)
        list_add(l, i);
    TEST_CHECK(list_size(l) == 10001);

    //Check current values of the jump_table entries.  
    for (i = 0; i < 11; i++)
    {
        TEST_CHECK_(l->jump_table[i]->value == (i * 1000),
                    "expected: %lu got: %lu\n",
                    i * 1000,
                    l->jump_table[i]->value);
    }

    for (j = 10; j > 1; j--)
    {
        //Pop 1000 items.  
        for (i = 0; i < 1000; i++)
            list_pop(l);

        //jump_table entries every 1000 items, so check last entry is removed.  
        TEST_CHECK(l->jump_table[j] == NULL);
        //j-1 jump_table entry was not affected.  
        TEST_CHECK(l->jump_table[j-1]->value == ((j-1) * 1000));
        TEST_CHECK(list_get(l, ((j-1)*1000)) == ((j-1)*1000));
    }

    for (i = 0; i < 1000; i++)
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
    List* l = new_list();
    TEST_ASSERT(l != NULL);

    size_t i = 0;
    for (; i <= 10000; i++)
        list_add(l, i);
    TEST_CHECK(list_size(l) == 10001);

    //remove in the middle.  
    TEST_CHECK(l->jump_table[10] != NULL);
    for (i = 0; i < 1000; i++)
    {
        list_remove(l, 5000);
        //Value should advance by 1 for each remove.  
        TEST_CHECK(list_get(l, 5000) == 5001+i);

        //every jump_table node after the 5th (removing at index 5000)
        //should be moved to the next node in the list.  
        size_t j = 5;
        for (; j < 10; j++)
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
    for (i = 0; i < 1000; i++)
    {
        list_remove(l, 0);
        //Removing the current head should cause the new_head to be the next
        //node in the list whose value should be 1 greater than the old head.  
        TEST_CHECK(list_get(l, 0) == i+1);
        TEST_CHECK(l->head->value == i+1);

        //Check first half of jump_table entries.  
        size_t j = 0;
        for (; j < 4; j++)
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
        for (; j < 9; j++)
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
    for (i = 0; i < 1000; i++)
    {
        list_remove(l, list_size(l)-1);
        TEST_CHECK(list_get(l, list_size(l)-1) == (10000 - i - 1));
        TEST_CHECK(l->tail->value == (10000 - i - 1));
        //Removing at the end will only affect the last jump_table node.  
    }
    TEST_CHECK(l->jump_table[8] == NULL);

    //Remove remaining nodes.   
    //we've removed 1000, 3 times so remove 7001 more.  
    for (i = 0; i <= 7000; i++)
        list_remove(l, 0);
    TEST_CHECK(list_size(l) == 0);
    TEST_CHECK(l->head == NULL);
    TEST_CHECK(l->tail == NULL);
    for (i = 0; i <= 10; i++)
        TEST_CHECK(l->jump_table[i] == NULL);

    check_error_status(not_in_error);
    free_list(l);
}


void test_random_remove_get(void)
{
    List* l = new_list();
    TEST_ASSERT(l != NULL);

    size_t i = 0;
    for (; i <= 10000; i++)
        list_add(l, i);

    for (i = 0; i < 10000; i++)
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
    for (i = 1; i <= 10; i++)
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
    List* l = new_list();
    list_add(l, 0);
    list_add(l, 100);

    size_t i = 99;
    for (; i > 0; --i)
        list_insert(l, 1, i);

    //check ordering
    for (i = 0; i < 101; i++)
    {
        long actual = list_get(l, i);
        TEST_CHECK_(actual == i,
                    "expected: %lu got: %lu\n",
                    i,
                    actual);
    }
    
    free_list(l);
}

void test_insert_adds_jt_nodes(void)
{
    List* l = new_list();
    int i = 9999;
    for (; i >= 0; i--)
        list_insert(l, 0, i);

    for (i = 0; i < 10000; i++)
        TEST_CHECK(list_get(l, i) == i);
    TEST_CHECK(l->head->value == 0);

    for (i = 0; i < 10; i++)
        TEST_CHECK(l->jump_table[i]->value == i * 1000);

    free_list(l);
}

/*
void test_sorting(void)
{
    List* l = new_list();
    long i = 9999;
    for (; i >= 0; i--)
    {
        list_add(l, (void*)i);
        TEST_CHECK_(list_size(l) == 10000 - i, "expected: %lu got:%lu\n",
                    list_size(l), 10000 - i);
        TEST_CHECK_(l->head->value == (void*)i,
                    "expected: %lu got: %lu\n", i, (size_t)l->head->value);
        TEST_CHECK(l->tail->value == (void*)9999);
        //no _head->next if there is only one node
        if (i < 9999) { TEST_CHECK(l->head->next->value == (void*)i+1); }
    }

    for (i = 9; i >= 1; i--) {
        TEST_CHECK_(l->jump_table[i]->value == (void*)(i * 1000),
                    "expected: %lu got: %lu\n",
                    (size_t)(i * 1000),
                    (size_t)l->jump_table[i]->value);
    }
    free_list(l);
}


void test_sorting_random(void)
{
    List* l = new_list();
    long i = 0;
    for (; i < 1000; i++) {
        list_add(l, (void*)(rand() % 100000));
    }

    for (i = 0; i < 999; i++) {
        TEST_CHECK(list_get(l, i) <= list_get(l, i+1));
    }
    free_list(l);
}
*/

TEST_LIST = {
    {"Constant values", test_constants},
    {"New list has correct intial values", test_new_list_intial_values},
    {"Trying to get an invalid index, is an error", test_get_invalid_index},
    {"Basic add and get checks", test_add_and_get},
    {"Basic pop checks", test_simple_pop},
    {"Basic remove checks", test_simple_remove},
    {"Remove and pop on invalid indicies cause an error", test_remove_pop_error_cases},
    {"Large add and effect on jump table", test_large_add},
    {"pops reorganize jump table", test_pop_effect_on_jt},
    {"removes reorganize jump table", test_remove_effect_on_jt},
    {"Series of random removes and gets", test_random_remove_get},
    {"Basic insert checks", test_basic_insert},
    {"Inserts populate jump table", test_insert_adds_jt_nodes},
    //{"List sorting", test_sorting},
    //{"List sorting - random value", test_sorting_random},
    {NULL, NULL}
};

#include <../acutest/include/acutest.h>

int ERROR_STATUS = 0;

void 
clear_error_status(void)
{
    ERROR_STATUS = 0;
}

int error_handler(char* func, char* arg, char* msg)
{
    ERROR_STATUS = 1;
    return 0;
}
#define ERROR_HANDLER error_handler
#define CHECK_ERROR_STATUS TEST_ASSERT(ERROR_STATUS); clear_error_status()

#include <clist.h>

void test_constants(void)
{
    TEST_CHECK(INITIAL_JT_SIZE == 10);
    TEST_CHECK(JT_INCREMENT == 1000);
}

void test_new_list_intial_values(void)
{
    List* list = new_list();
    TEST_ASSERT(list != NULL);
    TEST_CHECK(list_size(list) == 0);
    TEST_CHECK(list->head == NULL);
    TEST_CHECK(list->tail == NULL);
    TEST_CHECK(list->jump_table != NULL);
    size_t i = 0;
    for (; i < INITIAL_JT_SIZE; i++)
        TEST_CHECK(list->jump_table[i] == NULL);
    free_list(list);
}

void test_get_invalid_index(void)
{
    List* list = new_list();
    TEST_ASSERT(list != NULL);
    TEST_CHECK(list_get(list, 0) == NULL);
    CHECK_ERROR_STATUS;
    TEST_CHECK(list_get(list, 1) == NULL);
    CHECK_ERROR_STATUS;
    free_list(list);
}

void test_add_and_get(void)
{
    List* l = new_list();
    TEST_ASSERT(l != NULL);

    list_add(l, (void*)0);
    TEST_CHECK(list_size(l) == 1);
    TEST_CHECK(l->head->value == (void*)0);
    TEST_CHECK(l->tail->value == (void*)0);
    TEST_CHECK(l->head == l->tail);
    TEST_CHECK(list_get(l, 0) == (void*)0);

    size_t i; for (i = 1; i < 10000; i++)
    {
        list_add(l, (void*)i);
        TEST_CHECK(list_size(l) == i+1);
        TEST_CHECK(l->head->value == (void*)0);
        TEST_CHECK(l->tail->value == (void*)i);
        TEST_CHECK(l->head != l->tail);
        size_t value = (size_t)list_get(l, i);
        TEST_CHECK_(value == i, "expected: %lu got: %lu\n",
                    i, value);
    }

    for (i = 9; i > 0; i--)
        TEST_CHECK(l->jump_table[i]->value == (void*)(i * 1000));
    free_list(l);
}

void test_simple_pop(void)
{
    List* l = new_list();
    TEST_ASSERT(l != NULL);
    size_t i = 100;
    for (; i >= 1; i--)
        list_add(l, (void*)i);
    
    TEST_CHECK(list_size(l) == 100);
    TEST_CHECK(l->tail->value == (void*)1);
    for (i = 1; i <= 99; i++)
    {
        size_t value = (size_t)list_pop(l);
        TEST_CHECK_(value == i, "expected: %lu got: %lu\n",
                    i, value);
        TEST_CHECK(list_size(l) == 100 - i);
        TEST_CHECK(l->tail->value == (void*)i+1);
    }
    TEST_CHECK((size_t)list_pop(l) == 100);
    TEST_CHECK(list_size(l) == 0);
    TEST_CHECK(l->head == NULL);
    TEST_CHECK(l->tail == NULL);
    free_list(l);
}

void test_simple_remove(void)
{
    List* l = new_list();
    TEST_ASSERT(l != NULL);
    size_t i = 0;
    for (; i <= 100; i++)
    {
        list_add(l, (void*)i);
        //CHECK_ERROR_STATUS;
    }
    
    TEST_CHECK(list_size(l) == 101);
    TEST_CHECK(l->tail->value == (void*)100);
    TEST_CHECK(list_get(l, 0) == (void*)0);
    TEST_CHECK(list_get(l, 50) == (void*)50);
    for (i = 1; i < 50; i++)
    {
        list_remove(l, 50);
        TEST_CHECK(list_get(l, 50) == (void*)50 + i);
        size_t value = (size_t)list_get(l, 50);
        TEST_CHECK_(value == 50+i, "expected: %lu got: %lu\n",
                    50+i, value);
        TEST_CHECK(list_size(l) == 101 - i);
    }
    free_list(l);
}

void test_remove_pop_error_cases(void)
{
    List* l = new_list();
    TEST_ASSERT(l != NULL);
    TEST_CHECK(list_pop(l) == NULL);
    CHECK_ERROR_STATUS;
    list_remove(l, 0);
    CHECK_ERROR_STATUS;
    list_add(l, (void*)0);
    list_remove(l, 1);
    CHECK_ERROR_STATUS;
    free_list(l);
}

void test_large_add(void)
{
    List* l = new_list();
    TEST_ASSERT(l != NULL);
    size_t i = 0;
    for (; i < 1000000; i++)
    {
        list_add(l, (void*)i);
        TEST_ASSERT(!ERROR_STATUS);
    }
    TEST_CHECK(list_size(l) == 1000000);
    TEST_CHECK(l->jt_size >= 1000);
    TEST_CHECK(l->jump_table[999]->value == (void*)999000);
    free_list(l);
}

void test_pop_effect_on_jt(void)
{
    List* l = new_list();
    TEST_ASSERT(l != NULL);
    size_t i = 0, j = 0;
    for (; i <= 10000; i++)
        list_add(l, (void*)i);
    TEST_CHECK(list_size(l) == 10001);
    for (i = 0; i < 11; i++)
        TEST_CHECK_(l->jump_table[i]->value == (void*)(i * 1000),
                    "expected: %lu got: %lu\n",
                    i * 1000,
                    (size_t)l->jump_table[i]);
    for (j = 10; j > 1; j--)
    {
        for (i = 0; i < 1000; i++)
            list_pop(l);
        TEST_CHECK(l->jump_table[j] == NULL);
        TEST_CHECK(l->jump_table[j-1]->value == (void*)((j-1) * 1000));
        TEST_CHECK(list_get(l, ((j-1)*1000)) == (void*)((j-1)*1000));
    }
    for (i = 0; i < 1000; i++)
        list_pop(l);
    TEST_CHECK(l->jump_table[1] == NULL);
    TEST_CHECK(list_size(l) == 1);
    list_pop(l);
    TEST_CHECK(l->jump_table[0] == NULL);\
    TEST_CHECK(list_size(l) == 0);
    TEST_CHECK(l->head == NULL);
    TEST_CHECK(l->tail == NULL);
    free_list(l);
}

void test_remove_effect_on_jt(void)
{
    List* l = new_list();
    TEST_ASSERT(l != NULL);
    size_t i = 0;
    for (; i <= 10000; i++)
        list_add(l, (void*)i);
    TEST_CHECK(list_size(l) == 10001);

    //remove in the middle.  
    TEST_CHECK(l->jump_table[10] != NULL);
    for (i = 0; i < 1000; i++)
    {
        list_remove(l, 5000);
        TEST_CHECK(list_get(l, 5000) == (void*)5001+i);
        //every jump_table node after the 5th (removing at index 5000)
        //should be moved to the next node in the list.  
        size_t j = 5;
        for (; j < 10; j++)
            TEST_ASSERT_(l->jump_table[j]->value == (void*)(j*1000)+(i+1),
                       "expected: %lu got: %lu\n",
                        (j*1000)+(i+1),
                        (size_t)l->jump_table[j]->value);
    }
    //final node will be removed.  
    TEST_CHECK(l->jump_table[10] == NULL);

    //remove at the begining.  
    TEST_CHECK(l->jump_table[9] != NULL);
    for (i = 0; i < 1000; i++)
    {
        list_remove(l, 0);
        TEST_CHECK(list_get(l, 0) == (void*)i+1);
        TEST_CHECK(l->head->value == (void*)i+1);

        size_t j = 0;
        //First half.  
        for (; j < 4; j++)
            TEST_ASSERT_(l->jump_table[j]->value == (void*)(j*1000)+(i+1),
                        "expected: %lu got: %lu\n",
                        (size_t)l->jump_table[j]->value,
                        (j*1000)+(i+1));
        //Second half should all have values exactly 1000 larger than there index due to previous removes at 5000.  
        j = 5;
        for (; j < 9; j++)
            TEST_ASSERT_(l->jump_table[j]->value == (void*)((j+1)*1000)+(i+1),
                        "expected: %lu got: %lu\n",
                        (size_t)l->jump_table[j]->value,
                        ((j+1)*1000)+(i+1));
    }
    TEST_CHECK(l->jump_table[9] == NULL);

    //remove at end.  
    TEST_CHECK(l->jump_table[8] != NULL);
    for (i = 0; i < 1000; i++)
    {
        list_remove(l, list_size(l)-1);
        TEST_CHECK(list_get(l, list_size(l)-1) == (void*)(10000 - i - 1));
        TEST_CHECK(l->tail->value == (void*)10000 - i - 1);
    }
    TEST_CHECK(l->jump_table[8] == NULL);

    //remove rest.  
    //we've removed 1000, 3 times so remove 7001 more.  
    for (i = 0; i <= 7000; i++)
        list_remove(l, list_size(l)-1);
    TEST_CHECK(list_size(l) == 0);
    TEST_CHECK(l->head == NULL);
    TEST_CHECK(l->tail == NULL);
    for (i = 0; i <= 10; i++)
        TEST_CHECK(l->jump_table[i] == NULL);

    free_list(l);
}

void test_random_remove_get(void)
{
    List* l = new_list();
    TEST_ASSERT(l != NULL);
    size_t i = 0;
    for (; i <= 10000; i++)
        list_add(l, (void*)i);

    for (i = 0; i < 10000; i++)
    {
        //check that after a remove the same index will
        //have the value of the ->next of the previous node
        size_t index = rand() % (10000 - i);
        _ListNode* current_node = _list_pointer_at(l, index);
        size_t expected_value = (size_t)current_node->next->value;
        list_remove(l, index);
        size_t new_value = (size_t)list_get(l, index);
        TEST_CHECK_(expected_value == new_value,
                    "expected: %lu got: %lu\n",
                    expected_value,
                    new_value);
    }
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
    free_list(l);
}

void test_basic_insert(void)
{
    
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
    //{"List sorting", test_sorting},
    //{"List sorting - random value", test_sorting_random},
    {NULL, NULL}
};

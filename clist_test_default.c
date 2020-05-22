#include <../acutest/include/acutest.h>
#include <clist.h>

void test_constants(void) {
    TEST_CHECK(_INITIAL_JUMP_TABLE_SIZE == 10);
    TEST_CHECK(_JUMP_TABLE_INCREMENT == 1000);
}

void test_new_list_intial_values(void) {
    List* list = new_list();
    TEST_CHECK(list->size == 0);
    TEST_CHECK(list->_head == NULL);
    TEST_CHECK(list->_tail == NULL);
    free_list(list);
}

void test_get_invalid_index(void)
{
    List* list = new_list();
    TEST_CHECK(list_get(list, 0) == NULL);
    TEST_CHECK(list_get(list, 1) == NULL);
    free_list(list);
}

void test_add_and_get(void)
{
    List* l = new_list();

    list_add(l, (void*)0);
    TEST_CHECK(l->size == 1);
    TEST_CHECK(l->_head->value == (void*)0);
    TEST_CHECK(l->_tail->value == (void*)0);
    TEST_CHECK(l->_head == l->_tail);
    TEST_CHECK(list_get(l, 0) == (void*)0);

    size_t i; for (i = 1; i < 10000; i++)
    {
        list_add(l, (void*)i);
        TEST_CHECK(l->size == i+1);
        TEST_CHECK(l->_head->value == (void*)0);
        TEST_CHECK(l->_tail->value == (void*)i);
        TEST_CHECK(l->_head != l->_tail);
        TEST_CHECK(list_get(l, i) == (void*)i);
    }

    for (i = 9; i > 0; i--) {
        TEST_CHECK(l->_jump_table[i]->value == (void*)(i * 1000));
    }
    free_list(l);
}

void test_sorting(void)
{
    List* l = new_list();
    long i = 9999;
    for (; i >= 0; i--)
    {
        list_add(l, (void*)i);
        TEST_CHECK_(l->size == 10000 - i, "expected: %lu got:%lu\n",
                    l->size, 10000 - i);
        TEST_CHECK_(l->_head->value == (void*)i,
                    "expected: %lu got: %lu\n", i, (size_t)l->_head->value);
        TEST_CHECK(l->_tail->value == (void*)9999);
        //no _head->next if there is only one entry
        if (i < 9999) { TEST_CHECK(l->_head->next->value == (void*)i+1); }
    }

    for (i = 9; i >= 1; i--) {
        TEST_CHECK_(l->_jump_table[i]->value == (void*)(i * 1000),
                    "expected: %lu got: %lu\n",
                    (size_t)(i * 1000),
                    (size_t)l->_jump_table[i]->value);
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

TEST_LIST = {
    {"Constant values", test_constants},
    {"New list has correct intial values", test_new_list_intial_values},
    {"Trying to get an invalid index, is an error", test_get_invalid_index},
    {"Basic add and get checks", test_add_and_get},
    {"List sorting", test_sorting},
    {"List sorting - random value", test_sorting_random},
    {NULL, NULL}
};

#include <../acutest/include/acutest.h>
#include <clist.h>

void test_error_macros(void) {
    TEST_CHECK(_LIST_BASE_SIZE == 10000);
    TEST_CHECK(_JUMP_TABLE_INCREMENT == 1000);
}

void test_new_list_intial_values(void) {
    List* list = new_list();
    _ListEntry* current = list->_head;
    size_t i; for (i = 0; i < _LIST_BASE_SIZE; i++) {
        TEST_CHECK(current->value == 0);
        TEST_CHECK(current->next != NULL);
        current = current->next;
    }
    TEST_CHECK(current->value == 0);
    TEST_CHECK_(current->next == NULL, "actual: %p\n", current->next);
    free_list(list);
}

TEST_LIST = {
    {"Error Macros", test_error_macros},
    {"New list has correct intial values", test_new_list_intial_values},
    {NULL, NULL}
};

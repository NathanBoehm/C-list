#include <acutest/include/acutest.h>
#include <clist.h>

void test_error_macros(void) {
    TEST_CHECK(LIST_MEM == -1);
}

TEST_LIST = {
    {"Error Macros", test_error_macros},
    {NULL, NULL}
};

#include <../acutest/include/acutest.h>
#include <custom_free.h>

//operating on good faith... 
int get_int(List* l, list_index_t index)
{
    return list_get(l, index).i;
}

//operating on good faith... 
List* get_list(List* l, list_index_t index)
{
    return list_get(l, index).l;
}

void test_basic1(void)
{
    List* l = new_list();
    list_add(l, Data_t(new_list(), 0));
    list_add(l, Data_t(new_list(), 0));
    list_add(l, Data_t(new_list(), 0));

    //add to first sublist.  
    list_add(get_list(l, 0), Data_t(NULL, 0));
    list_add(get_list(l, 0), Data_t(NULL, 1));
    list_add(get_list(l, 0), Data_t(NULL, 2));

    //add to second sublist.  
    list_add(get_list(l, 1), Data_t(NULL, 3));
    list_add(get_list(l, 1), Data_t(NULL, 4));
    list_add(get_list(l, 1), Data_t(NULL, 5));

    //add to third sublist.  
    list_add(get_list(l, 2), Data_t(NULL, 6));
    list_add(get_list(l, 2), Data_t(NULL, 7));
    list_add(get_list(l, 2), Data_t(NULL, 8));

    //check first list.  
    List* list1 = get_list(l, 0);
    TEST_CHECK(get_int(list1, 0) == 0);
    TEST_CHECK(get_int(list1, 1) == 1);
    TEST_CHECK(get_int(list1, 2) == 2);

    //check second list.  
    List* list2 = get_list(l, 1);
    TEST_CHECK(get_int(list2, 0) == 3);
    TEST_CHECK(get_int(list2, 1) == 4);
    TEST_CHECK(get_int(list2, 2) == 5);

    //check third list.  
    List* list3 = get_list(l, 2);
    TEST_CHECK(get_int(list3, 0) == 6);
    TEST_CHECK(get_int(list3, 1) == 7);
    TEST_CHECK(get_int(list3, 2) == 8);

    custom_list_free(l);
}

TEST_LIST = {
    {"3 list with 3 ints each", test_basic1},
    {NULL, NULL}
};
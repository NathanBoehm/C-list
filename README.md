
# C-list
C-list is a simple, header-only linked list module with constant access time whose API includes standard operations: get, insert, remove, add, pop, etc.

# Usage
To start using, put
```C
#define LIST_DATA_TYPE <your list type>
#define ERROR_RETURN_VALUE <value to be returned on error>
```
before including clist.h

**note**: 
if the above values are not defined, then they default to void* and NULL respectively.

### optional:
If your data type requires a custom comparison function to be sorted, 
you can add
```C
#define LIST_COMPARATOR <your comparison function> 
```
before including clist.h

If you are adding dynamically allocated pointers to the list and would like them to be freed when the list is freed, put
```C
#define FREE_LIST_ITEMS 1
```
before including clist.h as well.


## Example
```C
#define  LIST_DATA_TYPE  long
#define  ERROR_RETURN_VALUE  -1

#include  "../include/clist.h"

List* l = new_list();
list_add(l, 0);
list_add(l, 1);
long one = list_get(l, 1);
...
```
**note**:
For more examples, see the files under the examples directory or the unit-tests under the test directory. An example of using a union/struct combination to store multiple data types and/or a list of lists is included in examples/custom_free.h and test/custom_free_test.c
# Definitions
| Type | Name | Defintion | Description |
| ------------- | ------------- | ------------- | ------------- |
| #define | LIST_DATA_TYPE | user set or void* | Value type to be stored in the list. |
| #define | ERROR_RETURN_VALUE | user set or NULL | Value to be return when the list encounters an error. |
| #define | LIST_COMPARATOR | user set or _default_less_than {return a < b} | Function used to compare two list elements for sorting. |
| #define | FREE_LIST_ITEMS | user set (1) or 0. | Determines whether or not list elements will be freed along with the list. |
| enum | JT_INCREMENT | 1000 | The number of elements between each jump_table node (allows for constant [bounded] access times). Lower values will allow for faster access but will require more memory. Not intended to be changed, but can be modified by editing clist.h |
| enum | INTIAL_JT_SIZE | 10 | initial size of the jump_table, allocates space for 10 nodes each time a new list is created. |
| typedef | struct list | List | List structure. Do not modify internal contents. |
| typedef | list_index_t | unsigned long | Default list indexing/size type. |
| typedef | filter_function | int (\*) (LIST_DATA_TYPE) | TBD |
| typedef | err_handler_ft | int (\*) (char\*, char*, char*) | Error handler function signature. |
| typedef | comparator_func | int (\*) (LIST_DATA_TYPE, LIST_DATA_TYPE) | Comparison function signature for use when sorting the list. |


# API Functions

| Name | Parameters | Return | Description | Notes
| ------------- | ------------- | ------------- | ------------- | ------------- |
| new_list(void) | void | List* | Returns a newly allocated list on success or NULL if memory allocation failed. | User must free with free_list if the value returned is not NULL. Does not call the list_error_handler function. |
| free_list(List*) | List*: list structure to be freed. | void | Frees the memory associated with the List* | List* must have been allocated with new_list(). |
| list_size(List*) | List*: list structure to get the size of. | LIST_DATA_TYPE | Returns the number of elements in the list | |
| list_add(List*, LIST_DATA_TYPE) | List*: list structure to be added to. LIST_DATA_TYPE: value to add. | void | Adds the given value to the given list. Calls list_error_handler if there is a memory allocation error. | user must free the list on a memory allocation error. |
| list_pop(List*) | List*: list to be popped. | LIST_DATA_TYPE | Removes the last node from the list and returns its value. | If the list has no items to pop, calls list_error_handler and returns ERROR_RETURN_VALUE. |
| list_get(List*,  list_index_t) | List*: list to retrieve from. list_index_t: index location to retrieve from. | LIST_DATA_TYPE | Returns the value at the given index. | If the index is invalid, calls list_error_handler and returns ERROR_RETURN_VALUE. |
| list_insert(List*,  list_index_t,  LIST_DATA_TYPE) | List*: list to insert into. list_index_t: location to insert at. LIST_DATA_TYPE: value to insert. | void | Inserts the given value at the specified position in the list. | Calls list_error_handler if the index is out of range. |
| list_remove(List*,  list_index_t) | List*: list to remove from. list_index_t: location to remove at. | LIST_DATA_TYPE | Removes the list entry at the given index and returns its value. | If the index is invalid, calls list_error_handler and returns ERROR_RETURN_VALUE. |
| sort_list(List*) | List*: list to be sorted. | void | Sorts the given list. | |
| list_error_handler(err_handler_ft) | err_handler_ft: function to be set as the list error handler or NULL. | err_handler_ft | If the argument is not NULL, sets the list_error_handler function to be called when the list encounters an error. Returns the current list_error_handler | |

# Notes
-  Internal functions/definitions are pretended with an underscore. These are not intended for use outside of the implementations of API functions.
- Both the internal contents of the list struct and the _list_node struct used to contain elements for the list are not intended to be accessed by the user. Modifying their contents is likely to break the list.

## TODO
 - [ ] Optimization for constant iteration time/faster accesses with nearby indices.
 - [ ] Linq-like API functions
# Hash Table

Simple implementation of a hash table in C as a single header file.

## Example usage (as provided in main.c):
```C
#define HASH_TABLE_IMPL
#include "hash_table.h"

#define INITIAL_CAPACITY 16

void ht_print_strings(const ht_t* table) {
    static size_t count = 0;
    printf("\n\033[0;32mTable contents (%lu):\033[0m\n", count + 1);
    for(size_t i = 0; i < table->capacity; i++ ) {
        if(table->items[i] != NULL)
            printf("%s\n", (char*) table->items[i]->data);
    }
    count++;
}

int main(void) {
    // Initialize hash table
    ht_t* some_table = ht_create_table(INITIAL_CAPACITY);

    // Insert some strings
    ht_insert(some_table,"Juliett", "bar", sizeof(char) * 4);
    ht_insert(some_table,"Sierra", "foo", sizeof(char) * 4);
    ht_insert(some_table, "Foxtrot", "baz", sizeof(char) * 4);
    ht_print_strings(some_table);

    // Overwrite the first string with a new one
    ht_insert(some_table,"Sierra", "qux", sizeof(char) * 4);
    ht_print_strings(some_table);

    // Delete the second string
    ht_delete(some_table, "Juliett");
    ht_print_strings(some_table);

    // Free everything
    ht_destroy_table(some_table);
    return EXIT_SUCCESS;
}
```

#pragma once
#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Hash_table_item {
    char* key;
    void* data;
} ht_item_t;

typedef struct Hash_table {
    ht_item_t** items;
    size_t capacity; // Size of the items array
    size_t length; // Number of items in the hash table
} ht_t;

ht_t* ht_create_table(const size_t capacity);
void ht_destroy_table(ht_t* table);
bool ht_insert(ht_t* table, const char *key, const void* data, const size_t bytes);
void ht_delete(ht_t* table, const char *key);
void* ht_search(const ht_t* table, const char *key);

//////////////////////////
/// INTERNAL FUNCTIONS ///
//////////////////////////

uint64_t fnv_1a_hash(const char* key);
ht_item_t* ht_create_item(const char *key, const void* data, const size_t bytes);
void ht_destroy_item(ht_item_t* item);
bool ht_expand_table(ht_t* table);

#endif // HASH_TABLE_H

#ifdef HASH_TABLE_IMPL
#include <stdio.h>
#include <string.h>

// Start with an initial value of FNV_OFFSET_BASIS. For each input byte,
// multiply the hash by FNV_PRIME and XOR it with the input byte
#define FNV_OFFSET_BASIS 0xcbf29ce484222325
#define FNV_PRIME 0x100000001b3

inline uint64_t fnv_1a_hash(const char* key) {
    uint64_t hash = FNV_OFFSET_BASIS;
    for(const char* ptr = key; *ptr; ptr++) {
        hash ^= *ptr;
        hash *= FNV_PRIME;
    }
    return hash;
}

inline ht_item_t* ht_create_item(const char *key, const void* data, const size_t bytes) {
    ht_item_t* item = malloc(sizeof(ht_item_t));
    if(item == NULL)
        return NULL;

    item->key = malloc(strlen(key) + 1);
    if(item->key == NULL) {
        free(item);
        item = NULL;
        return NULL;
    }

    item->data = malloc(bytes);
    if(item->data == NULL) {
        free(item->key);
        item->key = NULL;
        free(item);
        item = NULL;
        return NULL;
    }

    strcpy(item->key, key);
    memcpy(item->data, data, bytes);
    return item;
}

inline void ht_destroy_item(ht_item_t* item) {
    free(item->key);
    item->key = NULL;
    free(item->data);
    item->data = NULL;
    free(item);
    item = NULL;
}

inline ht_t* ht_create_table(const size_t capacity) {
    ht_t* new_table = malloc(sizeof(ht_t));
    if(new_table == NULL)
        return NULL;

    new_table->capacity = capacity;
    new_table->length = 0;
    new_table->items = malloc(new_table->capacity * sizeof(ht_item_t));

    for(size_t i = 0; i <  new_table->capacity; i++)
        new_table->items[i] = NULL;

    return new_table;
}


inline void ht_destroy_table(ht_t* table) {
    for(size_t i = 0; i < table->capacity; i++) {
        ht_item_t* temp = table->items[i];
        if(temp != NULL) {
            ht_destroy_item(temp);
        }
    }

    free(table->items);
    table->items = NULL;
    free(table);
    table = NULL;
}

inline bool ht_expand_table(ht_t* table) {
    const size_t new_capacity = table->capacity * 2;
    ht_item_t** new_items = realloc(table->items, new_capacity * sizeof(ht_item_t*));
    if(new_items == NULL)
        return false;

    table->items = new_items;
    for(size_t i = table->capacity; i < new_capacity; i++)
        table->items[i] = NULL;

    table->capacity = new_capacity;
    return true;
}

inline bool ht_insert(ht_t* table, const char *key, const void* data, const size_t bytes) {
    ht_item_t* new_item = ht_create_item(key, data, bytes);
    const uint64_t hash = fnv_1a_hash(key);
    // If there is no space left, attempt to expand the hash table
    if(table->length == table->capacity) {
        // if(!ht_expand_table(table)) { // Resizing currently not supported since it breaks the hashing, will revisit
            fprintf(stderr, "Insertion failed: insufficient memory");
            ht_destroy_item(new_item);
            return false;
        // }
    }

    size_t index = hash & (table->capacity - 1); // Make sure the index is not out of bounds
    // If the index points to an empty slot, insert the newly created item there
    if(table->items[index] == NULL) {
        table->items[index] = new_item;
        table->length++;
    } else {
        // If the index points to an occupied slot, and the slot's key is the same as the specified one for the
        // insertion, overwrite the item at the slot with the newly created one
        if(strcmp(table->items[index]->key, key) == 0) {
            ht_destroy_item(table->items[index]);
            table->items[index] = new_item;
            return true;
        }

        // If the index points to an occupied slot with a key different to the specified one for the insertion,
        // find an empty slot to insert the newly created item in
        while(table->items[index] != NULL) {
            index++;
            // Wrap around the array if the end is reached
            if(index >= table->capacity)
                index = 0;
        }
        table->items[index] = new_item;
        table->length++;
    }

    return true;
}

inline void ht_delete(ht_t* table, const char *key) {
    const uint64_t hash = fnv_1a_hash(key);
    const size_t index = hash & (table->capacity - 1); // Make sure the index is not out of bounds
    ht_item_t* temp = table->items[index];
    if(temp == NULL)
        return;

    table->items[index] = NULL;
    ht_destroy_item(temp);
    table->length--;
}

inline void* ht_search(const ht_t* table, const char *key) {
    const uint64_t hash = fnv_1a_hash(key);
    const size_t index = hash & (table->capacity - 1); // Make sure the index is not out of bounds
    const ht_item_t* item = table->items[index];
    if(item != NULL)
        return item->data;

    return NULL;
}

#endif // HASH_TABLE_IMPL
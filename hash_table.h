#pragma once
#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdlib.h>
#include <stdint.h>

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
void ht_insert(ht_t* table, const char *key, const void* data, const size_t bytes);
void ht_delete(ht_t* table, const char *key);
void* ht_search(const ht_t* table, const char *key);

//////////////////////////
/// INTERNAL FUNCTIONS ///
//////////////////////////

uint64_t fnv_1a_hash(const char* key);
ht_item_t* ht_create_item(const char *key, const void* data, const size_t bytes);
void ht_destroy_item(ht_item_t* item);

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

inline void ht_insert(ht_t* table, const char *key, const void* data, const size_t bytes) {
    ht_item_t* new_item = ht_create_item(key, data, bytes);
    const uint64_t hash = fnv_1a_hash(key);
    size_t index = hash & (table->capacity - 1); // Make sure the hash is not out of bounds
    if(table->length == table->capacity) {

        // TODO: Implement table expansion

        fprintf(stderr, "Insertion failed: hash table is full");
        ht_destroy_item(new_item);
        return;
    }

    while(table->items[index] != NULL) {
        if(strcmp(key, table->items[index]->key) == 0) {
            // If the key already exists, just update the data
            ht_destroy_item(table->items[index]);
            table->items[index] = new_item;
            return;
        }

        // Wrap around the array if the end is reached
        if(index >= table->capacity)
            index = 0;
    }

    table->items[index] = new_item;
    table->length++;
}

inline void ht_delete(ht_t* table, const char *key) {
    const uint64_t hash = fnv_1a_hash(key);
    const size_t index = hash & (table->capacity - 1); // Make sure the hash is not out of bounds
    ht_item_t* temp = table->items[index];
    if(temp == NULL)
        return;

    table->items[index] = NULL;
    ht_destroy_item(temp);
    table->length--;
}

inline void* ht_search(const ht_t* table, const char *key) {
    const uint64_t hash = fnv_1a_hash(key);
    const size_t index = hash & (table->capacity - 1); // Make sure the hash is not out of bounds
    const ht_item_t* item = table->items[index];
    if(item != NULL)
        return item->data;

    return NULL;
}

#endif // HASH_TABLE_IMPL
/**
The MIT License (MIT)
Copyright (c) 2015 Giannis Vrentzos <gvre@gvre.gr>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "hashtable.h"

/**
 * Create and initialize a hashtable
 *
 * @param uint32_t size Total buckets (must be power of 2)
 * @param size_t (*fn)(const char *key) Pointer to the hash function
 *
 * @return hashtable * Pointer to a newly created hashtable on success, NULL on memory allocation failure
 */
hashtable * hashtable_create(uint32_t size, size_t (*fn)(const char *key))
{
    assert(!(size & 1));

    hashtable *ht = malloc(sizeof *ht);
    if (ht == NULL) {
        perror("Could not allocate memory");
        return NULL;
    }

    ht->size = size;
    ht->fn = fn;
    ht->buckets = malloc(size * sizeof(hashtable_item **));
    if (ht->buckets == NULL) {
        perror("Could not allocate memory");
        return NULL;
    }
    memset(ht->buckets, 0, size * sizeof(hashtable_item **));
    ht->nitems = 0;

    return ht;
}

/**
 * Insert a key, value pair in the hashtable
 *
 * @param hashtable *ht Pointer to the hashtable
 * @param const char *key
 * @param void *value
 * @param uint8_t free_on_erase Releases memory of value pointer on erase
 *
 * @return hashtable_item * Pointer to the newly created item on success, NULL on memory allocation failure or if key exists
 */
hashtable_item * hashtable_insert(hashtable *ht, const char *key, void *value, uint8_t free_on_erase)
{
    uint32_t idx = ht->fn(key) & (ht->size - 1); 
    hashtable_item *ret;

    if (ht->buckets[idx] == NULL) {
        ht->buckets[idx] = malloc(sizeof *ht->buckets[idx]);
        if (ht->buckets[idx] == NULL) {
            perror("Could not allocate memory");
            return NULL;
        }
        ht->buckets[idx]->key = key;
        ht->buckets[idx]->value = value;
        ht->buckets[idx]->free_on_erase = free_on_erase;
        ht->buckets[idx]->next = NULL;

        ret = ht->buckets[idx];
    } else {
        size_t keylen = strlen(key);
        hashtable_item *previous, *current = ht->buckets[idx];
        while (current) {
            if (memcmp((void *)current->key, (void *)key, keylen) == 0)
                return NULL;
            previous = current;
            current = current->next;
        }

        hashtable_item *it = malloc(sizeof *it);
        if (it == NULL) {
            perror("Could not allocate memory");
            return NULL;
        }
        it->key = key;
        it->value = value;
        it->free_on_erase = free_on_erase;
        it->next = NULL;
        previous->next = it;

        ret = it;
    }
    ht->nitems++;

    return ret;
}

/**
 * Get value from the hashtable
 *
 * @param hashtable *ht
 * @param const char *key
 *
 * @return void * Pointer to value if key exists, otherwise NULL
 */
void * hashtable_get(hashtable *ht, const char *key)
{
    size_t keylen = strlen(key);
    uint32_t idx = ht->fn(key) & (ht->size - 1);
    hashtable_item *current = ht->buckets[idx];
    
    while (current) {
        if (memcmp((void *)current->key, (void *)key, keylen) == 0)
            return current->value;
        current = current->next;
    }

    return NULL;
}

/**
 * Set value for the specified key
 *
 * @param hashtable *ht
 * @param const char *key
 * @param void *value
 * @param uint8_t free_on_erase Releases memory of value pointer on erase
 *
 * @return hashtable_item * Pointer to value if key exists, otherwise NULL
 */
hashtable_item * hashtable_set(hashtable *ht, const char *key, void *value, uint8_t free_on_erase)
{
    size_t keylen = strlen(key);
    uint32_t idx = ht->fn(key) & (ht->size - 1);
    hashtable_item *current = ht->buckets[idx];
    
    while (current) {
        if (memcmp((void *)current->key, (void *)key, keylen) == 0) {
            if (current->free_on_erase)
                free(current->value);
            current->free_on_erase = free_on_erase;
            return current->value = value;
        }
        current = current->next;
    }

    return NULL;
}

/**
 * Erase an item with the specified key
 *
 * @param hashtable *ht
 * @param const char *key
 *
 * @return size_t 1 if key exists, otherwise 0
 */
size_t hashtable_erase(hashtable *ht, const char *key)
{
    uint32_t idx = ht->fn(key) & (ht->size - 1); 
    hashtable_item *current = ht->buckets[idx];
    if (current == NULL)
        return 0;

    size_t keylen = strlen(key);
    hashtable_item *previous = current; 
    while (current) {
        if (memcmp((void *)current->key, (void *)key, keylen) == 0) {
            if (current->free_on_erase)
                free(current->value);

            if (current == ht->buckets[idx]) {
                /* 1st item */
                if (current->next == NULL) {
                    /* list has 1 item */
                    free(current);
                    ht->buckets[idx] = NULL;
                } else {
                    /* list has many items */
                    ht->buckets[idx] = current->next;
                    free(current);
                }
            } else {
                previous->next = current->next;
                free(current);
            }
            
            ht->nitems--;
            return 1;
        }
        previous = current;
        current = current->next;
    }

    return 0;
}

/**
 * Deallocate the hashtable
 *
 * @param hashtable *ht
 *
 * @return void
 */
void hashtable_clear(hashtable *ht)
{
    hashtable_item *next, *current;
    for (uint32_t i = 0; i < ht->size; i++) {
        current = ht->buckets[i];
        while (current) {
            next = current->next;
            if (current->free_on_erase)
                free(current->value);
            free(current);
            current = next;
        }
    }

    free(ht->buckets);
    free(ht);
}

/**
 * Count hashtable's items
 *
 * @param const hashtable *ht
 *
 * @return size_t
 */
size_t hashtable_count_items(const hashtable *ht)
{
    return ht->nitems;
}

/**
 * Apply fn callback to each item. Stops if callback returns 0
 *
 * @param const hashtable *ht
 * @param size_t (*fn)(const char *key, void *value) Pointer to the callback function
 *
 * @return void
 */
void hashtable_foreach(const hashtable *ht, size_t (*fn)(const char *key, void *value))
{
    hashtable_item *current;
    for (uint32_t i = 0; i < ht->size; i++) {
        current = ht->buckets[i];
        while (current) {
            if (!fn(current->key, current->value))
                return;
            current = current->next;
        }
    }
}

/**
 * Get hashtable's keys
 * 
 * @param const hashtable *ht
 * @param size_t *nkeys
 *
 * @return const char **
 */
const char **hashtable_keys(const hashtable *ht, size_t *nkeys)
{
    size_t j = 0;     
    const char **keys = malloc(hashtable_count_items(ht) * sizeof *keys);
    if (keys == NULL) {
        *nkeys = 0;
        perror("Could not allocate memory");
        return NULL;
    }

    hashtable_item *current;
    for (uint32_t i = 0; i < ht->size; i++) {
        current = ht->buckets[i];
        while (current) {
            keys[j++] = current->key;
            current = current->next;
        }
    }
    *nkeys = j;
    return keys;
}


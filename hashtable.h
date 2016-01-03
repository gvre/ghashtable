#ifndef HASHTABLE_H
#define HASHTABLE_H
typedef struct h_item {
    const char *key;
    void *value;
    struct h_item *next;
    void (*dealloc_fn)(void *it);
} hashtable_item;

typedef struct h_table {
    uint32_t size;
    size_t (*fn)(const char *key);
    size_t nitems;
    hashtable_item **buckets;
} hashtable;

hashtable * hashtable_create(uint32_t size, size_t (*fn)(const char *key));
hashtable_item * hashtable_insert(hashtable *ht, const char *key, void *value, void (*dealloc_fn)(void *it));
void * hashtable_get(hashtable *ht, const char *key);
hashtable_item * hashtable_set(hashtable *ht, const char *key, void *value, void (*dealloc_fn)(void *it));
size_t hashtable_erase(hashtable *ht, const char *key);
void hashtable_clear(hashtable *ht);
size_t hashtable_count_items(const hashtable *ht);
void hashtable_foreach(const hashtable *ht, size_t (*fn)(const char *key, void *value));
const char **hashtable_keys(const hashtable *ht, size_t *nkeys);
#endif


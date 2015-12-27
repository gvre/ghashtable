#include <stdio.h>

#include "hash.h"

/* Jenkins hash function */
size_t hash(const char *key)
{
    size_t hash, i;
    for (hash = i = 0; key[i]; ++i) {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}


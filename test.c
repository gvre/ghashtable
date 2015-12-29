/**
The MIT License (MIT)
Copyright (c) 2015 Giannis Vrentzos <gvre@gvre.gr>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "hash.h"
#include "hashtable.h"

int *numptr(void);
int *numptr(void) { return malloc(sizeof(int)); }

char *charprt(void);
char *charprt(void) { return malloc(sizeof(char)); }

struct foo {
    char field[12];
};

struct foo* structptr(void);
struct foo* structptr(void) { 
    struct foo *f = malloc(sizeof *f);
    strcpy(f->field, "hello world");

    return f;
}

int main(void)
{
    hashtable *ht = hashtable_create(32, hash);
    assert(ht != NULL);

    hashtable_insert(ht, "key", "val", 0);
    assert(strcmp((char *)hashtable_get(ht, "key"), "val") == 0);

    assert(hashtable_insert(ht, "key", "val", 0) == NULL);

    assert(hashtable_set(ht, "key", "val2", 0) != NULL);
    assert(strcmp((char *)hashtable_get(ht, "key"), "val2") == 0);

    assert(hashtable_count_items(ht) == 1);
    assert(hashtable_erase(ht, "key") == 1);
    assert(hashtable_count_items(ht) == 0);

    hashtable_insert(ht, "numptr", numptr(), 1);
    hashtable_insert(ht, "charprt", charprt(), 1);
    hashtable_insert(ht, "structptr", structptr(), 1);
    assert(hashtable_count_items(ht) == 3);

    size_t nkeys;
    const char **keys = hashtable_keys(ht, &nkeys);
    free(keys);
    assert(nkeys == 3);

    hashtable_clear(ht);

    return EXIT_SUCCESS;
}


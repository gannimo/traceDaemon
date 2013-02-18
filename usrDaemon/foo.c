#include "avl.h"

#include <stdlib.h>
#include <stdio.h>

static long compare(void *left, void *right) {
    return (long)left - (long)right;
}

int main() {
    struct avl_node *foo = NULL;
    long i;
    for (i = 0; i < 15; i++) {
        foo = avl_insert(foo, (void*)i, compare);
        printf("inserting %d\n", i);
    }
    struct avl_node *bar = NULL;
    bar = avl_find(foo, (void*)2, compare);
    printf("%p\n", bar);
    return 0;
}

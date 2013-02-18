#include "avl.h"
#include "gtest/gtest.h"

static long compare(void *left, void *right) {
    return (long)left - (long)right;
}

TEST(AVLTest, Init) {
    struct avl_node *root = NULL;
    long i;
    for (i = 0; i < 255; i++)
        root = avl_insert(root, (void*)i, compare);

    EXPECT_TRUE((long)(avl_find(root, (void*)2, compare)->data) == 2);
    EXPECT_TRUE(avl_find(root, (void*)255, compare) == NULL);
}

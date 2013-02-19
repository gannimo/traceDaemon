/**
 * @file avl_test.cc
 * A set of unit tests that check the implementation of the AVL tree.
 *
 * Copyright (c) 2013 UC Berkeley
 * @author Mathias Payer <mathias.payer@nebelwelt.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 */

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

    for (i = 0; i < 255; i++)
        root = avl_delete(root, (void*)i, compare);

    EXPECT_TRUE(avl_find(root, (void*)2, compare) == NULL);
    EXPECT_TRUE(root == NULL);

}

TEST(AVLTest, InsertSearchDelete) {
    /* Insert, Search, and Delete a bunch of entries in the AVL tree */
    struct avl_node *root = NULL;
    long i;
    for (i = 0; i <= 255; i++)
        root = avl_insert(root, (void*)i, compare);
    for (i = 1024; i > 768; i--)
        root = avl_insert(root, (void*)i, compare);
    
    for (i = 256; i <= 512; i++) {
        root = avl_insert(root, (void*)i, compare);
        root = avl_insert(root, (void*)(i+256), compare);
    }
    
    for (i = 256; i < 512; i++) {
        root = avl_delete(root, (void*)i, compare);
        root = avl_delete(root, (void*)(i+256), compare);
    }

    EXPECT_TRUE(avl_find(root, (void*)256, compare) == NULL);
    EXPECT_TRUE(avl_find(root, (void*)512, compare) == NULL);

    for (i = 256; i < 512; i++) {
        root = avl_insert(root, (void*)i, compare);
        root = avl_insert(root, (void*)(i+256), compare);
    }

    EXPECT_TRUE((long)(avl_find(root, (void*)2, compare)->data) == 2);
    EXPECT_TRUE(avl_find(root, (void*)1025, compare) == NULL);

    for (i = 0; i <= 1024; i++) {
        EXPECT_TRUE((long)(avl_find(root, (void*)i, compare)->data) == i);
        root = avl_delete(root, (void*)i, compare);
    }

    EXPECT_TRUE(avl_find(root, (void*)2, compare) == NULL);
    EXPECT_TRUE(root == NULL);
}

/**
 * @file avl.c
 * A simple AVL search tree implementation.
 * Based on the algorithm description on Wikipedia and
 * http://anoopsmohan.blogspot.com/2011/11/avl-tree-implementation-in-c.html
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

#include <stdlib.h>
#include <stdio.h>

unsigned int avl_height(struct avl_node *node) {
    unsigned long height = 0;
    if (node != NULL) {
        unsigned long lheight = avl_height(node->left);
        unsigned long rheight = avl_height(node->right);
        height = 1 + ((lheight > rheight) ? lheight : rheight);
    }
    return height;
}

int avl_hdiff(struct avl_node *node) {
    return avl_height(node->left) - avl_height(node->right);
}

struct avl_node *avl_rrro(struct avl_node *parent) {
    struct avl_node *node = parent->right;
    parent->right = node->left;
    node->left = parent;
    return node;
}

struct avl_node *avl_llro(struct avl_node *parent) {
    struct avl_node *node = parent->left;
    parent->left = node->right;
    node->right = parent;
    return node;
}

struct avl_node *avl_rlro(struct avl_node *parent) {
    struct avl_node *node = parent->right;
    parent->right = avl_llro(node);
    return avl_rrro(parent);
}

struct avl_node *avl_lrro(struct avl_node *parent) {
    struct avl_node *node = parent->left;
    parent->left = avl_rrro(node);
    return avl_llro(parent);
}

struct avl_node *avl_balance(struct avl_node *node) {
    int height_diff = avl_hdiff(node);
    if (height_diff > 1) {
        node = (avl_hdiff(node->left) > 0) ?
            avl_llro(node) : avl_lrro(node);
    } else
    if (height_diff < -1) {
        node = (avl_hdiff(node->right) > 0) ?
            avl_rlro(node) : avl_rrro(node);
    }
    return node;
}

struct avl_node* avl_insert(struct avl_node *root, void *data, long(*cmp)(void*, void*)) {
    if (root == NULL) {
        root = (struct avl_node*)malloc(sizeof(struct avl_node));
        if (root == NULL) {
            puts("Error in memory allocation\n");
            abort();
        }
        root->data = data;
        root->left = NULL;
        root->right = NULL;
    } else
    if (cmp(data, root->data) < 0) {
        root->left = avl_insert(root->left, data, cmp);
        root = avl_balance(root);
    } else
    if (cmp(data, root->data) > 0) {
        root->right = avl_insert(root->right, data, cmp);
        root = avl_balance(root);
    }
    return root;
}

struct avl_node* avl_find(struct avl_node *node, void *data, long(*cmp)(void*, void*)) {
    long val;
    if (node == NULL) return NULL;
    val = cmp(data, node->data);
    if (val < 0) return avl_find(node->left, data, cmp);
    if (val > 0) return avl_find(node->right, data, cmp);
    return node;
}

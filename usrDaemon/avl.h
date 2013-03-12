/**
 * @file avl.h
 * A simple AVL search tree.
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

#ifndef AVL_H
#define AVL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Simple node in an AVL tree.
 */
struct avl_node {
    /** left side of the tree */
    struct avl_node *left;
    /** right side of the tree */
    struct avl_node *right;
    /** pointer to data node */
    void *data;
};

/**
 * Insert an element into the avl tree.
 * This function inserts data into the AVL tree. As a helper we have to pass a
 * cmp compare function.
 * @param root Root of the AVL tree
 * @data pointer to data
 * @cmp function that takes two data pointer and returns a strict ordering
 *      comparing left and right.
 * @return the new root.
 */
struct avl_node* avl_insert(struct avl_node *root, void *data, long(*cmp)(void*, void*));

    
/**
 * Deletes an element from the avl tree.
 * This function deletes data from the AVL tree. As a helper we have to pass a
 * cmp compare function.
 * @param root Root of the AVL tree
 * @data pointer to data
 * @cmp function that takes two data pointer and returns a strict ordering
 *      comparing left and right.
 * @return the new root.
 */
struct avl_node* avl_delete(struct avl_node *root, void *data, long(*cmp)(void*, void*));

/**
 * Searches the given AVL tree for given data.
 * @param node Root of the AVL tree.
 * @param data Data point that we try to find
 * @param cmp Compare function, left arg.: data point, right arg.: current node
 * @return AVL node or NULL
 */
struct avl_node* avl_find(struct avl_node *node, void *data, long(*cmp)(void*, void*));

/**
 * This function destroys the given AVL tree (inorder) and executes dest for
 * each element.
 * @param dest function that is executed for each data element.
 */
void avl_destroy(struct avl_node *node, void (*dest)(void *));

#ifdef __cplusplus
}
#endif

#endif  /* AVL_H */

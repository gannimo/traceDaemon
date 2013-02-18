/**
 * @file td_filestate.c
 * Implementation of the filestate subsystem that dynamically removes races
 * through system call analysis.
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

#include "td_filestate.h"

#include <stdlib.h>
#include <stdio.h>
//#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>

#include "avl.h"

struct avl_node *root_proc = NULL;

static long compare(void *left, void *right) {
    struct td_thread *trl, *trr;
    trl = (struct td_thread*)left;
    trr = (struct td_thread*)right;
    return trl->pid - trr->pid;
}

struct td_thread* process_create(unsigned long pid, unsigned long tid,
                                 unsigned long ppid) {
    struct td_thread *npid;
    
    if ((npid = (struct td_thread*)malloc(sizeof(struct td_thread))) == NULL) {
        puts("td_filestate.c: Unable to allocate memory\n");
        abort();
    }
    npid->pid = pid;
    npid->tid = tid;
    npid->ppid = ppid;
    npid->next_thread = NULL;
    npid->files = NULL;
    avl_insert(root_proc, npid, compare);
    return npid;
}

void handle_syscall(struct td_thread *process, unsigned long syscall, char *file, char *path);

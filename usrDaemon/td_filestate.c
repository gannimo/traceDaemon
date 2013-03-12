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
#include <sys/stat.h>
#include <string.h>

#include "avl.h"
//#include "syscall_nr.h"

struct avl_node *root_proc_tid = NULL;
struct avl_node *root_proc_pid = NULL;

static long compare_proc_tid(void *left, void *right) {
    struct td_thread *trl, *trr;
    trl = (struct td_thread*)left;
    trr = (struct td_thread*)right;
    return trl->tid - trr->tid;
}

static long compare_proc_pid(void *left, void *right) {
    struct td_thread *trl, *trr;
    trl = (struct td_thread*)left;
    trr = (struct td_thread*)right;
    return trl->pid - trr->pid;
}

struct td_thread* find_process(unsigned long tid) {
    struct td_thread proc;
    proc.tid = tid;
    struct avl_node *node = avl_find(root_proc_tid, (void*)(&proc), compare_proc_tid);
    return (node == NULL) ? NULL : node->data;
}

struct td_thread* find_process_pid(unsigned long pid) {
    struct td_thread proc;
    proc.pid = pid;
    struct avl_node *node = avl_find(root_proc_pid, (void*)(&proc), compare_proc_pid);
    return (node == NULL) ? NULL : node->data;
}

struct td_thread* process_create(unsigned long pid, unsigned long tid,
                                 unsigned long ppid) {
    struct td_thread *npid, *proc = NULL;
    if ((npid = (struct td_thread*)malloc(sizeof(struct td_thread))) == NULL) {
        puts("td_filestate.c: Unable to allocate memory\n");
        abort();
    }
    npid->pid = pid;
    npid->tid = tid;
    npid->ppid = ppid;
    npid->next_thread = NULL;
    npid->files = NULL;

    proc = find_process_pid(pid);
    if (proc != NULL) {
        npid->next_thread = proc->next_thread;
        proc->next_thread = npid;
    } else {
        root_proc_pid = avl_insert(root_proc_pid, npid, compare_proc_pid);
    }
    root_proc_tid = avl_insert(root_proc_tid, npid, compare_proc_tid);
    return npid;
}

long process_destroy(unsigned long tid) {
    struct td_thread *proc = find_process(tid);
    if (proc == NULL)
        return -1;
    root_proc_tid = avl_delete(root_proc_tid, (void*)proc, compare_proc_tid);
    // todo: delete if last pid
    //root_proc_pid = avl_delete(root_proc_pid, (void*)proc, compare_proc_pid);

    struct td_thread *pid = find_process_pid(proc->pid);
    
    // check for threads (if so, delete from linked list)
    if (pid != proc || proc->next_thread != NULL) {
        // we delete the root of the linked list
        if (pid == proc) {
            struct td_thread *first = pid->next_thread;
            root_proc_pid = avl_delete(root_proc_pid, (void*)pid, compare_proc_pid); 
            if (first != NULL)
                root_proc_pid = avl_insert(root_proc_pid, (void*)first, compare_proc_pid);
        } else {
            // delete from list
            struct td_thread *tr = pid;
            while (tr->next_thread != NULL) {
                if (tr->next_thread == proc) {
                    tr->next_thread = proc->next_thread;
                    break;
                }
                tr = tr->next_thread;
            }
        }
    } else {
        root_proc_pid = avl_delete(root_proc_pid, (void*)proc, compare_proc_pid); 
    }
    
    // free this process
    free(proc);
    return 0;
}


struct td_file *check_file(struct td_thread *proc, char *file, char *path,
                              struct stat *buf, enum transition next_state);

enum td_syscall_result handle_syscall(unsigned long tid, unsigned long syscall,
                                      char *file, char *path, struct stat *buf) {
    struct td_thread *proc = find_process(tid);
    if (proc == NULL) {
        printf("Could not find pid %ld (unable to handle system call %ld)\n",
               tid, syscall);
        return SYSCALL_PIDERR;
    }

    struct td_file *rc = NULL;
    switch (syscall) {
        SYS_ACCESS: rc = check_file(proc, file, path, buf, TRANS_TEST); break;
        SYS_STAT: rc = check_file(proc, file, path, buf, TRANS_TEST); break;
        SYS_CREAT: rc = check_file(proc, file, path, buf, TRANS_USE); rc->nropen++; break;
        SYS_OPEN: rc = check_file(proc, file, path, buf, TRANS_USE); rc->nropen++; break;
        SYS_CLOSE: rc = check_file(proc, file, path, buf, TRANS_CLOSE); break;        
    }
    return SYSCALL_PASS;
}

static long compare_file(void *left, void *right) {
    struct td_file *trl, *trr;
    trl = (struct td_file*)left;
    trr = (struct td_file*)right;
    return strncmp((const char*)&(trl->name), (const char*)&(trr->name), MAX_FILE_LEN);
}

struct td_file *check_file(struct td_thread *proc, char *file, char *path,
                              struct stat *buf, enum transition next_state) {
    struct td_file loc, *lfile;
    strncpy((char*)&(loc.name), file, MAX_FILE_LEN);

    lfile = (struct td_file*)avl_find(proc->files, (void*)(&loc), compare_file);

    /* we have not seen this file */
    if (lfile == NULL) {
        if ((lfile = (struct td_file*)malloc(sizeof(struct td_file))) == NULL) {
            puts("td_filestate.c: Unable to allocate memory\n");
            abort();
        }
        strncpy((char*)&(lfile->name), file, MAX_FILE_LEN);
        if (strnlen((const char*)&(lfile->name), MAX_FILE_LEN) == MAX_FILE_LEN) {
            printf("td_filestate.c: Input file name too long: '%s'\n", file);
            abort();
        }
        lfile->nropen = 0;
        lfile->fderr = 0;
        memcpy(&(lfile->stat), buf, sizeof(struct stat));
        lfile->state = next_state;

        return lfile;
    }

    /* check existing file according to buf and state */
    switch (lfile->state) {
        
    }
    return lfile;
}

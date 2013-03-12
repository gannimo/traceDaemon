/**
 * @file tD_filestate.h
 * Header functions for the file state subsystem that dynamically removes races
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

#ifndef TD_FILESTATE_H
#define TD_FILESTATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/stat.h>

#define MAX_FILE_LEN 255

enum td_file_state {
  STATE_UPDATE,  /*< file has already been checked (e.g., access'ed) */
  STATE_ENFORCE,  /*< file has been opened using checked data */
  STATE_RETIRE,  /*< file has been closed. might be stale data */
  STATE_DIR_OK,  /*< directory is OK and verified */
  STATE_DIR_ERR  /*< directory state is erroneous */
};

enum td_file_health {
    HEALTH_OK = 1, /*< no race possible */
    HEALTH_UNCHECKED = 2, /*< this file was used once without checking */
    HEALTH_BAD = 3 /*< race possible */
};

/* this struct represents a single file of a thread */
struct td_file {
  enum td_file_state state;  /*< state of the file (in the state machine) */
  enum td_file_health health;  /*< state of the file */
  long nropen; /*< how many times opened in app */
  long fderr;  /*< if !=0 error code (e.g., for illegal files) */
  char name[MAX_FILE_LEN+1];  /*< filename */
  struct stat stat;  /*< stat of the file */
  struct td_file *dir;  /*< descriptor of the dir */
};

struct td_files {
    struct avl_node *tree;
};

struct td_thread {
    unsigned long pid;
    unsigned long tid;
    unsigned long ppid;
    struct td_thread *next_thread;
    struct td_files *files; /*< all files; shared among all threads */
};

enum td_syscall_result {
    SYSCALL_PIDERR, /*< we have no information about the given PID */
    SYSCALL_RACE, /*< race condition detected */
    SYSCALL_UNCHECKED, /*< possible program error (unchecked file usage) */
    SYSCALL_PASS /*< all is fine */
};

enum transition {
    TRANS_TEST, /*< file is checked */
    TRANS_USE, /*< file is used */
    TRANS_CLOSE /*< file is no longer used */
};

/**
 * This function creates all the necessary data structures for
 * a new process.
 * @param pid the process id of the new process
 * @param tid the thread id of the new process
 * @param ppid the process id of the parent
 **/
struct td_thread* process_create(unsigned long pid, unsigned long tid, unsigned long ppid);

/**
 * Destroys an existing process, deletes the process from the
 * global AVL tree and frees all resources.
 * @param tid the tid that is deleted
 * @return 0 on successful deletion or an error code.
 **/
long process_destroy(unsigned long tid);

/**
 * Locates the process in the global process list and returns the associated
 * td_thread struct.
 * @param tid is the thread ID of the process
 * @return returns a pointer to the td_thread struct or NULL
 */
struct td_thread* find_process(unsigned long tid);

/**
 * Handle a system call that is fired from the external side.
 * @param tid Thread ID that executes the current system call.
 * @param syscall Syscall number (as specified in syscall_nr.h)
 * @param file Current file atom
 * @param path Path of the file (might be kicked?)
 * @param buf Result of the kernel-side stat system call (includes verified meta data)
 */
enum td_syscall_result handle_syscall(unsigned long tid, unsigned long syscall,
                                      const char *file, const char *path,
                                      struct stat *buf);

#ifdef __cplusplus
}
#endif

#endif  /* TD_FILESTATE_H */

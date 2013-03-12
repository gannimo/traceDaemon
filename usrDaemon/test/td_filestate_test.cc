/**
 * @file td_filestate_test.cc
 * A set of unit tests that check the implementation of the td_filestate buffer.
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

#include <sys/stat.h>

#include "avl.h"
#include "syscall_nr.h"
#include "td_filestate.h"

#include "gtest/gtest.h"


TEST(TDFilestateTest, Processes) {
    long i;
    // generate a bunch of processes and delete them
    // order of insert is not important, the tree internas are
    // tested in a different test.
    for (i = 0; i < 255; i++) {
        EXPECT_TRUE(process_create(i, i, 0) != NULL);
    }
    
    EXPECT_TRUE(find_process(0) != NULL);
    EXPECT_TRUE(find_process(2) != NULL);
    EXPECT_TRUE(find_process(254) != NULL);
    EXPECT_TRUE(find_process(255) == NULL);

    for (i = 0; i < 255; i++)
        EXPECT_EQ(process_destroy(i), 0);

    EXPECT_TRUE(find_process(0) == NULL);
    EXPECT_TRUE(find_process(2) == NULL);
    EXPECT_TRUE(find_process(254) == NULL);

}

TEST(TDFilestateTest, Threads) {
    // check thread handling (and internal linked list used to keep
    // the list of threads; so we test all different configurations)
    EXPECT_TRUE(process_create(0, 0, 0) != NULL);
    EXPECT_TRUE(process_create(1, 1, 0) != NULL);
    EXPECT_TRUE(process_create(1, 2, 0) != NULL);
    EXPECT_TRUE(process_create(1, 3, 0) != NULL);
    EXPECT_EQ(process_destroy(3), 0);
    EXPECT_EQ(process_destroy(1), 0);
    EXPECT_EQ(process_destroy(2), 0);
    
    EXPECT_TRUE(process_create(1, 1, 0) != NULL);
    EXPECT_TRUE(process_create(1, 2, 0) != NULL);
    EXPECT_TRUE(process_create(1, 3, 0) != NULL);
    EXPECT_EQ(process_destroy(3), 0);
    EXPECT_EQ(process_destroy(2), 0);
    EXPECT_EQ(process_destroy(1), 0);
    
    EXPECT_TRUE(process_create(1, 1, 0) != NULL);
    EXPECT_TRUE(process_create(1, 2, 0) != NULL);
    EXPECT_TRUE(process_create(1, 3, 0) != NULL);
    EXPECT_EQ(process_destroy(1), 0);
    EXPECT_EQ(process_destroy(2), 0);
    EXPECT_EQ(process_destroy(3), 0);

    EXPECT_TRUE(process_create(1, 1, 0) != NULL);
    EXPECT_TRUE(process_create(1, 2, 0) != NULL);
    EXPECT_TRUE(process_create(1, 3, 0) != NULL);
    EXPECT_EQ(process_destroy(1), 0);
    EXPECT_EQ(process_destroy(3), 0);
    EXPECT_EQ(process_destroy(2), 0);

    EXPECT_TRUE(process_create(1, 1, 0) != NULL);
    EXPECT_TRUE(process_create(1, 2, 0) != NULL);
    EXPECT_TRUE(process_create(1, 3, 0) != NULL);
    EXPECT_EQ(process_destroy(2), 0);
    EXPECT_EQ(process_destroy(1), 0);
    EXPECT_EQ(process_destroy(3), 0);

    EXPECT_TRUE(process_create(1, 1, 0) != NULL);
    EXPECT_TRUE(process_create(1, 2, 0) != NULL);
    EXPECT_TRUE(process_create(1, 3, 0) != NULL);
    EXPECT_EQ(process_destroy(2), 0);
    EXPECT_EQ(process_destroy(3), 0);
    EXPECT_EQ(process_destroy(1), 0);

    EXPECT_EQ(process_destroy(0), 0);

}

TEST(TDFilestateTest, SimpleFile) {
    struct stat buf1;
    memset(&buf1, 0, sizeof(struct stat));
    
    EXPECT_TRUE(process_create(0, 0, 0) != NULL);
    EXPECT_TRUE(process_create(1, 1, 0) != NULL);

    EXPECT_EQ(handle_syscall(1, SYS_STAT, "foo", "/", &buf1), SYSCALL_PASS);
    EXPECT_EQ(handle_syscall(1, SYS_OPEN, "foo", "/", &buf1), SYSCALL_PASS);
    EXPECT_EQ(handle_syscall(1, SYS_CLOSE, "foo", "/", &buf1), SYSCALL_PASS);
    
    EXPECT_EQ(process_destroy(1), 0);
    EXPECT_EQ(process_destroy(0), 0);
}

TEST(TDFilestateTest, SimpleFileUnchecked) {
    struct stat buf1;
    memset(&buf1, 0, sizeof(struct stat));
    
    EXPECT_TRUE(process_create(0, 0, 0) != NULL);
    EXPECT_TRUE(process_create(1, 1, 0) != NULL);

    EXPECT_EQ(handle_syscall(1, SYS_OPEN, "foo", "/", &buf1), SYSCALL_UNCHECKED);
    EXPECT_EQ(handle_syscall(1, SYS_CLOSE, "foo", "/", &buf1), SYSCALL_UNCHECKED);
    
    EXPECT_EQ(process_destroy(1), 0);
    EXPECT_EQ(process_destroy(0), 0);
}

TEST(TDFilestateTest, SimpleFileRace) {
    struct stat buf1, buf2;
    memset(&buf1, 0, sizeof(struct stat));
    memset(&buf2, 0, sizeof(struct stat));
    buf2.st_ino = 5;
    
    EXPECT_TRUE(process_create(0, 0, 0) != NULL);
    EXPECT_TRUE(process_create(1, 1, 0) != NULL);

    EXPECT_EQ(handle_syscall(1, SYS_STAT, "foo", "/", &buf1), SYSCALL_PASS);
    EXPECT_EQ(handle_syscall(1, SYS_OPEN, "foo", "/", &buf2), SYSCALL_RACE);
    
    EXPECT_EQ(process_destroy(1), 0);
    EXPECT_EQ(process_destroy(0), 0);
}

/*-
 * Copyright (c) 2009 Simon L. Nielsen <simon@FreeBSD.org>,
 *      Bjoern A. Zeeb <bz@FreeBSD.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"
#include "pritime.h"
#include "test_helper.h"
#include "test_driver.h"

#include <sys/mman.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>

const char rtems_test_name[] = "PSX MMAP01";
const char test_driver_name[] = "/dev/test_driver";

static const struct {
  void  *addr;
  int ok;
} map_at_zero_tests[] = {
  { (void *)0,                    0 },
  { (void *)1,                    0 },
  { (void *)(PAGE_SIZE - 1),      0 },
  { (void *)PAGE_SIZE,            1 },
  { (void *)-1,                   0 },
  { (void *)(-PAGE_SIZE),         0 },
  { (void *)(-1 - PAGE_SIZE),     0 },
  { (void *)(-1 - PAGE_SIZE - 1), 0 },
  { (void *)(0x1000 * PAGE_SIZE), 1 },
};

#define MAP_AT_ZERO_NITEMS (sizeof(map_at_zero_tests) / sizeof(map_at_zero_tests[0]))

static void* checked_mmap(
  int prot,
  int flags,
  int fd,
  int error,
  const char *msg
)
{
  void *p;
  int pagesize, err;

  rtems_test_assert((pagesize = getpagesize()) > 0);
  p = mmap(NULL, pagesize, prot, flags, fd, 0);
  if (p == MAP_FAILED) {
    if (error == 0)
      mmap_test_assert(0, "%s failed with errno %d\n", msg, errno);
    else {
      err = errno;
      mmap_test_assert_equal(error, err,
          "%s failed with wrong errno %d (expected %d)\n", msg,
          errno, error);
    }
  } else {
    mmap_test_check(error == 0, "%s succeeded\n", msg);
    munmap(p, pagesize);
  }
  return p;
}

static void mmap_map_at_zero( void )
{
  void *p;
  unsigned int i;

  for (i = 0; i < MAP_AT_ZERO_NITEMS; i++) {
    p = mmap((void *)map_at_zero_tests[i].addr, PAGE_SIZE,
        PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_FIXED | MAP_PRIVATE,
        -1, 0);
    if (p == MAP_FAILED) {
      mmap_test_assert( map_at_zero_tests[i].ok == 0,
          "mmap(%p, ...) failed\n", map_at_zero_tests[i].addr );
    } else {
      mmap_test_check( map_at_zero_tests[i].ok == 1,
          "mmap(%p, ...) succeeded: p=%p\n",
          map_at_zero_tests[i].addr, p );
    }
  }
}

static void mmap_bad_arguments( void )
{
  int devfd, pagesize, shmfd, zerofd;
  void* p;

  rtems_test_assert((pagesize = getpagesize()) > 0);
  rtems_test_assert((devfd = open(&test_driver_name[0], O_RDONLY)) >= 0);
  rtems_test_assert((shmfd = shm_open("/shm", O_CREAT | O_RDWR, 0644)) >= 0);
  rtems_test_assert(ftruncate(shmfd, pagesize) == 0);
  rtems_test_assert((zerofd = open("/dev/zero", O_RDONLY)) >= 0);

  /*
   * These should normally work on FREEBSD. Test cases below that fail are
   * due to unsupported features in RTEMS.
   */
  checked_mmap(PROT_READ | PROT_WRITE, MAP_ANON, -1, 0,
      "simple MAP_ANON");
  checked_mmap(PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0,
      "simple shm fd shared");
  checked_mmap(PROT_READ | PROT_WRITE, MAP_PRIVATE, shmfd, 0,
      "simple shm fd private");
  /* RTEMS cannot protect against writes so this will fail */
  checked_mmap(PROT_READ, MAP_SHARED, zerofd, ENOTSUP,
      "simple /dev/zero shared");
   /*
    * Repeat with no write protection. Will fail because of unimplemented
    * mmap handler in /dev/zero.
    */
  checked_mmap(PROT_READ | PROT_WRITE, MAP_SHARED, zerofd, ENOTSUP,
      "simple /dev/zero shared");
  /* RTEMS /dev/zero is a character device so this will fail */
  checked_mmap(PROT_READ | PROT_WRITE, MAP_PRIVATE, zerofd, EINVAL,
      "simple /dev/zero private");
  /* RTEMS cannot protect against writes so this will fail */
  checked_mmap(PROT_READ, MAP_SHARED, devfd, ENOTSUP,
      "simple test driver shared");
   /*
    * Repeat with no write protection. Should fail because of unimplemented
    * mmap handler in /dev/null.
    */
  p = checked_mmap(PROT_READ | PROT_WRITE, MAP_SHARED, devfd, 0,
      "simple test driver shared");
  rtems_test_assert(p == &test_data[0]);

  /* Extra PROT flags. */
  checked_mmap(PROT_READ | PROT_WRITE | 0x100000, MAP_ANON, -1, EINVAL,
      "MAP_ANON with extra PROT flags");
  checked_mmap(0xffff, MAP_SHARED, shmfd, EINVAL,
      "shm fd with garbage PROT");

  /* Undefined flag. */
  checked_mmap(PROT_READ | PROT_WRITE, MAP_ANON | MAP_RESERVED0080, -1,
      EINVAL, "Undefined flag");

  /* Both MAP_SHARED and MAP_PRIVATE */
  checked_mmap(PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE |
      MAP_SHARED, -1, EINVAL, "MAP_ANON with both SHARED and PRIVATE");
  checked_mmap(PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_SHARED, shmfd,
      EINVAL, "shm fd with both SHARED and PRIVATE");

  /* At least one of MAP_SHARED or MAP_PRIVATE without ANON */
  checked_mmap(PROT_READ | PROT_WRITE, 0, shmfd, EINVAL,
      "shm fd without sharing flag");

  /* MAP_ANON with sharing flag. Will fail on RTEMS*/
  checked_mmap(PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, EINVAL,
      "shared MAP_ANON");
  /* MAP_ANON with private flag*/
  checked_mmap(PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0,
      "private MAP_ANON");

  /* MAP_ANON should require an fd of -1. */
  checked_mmap(PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, 0, EINVAL,
      "MAP_ANON with fd != -1");

  /*
   * Writable MAP_SHARED should fail on read-only descriptors. Will fail
   * on RTEMS because of unimplemented mmap handler in /dev/null and the fact
   * that there is no read only protection.
   */
  checked_mmap(PROT_READ | PROT_WRITE, MAP_SHARED, zerofd, ENOTSUP,
      "MAP_SHARED of read-only /dev/zero");

  /*
   * Character devices other than /dev/zero do not support private
   * mappings. RTEMS cannot protect against writes so this will fail with
   * ENOTSUP
   */
  checked_mmap(PROT_READ, MAP_PRIVATE, devfd, ENOTSUP,
      "MAP_PRIVATE of test driver");

   /*
    * Repeat with no write protection.
    */
  checked_mmap(PROT_READ | PROT_WRITE, MAP_PRIVATE, devfd, EINVAL,
      "MAP_PRIVATE of test driver");

  close(devfd);
  close(shmfd);
  close(zerofd);
}

static void mmap_dev_zero_private( void )
{
  char *p1, *p2, *p3;
  int fd, pagesize;
  /*int i*/

  rtems_test_assert((pagesize = getpagesize()) > 0);
  rtems_test_assert((fd = open("/dev/zero", O_RDONLY)) >= 0);

  /* This should not return MAP_FAILED but does on RTEMS */
  p1 = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  /*rtems_test_assert(p1 != MAP_FAILED);*/
  rtems_test_assert(p1 == MAP_FAILED);

  /* This should not return MAP_FAILED but does on RTEMS */
  p2 = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  /*rtems_test_assert(p2 != MAP_FAILED);*/
  rtems_test_assert(p2 == MAP_FAILED);

  /* These tests can not run due to failures above */
  /*for (i = 0; i < pagesize; i++)
    mmap_test_assert_equal(0, p1[i], "byte at p1[%d] is %x", i, p1[i]);

  rtems_test_assert(memcmp(p1, p2, pagesize) == 0);

  p1[0] = 1;

  rtems_test_assert(p2[0] == 0);

  p2[0] = 2;

  rtems_test_assert(p1[0] == 1);*/

  /* This should not return MAP_FAILED but does on RTEMS */
  p3 = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  /*rtems_test_assert(p3 != MAP_FAILED);*/
  rtems_test_assert(p3 == MAP_FAILED);

  /*rtems_test_assert(p3[0] == 0);

  munmap(p1, pagesize);
  munmap(p2, pagesize);
  munmap(p3, pagesize);*/
  close(fd);
}

static void mmap_dev_zero_shared( void )
{
  char *p1, *p2, *p3;
  int fd, pagesize;
  /*int i*/

  rtems_test_assert((pagesize = getpagesize()) > 0);
  rtems_test_assert((fd = open("/dev/zero", O_RDWR)) >= 0);

  /* This should not return MAP_FAILED but does on RTEMS */
  p1 = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  /*rtems_test_assert(p1 != MAP_FAILED);*/
  rtems_test_assert(p1 == MAP_FAILED);

  /* This should not return MAP_FAILED but does on RTEMS */
  p2 = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  /*rtems_test_assert(p2 != MAP_FAILED);*/
  rtems_test_assert(p2 == MAP_FAILED);

  /* These tests can not run due to failures above */
  /*for (i = 0; i < pagesize; i++)
    mmap_test_assert_equal(0, p1[i], "byte at p1[%d] is %x", i, p1[i]);

  rtems_test_assert(memcmp(p1, p2, pagesize) == 0);

  p1[0] = 1;

  rtems_test_assert(p2[0] == 0);

  p2[0] = 2;

  rtems_test_assert(p1[0] == 1);*/

  p3 = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  /*rtems_test_assert(p3 != MAP_FAILED);*/
  rtems_test_assert(p3 == MAP_FAILED);

  /*rtems_test_assert(p3[0] == 0);

  munmap(p1, pagesize);
  munmap(p2, pagesize);
  munmap(p3, pagesize);*/
  close(fd);
}

void *POSIX_Init(
  void *argument
)
{
  int rv;

  TEST_BEGIN();

  rv = IMFS_make_generic_node(
    &test_driver_name[0],
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &node_control,
    NULL
  );
  rtems_test_assert(rv == 0);

  puts( "Init: mmap - map at zero" );
  mmap_map_at_zero();
  puts( "Init: mmap - bad arguments" );
  mmap_bad_arguments();
  /*
   * The following test does not work on RTEMS because /dev/zero is
   * implemented as a character device which does not support MAP_PRIVATE.
   */
  puts( "Init: mmap - /dev/zero private" );
  mmap_dev_zero_private();
  /*
   * The following test does not work on RTEMS because /dev/zero does not
   * implemented the mmap handler.
   */
  puts( "Init: mmap - /dev/zero shared" );
  mmap_dev_zero_shared();

  TEST_END();

  rtems_test_exit(0);
  return 0;
}

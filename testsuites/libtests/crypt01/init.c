/*
 * Copyright (c) 2011 The FreeBSD Project. All rights reserved.
 *
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
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
 */

/* Based on:
 * SHA256-based Unix crypt implementation. Released into the Public Domain by
 * Ulrich Drepper <drepper@redhat.com>. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <crypt.h>
#include <string.h>

#include "tmacros.h"

const char rtems_test_name[] = "CRYPT 1";

typedef struct {
  const char *salt;
  const char *input;
  const char *expected;
} test_case;

static void test_formats(void)
{
  struct crypt_data data;
  char *s;

  s = crypt_r("abc", "def", &data);
  rtems_test_assert(strcmp(s, "abc") == 0);

  crypt_add_format(&crypt_md5_format);
  crypt_add_format(&crypt_sha256_format);
  crypt_add_format(&crypt_sha512_format);
}

static void test_md5(void)
{
  const char key[] = "0.s0.l33t";
  const char salt_0[] = "$1$deadbeef$0Huu6KHrKLVWfqa4WljDE0";
  const char salt_1[] = "$1$cafebabe$0Huu6KHrKLVWfqa4WljDE0";
  struct crypt_data data;
  char *s;

  printf("test crypt_md5_r()\n");

  s = crypt_md5_r(&key[0], &salt_0[0], &data);
  rtems_test_assert(strcmp(s, &salt_0[0]) == 0);

  s = crypt_md5_r(&key[0], &salt_1[0], &data);
  rtems_test_assert(strcmp(s, &salt_1[0]) != 0);
}

static const test_case test_cases_sha256[] = {
  {
    "$5$saltstring", "Hello world!",
    "$5$saltstring$5B8vYYiY.CVt1RlTTf8KbXBH3hsxY/GNooZaBBGWEc5"
  }, {
    "$5$rounds=10000$saltstringsaltstring", "Hello world!",
    "$5$rounds=10000$saltstringsaltst$3xv.VbSHBb41AL9AvLeujZkZRBAwqFMz2."
    "opqey6IcA"
  }, {
    "$5$rounds=5000$toolongsaltstring", "This is just a test",
    "$5$rounds=5000$toolongsaltstrin$Un/5jzAHMgOGZ5.mWJpuVolil07guHPvOW8"
    "mGRcvxa5"
  }, {
    "$5$rounds=1400$anotherlongsaltstring",
    "a very much longer text to encrypt.  This one even stretches over more"
    "than one line.",
    "$5$rounds=1400$anotherlongsalts$Rx.j8H.h8HjEDGomFU8bDkXm3XIUnzyxf12"
    "oP84Bnq1"
  }, {
    "$5$rounds=77777$short",
    "we have a short salt string but not a short password",
    "$5$rounds=77777$short$JiO1O3ZpDAxGJeaDIuqCoEFysAe1mZNJRs3pw0KQRd/"
  }, {
    "$5$rounds=123456$asaltof16chars..", "a short string",
    "$5$rounds=123456$asaltof16chars..$gP3VQ/6X7UUEW3HkBn2w1/Ptq2jxPyzV/"
    "cZKmF/wJvD"
  }, {
    "$5$rounds=10$roundstoolow", "the minimum number is still observed",
    "$5$rounds=1000$roundstoolow$yfvwcWrQ8l/K0DAWyuPMDNHpIVlTQebY9l/gL97"
    "2bIC"
  }
};

static void print_test_case(const test_case *c, const char *s)
{
  printf(
    "input:    %s\nsalt:     %s\nexpected: %s\nactual:   %s\n",
    c->input,
    c->salt,
    c->expected,
    s
  );
}

static void test_sha256(void)
{
  size_t i;

  printf("test crypt_sha256_r()\n");

  for (i = 0; i < RTEMS_ARRAY_SIZE(test_cases_sha256); ++i) {
    const test_case *c = &test_cases_sha256[i];
    struct crypt_data data;
    char *s;

    s = crypt_sha256_r(c->input, c->salt, &data);
    print_test_case(c, s);
    rtems_test_assert(strcmp(s, c->expected) == 0);
  }
}

static const test_case test_cases_sha512[] = {
  {
    "$6$saltstring", "Hello world!",
    "$6$saltstring$svn8UoSVapNtMuq1ukKS4tPQd8iKwSMHWjl/O817G3uBnIFNjnQJu"
    "esI68u4OTLiBFdcbYEdFCoEOfaS35inz1"
  }, {
    "$6$rounds=10000$saltstringsaltstring", "Hello world!",
    "$6$rounds=10000$saltstringsaltst$OW1/O6BYHV6BcXZu8QVeXbDWra3Oeqh0sb"
    "HbbMCVNSnCM/UrjmM0Dp8vOuZeHBy/YTBmSK6H9qs/y3RnOaw5v."
  }, {
    "$6$rounds=5000$toolongsaltstring", "This is just a test",
    "$6$rounds=5000$toolongsaltstrin$lQ8jolhgVRVhY4b5pZKaysCLi0QBxGoNeKQ"
    "zQ3glMhwllF7oGDZxUhx1yxdYcz/e1JSbq3y6JMxxl8audkUEm0"
  }, {
    "$6$rounds=1400$anotherlongsaltstring",
    "a very much longer text to encrypt.  This one even stretches over more"
    "than one line.",
    "$6$rounds=1400$anotherlongsalts$POfYwTEok97VWcjxIiSOjiykti.o/pQs.wP"
    "vMxQ6Fm7I6IoYN3CmLs66x9t0oSwbtEW7o7UmJEiDwGqd8p4ur1"
  }, {
    "$6$rounds=77777$short",
    "we have a short salt string but not a short password",
    "$6$rounds=77777$short$WuQyW2YR.hBNpjjRhpYD/ifIw05xdfeEyQoMxIXbkvr0g"
    "ge1a1x3yRULJ5CCaUeOxFmtlcGZelFl5CxtgfiAc0"
  }, {
    "$6$rounds=123456$asaltof16chars..", "a short string",
    "$6$rounds=123456$asaltof16chars..$BtCwjqMJGx5hrJhZywWvt0RLE8uZ4oPwc"
    "elCjmw2kSYu.Ec6ycULevoBK25fs2xXgMNrCzIMVcgEJAstJeonj1"
  }, {
    "$6$rounds=10$roundstoolow", "the minimum number is still observed",
    "$6$rounds=1000$roundstoolow$kUMsbe306n21p9R.FRkW3IGn.S9NPN0x50YhH1x"
    "hLsPuWGsUSklZt58jaTfF4ZEQpyUNGc0dqbpBYYBaHHrsX."
  }
};

static void test_sha512(void)
{
  size_t i;

  printf("test crypt_sha512_r()\n");

  for (i = 0; i < RTEMS_ARRAY_SIZE(test_cases_sha512); ++i) {
    const test_case *c = &test_cases_sha512[i];
    struct crypt_data data;
    char *s;

    s = crypt_sha512_r(c->input, c->salt, &data);
    print_test_case(c, s);
    rtems_test_assert(strcmp(s, c->expected) == 0);
  }
}

static const test_case test_cases_generic[] = {
  {
    "saltstring", "Hello world!",
    "$6$saltstring$svn8UoSVapNtMuq1ukKS4tPQd8iKwSMHWjl/O817G3uBnIFNjnQJu"
    "esI68u4OTLiBFdcbYEdFCoEOfaS35inz1"
  }, {
    "$1$deadbeef", "0.s0.l33t",
    "$1$deadbeef$0Huu6KHrKLVWfqa4WljDE0"
  }, {
    "$5$saltstring", "Hello world!",
    "$5$saltstring$5B8vYYiY.CVt1RlTTf8KbXBH3hsxY/GNooZaBBGWEc5"
  }, {
    "$6$saltstring", "Hello world!",
    "$6$saltstring$svn8UoSVapNtMuq1ukKS4tPQd8iKwSMHWjl/O817G3uBnIFNjnQJu"
    "esI68u4OTLiBFdcbYEdFCoEOfaS35inz1"
  }
};

static void test_generic(void)
{
  size_t i;

  printf("test crypt_r()\n");

  for (i = 0; i < RTEMS_ARRAY_SIZE(test_cases_generic); ++i) {
    const test_case *c = &test_cases_generic[i];
    struct crypt_data data;
    char *s;

    s = crypt_r(c->input, c->salt, &data);
    print_test_case(c, s);
    rtems_test_assert(strcmp(s, c->expected) == 0);
  }
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test_formats();
  test_md5();
  test_sha256();
  test_sha512();
  test_generic();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_STACK_SIZE (2 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <stdio.h>
#include <pthread.h>

static int step;

static void destroy(void *arg)
{
  assert(step == 2);
  step = 3;
  printf("destroy\n");
}

static void cleanup(void *arg)
{
  assert(step == 1);
  step = 2;
  printf("cleanup\n");
}

static void *task(void *arg)
{
  pthread_key_t key;
  int eno;

  eno = pthread_key_create(&key, destroy);
  assert(eno == 0);

  pthread_cleanup_push(cleanup, NULL);

  eno = pthread_setspecific(key, &key);
  assert(eno == 0);

  assert(step == 0);
  step = 1;

  pthread_exit(NULL);
  pthread_cleanup_pop(0);

  return NULL;
}

int main(int argc, char **argv)
{
  pthread_t t;
  int eno;

  eno = pthread_create(&t, NULL, task, NULL);
  assert(eno == 0);

  eno = pthread_join(t, NULL);
  assert(eno == 0);

  return 0;
}

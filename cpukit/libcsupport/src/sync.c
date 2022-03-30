/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Synchronize Data on Disk with Memory 
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/reent.h>
#include <unistd.h>
#include <stdio.h>

#include <rtems.h>
#include <rtems/score/thread.h>
#include <rtems/score/percpu.h>

/* In Newlib this function is declared in a private header file */
int _fwalk_reent (struct _reent *, int (*)(struct _reent *, FILE *));

static int sync_wrapper(struct _reent *reent, FILE *f)
{
  int fn = fileno(f);

  (void) reent;

  /*
   * There is no way to report errors here.  So this is a best-effort approach.
   */
  (void) fsync(fn);
  (void) fdatasync(fn);

  return 0;
}

/* iterate over all FILE *'s for this thread */
static bool sync_per_thread(Thread_Control *t, void *arg)
{
   struct _reent *current_reent;
   struct _reent *this_reent;

   /*
    *  The sync_wrapper() function will operate on the current thread's
    *  reent structure so we will temporarily use that.
    */
   this_reent = t->libc_reent;
   if ( this_reent ) {
     Thread_Control *executing = _Thread_Get_executing();
     current_reent = executing->libc_reent;
     executing->libc_reent = this_reent;
     _fwalk_reent (this_reent, sync_wrapper);
     executing->libc_reent = current_reent;
   }

   return false;
}

/**
 * This function operates by as follows:
 *    for all threads
 *      for all FILE *
 *         fsync()
 *         fdatasync()
 */
void sync(void)
{

  /*
   *  Walk the one used initially by RTEMS.
   */
  _fwalk_reent(_GLOBAL_REENT, sync_wrapper);

  /*
   *  XXX Do we walk the one used globally by newlib?
   *  XXX Do we need the RTEMS global one?
   */

  /*
   *  Now walk all the per-thread reentrancy structures.
   */
  rtems_task_iterate(sync_per_thread, NULL);
}

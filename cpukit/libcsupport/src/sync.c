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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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

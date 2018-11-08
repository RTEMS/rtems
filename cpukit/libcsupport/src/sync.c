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

#if HAVE_CONFIG_H
#include "config.h"
#endif

/* Since we compile with strict ANSI we need to undef it to get
 * prototypes for extensions
 */
#undef __STRICT_ANSI__
int fdatasync(int);        /* still not always prototyped */


#include <unistd.h>
#include <stdio.h>

#include <rtems.h>
#include <rtems/score/thread.h>
#include <rtems/score/percpu.h>

/* XXX check standards -- Linux version appears to be void */
void _fwalk(struct _reent *, void *);


static void sync_wrapper(FILE *f)
{
  int fn = fileno(f);

  /*
   * There is no way to report errors here.  So this is a best-effort approach.
   */
  (void) fsync(fn);
  (void) fdatasync(fn);
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
     _fwalk (t->libc_reent, sync_wrapper);
     executing->libc_reent = current_reent;
   }

   return false;
}

/*
 * _global_impure_ptr is not prototyped in any .h files.
 * We have to extern it here.
 */
extern struct _reent * const _global_impure_ptr __ATTRIBUTE_IMPURE_PTR__;

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
  _fwalk(_global_impure_ptr, sync_wrapper);

  /*
   *  XXX Do we walk the one used globally by newlib?
   *  XXX Do we need the RTEMS global one?
   */

  /*
   *  Now walk all the per-thread reentrancy structures.
   */
  rtems_task_iterate(sync_per_thread, NULL);
}

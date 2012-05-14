/*
 *  sync() - XXX ??? where is this defined
 *
 *  This function operates by as follows:
 *    for all threads
 *      for all FILE *
 *         fsync()
 *         fdatasync()
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
/*
#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__

#include <rtems/libio_.h>
#include <rtems/seterr.h>
*/

/* XXX check standards -- Linux version appears to be void */
void _fwalk(struct _reent *, void *);


static void sync_wrapper(FILE *f)
{
  int fn = fileno(f);

  /*
   *  We are explicitly NOT checking the return values as it does not
   *  matter if they succeed.  We are just making a best faith attempt
   *  at both and trusting that we were passed a good FILE pointer.
   */
  fsync(fn);
  fdatasync(fn);
}

/* iterate over all FILE *'s for this thread */
static void sync_per_thread(Thread_Control *t)
{
   struct _reent *current_reent;
   struct _reent *this_reent;

   /*
    *  The sync_wrapper() function will operate on the current thread's
    *  reent structure so we will temporarily use that.
    */
   this_reent = t->libc_reent;
   if ( this_reent ) {
     current_reent = _Thread_Executing->libc_reent;
     _Thread_Executing->libc_reent = this_reent;
     _fwalk (t->libc_reent, sync_wrapper);
     _Thread_Executing->libc_reent = current_reent;
   }
}

/*
 * _global_impure_ptr is not prototyped in any .h files.
 * We have to extern it here.
 */
extern struct _reent * const _global_impure_ptr __ATTRIBUTE_IMPURE_PTR__;

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
  rtems_iterate_over_all_threads(sync_per_thread);
}

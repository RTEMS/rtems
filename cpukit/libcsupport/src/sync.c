/*
 *  sync() - XXX ??? where is this defined
 *
 *  This function operates by as follows:
 *    for all threads
 *      for all FILE *
 *         fsync()
 *         fdatasync()
 *
 *  COPYRIGHT (c) 1989-2003.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

/* this is needed to get the fileno() prototype */
#if defined(__STRICT_ANSI__)
#undef __STRICT_ANSI__
#endif
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

  fsync(fn);
  fdatasync(fn);
}

/* iterate over all FILE *'s for this thread */
static void sync_per_thread(Thread_Control *t)
{
   struct reent *current_reent;

   /*
    *  The sync_wrapper() function will operate on the current thread's
    *  reent structure so we will temporarily use that.
    */
   current_reent = _Thread_Executing->libc_reent;
   _Thread_Executing->libc_reent = t->libc_reent;
   _fwalk (t->libc_reent, sync_wrapper);
   _Thread_Executing->libc_reent = current_reent;
}

int sync(void)
{
  rtems_iterate_over_all_threads(sync_per_thread);
  return 0;
}

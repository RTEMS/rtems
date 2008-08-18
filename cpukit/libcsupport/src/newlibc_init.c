/*
 *  Implementation of hooks for the CYGNUS newlib libc
 *  These hooks set things up so that:
 *       + '_REENT' is switched at task switch time.
 *
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>

#if defined(RTEMS_NEWLIB)
#include <rtems/libcsupport.h>

/* Since we compile with strict ANSI we need to undef it to get
 * prototypes for extensions
 */
#undef __STRICT_ANSI__

#include <stdlib.h>             /* for free() */
#include <string.h>             /* for memset() */

#include <sys/reent.h>          /* for extern of _REENT (aka _impure_ptr) */
#include <errno.h>

/*
 *  Init libc for CYGNUS newlib
 * 
 *  Set up _REENT to use our global libc_global_reent.
 *  (newlib provides a global of its own, but we prefer our own name for it)
 *
 *  If reentrancy is desired (which it should be), then
 *  we install the task extension hooks to maintain the
 *  newlib reentrancy global variable _REENT on task
 *  create, delete, switch, exit, etc.
 *
 */


struct _reent    libc_global_reent
    __ATTRIBUTE_IMPURE_PTR__ = _REENT_INIT(libc_global_reent);
void
libc_init(void)
{
  _REENT = &libc_global_reent;

  _Thread_Set_libc_reent (&_REENT);
}

#endif

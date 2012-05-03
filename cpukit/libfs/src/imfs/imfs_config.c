/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio_.h>
#include "imfs.h"

/* XXX this structure should use real constants */

const rtems_filesystem_limits_and_options_t IMFS_LIMITS_AND_OPTIONS = {
   5,   /* link_max */
   6,   /* max_canon */
   7,   /* max_input */
   IMFS_NAME_MAX,       /* name_max */
   255, /* path_max */
   2,   /* pipe_buf */
   1,   /* posix_async_io */
   2,   /* posix_chown_restrictions */
   3,   /* posix_no_trunc */
   4,   /* posix_prio_io */
   5,   /* posix_sync_io */
   6    /* posix_vdisable */
};

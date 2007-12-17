/*
 *  $Id$
 */

/*
 * Trivial File Transfer Protocol (TFTP)
 *
 * Transfer file to/from remote host
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 */

/*
 * Usage:
 *
 * To open `/bootfiles/image' on `hostname' for reading:
 *         fd = open ("/TFTP/hostname/bootfiles/image", O_RDONLY);
 *
 * The `hostname' must be four dot-separated decimal values.
 */

#ifndef _RTEMS_TFTP_H
#define _RTEMS_TFTP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/libio.h>

/*
 * Filesystem initialization routine
 */

int rtems_bsdnet_initialize_tftp_filesystem( void );

/*
 * TFTP file system operations table.
 */

extern rtems_filesystem_operations_table  rtems_tftp_ops;

#ifdef __cplusplus
}
#endif

#endif

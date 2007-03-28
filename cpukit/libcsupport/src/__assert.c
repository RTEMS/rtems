/*  __assert - small RTEMS Specific Implementation 
 *
 *  COPYRIGHT (c) 2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/bspIo.h>
#include <rtems.h>

void __assert(const char *file, int line, const char *failedexpr)
{
    printk("assertion \"%s\" failed: file \"%s\", line %d\n",
               failedexpr, file, line);
    rtems_fatal_error_occurred(0);
}

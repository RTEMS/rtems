/*
 *  MALLOC_DUMP Shell Command Implmentation
 *
 *  Author: Fernando RUIZ CASAS
 *  Work: fernando.ruiz@ctv.es
 *  Home: correo@fernando-ruiz.com
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

/*----------------------------------------------------------------------------*/
int rtems_shell_main_malloc_dump(int argc,char * argv[]) {
 #ifdef MALLOC_STATS  /* /rtems/s/src/lib/libc/malloc.c */
   void malloc_dump(void);
   malloc_dump();
 #else
   fprintf(stdout, "No malloc dump built into RTEMS\n");
 #endif
 return 0;
}

rtems_shell_cmd_t rtems_Shell_MALLOC_DUMP_Command = {
  "malloc",                                   /* name */
  "mem  show memory malloc'ed",               /* usage */
  "mem",                                      /* topic */
  rtems_shell_main_malloc_dump,               /* command */
  NULL,                                       /* alias */
  NULL                                        /* next */
};


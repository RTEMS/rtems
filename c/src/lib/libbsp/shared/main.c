/*
 *  A simple main which can be used on any embedded target.
 *
 *  This style of initialization insures that the C++ global 
 *  constructors are executed after RTEMS is initialized.
 *
 *  Thanks to Chris Johns <cjohns@plessey.com.au> for this idea.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>

char *rtems_progname;

extern rtems_interrupt_level bsp_isr_level;

int c_rtems_main(int argc, char **argv)
{
  if ((argc > 0) && argv && argv[0])
    rtems_progname = argv[0];
  else
    rtems_progname = "RTEMS";

  rtems_initialize_executive_late( bsp_isr_level );

  return 0;
}


/*
 *  A simple main which can be used on any embedded target.
 *
 *  This style of initialization insures that the C++ global 
 *  constructors are executed after RTEMS is initialized.
 *
 *  Thanks to Chris Johns <cjohns@plessey.com.au> for this idea.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp.h>

char *rtems_progname;

extern void bsp_start( void );
extern void bsp_cleanup( void );

extern rtems_configuration_table  BSP_Configuration;
extern rtems_cpu_table            Cpu_table;

rtems_interrupt_level bsp_isr_level;

int main(int argc, char **argv, char **environ)
{

  if ((argc > 0) && argv && argv[0])
    rtems_progname = argv[0];
  else
    rtems_progname = "RTEMS";

  rtems_initialize_executive_late( bsp_isr_level );

  return 0;
}

void boot_card(void)
{
  int status;

  /* the atexit hook will be before the static destructor list's entry
     point */
  bsp_start();

  bsp_isr_level =
    rtems_initialize_executive_early( &BSP_Configuration, &Cpu_table );

  status = main(0, 0, 0);

  bsp_cleanup();  

  exit( status );
}


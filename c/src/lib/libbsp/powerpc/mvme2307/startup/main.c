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

extern rtems_configuration_table  Configuration;
extern rtems_configuration_table  BSP_Configuration;
extern rtems_cpu_table            Cpu_table;

rtems_interrupt_level bsp_isr_level;

int main(int argc, char **argv);

int boot_card(int argc, char **argv)
{
  int status;

  /*
   *  Set default values for the CPU Table fields all ports must have.
   *  These values can be overridden in bsp_start() but they are
   *  right most of the time.
   */

  Cpu_table.pretasking_hook                 = NULL;
  Cpu_table.predriver_hook                  = NULL;
  Cpu_table.postdriver_hook                 = NULL;
  Cpu_table.idle_task                       = NULL;
  Cpu_table.do_zero_of_workspace            = TRUE;
  Cpu_table.interrupt_stack_size            = RTEMS_MINIMUM_STACK_SIZE;
  Cpu_table.extra_mpci_receive_server_stack = 0;
  Cpu_table.stack_allocate_hook             = NULL;
  Cpu_table.stack_free_hook                 = NULL;


  /*
   *  Copy the configuration table so we and the BSP wants to change it.
   */

  BSP_Configuration = Configuration;

  /*
   *  The atexit hook will be before the static destructor list's entry
   *  point.
   */

  bsp_start();

  /*
   *  Initialize RTEMS but do NOT start multitasking.
   */

  bsp_isr_level =
    rtems_initialize_executive_early( &BSP_Configuration, &Cpu_table );

  /*
   *  Call main() and get the global constructors invoked if there
   *  are any.
   */

  status = main(argc, argv);

  /*
   *  Perform any BSP specific shutdown actions.
   */

  bsp_cleanup();  

  /*
   *  Now return to the start code.
   */

  return status;
}

int main(int argc, char **argv)
{

  if ((argc > 0) && argv && argv[0])
    rtems_progname = argv[0];
  else
    rtems_progname = "RTEMS";

  rtems_initialize_executive_late( bsp_isr_level );

  return 0;
}


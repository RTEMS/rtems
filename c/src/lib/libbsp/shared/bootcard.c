/*
 *  This is the C entry point for ALL RTEMS BSPs.  It is invoked
 *  from the assembly language initialization file usually called
 *  start.S.  It provides the framework for the BSP initialization
 *  sequence.  The basic flow of initialization is:
 *
 *  + start.S: basic CPU setup (stack, zero BSS) 
 *    + boot_card
 *      + bspstart.c: bsp_start - more advanced initialization
 *      + rtems_initialize_executive_early
 *        + all device drivers
 *      + rtems_initialize_executive_late
 *        + 1st task executes C++ global constructors
 *        .... appplication runs ...
 *        + exit
 *     + back to here eventually
 *     + bspclean.c: bsp_cleanup
 *
 *  This style of initialization ensures that the C++ global
 *  constructors are executed after RTEMS is initialized.
 *  Thanks to Chris Johns <cjohns@plessey.com.au> for the idea
 *  to move C++ global constructors into the first task.
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>

extern void bsp_start( void );
extern void bsp_cleanup( void );

/*
 *  Since there is a forward reference
 */

char *rtems_progname;

int boot_card(
  int    argc, 
  char **argv, 
  char **envp
)
{
  static char  *argv_pointer = NULL;
  static char  *envp_pointer = NULL;
  char **argv_p = &argv_pointer;
  char **envp_p = &envp_pointer;
  rtems_interrupt_level bsp_isr_level;

  /*
   *  Make sure interrupts are disabled.
   */

  rtems_interrupt_disable( bsp_isr_level );

  /*
   *  Set things up so we have real pointers for argv and envp.
   *  If the BSP has passed us something useful, then pass it on.
   *  Somehow we need to eventually make this available to
   *  a real main() in user land. :)
   */

  if ( argv )
    argv_p = argv;

  if ( envp )
    envp_p = envp;

  /*
   *  Set the program name in case some application cares.
   */

  if ((argc > 0) && argv && argv[0])
    rtems_progname = argv[0];
  else
    rtems_progname = "RTEMS";

  /*
   * Invoke Board Support Package initialization routine written in C.
   */

  bsp_start();

  /*
   *  Initialize RTEMS but do NOT start multitasking.
   */

  rtems_initialize_executive_early( &Configuration );

  /*
   *  Complete initialization of RTEMS and switch to the first task.
   *  Global C++ constructors will be executed in the context of that task.
   */

  rtems_initialize_executive_late( bsp_isr_level );

  /***************************************************************
   ***************************************************************
   *  APPLICATION RUNS HERE!!!  When it shuts down, we return!!! *
   ***************************************************************
   ***************************************************************
   */

  /*
   *  Perform any BSP specific shutdown actions which are written in C.
   */

  bsp_cleanup();

  /*
   *  Now return to the start code.
   */

  return 0;
}

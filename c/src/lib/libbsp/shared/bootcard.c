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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp.h>

#if defined(__USE_INIT_FINI__)
#include <stdlib.h> /* for atexit() */
#endif

extern void bsp_start( void );
extern void bsp_cleanup( void );

extern rtems_configuration_table  Configuration;
extern rtems_configuration_table  BSP_Configuration;
extern rtems_cpu_table            Cpu_table;

rtems_api_configuration_table BSP_RTEMS_Configuration;

#ifdef RTEMS_POSIX_API
posix_api_configuration_table BSP_POSIX_Configuration;
#endif

/* Initialize C++ global Ctor/Dtor and initializes exception handling. */
#if defined(__USE_INIT_FINI__)
extern void _fini( void );
#endif

rtems_interrupt_level bsp_isr_level;

/*
 *  Since there is a forward reference
 */

int c_rtems_main(int argc, char **argv, char **envp);

int boot_card(int argc, char **argv, char **envp)
{
  int    status;
  static char  *argv_pointer = NULL;
  static char  *envp_pointer = NULL;
  char **argv_p = &argv_pointer;
  char **envp_p = &envp_pointer;

  /*
   *  Set things up so c_rtems_main() is called with real pointers for
   *  argv and envp.  If the BSP has passed us something useful, then
   *  pass it on.  Somehow we need to eventually make this available to
   *  a real main() in user land. :)
   */

  if ( argv )
    argv_p = argv;

  if ( envp )
    envp_p = envp;


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

  BSP_Configuration       = Configuration;

  BSP_RTEMS_Configuration = *Configuration.RTEMS_api_configuration;
  BSP_Configuration.RTEMS_api_configuration = &BSP_RTEMS_Configuration;

#ifdef RTEMS_POSIX_API
  BSP_POSIX_Configuration = *Configuration.POSIX_api_configuration;
  BSP_Configuration.POSIX_api_configuration = &BSP_POSIX_Configuration;
#endif

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
   *  Call c_rtems_main() and eventually let the first task or the real
   *  main() invoke the global constructors if there are any.
   */

#if defined(__USE_INIT_FINI__)
   atexit( _fini );
#endif

  status = c_rtems_main( argc, argv_p, envp_p );

  /*
   *  Perform any BSP specific shutdown actions.
   */

  bsp_cleanup();  

  /*
   *  Now return to the start code.
   */

  return status;
}

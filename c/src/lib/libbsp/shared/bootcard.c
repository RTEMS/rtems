/*
 *  This is the C entry point for ALL RTEMS BSPs.  It is invoked
 *  from the assembly language initialization file usually called
 *  start.S.  It provides the framework for the BSP initialization
 *  sequence.  The basic flow of initialization is:
 *
 *  + start.S: basic CPU setup (stack, zero BSS) 
 *    + boot_card
 *      + if defined(BSP_BOOTCARD_HANDLES_RAM_ALLOCATION)
 *        - obtain information on BSP memory and allocate RTEMS Workspace
 *      + bspstart.c: bsp_start - more advanced initialization
 *      + rtems_initialize_data_structures
 *      + if defined(BSP_BOOTCARD_HANDLES_RAM_ALLOCATION)
 *        - Allocate memory to C Program Heap
 *        - initialize C Library and C Program Heap
 *      + bsp_pretasking_hook
 *      + if defined(RTEMS_DEBUG)
 *        - rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
 *      + rtems_initialize_before_drivers
 *      + bsp_predriver_hook
 *      + rtems_initialize_device_drivers
 *        - all device drivers
 *      + bsp_postdriver_hook
 *      + rtems_initialize_start_multitasking
 *        - 1st task executes C++ global constructors
 *          .... appplication runs ...
 *          - exit
 *     + back to here eventually
 *     + bspclean.c: bsp_cleanup
 *
 *  This style of initialization ensures that the C++ global
 *  constructors are executed after RTEMS is initialized.
 *  Thanks to Chris Johns <cjohns@plessey.com.au> for the idea
 *  to move C++ global constructors into the first task.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>

#include <bsp/bootcard.h>

/*
 *  Since there is a forward reference
 */
char *rtems_progname;

/*
 *  These are the prototypes and helper routines which are used
 *  when the BSP lets the framework handle RAM allocation between
 *  the RTEMS Workspace and C Program Heap.
 */
#if defined(BSP_BOOTCARD_HANDLES_RAM_ALLOCATION)
  static rtems_status_code bootcard_bsp_libc_helper(
    void   *work_area_start,
    size_t  work_area_size,
    void   *heap_start,
    size_t  heap_size
  )
  {
    size_t heap_size_default = 0;

    if (heap_start == BSP_BOOTCARD_HEAP_USES_WORK_AREA) {
      /* Use the work area start as heap start */
      heap_start = work_area_start;

      /* Ensure proper alignement */
      if ((uintptr_t) heap_start & (CPU_ALIGNMENT - 1)) {
        heap_start = (void *) (((uintptr_t) heap_start + CPU_ALIGNMENT)
	  & ~(CPU_ALIGNMENT - 1));
      }

      /*
       * For the default heap size use the free space from the start of the
       * work area up to the work space start as heap area.
       */
      heap_size_default = (size_t) ((char *) Configuration.work_space_start
        - (char *) heap_start);

      /* Keep it as a multiple of 16 bytes */
      heap_size_default &= ~((size_t) 0xf);

      /* Use default heap size if requested */
      if (heap_size == BSP_BOOTCARD_HEAP_SIZE_DEFAULT) {
        heap_size = heap_size_default;
      }
		
      /* Check heap size */
      if (heap_size > heap_size_default) {
        return RTEMS_INVALID_SIZE;
      }
    }

    bsp_libc_init(heap_start, heap_size, 0);

    return RTEMS_SUCCESSFUL;
  }
#endif

/*
 *  This is the initialization framework routine that weaves together
 *  calls to RTEMS and the BSP in the proper sequence to initialize
 *  the system while maximizing shared code and keeping BSP code in C
 *  as much as possible.
 */
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
  #if defined(BSP_BOOTCARD_HANDLES_RAM_ALLOCATION)
    rtems_status_code sc = RTEMS_SUCCESSFUL;
    void   *work_area_start = NULL;
    size_t  work_area_size = 0;
    void   *heap_start = NULL;
    size_t  heap_size = 0;
  #endif

  /*
   * Special case for PowerPC: The interrupt disable mask is stored in SPRG0.
   * It must be valid before we can use rtems_interrupt_disable().
   */
  #ifdef PPC_INTERRUPT_DISABLE_MASK_DEFAULT
    ppc_interrupt_set_disable_mask( PPC_INTERRUPT_DISABLE_MASK_DEFAULT );
  #endif /* PPC_INTERRUPT_DISABLE_MASK_DEFAULT */

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
  if ( argv ) argv_p = argv;
  if ( envp ) envp_p = envp;

  /*
   *  Set the program name in case some application cares.
   */
  if ((argc > 0) && argv && argv[0])
    rtems_progname = argv[0];
  else
    rtems_progname = "RTEMS";

  /*
   *  Find out where the block of memory the BSP will use for
   *  the RTEMS Workspace and the C Program Heap is.
   */
  #if defined(BSP_BOOTCARD_HANDLES_RAM_ALLOCATION)
    {
      void *work_space_start = NULL;
    
      bsp_get_work_area(
        &work_area_start,
        &work_area_size,
        &heap_start,
        &heap_size
      );
    
      work_space_start = (char *) work_area_start + work_area_size
        - rtems_configuration_get_work_space_size();
    
      if ((uintptr_t) work_space_start <= (uintptr_t) work_area_start) {
        printk( "bootcard: Work space to big for work area!\n");
        bsp_cleanup();
        return -1;
      }
    
      Configuration.work_space_start = work_space_start;
    
      #if (BSP_DIRTY_MEMORY == 1)
        memset( work_area_start, 0xCF,  work_area_size);
      #endif
    }
  #endif

  /*
   * Invoke Board Support Package initialization routine written in C.
   */
  bsp_start();

  /*
   *  Initialize RTEMS data structures
   */
  rtems_initialize_data_structures( &Configuration );

  /*
   *  Initialize the C library for those BSPs using the shared
   *  framework.
   */
  #if defined(BSP_BOOTCARD_HANDLES_RAM_ALLOCATION)
    sc = bootcard_bsp_libc_helper(
      work_area_start,
      work_area_size,
      heap_start,
      heap_size
    );
    if (sc != RTEMS_SUCCESSFUL) {
      printk( "bootcard: Cannot initialize C library!\n");
      bsp_cleanup();
      return -1;
    }
  #endif

  /*
   *  All BSP to do any required initialization now that RTEMS
   *  data structures are initialized.  In older BSPs or those
   *  which do not use the shared framework, this is the typical
   *  time when the C Library is initialized so malloc()
   *  can be called by device drivers.  For BSPs using the shared
   *  framework, this routine can be empty.
   */
  bsp_pretasking_hook();

  /*
   *  If debug is enabled, then enable all dynamic RTEMS debug 
   *  capabilities.
   *
   *  NOTE: Most debug features are conditionally compiled in
   *        or enabled via configure time plugins.
   */
  #ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
  #endif

  /*
   *  Let RTEMS perform initialization it requires before drivers
   *  are allowed to be initialized.
   */
  rtems_initialize_before_drivers();

  /*
   *  Execute BSP specific pre-driver hook. Drivers haven't gotten
   *  to initialize yet so this is a good chance to initialize
   *  buses, spurious interrupt handlers, etc.. 
   *
   *  NOTE: Many BSPs do not require this handler and use the
   *        shared stub.
   */
  bsp_predriver_hook();

  /*
   *  Initialize all device drivers.
   */
  rtems_initialize_device_drivers();

  /*
   *  Invoke the postdriver hook.  This normally opens /dev/console
   *  for use as stdin, stdout, and stderr.
   */
  bsp_postdriver_hook();

  /*
   *  Complete initialization of RTEMS and switch to the first task.
   *  Global C++ constructors will be executed in the context of that task.
   */
  rtems_initialize_start_multitasking();

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

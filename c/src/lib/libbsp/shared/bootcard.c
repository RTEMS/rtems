/**
 * @file
 *
 * @ingroup bsp_bootcard
 *
 * @brief Standard system startup.
 */

/*
 *  This is the C entry point for ALL RTEMS BSPs.  It is invoked
 *  from the assembly language initialization file usually called
 *  start.S.  It provides the framework for the BSP initialization
 *  sequence.  The basic flow of initialization is:
 *
 *  + start.S: basic CPU setup (stack, zero BSS)
 *    + boot_card
 *      + bspstart.c: bsp_start - more advanced initialization
 *      + obtain information on BSP memory and allocate RTEMS Workspace
 *      + rtems_initialize_data_structures
 *      + allocate memory to C Program Heap
 *      + initialize C Library and C Program Heap
 *      + bsp_pretasking_hook
 *      + if defined( RTEMS_DEBUG )
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
 *  COPYRIGHT (c) 1989-2011.
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
#include <rtems/bspIo.h>
#include <rtems/malloc.h>

#ifdef CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK
#include <unistd.h> /* for sbrk() */
#endif

/*
 *  At most a single pointer to the cmdline for those target
 *  short on memory and not supporting a command line.
 */
const char *bsp_boot_cmdline;

/*
 *  These are the prototypes and helper routines which are used
 *  when the BSP lets the framework handle RAM allocation between
 *  the RTEMS Workspace and C Program Heap.
 */
static void bootcard_bsp_libc_helper(
  void      *work_area_start,
  uintptr_t  work_area_size,
  void      *heap_start,
  uintptr_t  heap_size,
  uintptr_t  sbrk_amount
)
{
  if ( heap_start == BSP_BOOTCARD_HEAP_USES_WORK_AREA ) {
    if ( !rtems_configuration_get_unified_work_area() ) {
      uintptr_t work_space_size = rtems_configuration_get_work_space_size();

      heap_start = (char *) work_area_start + work_space_size;

      if (heap_size == BSP_BOOTCARD_HEAP_SIZE_DEFAULT) {
        uintptr_t heap_size_default = work_area_size - work_space_size;

        heap_size = heap_size_default;
      }
    } else {
      heap_start = work_area_start;
      if (heap_size == BSP_BOOTCARD_HEAP_SIZE_DEFAULT) {
        heap_size = work_area_size;
      }
    }
  }

  bsp_libc_init(heap_start, heap_size, sbrk_amount);
}

/*
 *  This is the initialization framework routine that weaves together
 *  calls to RTEMS and the BSP in the proper sequence to initialize
 *  the system while maximizing shared code and keeping BSP code in C
 *  as much as possible.
 */
uint32_t boot_card(
  const char *cmdline
)
{
  rtems_interrupt_level  bsp_isr_level;
  void                  *work_area_start = NULL;
  uintptr_t              work_area_size = 0;
  void                  *heap_start = NULL;
  uintptr_t              heap_size = 0;
  uintptr_t              sbrk_amount = 0;
  uintptr_t              work_space_size = 0;
  uint32_t               status = 0;

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

  bsp_boot_cmdline = cmdline;

  /*
   * Invoke Board Support Package initialization routine written in C.
   */
  bsp_start();

  /*
   *  Find out where the block of memory the BSP will use for
   *  the RTEMS Workspace and the C Program Heap is.
   */
  bsp_get_work_area(&work_area_start, &work_area_size,
                    &heap_start, &heap_size);

#ifdef CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK
  /* This routine may reduce the work area size with the
   * option to extend it later via sbrk(). If the application
   * was configured w/o CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK then
   * omit this step.
   */
  if ( rtems_malloc_sbrk_helpers ) {
    sbrk_amount = bsp_sbrk_init(work_area_start, &work_area_size);
    work_space_size = rtems_configuration_get_work_space_size();
    if ( work_area_size <  work_space_size && sbrk_amount > 0 ) {
      /* Need to use sbrk right now */
      uintptr_t sbrk_now;

      sbrk_now = (work_space_size - work_area_size) / sbrk_amount;
      sbrk( sbrk_now * sbrk_amount );
    }
  }
#else
  if ( rtems_malloc_sbrk_helpers ) {
    printk("Configuration error!\n"
           "Application was configured with CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK\n"
           "but BSP was configured w/o sbrk support\n");
    status = 1;
    bsp_cleanup( status );
    return status;
  }
#endif

  /*
   *  If the user has configured a set of objects which will require more
   *  workspace than is actually available, print a message indicating
   *  such and return to the invoking initialization code.
   *
   *  NOTE: Output from printk() may not work at this point on some BSPs.
   *
   *  NOTE: Use cast to (void *) and %p since these are uintptr_t types.
   */
  work_space_size = rtems_configuration_get_work_space_size();
  if ( work_area_size <= work_space_size ) {
    printk(
      "bootcard: work space too big for work area: %p >= %p\n",
      (void *) work_space_size,
      (void *) work_area_size
    );
    status = 1;
    bsp_cleanup( status );
    return status;
  }

  if ( !rtems_configuration_get_unified_work_area() ) {
    rtems_configuration_set_work_space_start( work_area_start );
  } else {
    rtems_configuration_set_work_space_start( work_area_start );
    rtems_configuration_set_work_space_size( work_area_size );
    if ( !rtems_configuration_get_stack_allocator_avoids_work_space() ) {
      rtems_configuration_set_stack_space_size( 0 );
    }
  }

  #if (BSP_DIRTY_MEMORY == 1)
    memset( work_area_start, 0xCF,  work_area_size );
  #endif

  /*
   *  Initialize RTEMS data structures
   */
  rtems_initialize_data_structures();

  /*
   *  Initialize the C library for those BSPs using the shared
   *  framework.
   */
  bootcard_bsp_libc_helper(
    work_area_start,
    work_area_size,
    heap_start,
    heap_size,
    sbrk_amount
  );

  /*
   *  Let the BSP do any required initialization now that RTEMS
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
  status = rtems_initialize_start_multitasking();

  /***************************************************************
   ***************************************************************
   *  APPLICATION RUNS HERE!!!  When it shuts down, we return!!! *
   ***************************************************************
   ***************************************************************
   */

  /*
   *  Perform any BSP specific shutdown actions which are written in C.
   */
  bsp_cleanup( status );

  /*
   *  Now return to the start code.
   */
  return status;
}

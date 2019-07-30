/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/bspIo.h>
#include <rtems/version.h>
#include <rtems/score/threadimpl.h>
#include <inttypes.h>

void bsp_fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code code
)
{
  #if BSP_VERBOSE_FATAL_EXTENSION
    Thread_Control *executing;

    printk(
      "\n"
      "*** FATAL ***\n"
      "fatal source: %i (%s)\n",
      source,
      rtems_fatal_source_text( source )
    );
  #endif

  #if (BSP_PRINT_EXCEPTION_CONTEXT) || BSP_VERBOSE_FATAL_EXTENSION
    if ( source == RTEMS_FATAL_SOURCE_EXCEPTION ) {
      rtems_exception_frame_print( (const rtems_exception_frame *) code );
    }
  #endif

  #if BSP_VERBOSE_FATAL_EXTENSION
    else if ( source == INTERNAL_ERROR_CORE ) {
      printk(
        "fatal code: %ju (%s)\n",
        (uintmax_t) code,
        rtems_internal_error_text( code )
      );
    } else {
      printk(
        "fatal code: %ju (0x%08jx)\n",
        (uintmax_t) code,
        (uintmax_t) code
      );
    }

    printk(
      "RTEMS version: %s\n"
      "RTEMS tools: %s\n",
      rtems_version(),
      __VERSION__
    );

    executing = _Thread_Get_executing();

    if ( executing != NULL ) {
      char name[ 2 * THREAD_DEFAULT_MAXIMUM_NAME_SIZE ];

      _Thread_Get_name( executing, name, sizeof( name ) );
      printk(
        "executing thread ID: 0x08%" PRIx32 "\n"
        "executing thread name: %s\n",
        executing->Object.id,
        name
      );
    } else {
      printk( "executing thread is NULL\n" );
    }
  #endif

  #if (BSP_PRESS_KEY_FOR_RESET)
    printk( "\nFATAL ERROR - Executive shutdown! Any key to reboot..." );

    /*
     * Wait for a key to be pressed
     */
    while ( getchark() == -1 )
      ;

    printk("\n");
  #endif

  /*
   *  Check both conditions -- if you want to ask for reboot, then
   *  you must have meant to reset the board.
   */
  #if (BSP_PRESS_KEY_FOR_RESET) || (BSP_RESET_BOARD_AT_EXIT)
    bsp_reset();
  #endif
}

/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsShared
 *
 * @brief This source file contains the implementation of
 *   bsp_fatal_extension().
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/bspIo.h>
#include <rtems/version.h>
#include <rtems/score/heap.h>
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
    const char* TYPE = "*** FATAL ***";
    const char* type = "fatal";

    if ( source == RTEMS_FATAL_SOURCE_EXIT ) {
      if ( code == 0 ) {
        TYPE = "[ RTEMS shutdown ]";
      } else {
        TYPE = "*** EXIT STATUS NOT ZERO ***";
      }
      type = "exit";
    }

    printk(
      "\n"
      "%s\n"
      ,
      TYPE
    );

    if ( source != RTEMS_FATAL_SOURCE_EXIT || code != 0 ) {
      printk(
        "%s source: %i (%s)\n"
        ,
        type,
        source,
        rtems_fatal_source_text( source )
      );
    }

    #ifdef RTEMS_SMP
      printk(
        "CPU: %" PRIu32 "\n"
        ,
        rtems_scheduler_get_processor()
      );
    #endif
  #endif

  #if (BSP_PRINT_EXCEPTION_CONTEXT) || BSP_VERBOSE_FATAL_EXTENSION
    if ( source == RTEMS_FATAL_SOURCE_EXCEPTION ) {
      rtems_exception_frame_print( (const rtems_exception_frame *) code );
    }
  #endif

  #if BSP_VERBOSE_FATAL_EXTENSION
    else if ( source == INTERNAL_ERROR_CORE ) {
      printk(
        "%s code: %ju (%s)\n",
        type,
        (uintmax_t) code,
        rtems_internal_error_text( code )
      );
    #if defined(HEAP_PROTECTION)
    } else if ( source == RTEMS_FATAL_SOURCE_HEAP ) {
      Heap_Error_context *error_context = (Heap_Error_context*) code;
      const char* reasons[] = {
        "HEAP_ERROR_BROKEN_PROTECTOR",
        "HEAP_ERROR_FREE_PATTERN",
        "HEAP_ERROR_DOUBLE_FREE",
        "HEAP_ERROR_BAD_USED_BLOCK",
        "HEAP_ERROR_BAD_FREE_BLOCK"
      };
      const char* reason = "invalid reason";
      if ( error_context->reason <= (int) HEAP_ERROR_BAD_FREE_BLOCK ) {
        reason = reasons[(int) error_context->reason];
      }
      printk(
        "heap error: heap=%p block=%p reason=%s(%d)",
        error_context->heap,
        error_context->block,
        reason,
        error_context->reason
      );
      if ( error_context->reason == HEAP_ERROR_BROKEN_PROTECTOR ) {
        uintptr_t *pb0 = &error_context->block->Protection_begin.protector [0];
        uintptr_t *pb1 = &error_context->block->Protection_begin.protector [1];
        uintptr_t *pe0 = &error_context->block->Protection_end.protector [0];
        uintptr_t *pe1 = &error_context->block->Protection_end.protector [1];
        printk(
          "=[%p,%p,%p,%p]",
          *pb0 != HEAP_BEGIN_PROTECTOR_0 ? pb0 : NULL,
          *pb1 != HEAP_BEGIN_PROTECTOR_1 ? pb1 : NULL,
          *pe0 != HEAP_END_PROTECTOR_0 ? pe0 : NULL,
          *pe1 != HEAP_END_PROTECTOR_1 ? pe1 : NULL
        );
        printk( "\n" );
      }
    #endif
    } else if ( source != RTEMS_FATAL_SOURCE_EXIT || code != 0 ) {
      printk(
        "%s code: %ju (0x%08jx)\n",
        type,
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
        "executing thread ID: 0x%08" PRIx32 "\n"
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

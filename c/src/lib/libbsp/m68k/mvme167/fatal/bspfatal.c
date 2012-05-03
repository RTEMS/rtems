/*  fatal.c
 *
 *  User-define fatal error handler.
 *
 *  Copyright (c) 1998, National Research Council of Canada
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <fatal.h>
#include <string.h>

/*
 *  bsp_fatal_error_occurred
 *
 *  Called when rtems_fatal_error_occurred() is called. Returns control to
 *  167Bug. The _Internal_error_Occurred() function has already saved the
 *  parameters in Internal_errors_What_happened. If the function returns,
 *  RTEMS will halt the CPU.
 *
 *  Make sure the CPU is
 *
 *  Input parameters:
 *    the_source  - what subsystem the error originated in
 *    is_internal - if the error was internally generated
 *    the_error   - fatal error status code
 *
 *  Output parameters:
 *    output to the 167Bug console
 *
 *  Return values: NONE.
 */
User_extensions_routine bsp_fatal_error_occurred(
  Internal_errors_Source  the_source,
  bool                    is_internal,
  uint32_t                the_error
)
{
  struct {
    char index;         /* First byte is number of chars in strbuf  */
    char strbuf[254];   /* In case count is bumped up by one by 167Bug */
  } my_p_str;

  strcat(my_p_str.strbuf,
      "\r\nRTEMS Fatal Error Occurred:\r\n    the_source  = " );

  switch ( the_source ) {
    case INTERNAL_ERROR_CORE:
      strcat(my_p_str.strbuf,
          "INTERNAL_ERROR_CORE\r\n    is_internal = " );
      break;

    case INTERNAL_ERROR_RTEMS_API:
      strcat(my_p_str.strbuf,
          "INTERNAL_ERROR_RTEMS_API\r\n    is_internal = " );
      break;

    case INTERNAL_ERROR_POSIX_API:
      strcat(my_p_str.strbuf,
          "INTERNAL_ERROR_POSIX_API\r\n    is_internal = " );
      break;

    default:
      strcat(my_p_str.strbuf,
          "UNKNOWN\r\n    is_internal = " );
      break;
  }

  if ( is_internal )
    strcat(my_p_str.strbuf,
        "TRUE\r\n    the_error   = 0x|10,8|\r\n" );
  else
    strcat(my_p_str.strbuf,
        "FALSE\r\n    the_error   = 0x|10,8|\r\n" );

  my_p_str.index = strlen(my_p_str.strbuf);
  lcsr->intr_ena = 0;               /* disable interrupts */
  m68k_set_vbr(0xFFE00000);         /* restore 167Bug vectors */

  __asm__ volatile( "movel  %0, -(%%a7)\n\t"
                "pea    (%%a7)\n\t"
                "pea    (%1)\n\t"
                "trap   #15\n\t"         /* trap to 167Bug (.WRITDLN) */
                ".short 0x25\n\t"
                "trap   #15\n\t"
                ".short 0x63"
    :: "d" (the_error), "a" (&my_p_str) );
}

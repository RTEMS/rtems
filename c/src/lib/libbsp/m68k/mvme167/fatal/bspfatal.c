/*  fatal.c
 *
 *  User-define fatal error handler.
 *
 *  Copyright (c) 1998, National Research Council of Canada
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp.h>
#include <fatal.h>

/*
 *  mystrcat
 *
 *  Can't rely on libc being operational. So we provide our own strcat-like
 *  function.
 *
 *  Input parameters:
 *    destination - string (buffer) to append to
 *    source - string to append to the end of destination
 *
 *  Output parameters:
 *    destination - source is appended to the end
 *
 *  Return values:
 *    Number of characters appended.
 */
static int mystrcat(
  char *destination,
  const char *source
)
{
  int i;
  
  for ( i = 0; ( *destination++ = *source++) != '\0'; i++ );
  return i;
}


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
  rtems_boolean           is_internal,
  rtems_unsigned32        the_error
)
{
  struct {
    char index;         /* First byte is number of chars in strbuf  */
    char strbuf[254];   /* In case count is bumped up by one by 167Bug */
  } my_p_str;
  
  my_p_str.index = 0;
  my_p_str.index += mystrcat(
      my_p_str.strbuf + my_p_str.index,
      "\r\nRTEMS Fatal Error Occurred:\r\n    the_source  = " );
  
  switch ( the_source ) {
    case INTERNAL_ERROR_CORE:
      my_p_str.index += mystrcat( 
          my_p_str.strbuf + my_p_str.index,
          "INTERNAL_ERROR_CORE\r\n    is_internal = " );
      break;
      
    case INTERNAL_ERROR_RTEMS_API:
      my_p_str.index += mystrcat( 
          my_p_str.strbuf + my_p_str.index,
          "INTERNAL_ERROR_RTEMS_API\r\n    is_internal = " );
      break;
      
    case INTERNAL_ERROR_POSIX_API:
      my_p_str.index += mystrcat( 
          my_p_str.strbuf + my_p_str.index,
          "INTERNAL_ERROR_POSIX_API\r\n    is_internal = " );
      break;
      
    default:
      my_p_str.index += mystrcat( 
          my_p_str.strbuf + my_p_str.index,
          "UNKNOWN\r\n    is_internal = " );
      break;
  }

  if ( is_internal )
    my_p_str.index += mystrcat(
        my_p_str.strbuf + my_p_str.index, 
        "TRUE\r\n    the_error   = 0x|10,8|\r\n" );
  else
    my_p_str.index += mystrcat( 
        my_p_str.strbuf + my_p_str.index, 
        "FALSE\r\n    the_error   = 0x|10,8|\r\n" );
  
  lcsr->intr_ena = 0;               /* disable interrupts */
  m68k_set_vbr(0xFFE00000);         /* restore 167Bug vectors */
  
  asm volatile( "movel  %0, -(%%a7)\n\t"
                "pea    (%%a7)\n\t"
                "pea    (%1)\n\t"
                "trap   #15\n\t"         /* trap to 167Bug (.WRITDLN) */
                ".short 0x25\n\t"
                "trap   #15\n\t"
                ".short 0x63"       
    :: "d" (the_error), "a" (&my_p_str) );
}

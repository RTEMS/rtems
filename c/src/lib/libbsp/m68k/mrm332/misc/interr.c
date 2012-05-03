/*
 *  Internal Error Handler
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/system.h>
#include <rtems/score/interr.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/userext.h>

/*PAGE
 *
 *  _Internal_error_Occurred
 *
 *  This routine will invoke the fatal error handler supplied by the user
 *  followed by the the default one provided by the executive.  The default
 *  error handler assumes no hardware is present to help inform the user
 *  of the problem.  Halt stores the error code in a known register,
 *  disables interrupts, and halts the CPU.  If the CPU does not have a
 *  halt instruction, it will loop to itself.
 *
 *  Input parameters:
 *    the_source  - what subsystem the error originated in
 *    is_internal - if the error was internally generated
 *    the_error   - fatal error status code
 *
 *  Output parameters:
 *    As much information as possible is stored in a CPU dependent fashion.
 *    See the CPU dependent code for more information.
 *
 *  NOTE: The the_error is not necessarily a directive status code.
 */

/*
 * Ugly hack.... _CPU_Fatal_halt() disonnects the bdm. Without this
 * change, the_error is only known only to the cpu :).
 *
 * From "bsp.h" which is not yet available in the arch tree during
 * this phase of install. jsg
 */
void outbyte(char);

#define RAW_PUTS(str) \
  { register char *ptr = str; \
    while (*ptr) outbyte(*ptr++); \
  }

#define RAW_PUTI(n) { \
    register int i, j; \
    \
    RAW_PUTS("0x"); \
    for (i=28;i>=0;i -= 4) { \
      j = (n>>i) & 0xf; \
      outbyte( (j>9 ? j-10+'a' : j+'0') ); \
    } \
  }

void volatile _Internal_error_Occurred(
  Internal_errors_Source  the_source,
  bool                    is_internal,
  uint32_t                the_error
)
{

  _Internal_errors_What_happened.the_source  = the_source;
  _Internal_errors_What_happened.is_internal = is_internal;
  _Internal_errors_What_happened.the_error   = the_error;

  _User_extensions_Fatal( the_source, is_internal, the_error );

  _System_state_Set( SYSTEM_STATE_FAILED );

  /* try to print error message to outbyte */
  RAW_PUTS("\r\nRTEMS: A fatal error has occured.\r\n");
  RAW_PUTS("RTEMS:    fatal error ");
  RAW_PUTI( the_error );
  RAW_PUTS(" (");
  outbyte( (char)((the_error>>24) & 0xff) );
  outbyte( (char)((the_error>>16) & 0xff) );
  outbyte( (char)((the_error>>8) & 0xff) );
  outbyte( (char)(the_error & 0xff) );
  RAW_PUTS(").\r\n");

  /* configure peripherals for a safe exit */
  bsp_cleanup(1);

  _CPU_Fatal_halt( the_error );

  /* will not return from this routine */
}

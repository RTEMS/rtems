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

/*
 * Ugly hack.... _CPU_Fatal_halt() disonnects the bdm. Without this
 * change, the_error is only known only to the cpu :).
 */

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

void bsp_fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code the_error
)
{
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
}

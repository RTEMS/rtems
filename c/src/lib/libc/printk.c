/*-------------------------------------------------------------------------+
| printk.c v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on code by: Jose Rufino - IST
|
|  $Id$
+--------------------------------------------------------------------------*/

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include <stdio.h>
#include <rtems/bspIo.h>

/*-------------------------------------------------------------------------+
|         Function: printNum
|      Description: print number in a given base.
| Global Variables: None.
|        Arguments: num - number to print, base - base used to print the number.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
static void
printNum(long unsigned int num, int base, int sign, int maxwidth, int lead)
{
  long unsigned int n;
  int count;
  char toPrint[20];

  if ( (sign == 1) && ((long)num <  0) ) {
    BSP_output_char('-');
    num = -num;
    if (maxwidth) maxwidth--;
  }
  
  count = 0;
  while ((n = num / base) > 0) {
    toPrint[count++] = (num - (n*base));
    num = n ;
  }
  toPrint[count++] = num;

  if (maxwidth) {
    for (n=maxwidth-count ; n ; n-- )
      BSP_output_char(lead);
  }

  for (n = 0; n < count; n++){
    BSP_output_char("0123456789ABCDEF"[(int)(toPrint[count-(n+1)])]);
  }
} /* printNum */


/*-------------------------------------------------------------------------+
|         Function: printk
|      Description: a simplified version of printf intended for use when the
                    console is not yet initialized or in ISR's.
| Global Variables: None.
|        Arguments: as in printf: fmt - format string, ... - unnamed arguments.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
printk(char *fmt, ...)
{
  va_list  ap;      /* points to each unnamed argument in turn */
  char     c, *str;
  int      lflag, base, sign, width, lead;
  /* unsigned int level; */

  /* _CPU_ISR_Disable(level); */
 
  va_start(ap, fmt); /* make ap point to 1st unnamed arg */
  for (; *fmt != '\0'; fmt++)
  {
    lflag = 0;
    base  = 0;
    sign = 0;
    width = 0;
    lead = ' ';
    if (*fmt == '%')
    {
      fmt++;
      if (*fmt == '0' ) {
        lead = '0';
        fmt++;
      }
      while (*fmt >= '0' && *fmt <= '9' ) {
        width *= 10;
        width += (*fmt - '0');
        fmt++;
      }

      if ((c = *fmt) == 'l')
      {
        lflag = 1;
        c = *++fmt;
      }
      switch (c)
      {
        case 'o': case 'O': base = 8; sign = 0; break;
        case 'd': case 'D': base = 10; sign = 1; break;
        case 'u': case 'U': base = 10; sign = 0; break;
        case 'x': case 'X': base = 16; sign = 0; break;
        case 's':
          for (str = va_arg(ap, char *); *str; str++) 
            BSP_output_char(*str);
          break;
        case 'c':
#if 0
#if defined(_TMS320C3x) || defined(_TMS320C4x)
          BSP_output_char(va_arg(ap, int));
#else
          BSP_output_char(va_arg(ap, char));
#endif
#else
          BSP_output_char(va_arg(ap, int));
#endif
          break;
        default:
          BSP_output_char(c);
          break;
      } /* switch*/

      if (base)
        printNum(lflag ? va_arg(ap, long int) : (long int)va_arg(ap, int),
                 base, sign, width, lead);
    }
    else
    {
      BSP_output_char(*fmt);
    }
  }
  va_end(ap); /* clean up when done */
  /* _CPU_ISR_Enable(level); */

} /* printk */


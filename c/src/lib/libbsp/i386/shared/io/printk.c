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


#include <stdarg.h>

#include <bspIo.h>

/*-------------------------------------------------------------------------+
|         Function: printNum
|      Description: print number in a given base.
| Global Variables: None.
|        Arguments: num - number to print, base - base used to print the number.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
static void
printNum(long int num, int base)
{
  long int n;

  if ((n = num / base) > 0)
    printNum(n, base);
  BSP_output_char("0123456789ABCDEF"[(int)(num % base)]);
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
  int      lflag, base;
 
  va_start(ap, fmt); /* make ap point to 1st unnamed arg */
  for (; *fmt != '\0'; fmt++)
  {
    lflag = 0;
    base  = 0;
    if (*fmt == '%')
    {
      if ((c = *++fmt) == 'l')
      {
        lflag = 1;
        c = *++fmt;
      }
      switch (c)
      {
        case 'o': case 'O': base = 8; break;
        case 'd': case 'D': base = 10; break;
        case 'x': case 'X': base = 16; break;
        case 's':
          for (str = va_arg(ap, char *); *str; str++) 
            BSP_output_char(*str);
          break;
        case 'c':
          BSP_output_char(va_arg(ap, char));
          break;
        default:
          BSP_output_char(c);
          break;
      } /* switch*/

      if (base)
        printNum(lflag ? va_arg(ap, long int) : (long int)va_arg(ap, int),
                 base);
    }
    else
    {
      BSP_output_char(*fmt);
    }
  }
  va_end(ap); /* clean up when done */
} /* printk */


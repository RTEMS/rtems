/*
 *  Author: Fernando RUIZ CASAS
 *  Work: fernando.ruiz@ctv.es
 *  Home: correo@fernando-ruiz.com
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <rtems/shell.h>
#include "internal.h"

/*
 * str to int "0xaffe" "0b010010" "0123" "192939"
 */
int rtems_shell_str2int(char * s) {
  int sign=1;
  int base=10;
  int value=0;
  int digit;

  if (!s) return 0;
  if (*s) {
    if (*s=='-') {
      sign=-1;
      s++;
      if (!*s) return 0;
    }
    if (*s=='0') {
      s++;
      switch(*s) {
        case 'x':
        case 'X':
          s++;
 	  base=16;
	  break;
        case 'b':
        case 'B':
          s++;
	  base=2;
	  break;
        default:
          base=8;
	  break;
      }
    }
    while (*s) {
      switch(*s) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          digit=*s-'0';
 	  break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
          digit=*s-'A'+10;
	  break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
          digit=*s-'a'+10;
	  break;
        default:
          return value*sign;
      }
      if (digit>base)
        return value*sign;
      value=value*base+digit;
      s++;
    }
 }
 return value*sign;
}

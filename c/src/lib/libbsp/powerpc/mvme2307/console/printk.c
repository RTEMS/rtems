#include <bsp.h>

#if 1

static void outbyte(char c) {
    volatile char * const pc16550 = (char *) IO_TO_LOCAL(0x03f8);
#   define THR  0
#   define LSR  5
#   define THRE 0x20
    while ((pc16550[LSR] & THRE) == 0) {
        ;
    }
    pc16550[THR] = c;
}

#else

/* printk to memory for debugging */
static void outbyte(char c) {
    static char *memory_log = (char *) 0x01F00000;

    *memory_log++ = c;
    if (memory_log >= (char *) 0x01F80000) {
        memory_log--;
    }
}

#endif

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

/*-------------------------------------------------------------------------+
|         Function: printNum
|      Description: print number in a given base.
| Global Variables: None.
|        Arguments: num - number to print, base - base used to print the number.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
static void printNum(long unsigned int num, int base, int sign, int width,
                     int zerofill) {
    long unsigned int n;
    int count, negative = 0;
    char toPrint[80];
    char *digits = "0123456789ABCDEF";

    if (width > sizeof(toPrint)) {
        width = sizeof(toPrint);
    }
    if ((sign == 1) && ((long) num < 0)) {
        negative = 1;
        num = -num;
    }

    count = 0;
    while ((n = num / base) > 0) {
        toPrint[count++] = digits[num - (n * base)];
        num = n;
    }
    toPrint[count++] = digits[num];
    if (count + negative < width) {
        /* this needs to be padded out to width */
        if (zerofill) {
            while (count + negative < width) {
                toPrint[count++] = '0';
            }
            if (negative) {
                toPrint[count++] = '-';
            }
        } else {
            if (negative) {
                toPrint[count++] = '-';
            }
            while (count < width) {
                toPrint[count++] = ' ';
            }
        }
    } else if (negative) {
        toPrint[count++] = '-';
    }

    for (n = 0; n < count; n++) {
        outbyte(toPrint[count - (n + 1)]);
    }
}


/*-------------------------------------------------------------------------+
|         Function: printk
|      Description: a simplified version of printf intended for use when the
                    console is not yet initialized or in ISR's.
| Global Variables: None.
|        Arguments: as in printf: fmt - format string, ... - unnamed arguments.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
int printk_enabled = 1;

void printk(char *fmt, ...) {
    va_list ap;			/* points to each unnamed argument in turn */
    char c, *str;
    int lflag, base, sign, width, zero;

    /* disable interrupts???  */

    if (printk_enabled) {
        va_start(ap, fmt);		/* make ap point to 1st unnamed arg */
        for (; *fmt != '\0'; fmt++) {
            lflag = 0;
            base = 0;
            sign = 0;
            width = 0;
            if (*fmt == '\n') {
                outbyte('\r');
            }
            if (*fmt == '%') {
                c = *++fmt;
                if (c == '0') {
                    zero = 1;
                    c = *++fmt;
                } else {
                    zero = 0;
                }
                for (; c >= '0' && c <= '9'; c = *++fmt) {
                    width = width * 10 + (c - '0');
                }
                if (c == 'l') {
                    lflag = 1;
                    c = *++fmt;
                }
                switch (c) {
                    case 'o':
                    case 'O':
                        base = 8;
                        sign = 0;
                        break;
                    case 'd':
                    case 'D':
                        base = 10;
                        sign = 1;
                        break;
                    case 'u':
                    case 'U':
                        base = 10;
                        sign = 0;
                        break;
                    case 'x':
                    case 'X':
                        base = 16;
                        sign = 0;
                        break;
                    case 's':
                        for (str = va_arg(ap, char *); *str; str++) {
                            outbyte(*str);
                            width--;
                        }
                        while (width-- > 0) {
                            outbyte(' ');
                        }
                        break;
                    case 'c':
                        outbyte(va_arg(ap, int));
                        break;
                    default:
                        outbyte(c);
                        break;
                }

                if (base) {
                    printNum(lflag ? va_arg(ap, long int) : 
                             (long int) va_arg(ap, int), base, sign, width, zero);
                }
            } else {
                outbyte(*fmt);
            }
        }
        va_end(ap);			/* clean up when done */
        /* enable interrupts???  */
    }
}

/*  efi68k.h
 *
 *  $Id$
 */

#ifndef _EFI68k_H_
#define _EFI68k_H_


/* interrupt levels */
#define WD_ISR_LEVEL 1
#define TCP_ISR_LEVEL 4
#define UART_ISR_LEVEL 6
#define INTR7 7


/* macro/function definitions */
static void reboot(void) __attribute__ ((noreturn));
__inline__ static void reboot(void) {asm("trap #15");}
     
#endif /* _EFI68k_H_ */

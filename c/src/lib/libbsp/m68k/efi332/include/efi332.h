/*  efi332.h
 *
 *  $Id$
 */

#ifndef _EFI332_H_
#define _EFI332_H_


/* SIM_MM (SIM Module Mapping) determines the location of the control
   register block. When MM=0, register addresses range fom 0x7ff000 to
   0x7FFFFF. When MM=1, register addresses range from 0xfff000 to
   0xffffff. */
#define SIM_MM 1


/* Interrupt related definitions */
#define SIM_IARB 15
#define QSM_IARB 10

#define EFI_PIV 64
#define ISRL_PIT 4		/* zero disables PIT */

#define EFI_QIVR 66		/* 66=>SCI and 67=>QSPI interrupt */
#define ISRL_QSPI 0

#define EFI_SPINT 24		/* spurious interrupt */
#define EFI_INT1 25		/* CTS interrupt */
#define ISRL_SCI 6



/* System Clock definitions */
#define XTAL 32768.0		/* crystal frequency in Hz */
#define EFI_W 0			/* system clock parameters */
#define EFI_X 1
#define EFI_Y 0x38
#define SYS_CLOCK (XTAL*4.0*(EFI_Y+1)*(1 << (2*EFI_W+EFI_X)))
#define SCI_BAUD 115200		/* RS232 Baud Rate */


/* macros/functions */
void reboot(void) __attribute__ ((noreturn));

#endif /* _EFI332_H_ */

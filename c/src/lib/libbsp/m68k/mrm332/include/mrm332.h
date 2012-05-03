/*  mrm332.h
 */

#ifndef _MRM332_H_
#define _MRM332_H_

/* SIM_MM (SIM Module Mapping) determines the location of the control
   register block. When MM=0, register addresses range fom 0x7ff000 to
   0x7FFFFF. When MM=1, register addresses range from 0xfff000 to
   0xffffff. */
#define SIM_MM 1

/* Interrupt related definitions */
#define SIM_IARB 15
#define QSM_IARB 10

#define MRM_PIV 64
#define ISRL_PIT 4		/* zero disables PIT */

#define EFI_QIVR 66		/* 66=>SCI and 67=>QSPI interrupt */
#define ISRL_QSPI 0

#define EFI_SPINT 24		/* spurious interrupt */
#define EFI_INT1 25		/* CTS interrupt */
#define ISRL_SCI 6

/* System Clock definitions */
#define XTAL 32768.0		/* crystal frequency in Hz */

#if 0
/* Default MRM clock rate (8.388688 MHz) set by CPU32: */
#define MRM_W 0			/* system clock parameters */
#define MRM_X 0
#define MRM_Y 0x3f
#endif

#if 1
/* 16.77722 MHz: */
#define MRM_W 1			/* system clock parameters */
#define MRM_X 1
#define MRM_Y 0x0f
#endif

#if 0
/* 25.16582 MHz: */
#define MRM_W 1			/* system clock parameters */
#define MRM_X 1
#define MRM_Y 0x17
#endif

#define SYS_CLOCK (XTAL*4.0*(MRM_Y+1)*(1 << (2*MRM_W+MRM_X)))
#define SCI_BAUD 19200		/* RS232 Baud Rate */

/* macros/functions */

#ifndef ASM

/*
 *  This prototype really should have the noreturn attribute but
 *  that causes a warning. Not sure how to fix that.
 */
/*   static void reboot(void) __attribute__ ((noreturn)); */
static void reboot(void);
__inline__ static void reboot() {__asm__ ("trap #15; .word 0x0063");}

#endif /* ASM */

#endif /* _MRM_H_ */

/*
 *-------------------------------------------------------------------
 *
 *   DP8570A -- header file for National Semiconducor's DP8570A TCP
 *
 * This file has been created by John S. Gwynne for the efi68k
 * project.
 * 
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *------------------------------------------------------------------
 *
 *  $Id$
 */

#ifndef _DP8570A_H_
#define _DP8570A_H_

/* base address is the physical location of register 0 */
#define TCP_BASE_ADDRESS 0x0600001


/* definitions of register addresses and associate bits */

/* ********************************************************* */
/* Control Registers */
/* ********************************************************* */

/* REMEMBER: if you are in an interrupt routine, you must
   reset RS and PS of MSR to the value they had on entry
   to the ISR before exiting */

#define MSR (volatile unsigned char * const)(0x00*2+TCP_BASE_ADDRESS)
				/* Main Status Register */
#define    INT 0x01		/*    Interrupt Status */
#define    PF  0x02		/*    Power Fail Interrupt */
#define    PER 0x04		/*    Period Interrupt */
#define    AL  0x08		/*    Alarm Interrupt */
#define    T0  0x10		/*    Timer 0 Interrupt */
#define    T1  0x20		/*    Timer 1 Interrupt */
#define    RS  0x40		/*    Register Select Bit */
#define    PS  0x80		/*    Page Select Bit */

#define T0CR (volatile unsigned char * const)(0x01*2+TCP_BASE_ADDRESS)
				/* Timer 0 Control Register */
#define T1CR (volatile unsigned char * const)(0x02*2+TCP_BASE_ADDRESS)
				/* Timer 1 Control Register */
#define    TSS 0x01		/*    Timer Start/!Stop */
#define    M0  0x02		/*    Mode Select */
#define    M1  0x04		/*    Mode Select */
#define    C0  0x08		/*    Input Clock Select */
#define    C1  0x10		/*    Input Clock Select */
#define    C2  0x20		/*    Input Clock Select */
#define    RD  0x40		/*    Timer Read */
#define    CHG 0x80		/*    Count Hold/Gate */

#define PFR (volatile unsigned char * const)(0x03*2+TCP_BASE_ADDRESS)
				/* Periodic Flag Register */
#define    R_1MIN  0x01		/*    Minute Flage */
#define    R_10S   0x02		/*    10 Second Flag */
#define    R_1S    0x04		/*    Second Flag */
#define    R_100MS 0x08		/*    100 Millisec Flag */
#define    R_10MS  0x10		/*    10 Millisec Flag */
#define    R_1MS   0x20		/*    1 Millisec Flag */
#define    OSF     0x40		/*    Oscillator Failed/Single Supply */
#define    TMODE   0x80		/*    Test Mode Enable */

#define IRR (volatile unsigned char * const)(0x04*2+TCP_BASE_ADDRESS)
				/* Interrupt Routing Register */
#define    PF_R 0x01		/*    Power Fail Route */
#define    PR_R 0x02		/*    Periodic Route */
#define    AL_R 0x04		/*    Alarm Route */
#define    T0_R 0x08		/*    Timer 0 Route */
#define    T1_R 0x10		/*    Timer 1 Route */
#define    PFD  0x20		/*    PF Delay Enable */
#define    LBF  0x40		/*    Low Battery Flag */
#define    TMSE 0x80		/*    Time Save Enable */

#define RTMR (volatile unsigned char * const)(0x01*2+TCP_BASE_ADDRESS)
				/* Real Time Mode Register */
#define    LY0 0x01		/*    Leap Year LSB */
#define    LY1 0x02		/*    Leap Year MSB */
#define    H12 0x04		/*    12/!24 Hour Mode */
#define    CSS 0x08		/*    Clock Start/!Stop */
#define    IPF 0x10		/*    Interrupt PF Operation */
#define    TPF 0x20		/*    Timer PF Operation */
#define    XT0 0x40		/*    Crystal Frequency LSB */
#define    XT1 0x80		/*    Crystal Frequency MSB */

#define OMR (volatile unsigned char * const)(0x02*2+TCP_BASE_ADDRESS)
				/* Output Mode Register */
#define    TH  0x01		/*    T1 Active Hi/!Low */
#define    TP  0x02		/*    T1 Push Pull/!Open Drain */
#define    IH  0x04		/*    INTR Active Hi/!Low */
#define    IP  0x08		/*    INTR Push Pull/!Open Drain */
#define    MH  0x10		/*    MFO Active Hi/!Low */
#define    MP  0x20		/*    MFO Push Pull/!Open Drain */
#define    MT  0x40		/*    MFO Pin as Timer 0 */
#define    MO  0x80		/*    MFO Pin as Oscillator */

#define ICR0 (volatile unsigned char * const)(0x03*2+TCP_BASE_ADDRESS)
				/* Interrupt control Register 0 */
#define    ME  0x01		/*    Minutes Enable */
#define    TSE 0x02		/*    10 Second Enable */
#define    SE  0x04		/*    Seconds Enable */
#define    HME 0x08		/*    100 Millisec Enable */
#define    TME 0x10		/*    10 Millisec Enable */
#define    OME 0x20		/*    Millisec Enable */
#define    T0E 0x40		/*    Timer 0 Enable */
#define    T1E 0x80		/*    Timer 1 Enable */

#define ICR1 (volatile unsigned char * const)(0x04*2+TCP_BASE_ADDRESS)
				/* Interrupt control Register 1 */
#define    SCE  0x01		/*    Second Compare Enable */
#define    MNE  0x02		/*    Minute Compare Enable */
#define    HRE  0x04		/*    Hour Compare Enable */
#define    DOME 0x08		/*    Day of Month Compare Enable */
#define    MOE  0x10		/*    Month Compare Enable */
#define    DOWE 0x20		/*    Day of Week Compare Enable */
#define    ALE  0x40		/*    Alarm Interrupt Enable */
#define    PFE  0x80		/*    Power Fail Interrupt Enable */



/* ********************************************************* */
/* Counters: Clock and Calendar (data is stored in BCD) */
/* ********************************************************* */
#define HOFS (volatile unsigned char * const)(0x05*2+TCP_BASE_ADDRESS)
				/* Hundredth of Seconds */
#define SEC (volatile unsigned char * const)(0x06*2+TCP_BASE_ADDRESS)
				/* Seconds */
#define MIN (volatile unsigned char * const)(0x07*2+TCP_BASE_ADDRESS)
				/* Minutes */
#define HRS (volatile unsigned char * const)(0x08*2+TCP_BASE_ADDRESS)
				/* Hours */
#define DOM (volatile unsigned char * const)(0x09*2+TCP_BASE_ADDRESS)
				/* Day of Month */
#define MON (volatile unsigned char * const)(0x0a*2+TCP_BASE_ADDRESS)
				/* Month */
#define YR (volatile unsigned char * const)(0x0b*2+TCP_BASE_ADDRESS)
				/* Year */
#define JD_LSB (volatile unsigned char * const)(0x0c*2+TCP_BASE_ADDRESS)
				/* Julian Date (LSB) */
#define JD_MSM (volatile unsigned char * const)(0x0d*2+TCP_BASE_ADDRESS)
				/* Julian Date (MSB) */
#define DOW (volatile unsigned char * const)(0x0e*2+TCP_BASE_ADDRESS)
				/* Day of week */


/* ********************************************************* */
/* Timer Data Registers */
/* ********************************************************* */
#define T0_LSB (volatile unsigned char * const)(0x0f*2+TCP_BASE_ADDRESS)
				/* Timer 0 LSB */
#define T0_MSB (volatile unsigned char * const)(0x10*2+TCP_BASE_ADDRESS)
				/* Timer 0 MSB */
#define T1_LSB (volatile unsigned char * const)(0x11*2+TCP_BASE_ADDRESS)
				/* Timer 1 LSB */
#define T1_MSB (volatile unsigned char * const)(0x12*2+TCP_BASE_ADDRESS)
				/* Timer 1 MSB */


/* ********************************************************* */
/* Timer Compare RAM */
/* ********************************************************* */
#define TC_SEC (volatile unsigned char * const)(0x13*2+TCP_BASE_ADDRESS)
				/* Seconds Compare RAM */
#define TC_MIN (volatile unsigned char * const)(0x14*2+TCP_BASE_ADDRESS)
				/* Minutes Compare RAM */
#define TC_HRS (volatile unsigned char * const)(0x15*2+TCP_BASE_ADDRESS)
				/* Hours Compare RAM */
#define TC_DOM (volatile unsigned char * const)(0x16*2+TCP_BASE_ADDRESS)
				/* Day of Month Compare RAM */
#define TC_MON (volatile unsigned char * const)(0x17*2+TCP_BASE_ADDRESS)
				/* Month Compare RAM */
#define TC_DOW (volatile unsigned char * const)(0x18*2+TCP_BASE_ADDRESS)
				/* Day of Week Compare RAM */


/* ********************************************************* */
/* Time Save RAM */
/* ********************************************************* */
#define S_SEC (volatile unsigned char * const)(0x19*2+TCP_BASE_ADDRESS)
				/* Seconds Save RAM */
#define S_MIN (volatile unsigned char * const)(0x1a*2+TCP_BASE_ADDRESS)
				/* Minutes Save RAM */
#define S_HRS (volatile unsigned char * const)(0x1b*2+TCP_BASE_ADDRESS)
				/* Hours Save RAM */
#define S_DOM (volatile unsigned char * const)(0x1c*2+TCP_BASE_ADDRESS)
				/* Day of Month Save RAM */
#define S_MON (volatile unsigned char * const)(0x1d*2+TCP_BASE_ADDRESS)
				/* Month Save RAM */


/* ********************************************************* */
/* Miscellaneous Registers */
/* ********************************************************* */
				/* rem: 0x1e is general purpose RAM */
#define TMR (volatile unsigned char * const)(0x1F*2+TCP_BASE_ADDRESS)
				/* RAM/Test Mode Register */



/* ********************************************************* */
/* RAM allocation */
/* ********************************************************* */
#define RAM_OSC_FAIL (volatile unsigned char * const)(0x01*2+TCP_BASE_ADDRESS)
				/* 1: osc. failed time lost */
#define RAM_POWERUP (volatile unsigned char * const)(0x02*2+TCP_BASE_ADDRESS)
				/* 1: power was removed and the applied
				      before last TCP init */
#define RAM_LOWBAT (volatile unsigned char * const)(0x03*2+TCP_BASE_ADDRESS)
				/* 1: battery voltage is low (2.2V) */
				/* not valid in single supply mode */
#define RAM_SINGLE_SUP (volatile unsigned char * const)(0x04*2+TCP_BASE_ADDRESS)
                                /* 1: single supply mode */
                                /* note: single supply mode will be
				   selected when no backup battery is
				   present and/or the first time the
				   system is booted after the loss of
				   backup battery voltage. */
#define RAM_TCP_FAILURE (volatile unsigned char * const)(0x05*2+TCP_BASE_ADDRESS)
				/* 1: TCP failed to start oscillating */


/* ********************************************************* */
/* TCP data structures */
/* ********************************************************* */

struct clock_counters {
  unsigned char hofs;
  unsigned char d0;		/* the dx's are place holders since */
  unsigned char sec;		/* the TCP is addressable only on */
  unsigned char d1;		/* odd addresses. */
  unsigned char min;
  unsigned char d2;
  unsigned char hrs;
  unsigned char d3;
  unsigned char dom;
  unsigned char d4;
  unsigned char mon;
  unsigned char d5;
  unsigned char yr;
  unsigned char d6;
  unsigned char jd0;
  unsigned char d7;
  unsigned char jd1;
  unsigned char d8;
  unsigned char dow;
};

extern struct clock_ram * const tcp_power_up;

struct clock_ram {
  unsigned char sec;
  unsigned char d0;		/* the dx's are place holders since */
  unsigned char min;		/* the TCP is addressable only on */
  unsigned char d1;		/* odd addresses. */
  unsigned char hrs;
  unsigned char d2;
  unsigned char dom;
  unsigned char d3;
  unsigned char mon;
};

extern struct clock_ram * const tcp_power_up;
extern struct clock_ram * const tcp_power_down;
extern struct clock_counters * const tcp_clock;
extern struct clock_ram * const tcp_save_ram;

void bsp_tcp_init(void);

#endif /* _DP8570A_H_ */

/*-------------------------------------------------------------------------+
| timer.c v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
| This file contains the PC386 timer package.
+--------------------------------------------------------------------------+
| NOTE: It is important that the timer start/stop overhead be determined
|       when porting or modifying this code.
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
| This code is base on:
|   timer.c,v 1.7 1995/12/19 20:07:43 joel Exp - go32 BSP
| With the following copyright notice:
| **************************************************************************
| * COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.                      *
| * On-Line Applications Research Corporation (OAR).                       *
| * All rights assigned to U.S. Government, 1994.                          *
| *                                                                        *
| * This material may be reproduced by or for the U.S. Government pursuant *
| * to the copyright license under the clause at DFARS 252.227-7013.  This *
| * notice must appear in all copies of this file and its derivatives.     *
| **************************************************************************
+--------------------------------------------------------------------------*/


#include <stdlib.h>

#include <bsp.h>
#include <irq.h>

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/
#define TIMER_IRQ     0x00           /* Timer IRQ.                            */
#define AVG_OVERHEAD  0              /* 0.1 microseconds to start/stop timer. */
#define LEAST_VALID   1              /* Don't trust a value lower than this.  */

/*-------------------------------------------------------------------------+
| Global Variables
+--------------------------------------------------------------------------*/
volatile rtems_unsigned32 Ttimer_val;
         rtems_boolean    Timer_driver_Find_average_overhead = TRUE;

/*-------------------------------------------------------------------------+
| Pentium optimized timer handling.
+--------------------------------------------------------------------------*/
#if defined(pentium)

/*-------------------------------------------------------------------------+
|         Function: rdtsc
|      Description: Read the value of PENTIUM on-chip cycle counter.
| Global Variables: None.
|        Arguments: None.
|          Returns: Value of PENTIUM on-chip cycle counter. 
+--------------------------------------------------------------------------*/
static inline unsigned long long
rdtsc(void)
{
  /* Return the value of the on-chip cycle counter. */
  unsigned long long result;
  asm volatile(".byte 0x0F, 0x31" : "=A" (result));
  return result;
} /* rdtsc */


/*-------------------------------------------------------------------------+
|         Function: Timer_exit
|      Description: Timer cleanup routine at RTEMS exit. NOTE: This routine is
|                   not really necessary, since there will be a reset at exit.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
Timer_exit(void)
{
  PC386_disableIrq(TIMER_IRQ);
} /* Timer_exit */


/*-------------------------------------------------------------------------+
|         Function: Timer_initialize
|      Description: Timer initialization routine.
| Global Variables: Ttimer_val.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
Timer_initialize(void)
{
  static rtems_boolean First = TRUE;

  if (First)
  {
    First = FALSE;

    atexit(Timer_exit); /* Try not to hose the system at exit. */
    PC386_enableIrq(TIMER_IRQ);
                  /* Disable the programmable timer so ticks don't interfere. */
  }
  Ttimer_val = rdtsc(); /* read starting time */
} /* Timer_initialize */


/*-------------------------------------------------------------------------+
|         Function: Read_timer
|      Description: Read hardware timer value.
| Global Variables: Ttimer_val, Timer_driver_Find_average_overhead.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
rtems_unsigned32
Read_timer(void)
{
  register rtems_unsigned32 total;

  total =  (rtems_unsigned32)(rdtsc() - Ttimer_val);

  if (Timer_driver_Find_average_overhead)
    return total;
  else if (total < LEAST_VALID)
    return 0; /* below timer resolution */
  else
    return (total - AVG_OVERHEAD);
} /* Read_timer */

#else /* pentium */

/*-------------------------------------------------------------------------+
| Non-Pentium timer handling.
+--------------------------------------------------------------------------*/
#define US_PER_ISR   250  /* Number of micro-seconds per timer interruption */

/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/
extern rtems_isr timerisr(rtems_vector_number);
       /* timer (int 08h) Interrupt Service Routine (defined in 'timerisr.s') */

/*-------------------------------------------------------------------------+
|         Function: Timer_exit
|      Description: Timer cleanup routine at RTEMS exit. NOTE: This routine is
|                   not really necessary, since there will be a reset at exit.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
Timer_exit(void)
{
  /* reset timer mode to standard (DOS) value */
  outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN);
  outport_byte(TIMER_CNTR0, 0);
  outport_byte(TIMER_CNTR0, 0);
} /* Timer_exit */


/*-------------------------------------------------------------------------+
|         Function: Timer_initialize
|      Description: Timer initialization routine.
| Global Variables: Ttimer_val.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
Timer_initialize(void)
{
  static rtems_boolean First = TRUE;

  if (First)
  {
    First = FALSE;

    atexit(Timer_exit); /* Try not to hose the system at exit. */

    /* install a raw interrupt handler for timer */
    PC386_installRawIrqHandler(TIMER_IRQ, timerisr);

    /* load timer for US_PER_ISR microsecond period */
    outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN);
    outport_byte(TIMER_CNTR0, US_TO_TICK(US_PER_ISR) >> 0 & 0xff);
    outport_byte(TIMER_CNTR0, US_TO_TICK(US_PER_ISR) >> 8 & 0xff);
  }
  /* wait for ISR to be called at least once */
  Ttimer_val = 0;
  while (Ttimer_val == 0)
    continue;
  Ttimer_val = 0;
} /* Timer_initialize */


/*-------------------------------------------------------------------------+
|         Function: Read_timer
|      Description: Read hardware timer value.
| Global Variables: Ttimer_val, Timer_driver_Find_average_overhead.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
rtems_unsigned32
Read_timer(void)
{
  register rtems_unsigned32 total, clicks;
  register rtems_unsigned8  lsb, msb;

  outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_LATCH);
  inport_byte(TIMER_CNTR0, lsb);
  inport_byte(TIMER_CNTR0, msb);
  clicks = (msb << 8) | lsb;
  total  = (Ttimer_val * US_PER_ISR) + (US_PER_ISR - TICK_TO_US(clicks));

  if (Timer_driver_Find_average_overhead)
    return total;
  else if (total < LEAST_VALID)
    return 0; /* below timer resolution */
  else
    return (total - AVG_OVERHEAD);
}

#endif /* pentium */


/*-------------------------------------------------------------------------+
|         Function: Empty_function
|      Description: Empty function used in time tests.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
rtems_status_code Empty_function(void)
{
  return RTEMS_SUCCESSFUL;
} /* Empty function */
 

/*-------------------------------------------------------------------------+
|         Function: Set_find_average_overhead
|      Description: Set internal Timer_driver_Find_average_overhead flag value.
| Global Variables: Timer_driver_Find_average_overhead.
|        Arguments: find_flag - new value of the flag.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
Set_find_average_overhead(rtems_boolean find_flag)
{
  Timer_driver_Find_average_overhead = find_flag;
} /* Set_find_average_overhead */

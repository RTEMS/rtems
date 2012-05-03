/*
 *  tm27.h
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

/*
 *  Stuff for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 1

#ifndef SH7750_EVT_WDT_ITI
#   error "..."
#endif

#define Install_tm27_vector( handler ) \
{ \
    rtems_isr_entry old_handler; \
    rtems_status_code status; \
    status = rtems_interrupt_catch( (handler), \
            SH7750_EVT_TO_NUM(SH7750_EVT_WDT_ITI), &old_handler); \
    if (status != RTEMS_SUCCESSFUL) \
        printf("Status of rtems_interrupt_catch = %d", status); \
}

#define Cause_tm27_intr() \
{ \
    *(volatile uint16_t*)SH7750_IPRB |= 0xf000; \
    *(volatile uint16_t*)SH7750_WTCSR = SH7750_WTCSR_KEY; \
    *(volatile uint16_t*)SH7750_WTCNT = SH7750_WTCNT_KEY | 0xfe; \
    *(volatile uint16_t*)SH7750_WTCSR = \
                            SH7750_WTCSR_KEY | SH7750_WTCSR_TME; \
}

#define Clear_tm27_intr() \
{ \
    *(volatile uint16_t*)SH7750_WTCSR = SH7750_WTCSR_KEY; \
}

#define Lower_tm27_intr() \
{ \
    sh_set_interrupt_level((SH7750_IPRB & 0xf000) << SH4_SR_IMASK_S); \
}

#endif

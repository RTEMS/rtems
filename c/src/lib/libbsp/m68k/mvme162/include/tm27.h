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
 *  Define the interrupt mechanism for Time Test 27
 *
 *  NOTE: We use software interrupt 0
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) \
            set_vector( (handler), VBR1 * 0x10 + 0x8, 1 ); \
            lcsr->intr_level[2] |= 3; \
            lcsr->intr_ena |= 0x100;

#define Cause_tm27_intr()  lcsr->intr_soft_set |= 0x100

#define Clear_tm27_intr()  lcsr->intr_clear |= 0x100

#define Lower_tm27_intr() /* empty */

#endif

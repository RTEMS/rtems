/**
 *  @file
 *
 *  @ingroup shared_tm27
 *
 *  @brief Empty stub for tm27.h
 */

/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

/**
 *  @defgroup shared_tm27 Time Test 27 Default
 *
 *  @ingroup RTEMSTimeTest27Support
 *
 *  @brief Stuff for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( _handler ) \
  do { \
    (void) _handler; \
  } while (0)

#define Cause_tm27_intr()  \
  do { \
    ; \
  } while (0)


#define Clear_tm27_intr() \
  do { \
    ; \
  } while (0)

#define Lower_tm27_intr() \
  do { \
    ; \
  } while (0)

#endif

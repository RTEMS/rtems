/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __INTERRUPT_CRITICAL_SECTIONS_TEST_SUPPORT_h
#define __INTERRUPT_CRITICAL_SECTIONS_TEST_SUPPORT_h

/**
 *  @brief Initialize Test Support
 *
 *  @param[in] tsr is the optional timer service routine to fire
 */
void interrupt_critical_section_test_support_initialize(
  rtems_timer_service_routine_entry tsr
);

/**
 *  @brief Delay Test Support
 *
 *  This method delays a varying amount of time each call.
 *
 *  @return This method returns true each time the delay counter has
 *          to be reset.
 */
bool interrupt_critical_section_test_support_delay(void);

/**
 * @brief Interrupt critical section test.
 *
 * This function first estimates the test body duration and then repeatedly
 * calls the test body with varying times to the next clock tick interrupt.
 *
 * @param[in] test_body The test body function.  In case the test body returns
 * true, then the test iteration stops.
 * @param[in] test_body_arg The argument for the test body function.
 * @param[in] tsr An optional timer service routine.
 *
 * @return The test body return status.
 */
bool interrupt_critical_section_test(
  bool                              ( *test_body )( void * ),
  void                                *test_body_arg,
  rtems_timer_service_routine_entry    tsr
);

#endif


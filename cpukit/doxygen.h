/**
 * @file
 *
 * @ingroup RTEMSImplDoxygen
 *
 * @brief This header file defines some Doxygen groups.
 */

/**
 * @defgroup RTEMSAPIIO IO
 *
 * @ingroup RTEMSAPI
 *
 * @brief IO
 */

/**
 * @defgroup RTEMSAPIPrintSupport Print Support
 *
 * @ingroup RTEMSAPIIO
 *
 * @brief Print Support
 */

/**
 * @defgroup RTEMSAPITracing Tracing
 *
 * @ingroup RTEMSAPI
 *
 * @brief Tracing
 */

/**
 * @defgroup RTEMSLegacyBenchmarkDrivers Legacy Benchmark Drivers
 *
 * @ingroup RTEMSDeviceDrivers
 *
 * @brief Legacy Benchmark Drivers
 */

/**
 * @defgroup RTEMSTimeTest27Support Time Test 27 Support
 *
 * @ingroup RTEMSDeviceDrivers
 *
 * @brief The Time Test 27 (TM27) support is used to measure the timing of
 *   the interrupt processing.
 *
 * The TMS27 support should provide two software generated interrupt requests,
 * one low priority request raised by Cause_tm27_intr() and one higher priority
 * request raised by Lower_tm27_intr().  Both requests should be cleared by
 * Clear_tm27_intr().  A handler provided by the test should be installed
 * through Install_tm27_vector().  This function should initialize the system
 * so that the software generated interrupt requests can be raised and cleared.
 *
 * If the raise functions Cause_tm27_intr() and Lower_tm27_intr() do not
 * trigger the interrupt immediately, then the TM27 support shall define
 * MUST_WAIT_FOR_INTERRUPT to one, otherwise MUST_WAIT_FOR_INTERRUPT shall be
 * defined to zero.
 *
 * The TM27 support may define TM27_INTERRUPT_VECTOR_DEFAULT to indicate the
 * interrupt vector of the interrupt request raised by Cause_tm27_intr().
 *
 * The TM27 support may define TM27_INTERRUPT_VECTOR_ALTERNATIVE to provide an
 * alternative software generated interrupt request which is raised by
 * _TM27_Raise_alternative() and cleared by _TM27_Clear_alternative().  Both
 * functions shall return an RTEMS status code.  This interrupt vector may be
 * used to test the interrupt controller support on targets which do not
 * provide generic software generated interrupts.
 */

/**
 * @defgroup libmisc_mouse Serial Mouse
 *
 * @ingroup RTEMSDeviceDrivers
 *
 * @brief Serial Mouse
 */


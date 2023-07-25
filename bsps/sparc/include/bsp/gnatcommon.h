/**
 * @file
 *
 * @ingroup RTEMSImplGnat
 *
 * @brief This header file provides interfaces of the
 *   gnat/rtems interrupts and exception handling.
 */

/**
 * @defgroup RTEMSImplGnat GNAT/RTEMS interrupts and exception handling
 *
 * @ingroup RTEMSImpl
 */

#ifndef __GNATCOMMON_H
#define __GNATCOMMON_H

/*
 * Support for gnat/rtems interrupts and exception handling.
 */

#ifdef __cplusplus
extern "C" {
#endif

void __gnat_install_handler (void);
void __gnat_install_handler_common (int t1, int t2);

#ifdef __cplusplus
}
#endif

#endif

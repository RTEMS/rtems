/*
 * Copyright (C) 2017 Cobham Gaisler AB
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef GRLIB_IMPL_H
#define GRLIB_IMPL_H

#include <rtems/score/basedefs.h>

/*
 * Use interrupt lock primitives compatible with SMP defined in RTEMS 4.11.99
 * and higher.
 */
#if (((__RTEMS_MAJOR__ << 16) | (__RTEMS_MINOR__ << 8) | __RTEMS_REVISION__) >= 0x040b63)

#include <rtems/score/isrlock.h>

/* map via rtems_interrupt_lock_* API: */
#define SPIN_DECLARE(lock) RTEMS_INTERRUPT_LOCK_MEMBER(lock)
#define SPIN_INIT(lock, name) rtems_interrupt_lock_initialize(lock, name)
#define SPIN_LOCK(lock, level) rtems_interrupt_lock_acquire_isr(lock, &level)
#define SPIN_LOCK_IRQ(lock, level) rtems_interrupt_lock_acquire(lock, &level)
#define SPIN_UNLOCK(lock, level) rtems_interrupt_lock_release_isr(lock, &level)
#define SPIN_UNLOCK_IRQ(lock, level) rtems_interrupt_lock_release(lock, &level)
#define SPIN_IRQFLAGS(k) rtems_interrupt_lock_context k
#define SPIN_ISR_IRQFLAGS(k) SPIN_IRQFLAGS(k)
#define SPIN_FREE(lock) rtems_interrupt_lock_destroy(lock)

/* turn on/off local CPU's interrupt to ensure HW timing - not SMP safe. */
#define IRQ_LOCAL_DECLARE(_level) rtems_interrupt_level _level
#define IRQ_LOCAL_DISABLE(_level) rtems_interrupt_local_disable(_level)
#define IRQ_LOCAL_ENABLE(_level) rtems_interrupt_local_enable(_level)

#else

#ifdef RTEMS_SMP
#error SMP mode not compatible with these interrupt lock primitives
#endif

/* maintain single-core compatibility with older versions of RTEMS: */
#define SPIN_DECLARE(name)
#define SPIN_INIT(lock, name)
#define SPIN_LOCK(lock, level)
#define SPIN_LOCK_IRQ(lock, level) rtems_interrupt_disable(level)
#define SPIN_UNLOCK(lock, level)
#define SPIN_UNLOCK_IRQ(lock, level) rtems_interrupt_enable(level)
#define SPIN_IRQFLAGS(k) rtems_interrupt_level k
#define SPIN_ISR_IRQFLAGS(k)
#define SPIN_FREE(lock)

/* turn on/off local CPU's interrupt to ensure HW timing - not SMP safe. */
#define IRQ_LOCAL_DECLARE(_level) rtems_interrupt_level _level
#define IRQ_LOCAL_DISABLE(_level) rtems_interrupt_disable(_level)
#define IRQ_LOCAL_ENABLE(_level) rtems_interrupt_enable(_level)

#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* GRLIB_IMPL_H */

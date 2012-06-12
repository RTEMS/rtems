/*
 *  Atomic Manager
 *
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 *  The functions in this file implement the API to the RTEMS Atomic Manager and
 *  . The API is designed to be compitable with C1X atomic definition as far as
 *  possible. And its implementation is reused the FreeBSD kernel atomic operation
 *  . The functions below are implemented with CPU dependent inline routines
 *  found in the path
 *
 *  rtems/cpukit/score/cpu/xxx/rtems/score/atomic_cpu.h
 *
 *  In the event that a CPU does not support a specific atomic function it has, the 
 *  CPU dependent routine does nothing (but does exist).
 */

#ifndef _RTEMS_ATOMIC_H
# error "Never use <rtems/score/atomic.inl> directly; include <rtems/score/atomic.h> instead."
#endif

#include <rtems/score/types.h>

RTEMS_INLINE_ROUTINE unsigned int _Atomic_Load_int(
  volatile unsigned int *address,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Load_acq_int(address);
  else
    return _CPU_Atomic_Load_int(address);
}

RTEMS_INLINE_ROUTINE unsigned long _Atomic_Load_long(
  volatile unsigned long *address,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Load_acq_long(address);
  else
    return _CPU_Atomic_Load_long(address);
}

RTEMS_INLINE_ROUTINE uintptr_t _Atomic_Load_ptr(
  volatile uintptr_t *address,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Load_acq_ptr(address);
  else
    return _CPU_Atomic_Load_ptr(address);
}

RTEMS_INLINE_ROUTINE uint32_t _Atomic_Load_32(
  volatile uint32_t *address,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Load_acq_32(address);
  else
    return _CPU_Atomic_Load_32(address);
}

RTEMS_INLINE_ROUTINE uint64_t _Atomic_Load_64(
  volatile uint64_t *address,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Load_acq_64(address);
  else
    return _CPU_Atomic_Load_64(address);
}


RTEMS_INLINE_ROUTINE void _Atomic_Store_int(
  volatile unsigned int *address,
  unsigned int value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Store_rel_int(address, value);
  else
    return _CPU_Atomic_Store_int(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Store_long(
  volatile unsigned long *address,
  unsigned long value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Store_rel_long(address, value);
  else
    return _CPU_Atomic_Store_long(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Store_ptr(
  volatile uintptr_t *address,
  uintptr_t value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Store_rel_ptr(address, value);
  else
    return _CPU_Atomic_Store_ptr(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Store_32(
  volatile uint32_t *address,
  uint32_t value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Store_rel_32(address, value);
  else
    return _CPU_Atomic_Store_32(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Store_64(
  volatile uint64_t *address,
  uint64_t value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Store_rel_64(address, value);
  else
    return _CPU_Atomic_Store_64(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_add_int(
  volatile unsigned int *address,
  unsigned int value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_add_acq_int(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_add_rel_int(address, value);
  else
    return _CPU_Atomic_Fetch_add_int(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_add_long(
  volatile unsigned long *address,
  unsigned long value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_add_acq_long(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_add_rel_long(address, value);
  else
    return _CPU_Atomic_Fetch_add_long(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_add_ptr(
  volatile uintptr_t *address,
  uintptr_t value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_add_acq_ptr(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_add_rel_ptr(address, value);
  else
    return _CPU_Atomic_Fetch_add_ptr(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_add_32(
  volatile uint32_t *address,
  uint32_t value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_add_acq_32(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_add_rel_32(address, value);
  else
    return _CPU_Atomic_Fetch_add_32(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_add_64(
  volatile uint64_t *address,
  uint64_t value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_add_acq_64(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_add_rel_64(address, value);
  else
    return _CPU_Atomic_Fetch_add_64(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_Sub_int(
  volatile unsigned int *address,
  unsigned int value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_sub_acq_int(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_sub_rel_int(address, value);
  else
    return _CPU_Atomic_Fetch_sub_int(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_sub_long(
  volatile unsigned long *address,
  unsigned long value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_sub_acq_long(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_sub_rel_long(address, value);
  else
    return _CPU_Atomic_Fetch_sub_long(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_sub_ptr(
  volatile uintptr_t *address,
  uintptr_t value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_sub_acq_ptr(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_sub_rel_ptr(address, value);
  else
    return _CPU_Atomic_Fetch_sub_ptr(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_sub_32(
  volatile uint32_t *address,
  uint32_t value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_sub_acq_32(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_sub_rel_32(address, value);
  else
    return _CPU_Atomic_Fetch_sub_32(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_sub_64(
  volatile uint64_t *address,
  uint64_t value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_sub_acq_64(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_sub_rel_64(address, value);
  else
    return _CPU_Atomic_Fetch_sub_64(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_or_int(
  volatile unsigned int *address,
  unsigned int value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_or_acq_int(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_or_rel_int(address, value);
  else
    return _CPU_Atomic_Fetch_or_int(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_or_long(
  volatile unsigned long *address,
  unsigned long value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_or_acq_long(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_or_rel_long(address, value);
  else
    return _CPU_Atomic_Fetch_or_long(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_or_ptr(
  volatile uintptr_t *address,
  uintptr_t value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_or_acq_ptr(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_or_rel_ptr(address, value);
  else
    return _CPU_Atomic_Fetch_or_ptr(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_or_32(
  volatile uint32_t *address,
  uint32_t value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_or_acq_32(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_or_rel_32(address, value);
  else
    return _CPU_Atomic_Fetch_or_32(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_or_64(
  volatile uint64_t *address,
  uint64_t value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_or_acq_64(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_or_rel_64(address, value);
  else
    return _CPU_Atomic_Fetch_or_64(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_and_int(
  volatile unsigned int *address,
  unsigned int value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_and_acq_int(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_and_rel_int(address, value);
  else
    return _CPU_Atomic_Fetch_and_int(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_and_long(
  volatile unsigned long *address,
  unsigned long value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_and_acq_long(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_and_rel_long(address, value);
  else
    return _CPU_Atomic_Fetch_and_long(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_and_ptr(
  volatile uintptr_t *address,
  uintptr_t value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_and_acq_ptr(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_and_rel_ptr(address, value);
  else
    return _CPU_Atomic_Fetch_and_ptr(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_and_32(
  volatile uint32_t *address,
  uint32_t value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_and_acq_32(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_and_rel_32(address, value);
  else
    return _CPU_Atomic_Fetch_and_32(address, value);
}

RTEMS_INLINE_ROUTINE void _Atomic_Fetch_and_64(
  volatile uint64_t *address,
  uint64_t value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_and_acq_64(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_and_rel_64(address, value);
  else
    return _CPU_Atomic_Fetch_and_64(address, value);
}

RTEMS_INLINE_ROUTINE int _Atomic_Compare_exchange_int(
  volatile unsigned int *address,
  unsigned int old_value,
  unsigned int new_value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_exchange_acq_int(address, old_value, new_value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_exchange_rel_int(address, old_value, new_value);
  else
    return _CPU_Atomic_Compare_exchange_int(address, old_value, new_value);
}

RTEMS_INLINE_ROUTINE int _Atomic_Compare_exchange_long(
  volatile unsigned long *address,
  unsigned long old_value,
  unsigned long new_value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_exchange_acq_long(address, old_value, new_value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_exchange_rel_long(address, old_value, new_value);
  else
    return _CPU_Atomic_Compare_exchange_long(address, old_value, new_value);
}

RTEMS_INLINE_ROUTINE int _Atomic_Compare_exchange_ptr(
  volatile uintptr_t *address,
  uintptr_t old_value,
  uintptr_t new_value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_exchange_acq_ptr(address, old_value, new_value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_exchange_rel_ptr(address, old_value, new_value);
  else
    return _CPU_Atomic_Compare_exchange_ptr(address, old_value, new_value);
}

RTEMS_INLINE_ROUTINE int _Atomic_Compare_exchange_32(
  volatile uint32_t *address,
  uint32_t old_value,  
  uint32_t new_value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_exchange_acq_32(address, old_value, new_value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_exchange_rel_32(address, old_value, new_value);
  else
    return _CPU_Atomic_Compare_exchange_32(address, old_value, new_value);
}

RTEMS_INLINE_ROUTINE int _Atomic_Compare_exchange_64(
  volatile uint64_t *address,
  uint64_t old_value,
  uint64_t new_value,
  Atomic_Memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_exchange_acq_64(address, old_value, new_value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_exchange_rel_64(address, old_value, new_value);
  else
    return _CPU_Atomic_Compare_exchange_64(address, old_value, new_value);
}

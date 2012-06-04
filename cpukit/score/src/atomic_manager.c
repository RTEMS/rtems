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

#include <rtems/score/types.h>
#include <rtems/score/atomic.h>


ATOMIC_int _Atomic_Load_int(
  volatile ATOMIC_int *address,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Load_acq_int(address);
  else
    return _CPU_Atomic_Load_int(address);
}

ATOMIC_long _Atomic_Load_long(
  volatile ATOMIC_long *address,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Load_acq_long(address);
  else
    return _CPU_Atomic_Load_long(address);
}

ATOMIC_ptr _Atomic_Load_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Load_acq_ptr(address);
  else
    return _CPU_Atomic_Load_ptr(address);
}

ATOMIC_32 _Atomic_Load_32(
  volatile ATOMIC_32 *address,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Load_acq_32(address);
  else
    return _CPU_Atomic_Load_32(address);
}

ATOMIC_64 _Atomic_Load_64(
  volatile ATOMIC_64 *address,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Load_acq_64(address);
  else
    return _CPU_Atomic_Load_64(address);
}


void _Atomic_Store_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Store_rel_int(address, value);
  else
    return _CPU_Atomic_Store_int(address, value);
}

void _Atomic_Store_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Store_rel_long(address, value);
  else
    return _CPU_Atomic_Store_long(address, value);
}

void _Atomic_Store_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Store_rel_ptr(address, value);
  else
    return _CPU_Atomic_Store_ptr(address, value);
}

void _Atomic_Store_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Store_rel_32(address, value);
  else
    return _CPU_Atomic_Store_32(address, value);
}

void _Atomic_Store_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Store_rel_64(address, value);
  else
    return _CPU_Atomic_Store_64(address, value);
}

void _Atomic_Fetch_Add_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Add_acq_int(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Add_rel_int(address, value);
  else
    return _CPU_Atomic_Fetch_Add_int(address, value);
}

void _Atomic_Fetch_Add_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Add_acq_long(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Add_rel_long(address, value);
  else
    return _CPU_Atomic_Fetch_Add_long(address, value);
}

void _Atomic_Fetch_Add_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Add_acq_ptr(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Add_rel_ptr(address, value);
  else
    return _CPU_Atomic_Fetch_Add_ptr(address, value);
}

void _Atomic_Fetch_Add_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Add_acq_32(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Add_rel_32(address, value);
  else
    return _CPU_Atomic_Fetch_Add_32(address, value);
}

void _Atomic_Fetch_Add_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Add_acq_64(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Add_rel_64(address, value);
  else
    return _CPU_Atomic_Fetch_Add_64(address, value);
}

void _Atomic_Fetch_Sub_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Sub_acq_int(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Sub_rel_int(address, value);
  else
    return _CPU_Atomic_Fetch_Sub_int(address, value);
}

void _Atomic_Fetch_Sub_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Sub_acq_long(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Sub_rel_long(address, value);
  else
    return _CPU_Atomic_Fetch_Sub_long(address, value);
}

void _Atomic_Fetch_Sub_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Sub_acq_ptr(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Sub_rel_ptr(address, value);
  else
    return _CPU_Atomic_Fetch_Sub_ptr(address, value);
}

void _Atomic_Fetch_Sub_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Sub_acq_32(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Sub_rel_32(address, value);
  else
    return _CPU_Atomic_Fetch_Sub_32(address, value);
}

void _Atomic_Fetch_Sub_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Sub_acq_64(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Sub_rel_64(address, value);
  else
    return _CPU_Atomic_Fetch_Sub_64(address, value);
}

void _Atomic_Fetch_Or_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Or_acq_int(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Or_rel_int(address, value);
  else
    return _CPU_Atomic_Fetch_Or_int(address, value);
}

void _Atomic_Fetch_Or_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Or_acq_long(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Or_rel_long(address, value);
  else
    return _CPU_Atomic_Fetch_Or_long(address, value);
}

void _Atomic_Fetch_Or_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Or_acq_ptr(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Or_rel_ptr(address, value);
  else
    return _CPU_Atomic_Fetch_Or_ptr(address, value);
}

void _Atomic_Fetch_Or_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Or_acq_32(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Or_rel_32(address, value);
  else
    return _CPU_Atomic_Fetch_Or_32(address, value);
}

void _Atomic_Fetch_Or_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Or_acq_64(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_Or_rel_64(address, value);
  else
    return _CPU_Atomic_Fetch_Or_64(address, value);
}

void _Atomic_Fetch_And_int(
  volatile ATOMIC_int *address,
  ATOMIC_int value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_And_acq_int(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_And_rel_int(address, value);
  else
    return _CPU_Atomic_Fetch_And_int(address, value);
}

void _Atomic_Fetch_And_long(
  volatile ATOMIC_long *address,
  ATOMIC_long value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_And_acq_long(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_And_rel_long(address, value);
  else
    return _CPU_Atomic_Fetch_And_long(address, value);
}

void _Atomic_Fetch_And_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_And_acq_ptr(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_And_rel_ptr(address, value);
  else
    return _CPU_Atomic_Fetch_And_ptr(address, value);
}

void _Atomic_Fetch_And_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_And_acq_32(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_And_rel_32(address, value);
  else
    return _CPU_Atomic_Fetch_And_32(address, value);
}

void _Atomic_Fetch_And_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_And_acq_64(address, value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Fetch_And_rel_64(address, value);
  else
    return _CPU_Atomic_Fetch_And_64(address, value);
}

int _Atomic_Compare_Exchange_int(
  volatile ATOMIC_int *address,
  ATOMIC_int old_value,
  ATOMIC_int new_value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_Exchange_acq_int(address, old_value, new_value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_Exchange_rel_int(address, old_value, new_value);
  else
    return _CPU_Atomic_Compare_Exchange_int(address, old_value, new_value);
}

int _Atomic_Compare_Exchange_long(
  volatile ATOMIC_long *address,
  ATOMIC_long old_value,
  ATOMIC_long new_value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_Exchange_acq_long(address, old_value, new_value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_Exchange_rel_long(address, old_value, new_value);
  else
    return _CPU_Atomic_Compare_Exchange_long(address, old_value, new_value);
}

int _Atomic_Compare_Exchange_ptr(
  volatile ATOMIC_ptr *address,
  ATOMIC_ptr old_value,
  ATOMIC_ptr new_value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_Exchange_acq_ptr(address, old_value, new_value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_Exchange_rel_ptr(address, old_value, new_value);
  else
    return _CPU_Atomic_Compare_Exchange_ptr(address, old_value, new_value);
}

int _Atomic_Compare_Exchange_32(
  volatile ATOMIC_32 *address,
  ATOMIC_32 old_value,  
  ATOMIC_32 new_value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_Exchange_acq_32(address, old_value, new_value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_Exchange_rel_32(address, old_value, new_value);
  else
    return _CPU_Atomic_Compare_Exchange_32(address, old_value, new_value);
}

int _Atomic_Compare_Exchange_64(
  volatile ATOMIC_64 *address,
  ATOMIC_64 old_value,
  ATOMIC_64 new_value,
  ATOMIC_memory_barrier memory_barrier
)
{
  if(ATOMIC_ACQUIRE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_Exchange_acq_64(address, old_value, new_value);
  else if(ATOMIC_RELEASE_BARRIER == memory_barrier)
    return _CPU_Atomic_Compare_Exchange_rel_64(address, old_value, new_value);
  else
    return _CPU_Atomic_Compare_Exchange_64(address, old_value, new_value);
}

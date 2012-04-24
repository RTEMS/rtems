/**
 *  @file
 *
 *  Scheduler Simple SMP Handler / Schedule
 */

/*
 *  COPYRIGHT (c) 2011- 2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/smp.h>
#include <rtems/score/schedulersimplesmp.h>

#include <stdio.h>

/*
 *  The following is very useful on the scheduler simulator when debugging
 *  this algorithm.  You are not likely to be able to print like this when
 *  running on a real system.
 *
 *  NOTE: This is NOT using RTEMS_DEBUG because this is not consistency
 *        checking and is VERY VERY noisy.  It is just for debugging
 *        the algorithm.
 */
#if 0
#define D(format,...) printk( format, __VA_ARGS__)
#else
#define D(format,...)
#endif

/* Declaration to avoid warnings */
bool _Scheduler_simple_smp_Assign(
  Thread_Control *consider
);

/**
 *  @brief Assign Heir Thread to CPU
 *
 *  This method attempts to find a core for the thread under consideration
 *  (@a consider) to become heir of.  The placement takes into account
 *  priority, preemptibility, and length of time on core.
 *
 *  Combined with the loop in _Scheduler_simple_smp_Schedule, it attempts
 *  to faithfully implement the behaviour of the Deterministic Priority
 *  Scheduler while spreading the threads across multiple cores.
 *
 *  @param[in] consider is the thread under consideration.  This means
 *             that the method is looking for the best core to place it
 *             as heir.
 *
 *  @return This method returns true if it was able to make @a consider
 *          the heir on a core and false otherwise.  When this returns
 *          false, there is no point in attempting to place an heir of
 *          of lower priority.
 */
bool _Scheduler_simple_smp_Assign(
  Thread_Control *consider
)
{
  bool            found;       /* have we found a cpu to place it on? */
  uint32_t        found_cpu;   /* CPU to place this thread */
  bool            blocked;     /* CPU has blocked thread? */
  Thread_Control *pheir;       /* heir on found cpu to potentially replace */
  Thread_Control *h;
  Thread_Control *e;
  uint32_t        cpu;

  /*
   *  Initialize various variables to indicate we have not found
   *  a potential core for the thread under consideration.
   */
  found     = false;
  blocked   = false;
  found_cpu = 0;
  pheir     = NULL;

  for ( cpu=0 ; cpu < _SMP_Processor_count ; cpu++ ) {
    D( "SCHED CPU=%d consider=0x%08x ASSIGN\n", cpu, consider->Object.id );

    /*
     *  If the thread under consideration is already executing or
     *  heir, then we don't have a better option for it.
     */
    e = _Per_CPU_Information[cpu].executing;
    if ( e == consider ) {
      D( "SCHED CPU=%d Executing=0x%08x considering=0x%08x ASSIGNED\n",
        cpu, e->Object.id, consider->Object.id );
      return true;
    }

    if ( !_States_Is_ready( e->current_state ) ) {
      pheir     = e;
      found_cpu = cpu;
      found     = true;
      blocked   = true;
      D( "SCHED CPU=%d PHeir=0x%08x considering=0x%08x BLOCKED\n",
        cpu, e->Object.id, consider->Object.id );
      continue;
    }

    h = _Per_CPU_Information[cpu].heir;
    if ( h == consider ) {
      D( "SCHED CPU=%d Heir=0x%08x considering=0x%08x ASSIGNED\n",
        cpu, h->Object.id, consider->Object.id );
      return true;
    }

    if ( blocked )
      continue;

    /*
     *  If we haven't found a potential CPU to locate the thread
     *  under consideration on, we need to consider if this is a
     *  more important threads first (e.g. priority <).
     *
     *  But when a thread changes its priority and ends up at the end of
     *  the priority group for the new heir, we also need to schedule
     *  a new heir.  This is the "=" part of the check.
     */
    if ( !found ) {
      if ( consider->current_priority <= h->current_priority ) {
        pheir     = h;
        found_cpu = cpu;
        found     = true;
        D( "SCHED CPU=%d PHeir=0x%08x considering=0x%08x MAYBE #1\n",
          cpu, h->Object.id, consider->Object.id );
      }

      continue;
    }

    /*
     *  ASSERTION: (found == true)
     *
     *  Past this point, we have found a potential heir and are considering
     *  whether the thread is better placed on another core. It is desirable
     *  to preempt the lowest priority thread using time on core and
     *  preemptibility as additional factors.
     */

    /*
     *  If we have found a potential CPU on which to make the
     *  thread under consideration the heir, then we need to
     *  check if the current CPU is a more appropriate place
     *  for this thread to be placed.
     *
     *  Check 1: heir of potential CPU is more important
     *           then heir of current CPU.  We want to
     *           replace the least important thread possible.
     */
    if ( h->current_priority > pheir->current_priority ) {
      pheir = h;
      found_cpu = cpu;
      D( "SCHED CPU=%d PHeir=0x%08x considering=0x%08x MAYBE #2\n",
        cpu, h->Object.id, consider->Object.id );
      continue;
    }

    /*
     *  If the current heir is more important than the potential
     *  heir, then we should not consider it further in scheduling.
     */
    if ( h->current_priority < pheir->current_priority )
      continue;

    /*
     *  ASSERTION: (h->current_priority == pheir->current_priority).
     *
     *  Past this point, this means we are considering the length of time
     *  the thread has spent on the time on the CPU core and if it is
     *  preemptible.
     */

    /*
     *  If heir of potential CPU and of the current CPU are of the SAME
     *  priority, then which has been running longer?
     *
     *           Which CPU has had its executing thread longer?
     */
    if ( _Timestamp_Less_than(
           &_Per_CPU_Information[cpu].time_of_last_context_switch,
           &_Per_CPU_Information[found_cpu].time_of_last_context_switch
         ) ) {
      pheir = h;
      found_cpu = cpu;
      D( "SCHED CPU=%d PHeir=0x%08x considering=0x%08x LONGER\n",
        cpu, h->Object.id, consider->Object.id );
      continue;
    }

    /*
     *  If we are looking at a core with a non-preemptible thread
     *  for potential placement, then favor a core with a preeemtible
     *  thread of the same priority.  This should help avoid priority
     *  inversions and let threads run earlier.
     */
    if ( !pheir->is_preemptible && h->is_preemptible ) {
      D( "SCHED CPU=%d PHeir==0x%08x is NOT PREEMPTIBLE\n", 
        cpu, pheir->Object.id );
      pheir = h;
      found_cpu = cpu;
      D( "SCHED CPU=%d PHeir=0x%08x considering=0x%08x PREEMPTIBLE\n",
        cpu, h->Object.id, consider->Object.id );
      continue;

    }
  }

  /*
   *  If we found a cpu to make this thread heir of, then we
   *  need to consider whether we need a dispatch on that CPU.
   */
  if ( found ) {
    e = _Per_CPU_Information[found_cpu].executing;
    D( "SCHED CPU=%d executing=0x%08x considering=0x%08x FOUND\n",
      found_cpu, e->Object.id, consider->Object.id );
    _Per_CPU_Information[found_cpu].heir = consider;
    if ( !_States_Is_ready( e->current_state ) ) {
       D( "SCHED CPU=%d executing not ready dispatch needed\n", found_cpu);
      _Per_CPU_Information[found_cpu].dispatch_necessary = true;
    } else if ( consider->current_priority < e->current_priority ) {
      if ( e->is_preemptible || consider->current_priority == 0 ) {
         D( "SCHED CPU=%d preempting\n", found_cpu);
        _Per_CPU_Information[found_cpu].dispatch_necessary = true;
      }
    }
  }

  /*
   *  Return true to indicate we changed an heir.  This indicates
   *  scheduling needs to examine more threads.
   */
  return found;
}

/*
 *  Reschedule threads -- select heirs for all cores
 */
void _Scheduler_simple_smp_Schedule(void)
{
  Chain_Control  *c;
  Chain_Node     *n;
  Thread_Control *t;
  uint32_t        cpu;

  c   = (Chain_Control *)_Scheduler.information;
  cpu = 0;

  /*
   *  Iterate over the first N (where N is the number of CPU cores) threads
   *  on the ready chain.  Attempt to assign each as heir on a core.  When
   *  unable to assign a thread as a new heir, then stop.
   */
  for (n = _Chain_First(c); !_Chain_Is_tail(c, n); n = _Chain_Next(n)) {
    t = (Thread_Control *)n;
    if ( !_Scheduler_simple_smp_Assign( t ) )
      break;
    if ( ++cpu >= _SMP_Processor_count )
      break;
  }
}

/**
 *  @file  rtems/score/cpuset.h
 *
 *  @brief Information About the CPU Set
 *
 *  This include file contains all information about the thread
 *  CPU Set.
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPUSET_H
#define _RTEMS_SCORE_CPUSET_H

#include <rtems/score/basedefs.h>

#ifdef __RTEMS_HAVE_SYS_CPUSET_H__

#include <sys/cpuset.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreCpuset SuperCore CPU Set
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which is used in the management
 *  of thread's CPU set.
 */
/**@{*/

/**
 *  The following defines the control block used to manage the cpuset.
 *  The names do not include affinity in the front in case the set is
 *  ever used for something other than affinity.  The usage in thread
 *  uses the attribute affinity such that accesses will read
 *  thread->affinity.set.
 */
typedef struct {
  /** This is the size of the set */
  size_t                  setsize;
  /** This is the preallocated space to store the set */
  cpu_set_t               preallocated;
  /** This is a pointer to the set in use */
  cpu_set_t               *set;
}   CPU_set_Control;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* __RTEMS_HAVE_SYS_CPUSET_H__ */

#endif /* _RTEMS_SCORE_CPUSET_H */
/* end of include file */

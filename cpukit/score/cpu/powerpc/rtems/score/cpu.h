/*
 * $Id$
 */
 
#ifndef _rtems_score_cpu_h
#define _rtems_score_cpu_h

#include <rtems/score/ppc.h>              /* pick up machine definitions */
#ifndef ASM
#include <rtems/score/types.h>
#endif

#ifdef _OLD_EXCEPTIONS
#include <rtems/old-exceptions/cpu.h>
#else
#include <rtems/new-exceptions/cpu.h>
#endif

#endif

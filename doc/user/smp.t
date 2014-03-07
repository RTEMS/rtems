@c
@c  COPYRIGHT (c) 2014.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c

@chapter Symmetric Multiprocessing Services

@section Introduction

This chapter describes the services related to Symmetric Multiprocessing
provided by RTEMS. 

The application level services currently provided are:

@itemize @bullet
@item @code{rtems_smp_get_processor_count} - Obtain Number of CPUs
@item @code{rtems_task_get_affinity} - Obtain Task Affinity
@item @code{rtems_task_set_affinity} - Set Task Affinity
@end itemize

@section Background

@section Operations

@section Directives

This section details the symmetric multiprocessing services.  A subsection
is dedicated to each of these services and describes the calling sequence,
related constants, usage, and status codes.

@c
@c rtems_smp_get_processor_count
@c
@page
@subsection rtems_smp_get_processor_count - Obtain Number of CPUs

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <rtems.h>

int rtems_smp_get_processor_count(void);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

Returns the number of processors being utilized.

@subheading DESCRIPTION:

The @code{rtems_smp_get_processor_count} routine is used to obtain the
number of processor cores being utilized.

@subheading NOTES:

NONE

@c
@c rtems_task_get_affinity
@c
@page
@subsection rtems_task_get_affinity - Obtain Task Affinity

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <rtems.h>

rtems_status_code rtems_task_get_affinity(
  rtems_id             id,
  size_t               cpusetsize,
  cpu_set_t           *cpuset 
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item RTEMS_UNSATISFIED
The cpuset pointer argument is invalid.

@item RTEMS_UNSATISFIED
The @code{cpusetsize} does not match the value of @code{affinitysetsize}
field in the thread attribute object.

@item RTEMS_INVALID_ID
The @code{id} is invalid.

@end table

@subheading DESCRIPTION:

The @code{rtems_task_get_affinity} routine is used to obtain the
@code{affinityset} field from the thread object @code{id}.
The value of this field is returned in @code{cpuset}

@subheading NOTES:

NONE

@c
@c rtems_task_set_affinity
@c
@page
@subsection rtems_task_set_affinity - Set Task Affinity

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <rtems.h>

rtems_status_code rtems_task_set_affinity(
  rtems_id             id,
  size_t               cpusetsize,
  cpu_set_t           *cpuset 
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item RTEMS_UNSATISFIED
The cpuset pointer argument is invalid.

@item RTEMS_UNSATISFIED
The @code{cpusetsize} does not match the value of @code{affinitysetsize}
field in the thread attribute object.

@item RTEMS_UNSATISFIED
The @code{cpuset} did not select a valid cpu.

@item RTEMS_UNSATISFIED
The @code{cpuset} selected a cpu that was invalid.

@item RTEMS_INVALID_ID
The @code{id} is invalid.

@end table

@subheading DESCRIPTION:

The @code{rtems_task_set_affinity} routine is used to set the
@code{affinity.set} field of the thread control object associated
with @code{id}.  This value controls the cpuset that the task can
execute on.

@subheading NOTES:

NONE

@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@c
@c  Open Issues
@c    - nicen up the tables
@c    - use math mode to print formulas
@c

@chapter Rate Monotonic Manager

@cindex rate mononitonic tasks
@cindex periodic tasks

@section Introduction

The rate monotonic manager provides facilities to
implement tasks which execute in a periodic fashion.  The
directives provided by the rate monotonic manager are:

@itemize @bullet
@item @code{@value{DIRPREFIX}rate_monotonic_create} - Create a rate monotonic period
@item @code{@value{DIRPREFIX}rate_monotonic_ident} - Get ID of a period
@item @code{@value{DIRPREFIX}rate_monotonic_cancel} - Cancel a period
@item @code{@value{DIRPREFIX}rate_monotonic_delete} - Delete a rate monotonic period
@item @code{@value{DIRPREFIX}rate_monotonic_period} - Conclude current/Start next period
@item @code{@value{DIRPREFIX}rate_monotonic_get_status} - Obtain status information on period
@end itemize

@section Background

The rate monotonic manager provides facilities to
manage the execution of periodic tasks.  This manager was
designed to support application designers who utilize the Rate
Monotonic Scheduling Algorithm (RMS) to insure that their
periodic tasks will meet their deadlines, even under transient
overload conditions.  Although designed for hard real-time
systems, the services provided by the rate monotonic manager may
be used by any application which requires periodic tasks.

@subsection Rate Monotonic Manager Required Support

A clock tick is required to support the functionality provided by this manager.

@subsection Rate Monotonic Manager Definitions

@cindex periodic task, definition

A periodic task is one which must be executed at a
regular interval.  The interval between successive iterations of
the task is referred to as its period.  Periodic tasks can be
characterized by the length of their period and execution time.
The period and execution time of a task can be used to determine
the processor utilization for that task.  Processor utilization
is the percentage of processor time used and can be calculated
on a per-task or system-wide basis.  Typically, the task's
worst-case execution time will be less than its period.  For
example, a periodic task's requirements may state that it should
execute for 10 milliseconds every 100 milliseconds.  Although
the execution time may be the average, worst, or best case, the
worst-case execution time is more appropriate for use when
analyzing system behavior under transient overload conditions.

@cindex aperiodic task, definition

In contrast, an aperiodic task executes at irregular
intervals and has only a soft deadline.  In other words, the
deadlines for aperiodic tasks are not rigid, but adequate
response times are desirable.  For example, an aperiodic task
may process user input from a terminal.

@cindex sporadic task, definition

Finally, a sporadic task is an aperiodic task with a
hard deadline and minimum interarrival time.  The minimum
interarrival time is the minimum period of time which exists
between successive iterations of the task.  For example, a
sporadic task could be used to process the pressing of a fire
button on a joystick.  The mechanical action of the fire button
insures a minimum time period between successive activations,
but the missile must be launched by a hard deadline.

@subsection Rate Monotonic Scheduling Algorithm

@cindex Rate Monotonic Scheduling Algorithm, definition
@cindex RMS Algorithm, definition

The Rate Monotonic Scheduling Algorithm (RMS) is
important to real-time systems designers because it allows one
to guarantee that a set of tasks is schedulable.  A set of tasks
is said to be schedulable if all of the tasks can meet their
deadlines.  RMS provides a set of rules which can be used to
perform a guaranteed schedulability analysis for a task set.
This analysis determines whether a task set is schedulable under
worst-case conditions and emphasizes the predictability of the
system's behavior.  It has been proven that:

@itemize @code{ }
@b{RMS is an optimal static priority algorithm for
scheduling independent, preemptible, periodic tasks
on a single processor.}
@end itemize

RMS is optimal in the sense that if a set of tasks
can be scheduled by any static priority algorithm, then RMS will
be able to schedule that task set.  RMS bases it schedulability
analysis on the processor utilization level below which all
deadlines can be met.

RMS calls for the static assignment of task
priorities based upon their period.  The shorter a task's
period, the higher its priority.  For example, a task with a 1
millisecond period has higher priority than a task with a 100
millisecond period.  If two tasks have the same period, then RMS
does not distinguish between the tasks.  However, RTEMS
specifies that when given tasks of equal priority, the task
which has been ready longest will execute first.  RMS's priority
assignment scheme does not provide one with exact numeric values
for task priorities.  For example, consider the following task
set and priority assignments:

@ifset use-ascii
@example
@group
+--------------------+---------------------+---------------------+
|        Task        |       Period        |      Priority       |
|                    |  (in milliseconds)  |                     |
+--------------------+---------------------+---------------------+
|         1          |         100         |         Low         |
+--------------------+---------------------+---------------------+
|         2          |          50         |       Medium        |
+--------------------+---------------------+---------------------+
|         3          |          50         |       Medium        |
+--------------------+---------------------+---------------------+
|         4          |          25         |        High         |
+--------------------+---------------------+---------------------+
@end group
@end example
@end ifset

@ifset use-tex
@sp 1
@tex
\centerline{\vbox{\offinterlineskip\halign{
\vrule\strut#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.25in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.25in{\enskip\hfil#\hfil}&
\vrule#\cr\noalign{\hrule}
&\bf Task&& \bf Period && \bf Priority &\cr
& && \bf (in milliseconds) && &\cr\noalign{\hrule}
& 1 && 100 && Low &\cr\noalign{\hrule}
& 2 && 50 && Medium &\cr\noalign{\hrule}
& 3 && 50 && Medium &\cr\noalign{\hrule}
& 4 && 25 && High &\cr\noalign{\hrule}
}}\hfil}
@end tex
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=3 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=center><STRONG>Task</STRONG></TD>
    <TD ALIGN=center><STRONG>Period (in milliseconds)</STRONG></TD>
    <TD ALIGN=center><STRONG>Priority</STRONG></TD></TR>
<TR><TD ALIGN=center>1</TD>
    <TD ALIGN=center>100 </TD>
    <TD ALIGN=center>Low</TD></TR>
<TR><TD ALIGN=center>2</TD>
    <TD ALIGN=center>50 </TD>
    <TD ALIGN=center>Medium</TD></TR>
<TR><TD ALIGN=center>3</TD>
    <TD ALIGN=center>50 </TD>
    <TD ALIGN=center>Medium</TD></TR>
<TR><TD ALIGN=center>4</TD>
    <TD ALIGN=center>25 </TD>
    <TD ALIGN=center>High</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

RMS only calls for task 1 to have the lowest
priority, task 4 to have the highest priority, and tasks 2 and 3
to have an equal priority between that of tasks 1 and 4.  The
actual RTEMS priorities assigned to the tasks must only adhere
to those guidelines.

Many applications have tasks with both hard and soft
deadlines.  The tasks with hard deadlines are typically referred
to as the critical task set, with the soft deadline tasks being
the non-critical task set.  The critical task set can be
scheduled using RMS, with the non-critical tasks not executing
under transient overload, by simply assigning priorities such
that the lowest priority critical task (i.e. longest period) has
a higher priority than the highest priority non-critical task.
Although RMS may be used to assign priorities to the
non-critical tasks, it is not necessary.  In this instance,
schedulability is only guaranteed for the critical task set.

@subsection Schedulability Analysis

@cindex RMS schedulability analysis

RMS allows application designers to insure that tasks
can meet all deadlines, even under transient overload, without
knowing exactly when any given task will execute by applying
proven schedulability analysis rules.

@lowersections

@subsection Assumptions

The schedulability analysis rules for RMS were
developed based on the following assumptions:


@itemize @bullet
@item The requests for all tasks for which hard deadlines
exist are periodic, with a constant interval between requests.

@item Each task must complete before the next request for it
occurs.

@item The tasks are independent in that a task does not depend
on the initiation or completion of requests for other tasks.

@item The execution time for each task without preemption or
interruption is constant and does not vary.

@item Any non-periodic tasks in the system are special.  These
tasks displace periodic tasks while executing and do not have
hard, critical deadlines.
@end itemize

Once the basic schedulability analysis is understood,
some of the above assumptions can be relaxed and the
side-effects accounted for.

@subsection Processor Utilization Rule

@cindex RMS Processor Utilization Rule

The Processor Utilization Rule requires that
processor utilization be calculated based upon the period and
execution time of each task.  The fraction of processor time
spent executing task index is Time(index) / Period(index).  The
processor utilization can be calculated as follows:

@example
@group
Utilization = 0

for index = 1 to maximum_tasks
  Utilization = Utilization + (Time(index)/Period(index))
@end group
@end example

To insure schedulability even under transient
overload, the processor utilization must adhere to the following
rule:

@example
Utilization = maximum_tasks * (2(1/maximum_tasks) - 1)
@end example

As the number of tasks increases, the above formula
approaches ln(2) for a worst-case utilization factor of
approximately 0.693.  Many tasks sets can be scheduled with a
greater utilization factor.  In fact, the average processor
utilization threshold for a randomly generated task set is
approximately 0.88.

@subsection Processor Utilization Rule Example

This example illustrates the application of the
Processor Utilization Rule to an application with three critical
periodic tasks.  The following table details the RMS priority,
period, execution time, and processor utilization for each task:

@ifset use-ascii
@example
@group
    +------------+----------+--------+-----------+-------------+
    |    Task    |   RMS    | Period | Execution |  Processor  |
    |            | Priority |        |   Time    | Utilization |
    +------------+----------+--------+-----------+-------------+
    |     1      |   High   |  100   |    15     |    0.15     |
    +------------+----------+--------+-----------+-------------+
    |     2      |  Medium  |  200   |    50     |    0.25     |
    +------------+----------+--------+-----------+-------------+
    |     3      |   Low    |  300   |   100     |    0.33     |
    +------------+----------+--------+-----------+-------------+
@end group
@end example
@end ifset

@ifset use-tex
@sp 1
@tex
\centerline{\vbox{\offinterlineskip\halign{
\vrule\strut#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.00in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.00in{\enskip\hfil#\hfil}&
\vrule#\cr\noalign{\hrule}
&\bf Task&& \bf RMS && \bf Period && \bf Execution &&\bf Processor&\cr
& && \bf Priority && &&\bf Time &&\bf Utilization &\cr\noalign{\hrule}
& 1 && High && 100 && 15 && 0.15 &\cr\noalign{\hrule}
& 2 && Medium && 200 && 50 && 0.25 &\cr\noalign{\hrule}
& 3 && Low && 300 && 100 && 0.33 &\cr\noalign{\hrule}
}}\hfil}
@end tex
@end ifset
 
@ifset use-html
@html
<CENTER>
  <TABLE COLS=5 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=center><STRONG>Task</STRONG></TD>
    <TD ALIGN=center><STRONG>RMS Priority</STRONG></TD>
    <TD ALIGN=center><STRONG>Period</STRONG></TD>
    <TD ALIGN=center><STRONG>Execution Time</STRONG></TD>
    <TD ALIGN=center><STRONG>Processor Utilization</STRONG></TD></TR>
<TR><TD ALIGN=center>1</TD>
    <TD ALIGN=center>High</TD>
    <TD ALIGN=center>100</TD>
    <TD ALIGN=center>15</TD>
    <TD ALIGN=center>0.15</TD></TR>
<TR><TD ALIGN=center>2</TD>
    <TD ALIGN=center>Medium</TD>
    <TD ALIGN=center>200</TD>
    <TD ALIGN=center>50</TD>
    <TD ALIGN=center>0.25</TD></TR>
<TR><TD ALIGN=center>3</TD>
    <TD ALIGN=center>Low</TD>
    <TD ALIGN=center>300</TD>
    <TD ALIGN=center>100</TD>
    <TD ALIGN=center>0.33</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

The total processor utilization for this task set is
0.73 which is below the upper bound of 3 * (2(1/3) - 1), or
0.779, imposed by the Processor Utilization Rule.  Therefore,
this task set is guaranteed to be schedulable using RMS.

@subsection First Deadline Rule

@cindex RMS First Deadline Rule

If a given set of tasks do exceed the processor
utilization upper limit imposed by the Processor Utilization
Rule, they can still be guaranteed to meet all their deadlines
by application of the First Deadline Rule.  This rule can be
stated as follows:

For a given set of independent periodic tasks, if
each task meets its first deadline when all tasks are started at
the same time, then the deadlines will always be met for any
combination of start times.

A key point with this rule is that ALL periodic tasks
are assumed to start at the exact same instant in time.
Although this assumption may seem to be invalid,  RTEMS makes it
quite easy to insure.  By having a non-preemptible user
initialization task, all application tasks, regardless of
priority, can be created and started before the initialization
deletes itself.  This technique insures that all tasks begin to
compete for execution time at the same instant -- when the user
initialization task deletes itself.

@subsection First Deadline Rule Example

The First Deadline Rule can insure schedulability
even when the Processor Utilization Rule fails.  The example
below is a modification of the Processor Utilization Rule
example where task execution time has been increased from 15 to
25 units.  The following table details the RMS priority, period,
execution time, and processor utilization for each task:

@ifset use-ascii
@example
@group
    +------------+----------+--------+-----------+-------------+
    |    Task    |   RMS    | Period | Execution |  Processor  |
    |            | Priority |        |   Time    | Utilization |
    +------------+----------+--------+-----------+-------------+
    |     1      |   High   |  100   |    25     |    0.25     |
    +------------+----------+--------+-----------+-------------+
    |     2      |  Medium  |  200   |    50     |    0.25     |
    +------------+----------+--------+-----------+-------------+
    |     3      |   Low    |  300   |   100     |    0.33     |
    +------------+----------+--------+-----------+-------------+
@end group
@end example
@end ifset

@ifset use-tex
@sp 1
@tex
\centerline{\vbox{\offinterlineskip\halign{
\vrule\strut#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.00in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.00in{\enskip\hfil#\hfil}&
\vrule#\cr\noalign{\hrule}
&\bf Task&& \bf RMS && \bf Period && \bf Execution &&\bf Processor&\cr
& && \bf Priority && &&\bf Time &&\bf Utilization &\cr\noalign{\hrule}
& 1 && High && 100 && 25 && 0.25 &\cr\noalign{\hrule}
& 2 && Medium && 200 && 50 && 0.25 &\cr\noalign{\hrule}
& 3 && Low && 300 && 100 && 0.33 &\cr\noalign{\hrule}
}}\hfil}
@end tex
@end ifset
 
@ifset use-html
@html
<CENTER>
  <TABLE COLS=5 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=center><STRONG>Task</STRONG></TD>
    <TD ALIGN=center><STRONG>RMS Priority</STRONG></TD>
    <TD ALIGN=center><STRONG>Period</STRONG></TD>
    <TD ALIGN=center><STRONG>Execution Time</STRONG></TD>
    <TD ALIGN=center><STRONG>Processor Utilization</STRONG></TD></TR>
<TR><TD ALIGN=center>1</TD>
    <TD ALIGN=center>High</TD>
    <TD ALIGN=center>100</TD>
    <TD ALIGN=center>25</TD>
    <TD ALIGN=center>0.25</TD></TR>
<TR><TD ALIGN=center>2</TD>
    <TD ALIGN=center>Medium</TD>
    <TD ALIGN=center>200</TD>
    <TD ALIGN=center>50</TD>
    <TD ALIGN=center>0.25</TD></TR>
<TR><TD ALIGN=center>3</TD>
    <TD ALIGN=center>Low</TD>
    <TD ALIGN=center>300</TD>
    <TD ALIGN=center>100</TD>
    <TD ALIGN=center>0.33</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

The total processor utilization for the modified task
set is 0.83 which is above the upper bound of 3 * (2(1/3) - 1),
or 0.779, imposed by the Processor Utilization Rule.  Therefore,
this task set is not guaranteed to be schedulable using RMS.
However, the First Deadline Rule can guarantee the
schedulability of this task set.  This rule calls for one to
examine each occurrence of deadline until either all tasks have
met their deadline or one task failed to meet its first
deadline.  The following table details the time of each deadline
occurrence, the maximum number of times each task may have run,
the total execution time, and whether all the deadlines have
been met.

@ifset use-ascii
@example
@group
+----------+------+------+------+----------------------+---------------+
| Deadline | Task | Task | Task |        Total         | All Deadlines |
|   Time   |  1   |  2   |  3   |    Execution Time    |     Met?      |
+----------+------+------+------+----------------------+---------------+
|   100    |  1   |  1   |  1   |  25 + 50 + 100 = 175 |      NO       |
+----------+------+------+------+----------------------+---------------+
|   200    |  2   |  1   |  1   |  50 + 50 + 100 = 200 |     YES       |
+----------+------+------+------+----------------------+---------------+
@end group
@end example
@end ifset

@ifset use-tex
@sp 1
@tex
\centerline{\vbox{\offinterlineskip\halign{
\vrule\strut#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 2.00in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.00in{\enskip\hfil#\hfil}&
\vrule#\cr\noalign{\hrule}
&\bf Deadline&& \bf Task &&\bf Task&&\bf Task&&\bf Total          &&\bf All Deadlines &\cr
&\bf Time    && \bf 1    &&\bf 2   &&\bf 3   &&\bf Execution Time &&\bf Net?&\cr\noalign{\hrule}
& 100&& 1 && 1 && 1 && 25 + 50 + 100 = 175 && NO &\cr\noalign{\hrule}
& 200&& 2 && 1 && 1 && 50 + 50 + 100 = 200 && YES &\cr\noalign{\hrule}
}}\hfil}
@end tex
@end ifset
 
@ifset use-html
@html
<CENTER>
  <TABLE COLS=6 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=center><STRONG>Deadline Time</STRONG></TD>
    <TD ALIGN=center><STRONG>Task 1</STRONG></TD>
    <TD ALIGN=center><STRONG>Task 2</STRONG></TD>
    <TD ALIGN=center><STRONG>Task 3</STRONG></TD>
    <TD ALIGN=center><STRONG>Total Execution Time</STRONG></TD>
    <TD ALIGN=center><STRONG>All Deadlines Met?</STRONG></TD></TR>
<TR><TD ALIGN=center>100</TD>
    <TD ALIGN=center>1</TD>
    <TD ALIGN=center>1</TD>
    <TD ALIGN=center>1</TD>
    <TD ALIGN=center>25 + 50 + 100 = 175</TD>
    <TD ALIGN=center>NO</TD></TR>
<TR><TD ALIGN=center>200</TD>
    <TD ALIGN=center>2</TD>
    <TD ALIGN=center>1</TD>
    <TD ALIGN=center>1</TD>
    <TD ALIGN=center>50 + 50 + 100 = 175</TD>
    <TD ALIGN=center>YES</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

The key to this analysis is to recognize when each
task will execute.  For example	at time 100, task 1 must have
met its first deadline, but tasks 2 and 3 may also have begun
execution.  In this example, at time 100 tasks 1 and 2 have
completed execution and thus have met their first deadline.
Tasks 1 and 2 have used (25 + 50) = 75 time units, leaving (100
- 75) = 25 time units for task 3 to begin.  Because task 3 takes
100 ticks to execute, it will not have completed execution at
time 100.  Thus at time 100, all of the tasks except task 3 have
met their first deadline.

At time 200, task 1 must have met its second deadline
and task 2 its first deadline.  As a result, of the first 200
time units, task 1 uses (2 * 25) = 50 and task 2 uses 50,
leaving (200 - 100) time units for task 3.  Task 3 requires 100
time units to execute, thus it will have completed execution at
time 200.  Thus, all of the tasks have met their first deadlines
at time 200, and the task set is schedulable using the First
Deadline Rule.

@subsection Relaxation of Assumptions

The assumptions used to develop the RMS
schedulability rules are uncommon in most real-time systems.
For example, it was assumed that tasks have constant unvarying
execution time.  It is possible to relax this assumption, simply
by using the worst-case execution time of each task.

Another assumption is that the tasks are independent.
This means that the tasks do not wait for one another or
contend for resources.  This assumption can be relaxed by
accounting for the amount of time a task spends waiting to
acquire resources.  Similarly, each task's execution time must
account for any I/O performed and any RTEMS directive calls.

In addition, the assumptions did not account for the
time spent executing interrupt service routines.  This can be
accounted for by including all the processor utilization by
interrupt service routines in the utilization calculation.
Similarly, one should also account for the impact of delays in
accessing local memory caused by direct memory access and other
processors accessing local dual-ported memory.

The assumption that nonperiodic tasks are used only
for initialization or failure-recovery can be relaxed by placing
all periodic tasks in the critical task set.  This task set can
be scheduled and analyzed using RMS.  All nonperiodic tasks are
placed in the non-critical task set.  Although the critical task
set can be guaranteed to execute even under transient overload,
the non-critical task set is not guaranteed to execute.

In conclusion, the application designer must be fully
cognizant of the system and its run-time behavior when
performing schedulability analysis for a system using RMS.
Every hardware and software factor which impacts the execution
time of each task must be accounted for in the schedulability
analysis.

@subsection Further Reading

For more information on Rate Monotonic Scheduling and
its schedulability analysis, the reader is referred to the
following:

@itemize @code{ }
@item @cite{C. L. Liu and J. W. Layland. "Scheduling Algorithms for
Multiprogramming in a Hard Real Time Environment." @b{Journal of
the Association of Computing Machinery}. January 1973. pp. 46-61.}

@item @cite{John Lehoczky, Lui Sha, and Ye Ding. "The Rate Monotonic
Scheduling Algorithm: Exact Characterization and Average Case
Behavior."  @b{IEEE Real-Time Systems Symposium}. 1989. pp. 166-171.}

@item @cite{Lui Sha and John Goodenough. "Real-Time Scheduling
Theory and Ada."  @b{IEEE Computer}. April 1990. pp. 53-62.}

@item @cite{Alan Burns. "Scheduling hard real-time systems: a
review."  @b{Software Engineering Journal}. May 1991. pp. 116-128.}
@end itemize

@raisesections

@section Operations

@subsection Creating a Rate Monotonic Period

The @code{@value{DIRPREFIX}rate_monotonic_create} directive creates a rate
monotonic period which is to be used by the calling task to
delineate a period.  RTEMS allocates a Period Control Block
(PCB) from the PCB free list.  This data structure is used by
RTEMS to manage the newly created rate monotonic period.  RTEMS
returns a unique period ID to the application which is used by
other rate monotonic manager directives to access this rate
monotonic period.

@subsection Manipulating a Period

The @code{@value{DIRPREFIX}rate_monotonic_period} directive is used to
establish and maintain periodic execution utilizing a previously
created rate monotonic period.   Once initiated by the
@code{@value{DIRPREFIX}rate_monotonic_period} directive, the period is
said to run until it either expires or is reinitiated.  The state of the rate
monotonic period results in one of the following scenarios:

@itemize @bullet
@item If the rate monotonic period is running, the calling
task will be blocked for the remainder of the outstanding period
and, upon completion of that period, the period will be
reinitiated with the specified period.

@item If the rate monotonic period is not currently running
and has not expired, it is initiated with a length of period
ticks and the calling task returns immediately.

@item If the rate monotonic period has expired before the task
invokes the @code{@value{DIRPREFIX}rate_monotonic_period} directive,
the period will be initiated with a length of period ticks and the calling task
returns immediately with a timeout error status.

@end itemize

@subsection Obtaining the Status of a Period

If the @code{@value{DIRPREFIX}rate_monotonic_period} directive is invoked
with a period of @code{@value{RPREFIX}PERIOD_STATUS} ticks, the current
state of the specified rate monotonic period will be returned.  The following
table details the relationship between the period's status and
the directive status code returned by the
@code{@value{DIRPREFIX}rate_monotonic_period}
directive:

@itemize @bullet
@item @code{@value{RPREFIX}SUCCESSFUL} - period is running

@item @code{@value{RPREFIX}TIMEOUT} - period has expired

@item @code{@value{RPREFIX}NOT_DEFINED} - period has never been initiated
@end itemize

Obtaining the status of a rate monotonic period does
not alter the state or length of that period.

@subsection Canceling a Period

The @code{@value{DIRPREFIX}rate_monotonic_cancel} directive is used to stop
the period maintained by the specified rate monotonic period.
The period is stopped and the rate monotonic period can be
reinitiated using the @code{@value{DIRPREFIX}rate_monotonic_period} directive.

@subsection Deleting a Rate Monotonic Period

The @code{@value{DIRPREFIX}rate_monotonic_delete} directive is used to delete
a rate monotonic period.  If the period is running and has not
expired, the period is automatically canceled.  The rate
monotonic period's control block is returned to the PCB free
list when it is deleted.  A rate monotonic period can be deleted
by a task other than the task which created the period.

@subsection Examples

The following sections illustrate common uses of rate
monotonic periods to construct periodic tasks.

@subsection Simple Periodic Task

This example consists of a single periodic task
which, after initialization, executes every 100 clock ticks.

@page
@example
rtems_task Periodic_task()
@{
  rtems_name        name;
  rtems_id          period;
  rtems_status_code status;

  name = rtems_build_name( 'P', 'E', 'R', 'D' );

  status = rate_monotonic_create( name, &period );
  if ( status != RTEMS_STATUS_SUCCESSFUL ) @{
    printf( "rtems_monotonic_create failed with status of %d.\n", rc );
    exit( 1 );
  @}


  while ( 1 ) @{
    if ( rate_monotonic_period( period, 100 ) == RTEMS_TIMEOUT )
      break;

    /* Perform some periodic actions */
  @}

  /* missed period so delete period and SELF */

  status = rate_monotonic_delete( period );
  if ( status != RTEMS_STATUS_SUCCESSFUL ) @{
    printf( "rate_monotonic_delete failed with status of %d.\n", status );
    exit( 1 );
  @}

  status = rtems_task_delete( SELF );    /* should not return */
  printf( "rtems_task_delete returned with status of %d.\n", status );
  exit( 1 );
@}
@end example


The above task creates a rate monotonic period as
part of its initialization.  The first time the loop is
executed, the @code{@value{DIRPREFIX}rate_monotonic_period}
directive will initiate the period for 100 ticks and return
immediately.  Subsequent invocations of the
@code{@value{DIRPREFIX}rate_monotonic_period} directive will result
in the task blocking for the remainder of the 100 tick period.
If, for any reason, the body of the loop takes more than 100
ticks to execute, the @code{@value{DIRPREFIX}rate_monotonic_period}
directive will return the @code{@value{RPREFIX}TIMEOUT} status.  
If the above task misses its deadline, it will delete the rate
monotonic period and itself.

@subsection Task with Multiple Periods

This example consists of a single periodic task
which, after initialization, performs two sets of actions every
100 clock ticks.  The first set of actions is performed in the
first forty clock ticks of every 100 clock ticks, while the
second set of actions is performed between the fortieth and
seventieth clock ticks.  The last thirty clock ticks are not
used by this task.

@page
@example
task Periodic_task()
@{
  rtems_name        name_1, name_2;
  rtems_id          period_1, period_2;
  rtems_status_code status;

  name_1 = rtems_build_name( 'P', 'E', 'R', '1' );
  name_2 = rtems_build_name( 'P', 'E', 'R', '2' );

  (void ) rate_monotonic_create( name_1, &period_1 );
  (void ) rate_monotonic_create( name_2, &period_2 );

  while ( 1 ) @{
    if ( rate_monotonic_period( period_1, 100 ) == TIMEOUT )
      break;

    if ( rate_monotonic_period( period_2, 40 ) == TIMEOUT )
      break;

    /*
     *  Perform first set of actions between clock
     *  ticks 0 and 39 of every 100 ticks.
     */

    if ( rate_monotonic_period( period_2, 30 ) == TIMEOUT )
      break;

    /*
     *  Perform second set of actions between clock 40 and 69
     *  of every 100 ticks.  THEN ...
     *
     *  Check to make sure we didn't miss the period_2 period.
     */

    if ( rate_monotonic_period( period_2, STATUS ) == TIMEOUT )
      break;

    (void) rate_monotonic_cancel( period_2 );
  @}

  /* missed period so delete period and SELF */

  (void ) rate_monotonic_delete( period_1 );
  (void ) rate_monotonic_delete( period_2 );
  (void ) task_delete( SELF );
@}
@end example

The above task creates two rate monotonic periods as
part of its initialization.  The first time the loop is
executed, the @code{@value{DIRPREFIX}rate_monotonic_period}
directive will initiate the period_1 period for 100 ticks
and return immediately.  Subsequent invocations of the
@code{@value{DIRPREFIX}rate_monotonic_period} directive
for period_1 will result in the task blocking for the remainder
of the 100 tick period.  The period_2 period is used to control
the execution time of the two sets of actions within each 100
tick period established by period_1.  The
@code{@value{DIRPREFIX}rate_monotonic_cancel( period_2 )}
call is performed to insure that the period_2 period
does not expire while the task is blocked on the period_1
period.  If this cancel operation were not performed, every time
the @code{@value{DIRPREFIX}rate_monotonic_period( period_1, 40 )}
call is executed, except for the initial one, a directive status
of @code{@value{RPREFIX}TIMEOUT} is returned.  It is important to
note that every time this call is made, the period_1 period will be
initiated immediately and the task will not block.

If, for any reason, the task misses any deadline, the
@code{@value{DIRPREFIX}rate_monotonic_period} directive will
return the @code{@value{RPREFIX}TIMEOUT}
directive status.  If the above task misses its deadline, it
will delete the rate monotonic periods and itself.

@section Directives

This section details the rate monotonic manager's
directives.  A subsection is dedicated to each of this manager's
directives and describes the calling sequence, related
constants, usage, and status codes.

@c
@c
@c
@page
@subsection RATE_MONOTONIC_CREATE - Create a rate monotonic period

@cindex create a period

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_rate_monotonic_create
@example
rtems_status_code rtems_rate_monotonic_create(
  rtems_name  name,
  rtems_id   *id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Rate_Monotonic_Create (
   Name   : in     RTEMS.Name;
   ID     :    out RTEMS.ID;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - rate monotonic period created successfully@*
@code{@value{RPREFIX}INVALID_NAME} - invalid task name@*
@code{@value{RPREFIX}TOO_MANY} - too many periods created

@subheading DESCRIPTION:

This directive creates a rate monotonic period.  The
assigned rate monotonic id is returned in id.  This id is used
to access the period with other rate monotonic manager
directives.  For control and maintenance of the rate monotonic
period, RTEMS allocates a PCB from the local PCB free pool and
initializes it.

@subheading NOTES:

This directive will not cause the calling task to be
preempted.

@c
@c
@c
@page
@subsection RATE_MONOTONIC_IDENT - Get ID of a period

@cindex get ID of a period
@cindex obtain ID of a period

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_rate_monotonic_ident
@example
rtems_status_code rtems_rate_monotonic_ident(
  rtems_name  name,
  rtems_id   *id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Rate_Monotonic_Ident (
   Name   : in     RTEMS.Name;
   ID     :    out RTEMS.ID;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - period identified successfully@*
@code{@value{RPREFIX}INVALID_NAME} - period name not found

@subheading DESCRIPTION:

This directive obtains the period id associated with
the period name to be acquired.  If the period name is not
unique, then the period id will match one of the periods with
that name.  However, this period id is not guaranteed to
correspond to the desired period.  The period id is used to
access this period in other rate monotonic manager directives.

@subheading NOTES:

This directive will not cause the running task to be
preempted.

@c
@c
@c
@page
@subsection RATE_MONOTONIC_CANCEL - Cancel a period

@cindex cancel a period

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_rate_monotonic_cancel
@example
rtems_status_code rtems_rate_monotonic_cancel(
  rtems_id id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Rate_Monotonic_Cancel (
   ID     : in     RTEMS.ID;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - period canceled successfully@*
@code{@value{RPREFIX}INVALID_ID} - invalid rate monotonic period id@*
@code{@value{RPREFIX}NOT_OWNER_OF_RESOURCE} - rate monotonic period not created by calling task

@subheading DESCRIPTION:

This directive cancels the rate monotonic period id.
This period will be reinitiated by the next invocation of
@code{@value{DIRPREFIX}rate_monotonic_period} with id.

@subheading NOTES:

This directive will not cause the running task to be
preempted.

The rate monotonic period specified by id must have
been created by the calling task.

@c
@c
@c
@page
@subsection RATE_MONOTONIC_DELETE - Delete a rate monotonic period

@cindex delete a period

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_rate_monotonic_delete
@example
rtems_status_code rtems_rate_monotonic_delete(
  rtems_id id
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Rate_Monotonic_Delete (
   ID     : in     RTEMS.ID;
   Result :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - period deleted successfully@*
@code{@value{RPREFIX}INVALID_ID} - invalid rate monotonic period id

@subheading DESCRIPTION:

This directive deletes the rate monotonic period
specified by id.  If the period is running, it is automatically
canceled.  The PCB for the deleted period is reclaimed by RTEMS.

@subheading NOTES:

This directive will not cause the running task to be
preempted.

A rate monotonic period can be deleted by a task
other than the task which created the period.

@c
@c
@c
@page
@subsection RATE_MONOTONIC_PERIOD - Conclude current/Start next period

@cindex conclude current period
@cindex start current period
@cindex period initiation

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_rate_monotonic_period
@example
rtems_status_code rtems_rate_monotonic_period(
  rtems_id       id,
  rtems_interval length
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Rate_Monotonic_Period (
   ID      : in     RTEMS.ID;
   Length  : in     RTEMS.Interval;
   Result  :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - period initiated successfully@*
@code{@value{RPREFIX}INVALID_ID} - invalid rate monotonic period id@*
@code{@value{RPREFIX}NOT_OWNER_OF_RESOURCE} - period not created by calling task@*
@code{@value{RPREFIX}NOT_DEFINED} - period has never been initiated (only
possible when period is set to PERIOD_STATUS)@*
@code{@value{RPREFIX}TIMEOUT} - period has expired

@subheading DESCRIPTION:

This directive initiates the rate monotonic period id
with a length of period ticks.  If id is running, then the
calling task will block for the remainder of the period before
reinitiating the period with the specified period.  If id was
not running (either expired or never initiated), the period is
immediately initiated and the directive returns immediately.

If invoked with a period of @code{@value{RPREFIX}PERIOD_STATUS} ticks, the
current state of id will be returned.  The directive status
indicates the current state of the period.  This does not alter
the state or period of the period.

@subheading NOTES:

This directive will not cause the running task to be preempted.

@c
@c
@c
@page
@subsection RATE_MONOTONIC_GET_STATUS - Obtain status information on period

@cindex get status of period
@cindex obtain status of period

@subheading CALLING SEQUENCE:

@ifset is-C
@findex rtems_rate_monotonic_get_status
@example
rtems_status_code rtems_rate_monotonic_get_status(
  rtems_id                            id,
  rtems_rate_monotonic_period_status *status
);
@end example
@end ifset

@ifset is-Ada
@example
procedure Rate_Monotonic_Get_Status (
   ID      : in     RTEMS.ID;
   Status  :    out RTEMS.Rate_Monotonic_Period_Status;
   Result  :    out RTEMS.Status_Codes
);
@end example
@end ifset

@subheading DIRECTIVE STATUS CODES:
@code{@value{RPREFIX}SUCCESSFUL} - period initiated successfully@*
@code{@value{RPREFIX}INVALID_ID} - invalid rate monotonic period id@*
@code{@value{RPREFIX}INVALID_ADDRESS} - invalid address of status@*

@subheading DESCRIPTION:

This directive returns status information associated with 
the rate monotonic period id in the following data @value{STRUCTURE}:

@ifset is-C
@findex rtems_rate_monotonic_period_status
@example
typedef struct @{
  rtems_rate_monotonic_period_states  state;
  rtems_unsigned32                    ticks_since_last_period;
  rtems_unsigned32                    ticks_executed_since_last_period;
@}  rtems_rate_monotonic_period_status;
@end example
@end ifset

@ifset is-Ada
@example
type Rate_Monotonic_Period_Status is
   begin
      State                    : RTEMS.Rate_Monotonic_Period_States;
      Ticks_Since_Last_Period  : RTEMS.Unsigned32;
      Ticks_Executed_Since_Last_Period : RTEMS.Unsigned32;
   end record;
@end example
@end ifset

@c RATE_MONOTONIC_INACTIVE does not have RTEMS_ in front of it.

If the period's state is @code{RATE_MONOTONIC_INACTIVE}, both
ticks_since_last_period and ticks_executed_since_last_period 
will be set to 0.  Otherwise, ticks_since_last_period will
contain the number of clock ticks which have occurred since
the last invocation of the
@code{@value{DIRPREFIX}rate_monotonic_period} directive.
Also in this case, the ticks_executed_since_last_period will indicate
how much processor time the owning task has consumed since the invocation
of the @code{@value{DIRPREFIX}rate_monotonic_period} directive.

@subheading NOTES:

This directive will not cause the running task to be preempted.

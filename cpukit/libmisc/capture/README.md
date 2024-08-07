libmisc
=======

          RTEMS Performance Monitoring and Measurement Framework

          Copyright 2002-2007 Chris Johns (chrisj@rtems.org)
                          23 April 2002

This directory contains the source code for the performance monitoring and
measurement framework. It is more commonly know as the capture engine.

The capture engine is in an early phase of development. Please review the Status
section of this document for the current status.

Performance.

The capture engine is designed to not effect the system it is
monitoring. Resources such as memory are used as well as a small performance
hit in task creation, deletion and context switch. The overhead is small and
will not be noticed unless the system is operating close to the performance
limit of the target.

Structure.

The capture engine is implemented in a couple of layers. This lowest layer is
the capture engine. Its interface is in the file 'capture.h'. Typically this
interface is directly used unless you are implementing a target interface. The
user interface is via a target interface.

Command Line Interface (CLI).

This is a target interface that provides a number of user commands via the
RTEMS monitor. To use you need to provide the following in your
application initialisation:

```c
  #include <rtems/monitor.h>
  #include <rtems/capture-cli.h>

  rtems_monitor_init (0);
  rtems_capture_cli_init (0);
```

Check the file capture-cli.h for documentation of the interface. The parameter
is a pointer to your board support package's time stamp handler. The time stamp
handler is yet to be tested so it is recommended this is left as 0, unless you
wish to test this part of the engine.

The commands are:

  copen    - Open the capture engine.
  cclose   - Close the capture engine.
  cenable  - Enable the capture engine.
  cdisable - Disable the capture engine.
  ctlist   - List the tasks known to the capture engine.
  ctload   - Display the current load (sort of top).
  cwlist   - List the watch and trigger controls.
  cwadd    - Add a watch.
  cwdel    - Delete a watch.
  cwctl    - Enable or disable a watch.
  cwglob   - Enable or disable the global watch.
  cwceil   - Set the watch ceiling.
  cwfloor  - Set the watch floor.
  ctrace   - Dump the trace records.
  ctrig    - Define a trigger.

Open

  usage: copen [-i] size

Open the capture engine. The size parameter is the size of the capture engine
trace buffer. A single record hold a single event, for example a task create or
a context in or out. The option '-i' will enable the capture engine after it is
opened.

Close

  usage: cclose

Close the capture engine and release all resources held by the capture engine.

Enable

  usage: cenable

Enable the capture engine if it has been opened.

Disable

  usage: cdisable

Disable the capture engine. The enable and disable commands provide a means of
removing the overhead of the capture engine from the context switch. This may
be needed when testing if it is felt the capture engines overhead is effecting
the system.

Task List

  usage: ctlist

List the tasks the capture engine knows about. This may contain tasks that have
been deleted.

Task Load

  usage: ctload

List the tasks in the order of load in a similar way top does on Unix. The
command sends ANSI terminal codes. You press enter to stop the update. The
update period is fixed at 5 seconds. The output looks like:

 Press ENTER to exit.

     PID NAME RPRI CPRI STATE  %CPU     %STK FLGS   EXEC TIME
04010001 IDLE  255  255 READY   96.012%   0% a-----g   1
08010009 CPlt    1    1 READY    3.815%  15% a------   0
08010003 ntwk   20   20 Wevnt    0.072%   0% at----g   0
08010004 CSr0   20   20 Wevnt    0.041%   0% at----g   0
08010001 main  250  250 DELAY    0.041%   0% a-----g   0
08010008 test  100  100 Wevnt    0.000%  20% at-T-+g   0
08010007 test  100  100 Wevnt    0.000%   0% at-T-+g   0
08010005 CSt0   20   20 Wevnt    0.000%   0% at----g   0
08010006 RMON    1    1 Wsem     0.000%   0% a------   0

There are 7 flags and from left to right are:

1) 'a' the task is active, and 'd' the task has been deleted.
2) 't' the task has been traced.
3) 'F' the task has a from (TO_ANY) trigger.
4) 'T' the task has a to (FROM_ANY) trigger.
5) 'E' the task has an edge (FROM_TO) trigger.
6) '+' the task as a watch control attached, 'w' a watch is enabled.
7) 'g' the task is part of a global trigger.

The %STK is the percentage of stack used by a task. Currently only tasks
created while the capture engine is enabled can be monitored.

The RPRI is the real priority. This is the priority set for the task. The
current priority is the executing priority that may reflect a level set as a
result of priority inversion.

Watch List

  usage: cwlist

This command lists the watch and trigger controls the capture engine has. A
control is a structure used by the capture engine to determine if a task is
watched or triggers capturing.

Watch Add

  usage: cwadd [task name] [id]

Add a watch for a task. You can provide a name or id or both. A name will cause
all tasks with that name to have the watch added. An id results in a watch
being for a specific task.

Using a name is useful when the task is not yet created.

Watch Delete

  usage: cwdel [task name] [id]

Delete a watch that has been added.

Watch Control

  usage: cwctl [task name] [id] on/off

Enable or disable a watch. The name and id parameters are the same as the watch
add command.

Global Watch

  usage: cwglob on/off

Enable or disable the global watch. A global watch is an easy way to enable
watches for all tasks with real priorities between the watch ceiling and floor
priorities.

Watch Priority Ceiling

  usage: cwceil priority

Set the watch priority ceiling. All tasks with a priority less than the ceiling
priority are not watched. This allow you to ignore high priority system and
driver tasks.

Watch Priority Floor

  usage: cwfloor priority

Set the watch priority floor. All tasks with a priority greater than the floor
priority level are not watched. This allows you to remove tasks such as IDLE
from being monitored.

Trace

  usage: ctrace [-c] [-r records]

Dump the trace record. The option '-c' will output the records in comma
separated variables (CSV). The '-r' option controls the number of records
dumped. This can help stop the command looping for-ever.

Trigger

 usage: ctrig type [from name] [from id] [to name] [to id]

Set a trigger. The types of triggers are :

 from : trigger on a context switch from a task
 to   : trigger on a context switch to a task
 edge : trigger on a context switch from a task to a task

The from and to trigger types requires a task name or task id or both be
provided. The edge requires a from name and/or id and a to name and/or id be
provided.

Flush

  usage: cflush [-n]

Flush the trace record. The option '-n' stops the capture engine be
primed. This means an exising trigger state will not be cleared and tracing
will continue.

Status.

The following is a list of outstanding issues or bugs.

1) The capture engine does not scan the existing list of tasks in the kernel
   when initialised. This means tasks that exist but are not active are not
   seen. Not sure how to implement this one.

2) The blocking read of trace records has not been completely implemented or
   tested.  This will wait until I complete the csv support for the cli for a
   serial UI or the tcp server is implemented.

3) Task control block clean up is not implemented. The control block should be
   dumped to the trace buffer. This requires extended record formats. This can
   be implemented using an event flag to indicate an extended record follows
   the trace record. This would allow a task delete record to be directly
   followed by the task information.

4) Complete csv (comma separated variable) support for the CLI.

5) Implement a tcp server interface.

6) Complete the capture engine API documentation.

7) Test the user supplied time stamp handler.

8) Task name support is only for the rtems_name type. This means the only the
   classic API tasks are currently supported. Partial support for the different
   task names is provided how-ever this is not clean and does not support the
   variable length task name such as found in the POSIX tasks.

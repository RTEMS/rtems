Introduction
============

This directory contains a stack bounds checker.  It provides two
primary features:

   + check for stack overflow at each context switch
   + provides an educated guess at each task's stack usage

Enabling
========

Add the stack checker extension to the initial user extension set.
If using confdefs.h to build your configuration table, this is
as simple as adding -DSTACK_CHECK_ON to the gcc command line which
compiles the file defining the configuration table.  In the RTEMS
test suites and samples, this is always init.c

Background
==========

The stack overflow check at context switch works by looking for
a 16 byte pattern at the logical end of the stack to be corrupted.
The "guesser" assumes that the entire stack was prefilled with a known
pattern and assumes that the pattern is still in place if the memory
has not been used as a stack.

Both of these can be fooled by pushing large holes onto the stack
and not writing to them... or (much more unlikely) writing the
magic patterns into memory.

This code has not been extensively tested.  It is provided as a tool
for RTEMS users to catch the most common mistake in multitasking
systems ... too little stack space.  Suggestions and comments are appreciated.

NOTES:

1.  Stack usage information is questionable on CPUs which push
    large holes on stack.

2.  The stack checker has a tendency to generate a fault when
    trying to print the helpful diagnostic message.  If it comes
    out, congratulations. If not, then the variable Stack_check_Blown_task
    contains a pointer to the TCB of the offending task.  This
    is usually enough to go on.

FUTURE:

1.  Determine how/if gcc will generate stack probe calls and support that.

@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter RTEMS Concepts

The questions in this category are hints that help basic understanding.

@section RTEMS Workspace versus Program Heap

The RTEMS Workspace is used to allocate space for objects created
by RTEMS such as tasks, semaphores, message queues, etc..  It is
primarily used during system initialization although task stacks
and message buffer areas are also allocated from here. 
@ref{How do I determine how much memory is left?}.

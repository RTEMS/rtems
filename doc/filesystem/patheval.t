@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Pathname Evaluation

XXX Include graphic of the path evaluation process with Jennifer's
explanations

@itemize @bullet

@item There are two pathname evaluation routines.  The handler patheval()
is called to find, verify privlages on and return information on a node
that exists.  The handler evalformake() is called to find, verify
permissions, and return information on a node that is to become a parent.  
Additionally, evalformake() returns a pointer to the start of the name of
the new node to be created.

@item Pathname evaluation is specific to a filesystem

@item Mechanics of crossing a mount point during the evaluation of a path
name

@item Role of rtems_filesystem_location_info_t structure

@itemize @bullet

@item Finding filesystem node information

@item Finding filesystem node handlers

@item Finding filesystem node operations table

@item Finding mount table entry for the filesystem that this node is part
of 

@end itemize

@end itemize



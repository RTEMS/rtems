@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Pathname Evaluation

XXX Include graphic of the path evaluation process with Jennifer's explanations 

? There are two pathname evaluation routines.  The handler patheval() is called to find, 
verify privlages on and return information on a node that exists.  The handler 
evalformake() is called to find, verify permissions, and return information on a node 
that is to become a parent.  Additionally, evalformake() returns a pointer to the start 
of the name of the new node to be created. 
? Pathname evaluation is specific to a file system
? Mechanics of crossing a mount point during the evaluation of a path name
? Role of rtems_filesystem_location_info_t structure
? Finding file system node information
? Finding file system node handlers
? Finding file system node operations table
? Finding mount table entry for the file system that this node is part of



@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter = Introduction

Before reading this documentation, it is strongly advised to read the
RTEMS Development Environment Guide to get acquainted with the RTEMS
directory structure.  This document describes how to do a RTEMS Board
Support Package, i.e. how to port RTEMS on a new target board. Discussions
are provided for the following topics:

@itemize @bullet

@item RTEMS Board Support Packages organization,

@item Makefiles and the linker command script,

@item Board initialization code,

@item Clock driver,

@item Console driver,

@item Timer driver.

@end itemize

The original version of this manual was written by Geoffroy Montel
<g_montel@yahoo.com>.  When he started development of the gen68340
BSP, this manual did not exist.  He wrote this manual as the result
of his experiences.  At that time, this document was viewed internally
as the most important "missing manual" in the RTEMS documentation set.

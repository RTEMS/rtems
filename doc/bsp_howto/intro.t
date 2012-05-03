@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Introduction

Before reading this documentation, it is strongly advised to read the
RTEMS Development Environment Guide to get acquainted with the RTEMS
directory structure.  This document describes how to do a RTEMS Board
Support Package, i.e. how to port RTEMS on a new target board. Discussions
are provided for the following topics:

@itemize @bullet

@item RTEMS Board Support Package Organization

@item Makefiles and the Linker Command Script

@item Board Initialization Sequence

@item Device Drivers Including:

@itemize @bullet

@item Console Driver

@item Clock Driver

@item Timer Driver

@item Real-Time Clock Driver

@item Non-Volatile Memory Driver

@item Networking Driver

@item Shared Memory Support Driver

@item Analog Driver

@item Discrete Driver

@end itemize

@end itemize

The original version of this manual was written by Geoffroy Montel
<g_montel@@yahoo.com>.  When he started development of the gen68340
BSP, this manual did not exist.  He wrote the initial version of
this manual as the result of his experiences.  At that time, this
document was viewed internally as the most important "missing manual"
in the RTEMS documentation set.

The gen68340 BSP is a good example of the life of an RTEMS BSP.  It is
based upon a part not recommended for new designs and none of the core RTEMS
Project team members have one of these boards.  Thus we are unlikely to
perform major updates on this BSP.  So as long as it compiles and links all
tests, it will be available.  

The RTEMS Project team members are always trying to identify common
code across BSPs and refactoring the code into shared routines.
As part of this effort, the we will enhance the common BSP Framework.
Not surprisingly, not every BSP takes advantage of every feature in
the framework.  The gen68340 does not take advantage of as many features
as the ERC32 BSP does.  So in many ways, the ERC32 is a better example
BSP at this point.  But even the ERC32 BSP does not include examples
of every driver template and framework available to the BSP author.
So in this guide we will try to point out good examples from other BSPs.

Our goal is for you to be able to reuse as much code as possible and
write as little board specific code as possible.


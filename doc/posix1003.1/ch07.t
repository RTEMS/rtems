@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Device- and Class-Specific Functions

@section General Terminal Interface

@subsection Interface Characteristics

@subsubsection Opening a Terminal Device File

@subsubsection Process Groups (TTY)

@subsubsection The Controlling Terminal

@subsubsection Terminal Access Control

@subsubsection Input Processing and Reading Data

@subsubsection Canonical Mode Input Processing

@subsubsection Noncanonical Mode Input Processing

@subsubsection Writing Data and Output Processing

@subsubsection Special Characters

@subsubsection Modem Disconnect

@subsubsection Closing a Terminal Device File

@subsection Parameters That Can Be Set

@subsubsection termios Structure

@subsubsection Input Modes

@subsubsection Output Modes

@subsubsection Control Modes

@subsubsection Local Modes

@subsubsection Special Control Characters

@subsection Baud Rate Values

@subsubsection Baud Rate Functions

@subsubsection Synopsis

@subsubsection Description

@subsubsection Returns

@subsubsection Errors

@subsubsection Cross-References

@section General Terminal Interface Control Functions

@subsection Get and Set State

@example
tcgetattr(), Function
tcsetattr(), Function
@end example

@subsection Line Control Functions

@example
tcsendbreak(), Function
tcdrain(), Function
tcflush(), Function
tcflow(), Function
@end example

@subsection Get Foreground Process Group ID

@example
tcgetprgrp(), Function
@end example

@subsection Set Foreground Process Group ID

@example
tcsetprgrp(), Function
@end example


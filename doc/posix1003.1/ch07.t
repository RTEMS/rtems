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

@subsubsection @code{termios} Structure

@example
struct termios, Type, Partial Implementation
@end example

@subsubsection Input Modes

@subsubsection Output Modes

@subsubsection Control Modes

@subsubsection Local Modes

@subsubsection Special Control Characters

@subsection Baud Rate Values

@subsubsection Baud Rate Functions

@example
cfgetospeed(), Function, Unimplemented
cfsetospeed(), Function, Unimplemented
cfgetispeed(), Function, Unimplemented
cfsetispeed(), Function, Unimplemented
@end example

@subsubsection Synopsis

@subsubsection Description

@subsubsection Returns

@subsubsection Errors

@subsubsection Cross-References

@section General Terminal Interface Control Functions

@subsection Get and Set State

@example
tcgetattr(), Function, Implemented
tcsetattr(), Function, Implemented
@end example

@subsection Line Control Functions

@example
tcsendbreak(), Function, Unimplemented
tcdrain(), Function, Unimplemented
tcflush(), Function, Unimplemented
tcflow(), Function, Unimplemented
@end example

@subsection Get Foreground Process Group ID

@example
tcgetprgrp(), Function, Unimplemented
@end example

@subsection Set Foreground Process Group ID

@example
tcsetprgrp(), Function, Unimplemented
@end example


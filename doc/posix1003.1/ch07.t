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

@lowersections

@subsubsection Case A --- MIN > 0 and TIME > 0

@subsubsection Case B --- MIN > 0 and TIME = 0

@subsubsection Case C --- MIN = 0 and TIME > 0

@subsubsection Case D --- MIN = 0 and TIME = 0

@raisesections

@subsubsection Writing Data and Output Processing

@subsubsection Special Characters

@example
INTR, Constant, 
QUIT, Constant, 
ERASE, Constant, 
KILL, Constant, 
EOF, Constant, 
NL, Constant, 
EOL, Constant, 
SUSP, Constant, 
STOP, Constant, 
START, Constant, 
CR, Constant, 
@end example

@subsubsection Modem Disconnect

@subsubsection Closing a Terminal Device File

@subsection Parameters That Can Be Set

@subsubsection @code{termios} Structure

@example
tcflag_t, Type, 
cc_t, Type, 
struct termios, Type, Partial Implementation
@end example

@subsubsection Input Modes

@example
BRKINT, Constant, 
ICRNL, Constant, 
IGNBREAK, Constant, 
IGNCR, Constant, 
IGNPAR, Constant, 
INLCR, Constant, 
INPCK, Constant, 
ISTRIP, Constant, 
IXOFF, Constant, 
IXON, Constant, 
PARMRK, Constant, 
@end example

@subsubsection Output Modes

@example
OPOST, Constant, 
@end example

@subsubsection Control Modes

@example
CLOCKAL, Constant, 
CREAD, Constant, 
CSIZE, Constant, 
CS5, Constant, 
CS6, Constant, 
CS7, Constant, 
CS8, Constant, 
CSTOPB, Constant, 
HUPCL, Constant, 
PARENB, Constant, 
PARODD, Constant, 
@end example

@subsubsection Local Modes

@example
ECHO, Constant, 
ECHOE, Constant, 
ECHOK, Constant, 
ECHONL, Constant, 
ICANON, Constant, 
IEXTEN, Constant, 
ISIG, Constant, 
NOFLSH, Constant, 
TOSTOP, Constant, 
@end example

@subsubsection Special Control Characters

@example
VEOF, Constant, 
VEOL, Constant, 
VERASE, Constant, 
VINTR, Constant, 
VKILL, Constant, 
VQUIT, Constant, 
VSUSP, Constant, 
VSTART, Constant, 
VSTOP, Constant, 
VMIN, Constant, 
VTIME, Constant, 
@end example

@subsection Baud Rate Values

@example
B0, Constant, 
B50, Constant, 
B75, Constant, 
B110, Constant, 
B134, Constant, 
B150, Constant, 
B200, Constant, 
B300, Constant, 
B600, Constant, 
B1200, Constant, 
B1800, Constant, 
B2400, Constant, 
B4800, Constant, 
B9600, Constant, 
B19200, Constant, 
B38400, Constant, 
@end example

@subsubsection Baud Rate Functions

@example
cfgetospeed(), Function, Unimplemented
cfsetospeed(), Function, Unimplemented
cfgetispeed(), Function, Unimplemented
cfsetispeed(), Function, Unimplemented
TCIFLUSH, Constant, 
TCOFLUSH, Constant, 
TCIOFLUSH, Constant, 
TCOOFF, Constant, 
TCOON, Constant, 
TCIOOFF, Constant, 
TCIOON, Constant, 
@end example

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


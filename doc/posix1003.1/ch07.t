@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

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

@itemize

@item Case A - MIN > 0 and TIME > 0

@item Case B - MIN > 0 and TIME = 0

@item Case C - MIN = 0 and TIME > 0

@item Case D - MIN = 0 and TIME = 0

@end itemize

@subsubsection Writing Data and Output Processing

@subsubsection Special Characters

@example
INTR, Constant, Implemented
QUIT, Constant, Implemented
ERASE, Constant, Implemented
KILL, Constant, Implemented
EOF, Constant, Implemented
NL, Constant, Implemented
EOL, Constant, Implemented
SUSP, Constant, Implemented
STOP, Constant, Implemented
START, Constant, Implemented
CR, Constant, Implemented
@end example

@subsubsection Modem Disconnect

@subsubsection Closing a Terminal Device File

@subsection Parameters That Can Be Set

@subsubsection termios Structure

@example
tcflag_t, Type, Implemented
cc_t, Type, Implemented
struct termios, Type, Implemented
@end example

@subsubsection Input Modes

@example
BRKINT, Constant, Implemented
ICRNL, Constant, Implemented
IGNBREAK, Constant, Unimplemented
IGNCR, Constant, Implemented
IGNPAR, Constant, Implemented
INLCR, Constant, Implemented
INPCK, Constant, Implemented
ISTRIP, Constant, Implemented
IXOFF, Constant, Implemented
IXON, Constant, Implemented
PARMRK, Constant, Implemented
@end example

@subsubsection Output Modes

@example
OPOST, Constant, Implemented
@end example

@subsubsection Control Modes

@example
CLOCAL, Constant, Implemented
CREAD, Constant, Implemented
CSIZE, Constant, Implemented
CS5, Constant, Implemented
CS6, Constant, Implemented
CS7, Constant, Implemented
CS8, Constant, Implemented
CSTOPB, Constant, Implemented
HUPCL, Constant, Implemented
PARENB, Constant, Implemented
PARODD, Constant, Implemented
@end example

@subsubsection Local Modes

@example
ECHO, Constant, Implemented
ECHOE, Constant, Implemented
ECHOK, Constant, Implemented
ECHONL, Constant, Implemented
ICANON, Constant, Implemented
IEXTEN, Constant, Implemented
ISIG, Constant, Implemented
NOFLSH, Constant, Implemented
TOSTOP, Constant, Implemented
@end example

@subsubsection Special Control Characters

@example
VEOF, Constant, Implemented
VEOL, Constant, Implemented
VERASE, Constant, Implemented
VINTR, Constant, Implemented
VKILL, Constant, Implemented
VQUIT, Constant, Implemented
VSUSP, Constant, Implemented
VSTART, Constant, Implemented
VSTOP, Constant, Implemented
VMIN, Constant, Implemented
VTIME, Constant, Implemented
@end example

@subsection Baud Rate Values

@example
B0, Constant, Implemented
B50, Constant, Implemented
B75, Constant, Implemented
B110, Constant, Implemented
B134, Constant, Implemented
B150, Constant, Implemented
B200, Constant, Implemented
B300, Constant, Implemented
B600, Constant, Implemented
B1200, Constant, Implemented
B1800, Constant, Implemented
B2400, Constant, Implemented
B4800, Constant, Implemented
B9600, Constant, Implemented
B19200, Constant, Implemented
B38400, Constant, Implemented
@end example

@subsubsection Baud Rate Functions

@example
cfgetospeed(), Function, Implemented
cfsetospeed(), Function, Implemented
cfgetispeed(), Function, Implemented
cfsetispeed(), Function, Implemented
TCIFLUSH, Constant, Implemented
TCOFLUSH, Constant, Implemented
TCIOFLUSH, Constant, Implemented
TCOOFF, Constant, Implemented
TCOON, Constant, Implemented
TCIOOFF, Constant, Implemented
TCIOON, Constant, Implemented
@end example

@section General Terminal Interface Control Functions

@subsection Get and Set State

@example
tcgetattr(), Function, Implemented
tcsetattr(), Function, Implemented
@end example

@subsection Line Control Functions

@example
tcsendbreak(), Function, Dummy Implementation
tcdrain(), Function, Implemented
tcflush(), Function, Dummy Implementation
tcflow(), Function, Dummy Implementation
@end example

@subsection Get Foreground Process Group ID

@example
tcgetprgrp(), Function, Implemented, SUSP
@end example

@subsection Set Foreground Process Group ID

@example
tcsetprgrp(), Function, Dummy Implementation
@end example


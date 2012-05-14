@c
@c COPYRIGHT (c) 1988-2002.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.

@chapter Device- and Class- Specific Functions Manager

@section Introduction

The device- and class- specific functions manager is ...

The directives provided by the device- and class- specific functions
manager are:

@itemize @bullet
@item @code{cfgetispeed} - Reads terminal input baud rate
@item @code{cfgetospeed} - Reads terminal output baud rate
@item @code{cfsetispeed} - Sets terminal input baud rate
@item @code{cfsetospeed} - Set terminal output baud rate
@item @code{tcgetattr} - Gets terminal attributes
@item @code{tcsetattr} - Set terminal attributes
@item @code{tcsendbreak} - Sends a break to a terminal
@item @code{tcdrain} - Waits for all output to be transmitted to the terminal
@item @code{tcflush} - Discards terminal data
@item @code{tcflow} - Suspends/restarts terminal output
@item @code{tcgetpgrp} - Gets foreground process group ID
@item @code{tcsetpgrp} - Sets foreground process group ID
@end itemize

@section Background

There is currently no text in this section.

@section Operations

There is currently no text in this section.

@section Directives

This section details the device- and class- specific functions manager's
directives. A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@c
@c
@c
@page
@subsection cfgetispeed - Reads terminal input baud rate

@findex cfgetispeed
@cindex  reads terminal input baud rate

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <termios.h>

int cfgetispeed(
  const struct termios *p
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

The @code{cfgetispeed()} function returns a code for baud rate.

@subheading DESCRIPTION:

The @code{cfsetispeed()} function stores a code for the terminal speed
stored in a struct termios. The codes are defined in @code{<termios.h>}
by the macros BO, B50, B75, B110, B134, B150, B200, B300, B600, B1200,
B1800, B2400, B4800, B9600, B19200, and B38400.

The @code{cfsetispeed()} function does not do anything to the hardware.
It merely stores a value for use by @code{tcsetattr()}.

@subheading NOTES:

Baud rates are defined by symbols, such as B110, B1200, B2400. The actual
number returned for any given speed may change from system to system.

@c
@c
@c
@page
@subsection cfgetospeed - Reads terminal output baud rate

@findex cfgetospeed
@cindex  reads terminal output baud rate

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <termios.h>

int cfgetospeed(
  const struct termios *p
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

The @code{cfgetospeed()} function returns the termios code for the baud rate.

@subheading DESCRIPTION:

The @code{cfgetospeed()} function returns a code for the terminal speed
stored in a @code{struct termios}. The codes are defined in @code{<termios.h>}
by the macros BO, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800,
B2400, B4800, B9600, B19200, and B38400.

The @code{cfgetospeed()} function does not do anything to the hardware.
It merely returns the value stored by a previous call to @code{tcgetattr()}.

@subheading NOTES:

Baud rates are defined by symbols, such as B110, B1200, B2400. The actual
number returned for any given speed may change from system to system.

@c
@c
@c
@page
@subsection cfsetispeed - Sets terminal input baud rate

@findex cfsetispeed
@cindex  sets terminal input baud rate

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <termios.h>

int cfsetispeed(
  struct termios *p,
  speed_t         speed
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

The @code{cfsetispeed()} function returns a zero when successful and
returns -1 when an error occurs.

@subheading DESCRIPTION:

The @code{cfsetispeed()} function stores a code for the terminal speed
stored in a struct termios. The codes are defined in @code{<termios.h>}
by the macros B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200,
B1800, B2400, B4800, B9600, B19200, and B38400.

@subheading NOTES:

This function merely stores a value in the @code{termios} structure. It
does not change the terminal speed until a @code{tcsetattr()} is done.
It does not detect impossible terminal speeds.

@c
@c
@c
@page
@subsection cfsetospeed - Sets terminal output baud rate

@findex cfsetospeed
@cindex  sets terminal output baud rate

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <termios.h>

int cfsetospeed(
  struct termios *p,
  speed_t         speed
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

The @code{cfsetospeed()} function returns a zero when successful and
returns -1 when an error occurs.


@subheading DESCRIPTION:

The @code{cfsetospeed()} function stores a code for the terminal speed stored
in a struct @code{termios}. The codes are defiined in @code{<termios.h>} by the
macros B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400,
B4800, B9600, B19200, and B38400.

The @code{cfsetospeed()} function does not do anything to the hardware. It
merely stores a value for use by @code{tcsetattr()}.

@subheading NOTES:

This function merely stores a value in the @code{termios} structure.
It does not change the terminal speed until a @code{tcsetattr()} is done.
It does not detect impossible terminal speeds.

@c
@c
@c
@page
@subsection tcgetattr - Gets terminal attributes

@findex tcgetattr
@cindex  gets terminal attributes

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <termios.h>
#include <unistd.h>

int tcgetattr(
  int             fildes,
  struct termios *p
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
Invalid file descriptor

@item ENOOTY
Terminal control function attempted for a file that is not a terminal.
@end table

@subheading DESCRIPTION:

The @code{tcgetattr()} gets the parameters associated with the terminal
referred to by @code{fildes} and stores them into the @code{termios()}
structure pointed to by @code{termios_p}.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection tcsetattr - Set terminal attributes

@findex tcsetattr
@cindex  set terminal attributes

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <termios.h>
#include <unistd.h>

int tcsetattr(
  int                   fildes,
  int                   options,
  const struct termios *tp
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@c
@c
@c
@page
@subsection tcsendbreak - Sends a break to a terminal

@findex tcsendbreak
@cindex  sends a break to a terminal

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int tcsendbreak(
  int fd
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This routine is not currently supported by RTEMS but could be
in a future version.

@c
@c
@c
@page
@subsection tcdrain - Waits for all output to be transmitted to the terminal.

@findex tcdrain
@cindex  waits for all output to be transmitted to the terminal.

@subheading CALLING SEQUENCE:

@ifset is-C
@example
#include <termios.h>
#include <unistd.h>

int tcdrain(
  int fildes
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
Invalid file descriptor

@item EINTR
Function was interrupted by a signal

@item ENOTTY
Terminal control function attempted for a file that is not a terminal.

@end table

@subheading DESCRIPTION:

The @code{tcdrain()} function waits until all output written to
@code{fildes} has been transmitted.

@subheading NOTES:

NONE

@c
@c
@c
@page
@subsection tcflush - Discards terminal data

@findex tcflush
@cindex  discards terminal data

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int tcflush(
  int fd
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This routine is not currently supported by RTEMS but could be
in a future version.

@c
@c
@c
@page
@subsection tcflow - Suspends/restarts terminal output.

@findex tcflow
@cindex  suspends/restarts terminal output.

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int tcflow(
  int fd
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This routine is not currently supported by RTEMS but could be
in a future version.

@c
@c
@c
@page
@subsection tcgetpgrp - Gets foreground process group ID

@findex tcgetpgrp
@cindex  gets foreground process group id

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int tcgetpgrp(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This routine is not currently supported by RTEMS but could be
in a future version.

@c
@c
@c
@page
@subsection tcsetpgrp - Sets foreground process group ID

@findex tcsetpgrp
@cindex  sets foreground process group id

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int tcsetpgrp(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

This routine is not currently supported by RTEMS but could be
in a future version.



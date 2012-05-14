@c
@c  COPYRIGHT (c) 1989-2007.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter RTEMS Specific Utilities

This section describes the additional commands
available within the @b{RTEMS Development Environment}.  Although
some of these commands are of general use, most are included to
provide some capability necessary to perform a required function
in the development of the RTEMS executive, one of its support
components, or an RTEMS based application.  

Some of the commands are implemented as C programs.
However, most commands are implemented as Bourne shell scripts.
Even if the current user has selected a different shell, the
scripts will automatically invoke the Bourne shell during their
execution lifetime.

The commands are presented in UNIX manual page style
for compatibility and convenience.  A standard set of paragraph
headers were used for all of the command descriptions.  If a
section contained no data, the paragraph header was omitted to
conserve space.  Each of the permissible paragraph headers and
their contents are described below:

@table @code
@item SYNOPSIS
describes the command syntax

@item DESCRIPTION
a full description of the command

@item OPTIONS
describes each of the permissible options for the command

@item NOTES
lists any special noteworthy comments about the command

@item ENVIRONMENT
describes all environment variables utilized by the command

@item EXAMPLES
illustrates the use of the command with specific examples

@item FILES
provides a list of major files that the command references

@item SEE ALSO
lists any relevant commands which can be consulted
@end table

Most environment variables referenced by the commands
are defined for the RTEMS Development Environment during the
login procedure.  During login, the user selects a default RTEMS
environment through the use of the Modules package.  This tool
effectively sets the environment variables to provide a
consistent development environment for a specific user.
Additional environment variables within the RTEMS environment
were set by the system administrator during installation.  When
specifying paths, a command description makes use of these
environment variables.

When referencing other commands in the SEE ALSO
paragraph, the following notation is used:   command(code).
Where command is the name of a related command, and code is a
section number.  Valid section numbers are as follows:

@table @code
@item 1 
Section 1 of the standard UNIX documentation

@item  1G 
Section 1 of the GNU documentation

@item  1R 
a manual page from this document, the RTEMS Development Environment Guide
@end table

For example, ls(1) means see the standard ls command
in section 1 of the UNIX documentation.  gcc020(1G) means see
the description of gcc020 in section 1 of the GNU documentation.

@c
@c packhex
@c
@section packhex - Compress Hexadecimal File

@subheading SYNOPSIS

@example
packhex <source >destination
@end example

@subheading DESCRIPTION

packhex accepts Intel Hexadecimal or Motorola Srecord
on its standard input and attempts to pack as many contiguous
bytes as possible into a single hexadecimal record.  Many
programs output hexadecimal records which are less than 80 bytes
long (for human viewing).  The overhead required by each
unnecessary record is significant and packhex can often reduce
the size of the download image by 20%.  packhex attempts to
output records which are as long as the hexadecimal format
allows.

@subheading OPTIONS

This command has no options.

@subheading EXAMPLES

Assume the current directory contains the Motorola
Srecord file download.sr. Then executing the command:

@example
packhex <download.sr >packed.sr
@end example

will generate the file packed.sr which is usually
smaller than download.sr.

@subheading CREDITS

The source for packhex first appeared in the May 1993
issue of Embedded Systems magazine.  The code was downloaded
from their BBS.  Unfortunately, the author's name was not
provided in the listing.

@c
@c unhex
@c
@section unhex - Convert Hexadecimal File into Binary Equivalent

@subheading SYNOPSIS

@example
unhex [-valF] [-o file] [file [file ...] ]
@end example

@subheading DESCRIPTION

unhex accepts Intel Hexadecimal, Motorola Srecord, or
TI 'B' records and converts them to their binary equivalent.
The output may sent to standout or may be placed in a specified
file with the -o option.  The designated output file may not be
an input file.  Multiple input files may be specified with their
outputs logically concatenated into the output file.

@subheading OPTIONS

This command has the following options:

@table @code
@item v
Verbose

@item a base
First byte of output corresponds with base
address

@item l
Linear Output

@item o file
Output File

@item F k_bits
Fill holes in input with 0xFFs up to k_bits * 1024 bits
@end table

@subheading EXAMPLES

The following command will create a binary equivalent
file for the two Motorola S record files in the specified output
file binary.bin:

@example
unhex -o binary.bin downloadA.sr downloadB.sr
@end example


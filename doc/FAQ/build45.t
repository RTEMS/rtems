@c
@c  $Id$
@c

@chapter Building RTEMS 4.5

Building any package in a cross-compilation fashion can be difficult,
but configuring and building a real-time operating system that 
supports many CPU families and target boards can be confusing.  The
RTEMS development team has made every effort to make this process as
straight forward as possible but there are going to be questions.

Moreover, between RTEMS 4.0 and 4.5, the configure and Makefile system in RTEMS
was changed to be more compatible with GNU standards.  This transition
has lead to a number of subtle differences.  

This section of the FAQ tries to address the more frequently asked
questions about building RTEMS 4.5.  Thanks to Ralf Corsepius for
compiling this section from excerpts from various postings to the
rtems-users mailing list.

@section Required Tools

@subsection Which tools are required to build RTEMS?

@itemize @bullet

@item A native C-Toolchain, gcc prefered
@item GNU-Bash and shell utils
@item GNU-make.
@item A target (typically cross) C- and (optional) C++-Toolchain.
@item autoconf/automake (optional, recommended)
@item Perl (optional, recommended, needed by automake and some tools within RTEMS)
@item m4 (optional, recommended, needed by autoconf, GNU-m4 preferred)

@end itemize

@subsection Do I need autoconf and automake to build RTEMS?

No, you don't.  Or to be more accurate, you won't need them until you
modify something in RTEMS's Makefile.ams/configure.ins or start to develop
with RTEMS.
 
I.e.  you won't need them to get started, but you will need them when getting
serious.

@subsection Do I need a native gcc on my host?

No, you should be able to use any native, ansi-compliant C-compiler, but
using a native gcc is highly recommended.

@subsection Can I use a non-gcc cross-toolchain?


Generally speaking, it should be possible.
However, most RTEMS-4.5 development has taken place using gcc-2.9x, therefore
getting it working may not be easy.

@subsection Do I need gcc-2.9x for cross compilation?

Not necessarily, but gcc-2.9x is highly recommended, because most development
has taken place using gcc-2.9x and previous versions of gcc are not actively
supported in RTEMS anymore (@ref{Can I use a non-gcc cross-toolchain?}).

@subsection Where to get autoconf automake ld gcc etc.  ?

The sources of all gnutools are available at any 
@uref{GNU,ftp://ftp.gnu.org} mirror.
Native Linux binaries should come with any Linux distribution.
Native Cygwin binaries should be available at Cygnus.

GNU-Toolchain binaries (gcc, binutils etc.) for Linux and patches required
to build them from source are available from 
@uref{OAR Corporation,ftp://ftp.oarcorp.com}.


@section Issues when building RTEMS

@subsection When running ./configure weird thing start to happen

You are probably trying to build within the source-tree.
RTEMS requires a separate build directory.  I.e.  if the
sources are located at @code{/usr/local/src/rtems-4.5.0},
use something similar to this to configure RTEMS: 

@example
cd somewhere
mkdir build
cd build
/usr/local/src/rtems-4.5.0/configure [options]
@end example

@subsection Why can I not build RTEMS inside of the source tree?


The build-directory hierarchy is setup dynamically at configuration time.

Configuring inside of the source tree would prevent being able to configure
for multiple targets simultaneously.

Using a separate build-tree simplifies Makefiles and configure scripts
significantly.

Adaptation to GNU/Cygnus conventions.

@subsection Which environment variables to set?

None.  Unlike for previous releases, it is not recommended anymore to set any
RTEMS related environment variable (Exception: $PATH, cf.
@ref{How to set up $PATH?}).


@subsection Compiler /Assembler /Linker report errors

If you see a bunch of the error messages related to invalid instructions
or similar, then probably your @code{$PATH} environment variable is not
set up correctly (cf.  @ref{How to set up $PATH?}).  Otherwise you might
have found a bug either in RTEMS or parts of the toolchain.

@subsection How to set up $PATH?

All target tools are supposed to be prefixed with a target-canonicalization
prefix, eg.  i386-rtems-gcc, m68k-rtems-ld are target tools.

Host tools are supposed not to be prefixed.
e.g.: cc, ld, gcc etc.

If using OAR Corporation's rpms for the toolchain, simply prepend
@code{/opt/rtems/bin} to @code{$PATH}.

@subsection Can I build RTEMS Canadian Cross?

Unfortunately, not (yet).

@subsection Building RTEMS is slow

RTEMS has become fairly large :).

In comparison to building previous versions, building RTEMS-4.5 is slow,
 but that's the tradeoff to pay for simplier and safer configuration.

If using Cygwin, remember that Cygwin is emulating one OS ontop of another
 -- this necessarily must be significantly slower than using U*nix on the
 same hardware.

@subsection Building my pre-4.5.x BSPs does not work anymore

See @ref{How to merge pre-RTEMS-4.5.0 BSPs into RTEMS-4.5.0?}.

@subsection make debug_install / make profile_install

These make targets are not supported anymore.  Instead, use:

@example
make VARIANT=DEBUG install
make VARIANT=PROFILE install
@end example

@subsection make debug / make profile

These make targets are not supported anymore.
Instead, use:

@example
make VARIANT=DEBUG all
make VARIANT=PROFILE all
@end example

@subsection Building RTEMS does not honor XXX_FOR_TARGET

RTEMS currently does not support passing flags from the environment.
Instead add a @code{make/custom/mybsp.cfg} for your bsp and set
appropriate flags there.

@subsection Editing Makefile.in Makefile configure

These files are generated by auto* tools, cf.
@ref{Editing auto* generated files}).

@subsection Editing auto* generated files


RTEMS uses automake, therefore @b{never}, @b{ever}, @b{ever}
edit Makefile.ins, Makefiles, configure or other auto* generated files.
Changes to them will be swapped away soon and will get lost.

Instead edit the sources (eg.: Makefile.ams, configure.ins) auto* generated
files are generated from directly.

If sending patches always send Makefile.ams and configure.ins.
Sending Makefile.ins, Makefiles and configure scripts is pretty much useless.
If sending larger patches, consider removing all auto* generated files
by running @code{bootstrap -c} (cf. See @ref{./bootstrap})
before running diff to cut a patch.

If you don't understand what this is all about, try start getting familiar
with auto* tools by reading autoconf.info and automake.info, or feel free
to ask for assistance on the RTEMS Mailing List
(See @ref{Are there any mailing lists?}.

@section Host Operating Systems and RTEMS

@subsection Can I use Windows or DOS?


No, plain DOS and plain Win will not work, but Cygwin should.
Other U*nix emulations, such as Mingw and DJGPP are not supported and very
likely will not work.
Cywin / WinNT is known to work, but at the time of writing this, there
seem to persist non-RTEMS related issues with Cygwin under Win9x which
seem to prevent success on those systems.

@subsection Do I need Linux?


No, you should be able to build RTEMS on any U*ix OS and under Cygwin/NT
(cf. @ref{Can I use Windows or DOS?}).
 
@subsection Which Linux distribution is recommended?

None, any recent U*nix should work, i.e.
any recent Linux distribution should work, too.

@section Development related questions

@subsection How to merge pre-RTEMS-4.5.0 BSPs into RTEMS-4.5.0?

The simple answer is that between 4.0 and now, RTEMS has moved to automake
and greater compliance with GNU conventions.
In 4.0, there was a single configure script at the top of the tree.
Now RTEMS is configured more like other GNU tools -- as a collection of
configurable entities.
 

Each BSP now has its own configure script.
I highly recommend you look at the Makefile.am's, configure.in, of a similar
BSP.  You might even want to consider running "bootstrap -c" from the top of
the tree and looking at what is left.  bootstrap (cf. @ref{./bootstrap})
generates/removes all automatically generated files.

@subsection What is no_bsp / no_cpu?

@code{no_bsp} is a fictional BSP for a fictional CPU of type 
@code{no_cpu}.  @code{no_cpu/no_bsp} support files in RTEMS can be used as
templates when implementing BSPs or porting RTEMS to new CPUs.

@subsection What is the bare-BSP?

At the time being RTEMS is build per BSP, with all support files being build
separately for each BSP.  This can become unhandy when using several similar
but not identical boards (e.g.  a PC with different peripherial cards plugged
in), because this in general requires to implement a BSP for each setup.
The bare BSP is a general, setup independent BSP which can be used when
keeping all BSP specific parts external from RTEMS.

At present time the bare BSP is in its infancy.
It is known that it can be build for most CPUs RTEMS supports.
It is also known to work in individual cases, but your mileage may vary.

@subsection Multilib vs.  RTEMS CPU-variants

The GNU toolchain applies a specific classification of similar CPUs into
CPU variants (eg.  SH1, SH2 etc.) to provide better support for each CPU variant.

RTEMS uses a different classification because it internally requires more
details about a specific CPU than the GNU toolchain's multilib classification
provides.

@subsection Keeping auto* generated files in CVS

When using CVS to archive source code, problems arise from keeping generated
files in CVS.  In general, two possible solutions exist:

@itemize @bullet

@item Find a way to get correct timestamps after checking out the sources
from CVS.  Some people try to achieve this by 

@itemize @bullet
@item carefully checking in files into CVS in appropriate order 
@item applying scripts to fix timestamps accordingling (eg.  by applying 
@code{touch} and @code{find}).
@end itemize

@item Not keeping generated files in CVS, but regenerate them after
having checked them out from CVS.

@end itemize

RTEMS favors the the latter variant, because it appears to be less error-prone
and easier to handle (cf. @ref{./bootstrap} for details).

@subsection Importing RTEMS into CVS/RCS

When importing RTEMS into CVS/RCS or similar, we recommend not to import
auto* generated files (cf. @ref{Keeping auto* generated files in CVS}).

To remove them before importing, run

@example
./bootstrap -c
@end example

from the toplevel directory of the source tree (cf. @ref{./bootstrap}).
 
@subsection ./bootstrap


@code{bootstrap} is a simple shell script which automatically generates all
auto* generated files within RTEMS's source tree (Other packages use the name 
@code{autogen.sh} for similar scripts).  You will need to have autoconf,
automake and further underlying packages installed to apply it.

It typically should be applied when having:

@itemize @bullet

@item checked out RTEMS sources from a CVS repository which does
not contain generated files.

@item added new automake / autoconf files to the source tree (eg.
having added a new BSP), and when not being sure about what needs to be
updated.

@end itemize

Once all autoconf/automake generated files are present, you will rarely
need to run @code{bootstrap}, because automake automatically updates
generated files when it detects some files need to be updated (Cf.
@ref{--enable-maintainer-mode}).

@subsection --enable-maintainer-mode

When working within the source-tree, consider to append 
@code{--enable-maintainer-mode} to the options passed to configure RTEMS.

@example
<path>/rtems-4.5.0/configure <options> --enable-maintainer-mode
@end example

This will enable the maintainer-mode in automake generated Makefiles, which
will let automake take care about dependencies between auto* generated
files.  I.e.  auto* generated files will get automatically updated.

Configuring RTEMS in maintainer-mode will require to have autoconf, automake
and underlying tools installed (Cf. @ref{Required Tools}).

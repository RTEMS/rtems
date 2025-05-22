POSIX
=====

This testsuite consists of a collection of (non-functional) program snippets 
to check an RTEMS toolchain consisting of 
* GCC (libgcc etc.)
* binutils
* newlib (libc)
* RTEMS (librtemscpu + librtemsbsp)
for IEEE Std 1003.1-2008 (aka. POSIX[1]) compliance by link-tests.

All of these programs are supposed to be compilable without any warning,
independently of the configuration used in any of the components
involved.

NB: 
- Using "advanced GCC warning flags" may trigger warnings. Such warnings
  should be avoided if possible.

- These program snippets are not supposed to be functional.

- This suite only checks for a subset of POSIX library calls, which are
  known to have been critical to RTEMS/GCC/newlib interaction at some point
  in RTEMS/GCC/newlib's history.

References:
[1] The Open Group Base Specifications Issue 7, IEEE Std 1003.1(tm)-2008,
    http://www.opengroup.org/onlinepubs/9699919799

